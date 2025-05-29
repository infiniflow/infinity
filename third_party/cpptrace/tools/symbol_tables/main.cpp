#include <lyra/lyra.hpp>
#include <fmt/format.h>
#include <fmt/std.h>
#include <fmt/ostream.h>
#include <cpptrace/cpptrace.hpp>
#include <cpptrace/from_current.hpp>

#include <filesystem>

#include "binary/elf.hpp"
#include "binary/mach-o.hpp"

using namespace std::literals;
using namespace cpptrace::detail;

template<> struct fmt::formatter<lyra::cli> : ostream_formatter {};

#if IS_LINUX
void dump_symtab_result(const Result<optional<std::vector<elf::symbol_entry>>, internal_error>& res) {
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
        fmt::println("{:016x} {:016x} {:04x} {}", entry.st_value, entry.st_size, entry.st_shndx, entry.st_name);
    }
}

void dump_symbols(const std::filesystem::path& path) {
    auto elf_ = elf::open(path.native());
    if(!elf_) {
        fmt::println(stderr, "Error reading file: {}", elf_.unwrap_error().what());
    }
    auto& elf = elf_.unwrap_value();
    fmt::println("Symtab:");
    dump_symtab_result(elf.get_symtab_entries());
    fmt::println("Dynamic symtab:");
    dump_symtab_result(elf.get_dynamic_symtab_entries());
}
#elif IS_APPLE
void dump_symbols(const std::filesystem::path& path) {
    fmt::println("Not implemented yet (TODO)");
}
#else
void dump_symbols(const std::filesystem::path&) {
    fmt::println("Unable to dump symbol table on this platform");
}
#endif

int main(int argc, char** argv) CPPTRACE_TRY {
    bool show_help = false;
    std::filesystem::path path;
    auto cli = lyra::cli()
        | lyra::help(show_help)
        | lyra::arg(path, "binary path")("binary to dump symbol tables for").required();
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
    dump_symbols(path);
    return 0;
} CPPTRACE_CATCH(const std::exception& e) {
    fmt::println(stderr, "Caught exception {}: {}", cpptrace::demangle(typeid(e).name()), e.what());
    cpptrace::from_current_exception().print();
}
