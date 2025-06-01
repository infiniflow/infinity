#include <cpptrace/formatting.hpp>
#include <cpptrace/utils.hpp>

#include "utils/optional.hpp"
#include "utils/utils.hpp"
#include "snippets/snippet.hpp"

#include <memory>
#include <cstdio>
#include <string>
#include <functional>
#include <iostream>
#include <sstream>

namespace cpptrace {
    class formatter::impl {
        struct {
            std::string header = "Stack trace (most recent call first):";
            color_mode color = color_mode::automatic;
            address_mode addresses = address_mode::raw;
            path_mode paths = path_mode::full;
            bool snippets = false;
            int context_lines = 2;
            bool columns = true;
            bool show_filtered_frames = true;
            std::function<bool(const stacktrace_frame&)> filter;
            std::function<stacktrace_frame(stacktrace_frame)> transform;
        } options;

    public:
        void header(std::string header) {
            options.header = std::move(header);
        }
        void colors(formatter::color_mode mode) {
            options.color = mode;
        }
        void addresses(formatter::address_mode mode) {
            options.addresses = mode;
        }
        void paths(path_mode mode) {
            options.paths = mode;
        }
        void snippets(bool snippets) {
            options.snippets = snippets;
        }
        void snippet_context(int lines) {
            options.context_lines = lines;
        }
        void columns(bool columns) {
            options.columns = columns;
        }
        void filtered_frame_placeholders(bool show) {
            options.show_filtered_frames = show;
        }
        void filter(std::function<bool(const stacktrace_frame&)> filter) {
            options.filter = filter;
        }
        void transform(std::function<stacktrace_frame(stacktrace_frame)> transform) {
            options.transform = std::move(transform);
        }

        std::string format(
            const stacktrace_frame& input_frame,
            detail::optional<bool> color_override = detail::nullopt
        ) const {
            std::ostringstream oss;
            detail::optional<stacktrace_frame> transformed_frame;
            if(options.transform) {
                transformed_frame = options.transform(input_frame);
            }
            const stacktrace_frame& frame = options.transform ? transformed_frame.unwrap() : input_frame;
            print_frame_inner(oss, frame, color_override.value_or(options.color == color_mode::always));
            return std::move(oss).str();
        }

        std::string format(const stacktrace& trace, detail::optional<bool> color_override = detail::nullopt) const {
            std::ostringstream oss;
            print_internal(oss, trace, false, color_override);
            return std::move(oss).str();
        }

        void print(const stacktrace_frame& frame, detail::optional<bool> color_override = detail::nullopt) const {
            print(std::cout, frame, color_override);
        }
        void print(
            std::ostream& stream,
            const stacktrace_frame& frame,
            detail::optional<bool> color_override = detail::nullopt
        ) const {
            print_frame_internal(stream, frame, color_override);
        }
        void print(
            std::FILE* file,
            const stacktrace_frame& frame,
            detail::optional<bool> color_override = detail::nullopt
        ) const {
            auto str = format(frame, color_override);
            std::fwrite(str.data(), 1, str.size(), file);
        }

        void print(const stacktrace& trace, detail::optional<bool> color_override = detail::nullopt) const {
            print(std::cout, trace, color_override);
        }
        void print(
            std::ostream& stream,
            const stacktrace& trace,
            detail::optional<bool> color_override = detail::nullopt
        ) const {
            print_internal(stream, trace, true, color_override);
        }
        void print(
            std::FILE* file,
            const stacktrace& trace,
            detail::optional<bool> color_override = detail::nullopt
        ) const {
            auto str = format(trace, color_override);
            std::fwrite(str.data(), 1, str.size(), file);
        }

    private:
        bool stream_is_tty(std::ostream& stream) const {
            // not great, but it'll have to do
            return (&stream == &std::cout && isatty(stdout_fileno))
                || (&stream == &std::cerr && isatty(stderr_fileno));
        }

        void maybe_ensure_virtual_terminal_processing(std::ostream& stream, bool color) const {
            if(color && stream_is_tty(stream)) {
                detail::enable_virtual_terminal_processing_if_needed();
            }
        }

        bool should_do_color(std::ostream& stream, detail::optional<bool> color_override) const {
            bool do_color = options.color == color_mode::always || color_override.value_or(false);
            if(
                (options.color == color_mode::automatic || options.color == color_mode::always) &&
                (!color_override || color_override.unwrap() != false) &&
                stream_is_tty(stream)
            ) {
                detail::enable_virtual_terminal_processing_if_needed();
                do_color = true;
            }
            return do_color;
        }

        void print_internal(std::ostream& stream, const stacktrace& trace, bool newline_at_end, detail::optional<bool> color_override) const {
            bool do_color = should_do_color(stream, color_override);
            maybe_ensure_virtual_terminal_processing(stream, do_color);
            print_internal(stream, trace, newline_at_end, do_color);
        }

        void print_internal(std::ostream& stream, const stacktrace& trace, bool newline_at_end, bool color) const {
            if(!options.header.empty()) {
                stream << options.header << '\n';
            }
            std::size_t counter = 0;
            const auto& frames = trace.frames;
            if(frames.empty()) {
                stream << "<empty trace>\n";
                return;
            }
            const auto frame_number_width = detail::n_digits(static_cast<int>(frames.size()) - 1);
            for(size_t i = 0; i < frames.size(); ++i) {
                detail::optional<stacktrace_frame> transformed_frame;
                if(options.transform) {
                    transformed_frame = options.transform(frames[i]);
                }
                const stacktrace_frame& frame = options.transform ? transformed_frame.unwrap() : frames[i];
                if(options.filter && !options.filter(frame)) {
                    if(!options.show_filtered_frames) {
                        counter++;
                        continue;
                    }
                    print_placeholder_frame(stream, frame_number_width, counter);
                } else {
                    print_frame_internal(stream, frame, color, frame_number_width, counter);
                    if(frame.line.has_value() && !frame.filename.empty() && options.snippets) {
                        auto snippet = detail::get_snippet(
                            frame.filename,
                            frame.line.value(),
                            options.context_lines,
                            color
                        );
                        if(!snippet.empty()) {
                            stream << '\n';
                            stream << snippet;
                        }
                    }
                }
                if(newline_at_end || i + 1 != frames.size()) {
                    stream << '\n';
                }
                counter++;
            }
        }

        void print_frame_internal(
            std::ostream& stream,
            const stacktrace_frame& frame,
            bool color,
            unsigned frame_number_width,
            std::size_t counter
        ) const {
            microfmt::print(stream, "#{<{}} ", frame_number_width, counter);
            print_frame_inner(stream, frame, color);
        }

        void print_placeholder_frame(std::ostream& stream, unsigned frame_number_width, std::size_t counter) const {
            microfmt::print(stream, "#{<{}} (filtered)", frame_number_width, counter);
        }

        void print_frame_internal(
            std::ostream& stream,
            const stacktrace_frame& frame,
            detail::optional<bool> color_override
        ) const {
            bool do_color = should_do_color(stream, color_override);
            maybe_ensure_virtual_terminal_processing(stream, do_color);
            print_frame_inner(stream, frame, do_color);
        }

        void print_frame_inner(std::ostream& stream, const stacktrace_frame& frame, bool color) const {
            const auto reset  = color ? RESET : "";
            const auto green  = color ? GREEN : "";
            const auto yellow = color ? YELLOW : "";
            const auto blue   = color ? BLUE : "";
            if(frame.is_inline) {
                microfmt::print(stream, "{<{}} ", 2 * sizeof(frame_ptr) + 2, "(inlined)");
            } else if(options.addresses != address_mode::none) {
                auto address = options.addresses == address_mode::raw ? frame.raw_address : frame.object_address;
                microfmt::print(stream, "{}0x{>{}:0h}{} ", blue, 2 * sizeof(frame_ptr), address, reset);
            }
            if(!frame.symbol.empty()) {
                microfmt::print(stream, "in {}{}{}", yellow, frame.symbol, reset);
            }
            if(!frame.filename.empty()) {
                microfmt::print(
                    stream,
                    "{}at {}{}{}",
                    frame.symbol.empty() ? "" : " ",
                    green,
                    options.paths == path_mode::full ? frame.filename : detail::basename(frame.filename, true),
                    reset
                );
                if(frame.line.has_value()) {
                    microfmt::print(stream, ":{}{}{}", blue, frame.line.value(), reset);
                    if(frame.column.has_value() && options.columns) {
                        microfmt::print(stream, ":{}{}{}", blue, frame.column.value(), reset);
                    }
                }
            }
        }
    };

    formatter::formatter() : pimpl(new impl) {}
    formatter::~formatter() {
        delete pimpl;
    }

    formatter::formatter(formatter&& other) : pimpl(detail::exchange(other.pimpl, nullptr)) {}
    formatter::formatter(const formatter& other) : pimpl(new impl(*other.pimpl)) {}
    formatter& formatter::operator=(formatter&& other) {
        if(pimpl) {
            delete pimpl;
        }
        pimpl = detail::exchange(other.pimpl, nullptr);
        return *this;
    }
    formatter& formatter::operator=(const formatter& other) {
        if(pimpl) {
            delete pimpl;
        }
        pimpl = new impl(*other.pimpl);
        return *this;
    }

    formatter& formatter::header(std::string header) {
        pimpl->header(std::move(header));
        return *this;
    }
    formatter& formatter::colors(color_mode mode) {
        pimpl->colors(mode);
        return *this;
    }
    formatter& formatter::addresses(address_mode mode) {
        pimpl->addresses(mode);
        return *this;
    }
    formatter& formatter::paths(path_mode mode) {
        pimpl->paths(mode);
        return *this;
    }
    formatter& formatter::snippets(bool snippets) {
        pimpl->snippets(snippets);
        return *this;
    }
    formatter& formatter::snippet_context(int lines) {
        pimpl->snippet_context(lines);
        return *this;
    }
    formatter& formatter::columns(bool columns) {
        pimpl->columns(columns);
        return *this;
    }
    formatter& formatter::filtered_frame_placeholders(bool show) {
        pimpl->filtered_frame_placeholders(show);
        return *this;
    }
    formatter& formatter::filter(std::function<bool(const stacktrace_frame&)> filter) {
        pimpl->filter(std::move(filter));
        return *this;
    }
    formatter& formatter::transform(std::function<stacktrace_frame(stacktrace_frame)> transform) {
        pimpl->transform(std::move(transform));
        return *this;
    }

    std::string formatter::format(const stacktrace_frame& frame) const {
        return pimpl->format(frame);
    }
    std::string formatter::format(const stacktrace_frame& frame, bool color) const {
        return pimpl->format(frame, color);
    }

    std::string formatter::format(const stacktrace& trace) const {
        return pimpl->format(trace);
    }
    std::string formatter::format(const stacktrace& trace, bool color) const {
        return pimpl->format(trace, color);
    }

    void formatter::print(const stacktrace& trace) const {
        pimpl->print(trace);
    }
    void formatter::print(const stacktrace& trace, bool color) const {
        pimpl->print(trace, color);
    }
    void formatter::print(std::ostream& stream, const stacktrace& trace) const {
        pimpl->print(stream, trace);
    }
    void formatter::print(std::ostream& stream, const stacktrace& trace, bool color) const {
        pimpl->print(stream, trace, color);
    }
    void formatter::print(std::FILE* file, const stacktrace& trace) const {
        pimpl->print(file, trace);
    }
    void formatter::print(std::FILE* file, const stacktrace& trace, bool color) const {
        pimpl->print(file, trace, color);
    }

    void formatter::print(const stacktrace_frame& frame) const {
        pimpl->print(frame);
    }
    void formatter::print(const stacktrace_frame& frame, bool color) const {
        pimpl->print(frame, color);
    }
    void formatter::print(std::ostream& stream, const stacktrace_frame& frame) const {
        pimpl->print(stream, frame);
    }
    void formatter::print(std::ostream& stream, const stacktrace_frame& frame, bool color) const {
        pimpl->print(stream, frame, color);
    }
    void formatter::print(std::FILE* file, const stacktrace_frame& frame) const {
        pimpl->print(file, frame);
    }
    void formatter::print(std::FILE* file, const stacktrace_frame& frame, bool color) const {
        pimpl->print(file, frame, color);
    }

    const formatter& get_default_formatter() {
        static formatter formatter;
        return formatter;
    }
}
