#include <lyra/lyra.hpp>
#include <fmt/format.h>
#include <fmt/std.h>
#include <fmt/ostream.h>
#include <cpptrace/cpptrace.hpp>
#include <cpptrace/from_current.hpp>

#include <filesystem>

#include "binary/elf.hpp"
#include "binary/mach-o.hpp"
#include "cpptrace/utils.hpp"

using namespace std::literals;
using namespace cpptrace::detail;

template<> struct fmt::formatter<lyra::cli> : ostream_formatter {};

struct options {
    std::filesystem::path path;
    bool demangle = false;
    bool prune = false;
};

#if IS_LINUX
void dump_symtab_result(
    const Result<optional<std::vector<elf::symbol_entry>>, internal_error>& res,
    const options& options
) {
    if(!res) {
        fmt::println(stderr, "Error loading: {}", res.unwrap_error().what());
    }
    const auto& entries_ = res.unwrap_value();
    if(!entries_) {
        fmt::println("Empty symbol table");
    }
    const auto& entries = entries_.unwrap();
    fmt::println("{:16} {:16} {:4} {}", "value", "size", "shdx", "symbol");
    for(const auto& entry : entries) {
        std::string name;
        if(options.demangle) {
            name = cpptrace::demangle(entry.st_name);
            if(options.prune) {
                name = cpptrace::prune_symbol(name);
            }
        } else {
            name = entry.st_name;
        }
        fmt::println("{:016x} {:016x} {:04x} {}", entry.st_value, entry.st_size, entry.st_shndx, name);
    }
}

auto get_elf(const std::filesystem::path& path) {
    auto elf_ = elf::open(path.native());
    if(!elf_) {
        fmt::println(stderr, "Error reading file: {}", elf_.unwrap_error().what());
    }
    return std::move(elf_).unwrap_value();
}

void dump_symbols(const options& options) {
    auto elf = get_elf(options.path);
    fmt::println("Symtab:");
    dump_symtab_result(elf.get_symtab_entries(), options);
    fmt::println("Dynamic symtab:");
    dump_symtab_result(elf.get_dynamic_symtab_entries(), options);
}
void lookup_symbol(const options& options, cpptrace::frame_ptr address) {
    auto elf = get_elf(options.path);
    if(auto symbol = elf.lookup_symbol(address)) {
        fmt::println("Symbol: {}", options.demangle ? cpptrace::demangle(symbol.unwrap()) : symbol.unwrap());
    } else {
        fmt::println("Could not find symbol");
    }
}
#elif IS_APPLE
void dump_symbols(const options&) {
    fmt::println("Not implemented yet (TODO)");
}
void lookup_symbol(const options&, cpptrace::frame_ptr) {
    fmt::println("Not implemented yet (TODO)");
}
#else
void dump_symbols(const std::filesystem::path&) {
    fmt::println("Not implemented yet (TODO)");
}
void lookup_symbol(const std::filesystem::path&, cpptrace::frame_ptr) {
    fmt::println("Not implemented yet (TODO)");
}
#endif

int symbol_tables(int argc, char** argv) {
    bool show_help = false;
    std::optional<std::string> lookup;
    options options;
    auto cli = lyra::cli()
        | lyra::help(show_help)
        | lyra::opt(lookup, "address")["--lookup"]("address in hex to lookup")
        | lyra::opt(options.demangle)["--demangle"]("demangle the symbol")
        | lyra::opt(options.prune)["--prune"]("prune the symbol")
        | lyra::arg(options.path, "binary path")("binary to dump symbol tables for").required();
    if(auto result = cli.parse({ argc, argv }); !result) {
        fmt::println(stderr, "Error in command line: {}", result.message());
        fmt::println("{}", cli);
        return 1;
    }
    if(show_help) {
        fmt::println("{}", cli);
        return 0;
    }
    if(!std::filesystem::exists(options.path)) {
        fmt::println(stderr, "Error: Path doesn't exist {}", options.path);
        return 1;
    }
    if(!std::filesystem::is_regular_file(options.path)) {
        fmt::println(stderr, "Error: Path isn't a regular file {}", options.path);
        return 1;
    }
    if(lookup) {
        auto address = std::stoull(*lookup, nullptr, 16);
        fmt::println(stderr, "Looking up address {:016x}", address);
        lookup_symbol(options, address);
        return 0;
    }
    dump_symbols(options);
    return 0;
}

int main(int argc, char** argv) {
    int ret = 0;
    CPPTRACE_TRY {
        ret = symbol_tables(argc, argv);
    } CPPTRACE_CATCH(const std::exception& e) {
        fmt::println(stderr, "Caught exception {}: {}", cpptrace::demangle(typeid(e).name()), e.what());
        cpptrace::from_current_exception().print();
        ret = 1;
    }
    return ret;
}
