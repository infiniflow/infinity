/*******************************************************************************
 * tlx/die/core.cpp
 *
 * Part of tlx - http://panthema.net/tlx
 *
 * Copyright (C) 2016-2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the Boost Software License, Version 1.0
 ******************************************************************************/

#include "core.hpp"

#include <atomic>
#include <iostream>
#include <sstream>

namespace tlx {

/******************************************************************************/

static std::atomic<bool> s_die_with_exception {
#if TLX_DIE_WITH_EXCEPTION
    true
#else
    false
#endif
};

void die_with_message(const std::string& msg) {
    if (s_die_with_exception) {
        throw DieException(msg);
    }
    else {
        std::cerr << msg << std::endl;
        std::terminate();
    }
}

void die_with_message(const char* msg, const char* file, size_t line) {
    std::ostringstream oss;
    oss << msg << " @ " << file << ':' << line;
    die_with_message(oss.str());
}

void die_with_message(const std::string& msg, const char* file, size_t line) {
    return die_with_message(msg.c_str(), file, line);
}

DieException::DieException(const std::string& message)
    : std::runtime_error(message) { }

bool set_die_with_exception(bool b) {
    return s_die_with_exception.exchange(b);
}

/******************************************************************************/
/** \page tlx_die die() - Simple Invariant Testing

tlx contains a set of macros called `die_...` for simple invariant testing. They
test some condition and fail with nice output containing both the condition, and
file/line information where it occurred.

- `die(message)` - always terminates with given message.
- `die_unless(condition)` - terminates if condition is false
- `die_if(condition)` - terminates if condition is true
- `die_verbose_unless(condition,message)` - terminates if condition is false
- `die_verbose_if(condition,message)` - terminates if condition is true

- `die_unequal(a,b)` - terminates unless a == b.
- `die_unequal_eps6(a,b)` - terminates unless abs(a - b) < 1e-6 for approximate equality.
- `die_equal(a,b)` - terminates if a == b.
- `die_unless_throws(code,exception)` - terminate if code does not throw the exception

Furthermore, some additional assert macros are also available. These are only
active in Debug mode, if NDEBUG is defined they are compiled out.

- `assert_equal(a,b)` - checks if a == b.
- `assert_unequal(a,b)` - checks if a != b.

tlx die macros can also be modified to throw a DieException instead of calling
std::terminate. Either call `set_die_with_exception(true)` to define
TLX_DIE_WITH_EXCEPTION=1 using the preprocessor.

 */

} // namespace tlx

/******************************************************************************/
