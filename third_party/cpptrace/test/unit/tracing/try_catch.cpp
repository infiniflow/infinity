#include <algorithm>
#include <exception>
#include <stdexcept>
#include <string_view>
#include <string>

#include <gtest/gtest.h>
#include <gtest/gtest-matchers.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>

#include "common.hpp"

#ifdef TEST_MODULE
import cpptrace;

#include <cpptrace/exceptions_macros.hpp>
#else
#include <cpptrace/cpptrace.hpp>
#include <cpptrace/from_current.hpp>
#include "cpptrace/basic.hpp"
#endif

using namespace std::literals;

namespace {
    template<typename E, typename... Args>
    CPPTRACE_FORCE_NO_INLINE
    void do_throw(Args&&... args) {
        throw E(std::forward<Args>(args)...);
    }

    void check_trace(const cpptrace::stacktrace& trace, std::string_view file, int line) {
        (void)trace;
        (void)file;
        (void)line;
        #ifndef CPPTRACE_BUILD_NO_SYMBOLS
        for(const auto& frame : trace) {
            if(frame.filename.find(file) != std::string::npos && frame.line == line) {
                SUCCEED();
                return;
            }
        }
        FAIL() << "Trace does not contain "<<file<<":"<<line<<"\n"<<trace.to_string();
        #endif
    }

    void check_trace(const cpptrace::stacktrace& trace, std::string_view try_name) {
        EXPECT_NE(
            std::find_if(
                trace.begin(),
                trace.end(),
                [&] (const cpptrace::stacktrace_frame& frame) {
                    return frame.symbol.find(try_name) != std::string::npos;
                }
            ),
            trace.end()
        ) << trace;
        EXPECT_NE(
            std::find_if(
                trace.begin(),
                trace.end(),
                [&] (const cpptrace::stacktrace_frame& frame) {
                    return frame.symbol.find("do_throw") != std::string::npos;
                }
            ),
            trace.end()
        ) << trace;
    }
}

TEST(TryCatch, Basic) {
    constexpr std::string_view test_name = __func__;
    int line = 0;
    bool did_catch = false;
    cpptrace::try_catch(
        [&] {
            line = __LINE__ + 1;
            do_throw<std::runtime_error>("foobar");
        },
        [&] (const std::runtime_error& e) {
            did_catch = true;
            EXPECT_EQ(e.what(), "foobar"sv);
            check_trace(cpptrace::from_current_exception(), "try_catch.cpp", line);
            check_trace(cpptrace::from_current_exception(), test_name);
        }
    );
    EXPECT_TRUE(did_catch);
}

TEST(TryCatch, NoException) {
    bool did_try = false;
    cpptrace::try_catch(
        [&] {
            did_try = true;
        },
        [&] (const std::runtime_error&) {
            FAIL();
        }
    );
    EXPECT_TRUE(did_try);
}

TEST(TryCatch, Upcast) {
    constexpr std::string_view test_name = __func__;
    int line = 0;
    bool did_catch = false;
    cpptrace::try_catch(
        [&] {
            line = __LINE__ + 1;
            do_throw<std::runtime_error>("foobar");
        },
        [&] (const std::exception& e) {
            did_catch = true;
            EXPECT_EQ(e.what(), "foobar"sv);
            check_trace(cpptrace::from_current_exception(), "try_catch.cpp", line);
            check_trace(cpptrace::from_current_exception(), test_name);
        }
    );
    EXPECT_TRUE(did_catch);
}

TEST(TryCatch, NoHandler) {
    int line = 0;
    bool did_catch = false;
    try {
        cpptrace::try_catch(
            [&] {
                line = __LINE__ + 1;
                do_throw<std::exception>();
            },
            [&] (const std::runtime_error&) {
                FAIL();
            }
        );
        FAIL();
    } catch(...) {
        did_catch = true;
    }
    EXPECT_TRUE(did_catch);
}

TEST(TryCatch, NoMatchingHandler) {
    int line = 0;
    bool did_catch = false;
    try {
        cpptrace::try_catch(
            [&] {
                line = __LINE__ + 1;
                do_throw<std::exception>();
            }
        );
        FAIL();
    } catch(...) {
        did_catch = true;
    }
    EXPECT_TRUE(did_catch);
}

TEST(TryCatch, CorrectHandler) {
    constexpr std::string_view test_name = __func__;
    int line = 0;
    bool did_catch = false;
    cpptrace::try_catch(
        [&] {
            line = __LINE__ + 1;
            do_throw<std::runtime_error>("foobar");
        },
        [&] (int) {
            FAIL();
        },
        [&] (const std::logic_error&) {
            FAIL();
        },
        [&] (const std::runtime_error& e) {
            did_catch = true;
            EXPECT_EQ(e.what(), "foobar"sv);
            check_trace(cpptrace::from_current_exception(), "try_catch.cpp", line);
            check_trace(cpptrace::from_current_exception(), test_name);
        },
        [&] (const std::exception&) {
            FAIL();
        }
    );
    EXPECT_TRUE(did_catch);
}

TEST(TryCatch, BlanketHandler) {
    constexpr std::string_view test_name = __func__;
    int line = 0;
    bool did_catch = false;
    cpptrace::try_catch(
        [&] {
            line = __LINE__ + 1;
            do_throw<std::exception>();
        },
        [&] (int) {
            FAIL();
        },
        [&] (const std::logic_error&) {
            FAIL();
        },
        [&] (const std::runtime_error&) {
            FAIL();
        },
        [&] () {
            did_catch = true;
            check_trace(cpptrace::from_current_exception(), "try_catch.cpp", line);
            check_trace(cpptrace::from_current_exception(), test_name);
        }
    );
    EXPECT_TRUE(did_catch);
}

TEST(TryCatch, CatchOrdering) {
    constexpr std::string_view test_name = __func__;
    int line = 0;
    bool did_catch = false;
    cpptrace::try_catch(
        [&] {
            line = __LINE__ + 1;
            do_throw<std::runtime_error>("foobar");
        },
        [&] (int) {
            FAIL();
        },
        [&] (const std::logic_error&) {
            FAIL();
        },
        [&] () {
            did_catch = true;
            check_trace(cpptrace::from_current_exception(), "try_catch.cpp", line);
            check_trace(cpptrace::from_current_exception(), test_name);
        },
        [&] (const std::runtime_error&) {
            FAIL();
        }
    );
    EXPECT_TRUE(did_catch);
}

namespace {
    struct copy_move_tracker {
        inline static int copy = 0;
        inline static int move = 0;
        copy_move_tracker() = default;
        copy_move_tracker(const copy_move_tracker&) {
            copy++;
        }
        copy_move_tracker(copy_move_tracker&&) {
            move++;
        }
        copy_move_tracker& operator=(const copy_move_tracker&) {
            copy++;
            return *this;
        }
        copy_move_tracker& operator=(copy_move_tracker&&) {
            move++;
            return *this;
        }
        static void reset() {
            copy = 0;
            move = 0;
        }
    };
}

TEST(TryCatch, Value) {
    bool did_catch = false;
    copy_move_tracker::reset();
    cpptrace::try_catch(
        [&] {
            do_throw<copy_move_tracker>();
        },
        [&] (copy_move_tracker) {
            did_catch = true;
        }
    );
    EXPECT_TRUE(did_catch);
    EXPECT_EQ(copy_move_tracker::copy, 1); // from the catch
    EXPECT_EQ(copy_move_tracker::move, 1); // from the forward to the catcher
}

TEST(TryCatch, Ref) {
    bool did_catch = false;
    copy_move_tracker::reset();
    cpptrace::try_catch(
        [&] {
            do_throw<copy_move_tracker>();
        },
        [&] (copy_move_tracker&) {
            did_catch = true;
        }
    );
    EXPECT_TRUE(did_catch);
    EXPECT_EQ(copy_move_tracker::copy, 0);
    EXPECT_EQ(copy_move_tracker::move, 0);
}

TEST(TryCatch, ConstRef) {
    bool did_catch = false;
    copy_move_tracker::reset();
    cpptrace::try_catch(
        [&] {
            do_throw<copy_move_tracker>();
        },
        [&] (const copy_move_tracker&) {
            did_catch = true;
        }
    );
    EXPECT_TRUE(did_catch);
    EXPECT_EQ(copy_move_tracker::copy, 0);
    EXPECT_EQ(copy_move_tracker::move, 0);
}

namespace {
    struct copy_move_tracker_callable {
        inline static int copy = 0;
        inline static int move = 0;
        copy_move_tracker_callable() = default;
        copy_move_tracker_callable(const copy_move_tracker&) {
            copy++;
        }
        copy_move_tracker_callable(copy_move_tracker&&) {
            move++;
        }
        copy_move_tracker_callable& operator=(const copy_move_tracker&) {
            copy++;
            return *this;
        }
        copy_move_tracker_callable& operator=(copy_move_tracker&&) {
            move++;
            return *this;
        }
        static void reset() {
            copy = 0;
            move = 0;
        }
        void operator()() const {}
    };
}

TEST(TryCatch, LvalueCallable) {
    copy_move_tracker_callable::reset();
    copy_move_tracker_callable c;
    cpptrace::try_catch(
        c,
        c
    );
    EXPECT_EQ(copy_move_tracker_callable::copy, 0);
    EXPECT_EQ(copy_move_tracker_callable::move, 0);
}

TEST(TryCatch, RvalueCallable) {
    copy_move_tracker_callable::reset();
    cpptrace::try_catch(
        copy_move_tracker_callable{},
        copy_move_tracker_callable{}
    );
    EXPECT_EQ(copy_move_tracker_callable::copy, 0);
    EXPECT_EQ(copy_move_tracker_callable::move, 0);
}

TEST(TryCatch, ClvalueCallable) {
    copy_move_tracker_callable::reset();
    const copy_move_tracker_callable c;
    cpptrace::try_catch(
        c,
        c
    );
    EXPECT_EQ(copy_move_tracker_callable::copy, 0);
    EXPECT_EQ(copy_move_tracker_callable::move, 0);
}

TEST(TryCatch, CrvalueCallable) {
    copy_move_tracker_callable::reset();
    cpptrace::try_catch(
        static_cast<const copy_move_tracker_callable&&>(copy_move_tracker_callable{}),
        static_cast<const copy_move_tracker_callable&&>(copy_move_tracker_callable{})
    );
    EXPECT_EQ(copy_move_tracker_callable::copy, 0);
    EXPECT_EQ(copy_move_tracker_callable::move, 0);
}
