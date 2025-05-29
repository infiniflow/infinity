#include <iostream>

#ifdef TEST_MODULE
import cpptrace;
#else
#include <cpptrace/basic.hpp>
#include <cpptrace/utils.hpp>
#include <cpptrace/formatting.hpp>
#include <cpptrace/exceptions.hpp>
#endif

// These are just here to make sure all symbols are found

int main() {
    auto r = cpptrace::raw_trace::current();
    r = cpptrace::raw_trace::current(0, 100);
    r.resolve();
    r.clear();
    r.empty();

    auto o = cpptrace::object_trace::current();
    o = cpptrace::object_trace::current(0, 100);
    o.resolve();
    o.clear();
    o.empty();

    auto s = cpptrace::stacktrace::current();
    s = cpptrace::stacktrace::current(0, 100);
    s.print();
    s.print(std::cerr);
    s.print(std::cerr, true);
    s.print_with_snippets();
    s.print_with_snippets(std::cerr);
    s.print_with_snippets(std::cerr, true);
    s.to_string();
    std::cerr<<s<<std::endl;
    auto f = s.frames[0];
    f.get_object_info();
    f.to_string();
    f.to_string(true);
    std::cerr<<f<<std::endl;
    s.clear();
    s.empty();

    cpptrace::generate_raw_trace();
    cpptrace::generate_raw_trace(0, 100);
    cpptrace::generate_object_trace();
    cpptrace::generate_object_trace(0, 100);
    cpptrace::generate_trace();
    cpptrace::generate_trace(0, 100);

    cpptrace::safe_generate_raw_trace(nullptr, 0, 0);
    cpptrace::safe_generate_raw_trace(nullptr, 0, 0, 0);

    cpptrace::safe_object_frame sf;
    cpptrace::get_safe_object_frame(0, &sf);
    cpptrace::can_signal_safe_unwind();
    cpptrace::can_get_safe_object_frame();

    cpptrace::register_jit_object(nullptr, 0);
    cpptrace::unregister_jit_object(nullptr);
    cpptrace::clear_all_jit_objects();

    cpptrace::lazy_exception l;
    l.what();
    l.message();
    l.trace();

    cpptrace::demangle("test");
    cpptrace::get_snippet(__FILE__, 1, 1);
    cpptrace::isatty(cpptrace::stderr_fileno);

    cpptrace::register_terminate_handler();

    cpptrace::absorb_trace_exceptions(true);

    cpptrace::enable_inlined_call_resolution(true);

    cpptrace::set_log_level(cpptrace::log_level::debug);
    cpptrace::set_log_callback([](cpptrace::log_level, const char*) {});
    cpptrace::use_default_stderr_logger();

    cpptrace::experimental::set_cache_mode(cpptrace::cache_mode::hybrid);

    cpptrace::experimental::set_dwarf_resolver_line_table_cache_size(100);
    cpptrace::experimental::set_dwarf_resolver_disable_aranges(true);

    cpptrace::detail::lazy_trace_holder lh;
    lh.is_resolved();

    cpptrace::basename("");
    cpptrace::prettify_symbol("");
    cpptrace::prune_symbol("");
}
