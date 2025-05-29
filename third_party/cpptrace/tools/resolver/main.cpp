#include "cpptrace/basic.hpp"
#include "cpptrace/formatting.hpp"
#include "cpptrace/forward.hpp"
#include <chrono>
#include <lyra/lyra.hpp>
#include <fmt/format.h>
#include <fmt/std.h>
#include <fmt/ostream.h>
#include <fmt/chrono.h>
#include <cpptrace/cpptrace.hpp>
#include <cpptrace/from_current.hpp>

#include <filesystem>
#include <stdexcept>
#include <string>
#include <thread>

#include "symbols/symbols.hpp"
#include "demangle/demangle.hpp"

using namespace std::literals;
using namespace cpptrace::detail;

template<> struct fmt::formatter<lyra::cli> : ostream_formatter {};

auto formatter = cpptrace::formatter{}.addresses(cpptrace::formatter::address_mode::object);

struct options {
    bool show_help = false;
    std::filesystem::path path;
    std::vector<std::string> address_strings;
    bool from_stdin = false;
    bool keepalive = false;
    bool timing = false;
    bool disable_aranges = false;
    cpptrace::nullable<std::size_t> line_table_cache_size;
};

void resolve(const options& opts, cpptrace::frame_ptr address) {
    cpptrace::object_frame obj_frame{0, address, opts.path.string()};
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<cpptrace::stacktrace_frame> trace = cpptrace::detail::resolve_frames({obj_frame});
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    if(trace.size() != 1) {
        throw std::runtime_error("Something went wrong, trace vector size didn't match");
    }
    trace[0].symbol = cpptrace::demangle(trace[0].symbol);
    formatter.print(trace[0]);
    std::cout<<std::endl;
    if(opts.timing) {
        fmt::println("resolve time: {}", duration);
    }
}

int main(int argc, char** argv) CPPTRACE_TRY {
    options opts;
    auto cli = lyra::cli()
        | lyra::help(opts.show_help)
        | lyra::opt(opts.from_stdin)["--stdin"]("read addresses from stdin")
        | lyra::opt(opts.keepalive)["--keepalive"]("keep the program alive after resolution finishes (useful for debugging)")
        | lyra::opt(opts.timing)["--timing"]("provide timing stats")
        | lyra::opt(opts.disable_aranges)["--disable-aranges"]("don't use the .debug_aranges accelerated address lookup table")
        | lyra::opt(opts.line_table_cache_size.raw_value, "line table cache size")["--line-table-cache-size"]("limit the size of cpptrace's line table cache")
        | lyra::arg(opts.path, "binary path")("binary to look in").required()
        | lyra::arg(opts.address_strings, "addresses")("addresses");
    if(auto result = cli.parse({ argc, argv }); !result) {
        fmt::println(stderr, "Error in command line: {}", result.message());
        fmt::println("{}", cli);
        return 1;
    }
    if(opts.show_help) {
        fmt::println("{}", cli);
        return 0;
    }
    if(!std::filesystem::exists(opts.path)) {
        fmt::println(stderr, "Error: Path doesn't exist {}", opts.path);
        return 1;
    }
    if(!std::filesystem::is_regular_file(opts.path)) {
        fmt::println(stderr, "Error: Path isn't a regular file {}", opts.path);
        return 1;
    }
    if(opts.disable_aranges) {
        cpptrace::experimental::set_dwarf_resolver_disable_aranges(true);
    }
    if(opts.line_table_cache_size.has_value()) {
        cpptrace::experimental::set_dwarf_resolver_line_table_cache_size(opts.line_table_cache_size);
    }
    for(const auto& address : opts.address_strings) {
        resolve(opts, std::stoi(address, nullptr, 16));
    }
    if(opts.from_stdin) {
        std::string word;
        while(std::cin >> word) {
            resolve(opts, std::stoi(word, nullptr, 16));
        }
    }
    if(opts.keepalive) {
        fmt::println("Done");
        while(true) {
            std::this_thread::sleep_for(std::chrono::seconds(60));
        }
    }
    return 0;
} CPPTRACE_CATCH(const std::exception& e) {
    fmt::println(stderr, "Caught exception {}: {}", cpptrace::demangle(typeid(e).name()), e.what());
    cpptrace::from_current_exception().print();
}
