#include <lyra/lyra.hpp>
#include <fmt/format.h>
#include <fmt/std.h>
#include <fmt/ostream.h>
#include <cpptrace/cpptrace.hpp>
#include <cpptrace/from_current.hpp>

#include <filesystem>

#include "symbols/dwarf/dwarf.hpp"

using namespace std::literals;
using namespace cpptrace::detail::libdwarf;

template<> struct fmt::formatter<lyra::cli> : ostream_formatter {};

class DwarfDumper {
    std::string object_path;
    Dwarf_Debug dbg = nullptr;

    // Error handling helper
    // For some reason R (*f)(Args..., void*)-style deduction isn't possible, seems like a bug in all compilers
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56190
    // TODO: Duplicate
    template<
        typename... Args,
        typename... Args2,
        typename std::enable_if<
            std::is_same<
                decltype(
                    (void)std::declval<int(Args...)>()(std::forward<Args2>(std::declval<Args2>())..., nullptr)
                ),
                void
            >::value,
            int
        >::type = 0
    >
    int wrap(int (*f)(Args...), Args2&&... args) const {
        Dwarf_Error error = nullptr;
        int ret = f(std::forward<Args2>(args)..., &error);
        if(ret == DW_DLV_ERROR) {
            handle_dwarf_error(dbg, error);
        }
        return ret;
    }

    // TODO: Duplicate
    // walk all CU's in a dbg, callback is called on each die and should return true to
    // continue traversal
    void walk_compilation_units(const std::function<bool(const die_object&)>& fn) {
        // libdwarf keeps track of where it is in the file, dwarf_next_cu_header_d is statefull
        Dwarf_Unsigned next_cu_header;
        Dwarf_Half header_cu_type;
        while(true) {
            int ret = wrap(
                dwarf_next_cu_header_d,
                dbg,
                true,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                &next_cu_header,
                &header_cu_type
            );
            if(ret == DW_DLV_NO_ENTRY) {
                fmt::println("End walk_dbg");
                return;
            }
            if(ret != DW_DLV_OK) {
                PANIC("Unexpected return code from dwarf_next_cu_header_d");
                return;
            }
            // 0 passed as the die to the first call of dwarf_siblingof_b immediately after dwarf_next_cu_header_d
            // to fetch the cu die
            die_object cu_die(dbg, nullptr);
            cu_die = cu_die.get_sibling();
            if(!cu_die) {
                break;
            }
            if(!walk_die_list(cu_die, fn)) {
                break;
            }
        }
        fmt::println("End walk_compilation_units");
    }

    void dump_die_tree(const die_object& die, int depth) {
        walk_die_list(
            die,
            [this, depth] (const die_object& die) {
                fmt::println("{:016x}{: <{}} {}", die.get_global_offset(), "", depth * 2, die.get_tag_name());
                fmt::println("{: <16}{: <{}}   name: {}", "", "", depth * 2, die.get_name());
                fmt::println("");
                auto child = die.get_child();
                if(child) {
                    dump_die_tree(child, depth + 1);
                }
                return true;
            }
        );
    }

    void dump_cu(const die_object& cu_die) {
        Dwarf_Half offset_size = 0;
        Dwarf_Half dwversion = 0;
        dwarf_get_version_of_die(cu_die.get(), &dwversion, &offset_size);
        fmt::println("{:016x} Compile Unit: version = {}, unit type = {}", cu_die.get_global_offset(), dwversion, cu_die.get_tag_name());
        dump_die_tree(cu_die, 0);
    }

public:
    DwarfDumper() = default;
    ~DwarfDumper() {
        dwarf_finish(dbg);
    }

    void dump(std::filesystem::path path) {
        object_path = path;
        auto ret = wrap(
            dwarf_init_path_a,
            object_path.c_str(),
            nullptr,
            0,
            DW_GROUPNUMBER_ANY,
            0,
            nullptr,
            nullptr,
            &dbg
        );
        if(ret == DW_DLV_OK) {
            // ok
        } else if(ret == DW_DLV_NO_ENTRY) {
            // fail, no debug info
            fmt::println(stderr, "No debug info");
            std::exit(1);
        } else {
            fmt::println(stderr, "Error: Unknown return code from dwarf_init_path {}", ret);
            std::exit(1);
        }
        walk_compilation_units([this] (const die_object& cu_die) {
            dump_cu(cu_die);
            return true;
        });
    }
};

int main(int argc, char** argv) CPPTRACE_TRY {
    bool show_help = false;
    std::filesystem::path path;
    auto cli = lyra::cli()
        | lyra::help(show_help)
        | lyra::arg(path, "binary path")("binary to dwarfdump").required();
    if(auto result = cli.parse({ argc, argv }); !result) {
        fmt::println(stderr, "Error in command line: {}", result.message());
        fmt::println("{}", cli);
        return 1;
    }
    if(show_help) {
        fmt::println("{}", cli);
        return 0;
    }
    if(!std::filesystem::exists(path)) {
        fmt::println(stderr, "Error: Path doesn't exist {}", path);
        return 1;
    }
    if(!std::filesystem::is_regular_file(path)) {
        fmt::println(stderr, "Error: Path isn't a regular file {}", path);
        return 1;
    }
    DwarfDumper{}.dump(path);
} CPPTRACE_CATCH(const std::exception& e) {
    fmt::println(stderr, "Caught exception {}: {}", cpptrace::demangle(typeid(e).name()), e.what());
    cpptrace::from_current_exception().print();
}
