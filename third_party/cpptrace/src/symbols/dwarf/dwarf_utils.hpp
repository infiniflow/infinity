#ifndef DWARF_UTILS_HPP
#define DWARF_UTILS_HPP

#include <cpptrace/basic.hpp>
#include "symbols/dwarf/dwarf.hpp"  // has dwarf #includes
#include "utils/error.hpp"
#include "utils/microfmt.hpp"
#include "utils/utils.hpp"

namespace cpptrace {
namespace detail {
namespace libdwarf {
    class srcfiles {
        Dwarf_Debug dbg = nullptr;
        char** dw_srcfiles = nullptr;
        Dwarf_Unsigned dw_filecount = 0;

    public:
        srcfiles(Dwarf_Debug dbg, char** dw_srcfiles, Dwarf_Signed filecount)
            : dbg(dbg), dw_srcfiles(dw_srcfiles), dw_filecount(static_cast<Dwarf_Unsigned>(filecount))
        {
            if(filecount < 0) {
                throw internal_error(microfmt::format("Unexpected dw_filecount {}", filecount));
            }
        }
        ~srcfiles() {
            release();
        }
        void release() {
            if(dw_srcfiles) {
                for(unsigned i = 0; i < dw_filecount; i++) {
                    dwarf_dealloc(dbg, dw_srcfiles[i], DW_DLA_STRING);
                    dw_srcfiles[i] = nullptr;
                }
                dwarf_dealloc(dbg, dw_srcfiles, DW_DLA_LIST);
                dw_srcfiles = nullptr;
            }
        }
        srcfiles(const srcfiles&) = delete;
        srcfiles(srcfiles&& other) {
            *this = std::move(other);
        }
        srcfiles& operator=(const srcfiles&) = delete;
        srcfiles& operator=(srcfiles&& other) {
            release();
            dbg = exchange(other.dbg, nullptr);
            dw_srcfiles = exchange(other.dw_srcfiles, nullptr);
            dw_filecount = exchange(other.dw_filecount, 0);
            return *this;
        }
        // note: dwarf uses 1-indexing
        const char* get(Dwarf_Unsigned file_i) const {
            if(file_i >= dw_filecount) {
                throw internal_error(microfmt::format(
                    "Error while accessing the srcfiles list, requested index {} is out of bounds (count = {})",
                    file_i,
                    dw_filecount
                ));
            }
            return dw_srcfiles[file_i];
        }
        Dwarf_Unsigned count() const {
            return dw_filecount;
        }
    };

    // sorted range entries for dies
    template<
        typename T,
        typename std::enable_if<
            // old gcc doesn't support this trait https://godbolt.org/z/fKWT9jTK7
            #if !(defined(__GNUC__) && (__GNUC__ < 5))
             std::is_trivially_copyable<T>::value &&
            #endif
             sizeof(T) <= 16,
            int
        >::type = 0
    >
    class die_cache {
    public:
        struct die_handle {
            std::uint32_t die_index;
        };
    private:
        struct PACKED basic_range_entry {
            die_handle die;
            Dwarf_Addr low;
            Dwarf_Addr high;
        };
        struct PACKED annotated_range_entry {
            die_handle die;
            Dwarf_Addr low;
            Dwarf_Addr high;
            T data;
        };
        using range_entry = typename std::conditional<
            std::is_same<T, monostate>::value,
            basic_range_entry,
            annotated_range_entry
        >::type;
        std::vector<die_object> dies;
        std::vector<range_entry> range_entries;
    public:
        die_handle add_die(die_object&& die) {
            dies.push_back(std::move(die));
            VERIFY(dies.size() < std::numeric_limits<std::uint32_t>::max());
            return die_handle{static_cast<std::uint32_t>(dies.size() - 1)};
        }
        template<typename Void = void>
        auto insert(die_handle die, Dwarf_Addr low, Dwarf_Addr high)
            -> typename std::enable_if<std::is_same<T, monostate>::value, Void>::type
        {
            range_entries.push_back({die, low, high});
        }
        template<typename Void = void>
        auto insert(die_handle die, Dwarf_Addr low, Dwarf_Addr high, const T& t)
            -> typename std::enable_if<!std::is_same<T, monostate>::value, Void>::type
        {
            range_entries.push_back({die, low, high, t});
        }
        void finalize() {
            std::sort(range_entries.begin(), range_entries.end(), [] (const range_entry& a, const range_entry& b) {
                return a.low < b.low;
            });
        }
        std::size_t ranges_count() const {
            return range_entries.size();
        }

        struct die_and_data {
            const die_object& die;
            T data;
        };
        template<typename Ret = const die_object&>
        auto make_lookup_result(typename std::vector<range_entry>::const_iterator vec_it) const
            -> typename std::enable_if<std::is_same<T, monostate>::value, Ret>::type
        {
            return dies.at(vec_it->die.die_index);
        }
        template<typename Ret = die_and_data>
        auto make_lookup_result(typename std::vector<range_entry>::const_iterator vec_it) const
            -> typename std::enable_if<!std::is_same<T, monostate>::value, Ret>::type
        {
            return die_and_data{dies.at(vec_it->die.die_index), vec_it->data};
        }
        using lookup_result = typename std::conditional<
            std::is_same<T, monostate>::value,
            const die_object&,
            die_and_data
        >::type;
        optional<lookup_result> lookup(Dwarf_Addr pc) const {
            auto vec_it = first_less_than_or_equal(
                range_entries.begin(),
                range_entries.end(),
                pc,
                [] (Dwarf_Addr pc, const range_entry& entry) {
                    return pc < entry.low;
                }
            );
            if(vec_it == range_entries.end()) {
                return nullopt;
            }
            // This would be an if constexpr if only C++17...
            return make_lookup_result(vec_it);
        }
    };

    struct line_entry {
        Dwarf_Addr low;
        // Dwarf_Addr high;
        // int i;
        Dwarf_Line line;
        optional<std::string> path;
        optional<std::uint32_t> line_number;
        optional<std::uint32_t> column_number;
        line_entry(Dwarf_Addr low, Dwarf_Line line) : low(low), line(line) {}
    };

    struct line_table_info {
        Dwarf_Unsigned version = 0;
        Dwarf_Line_Context line_context = nullptr;
        // sorted by low_addr
        // TODO: Make this optional at some point, it may not be generated if cache mode switches during program exec...
        std::vector<line_entry> line_entries;

        line_table_info(
            Dwarf_Unsigned version,
            Dwarf_Line_Context line_context,
            std::vector<line_entry>&& line_entries
        ) : version(version), line_context(line_context), line_entries(std::move(line_entries)) {}
        ~line_table_info() {
            release();
        }
        void release() {
            dwarf_srclines_dealloc_b(line_context);
            line_context = nullptr;
        }
        line_table_info(const line_table_info&) = delete;
        line_table_info(line_table_info&& other) {
            *this = std::move(other);
        }
        line_table_info& operator=(const line_table_info&) = delete;
        line_table_info& operator=(line_table_info&& other) {
            release();
            version = other.version;
            line_context = exchange(other.line_context, nullptr);
            line_entries = std::move(other.line_entries);
            return *this;
        }
    };
}
}
}

#endif
