/*
    src/error.cpp: libnanobind functionality for exceptions

    Copyright (c) 2022 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#include <nanobind/nanobind.h>
#include <cstdarg>
#include "buffer.h"
#include "nb_internals.h"

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

Buffer buf(128);

NAMESPACE_END(detail)

#if PY_VERSION_HEX >= 0x030C0000
python_error::python_error() {
    m_value = PyErr_GetRaisedException();
    check(m_value,
          "nanobind::python_error::python_error(): error indicator unset!");
}

python_error::~python_error() {
    if (m_value) {
        gil_scoped_acquire acq;
        /* With GIL held */ {
            // Clear error status in case the following executes Python code
            error_scope scope;
            Py_DECREF(m_value);
        }
    }

    free(m_what);
}

python_error::python_error(const python_error &e)
    : std::exception(e), m_value(e.m_value) {
    if (m_value) {
        gil_scoped_acquire acq;
        Py_INCREF(m_value);
    }
    if (e.m_what)
        m_what = detail::strdup_check(e.m_what);
}

python_error::python_error(python_error &&e) noexcept
    : std::exception(e), m_value(e.m_value), m_what(e.m_what) {
    e.m_value = nullptr;
    e.m_what = nullptr;
}

void python_error::restore() noexcept {
    check(m_value,
          "nanobind::python_error::restore(): error was already restored!");

    PyErr_SetRaisedException(m_value);
    m_value = nullptr;
}

#else /* Exception handling for Python 3.11 and older versions */

python_error::python_error() {
    PyErr_Fetch(&m_type, &m_value, &m_traceback);
    check(m_type,
          "nanobind::python_error::python_error(): error indicator unset!");
}

python_error::~python_error() {
    if (m_type) {
        gil_scoped_acquire acq;
        /* With GIL held */ {
            // Clear error status in case the following executes Python code
            error_scope scope;
            Py_XDECREF(m_type);
            Py_XDECREF(m_value);
            Py_XDECREF(m_traceback);
        }
    }
    free(m_what);
}

python_error::python_error(const python_error &e)
    : std::exception(e), m_type(e.m_type), m_value(e.m_value),
      m_traceback(e.m_traceback) {
    if (m_type) {
        gil_scoped_acquire acq;
        Py_INCREF(m_type);
        Py_XINCREF(m_value);
        Py_XINCREF(m_traceback);
    }
    if (e.m_what)
        m_what = detail::strdup_check(e.m_what);
}

python_error::python_error(python_error &&e) noexcept
    : std::exception(e), m_type(e.m_type), m_value(e.m_value),
      m_traceback(e.m_traceback), m_what(e.m_what) {
    e.m_type = e.m_value = e.m_traceback = nullptr;
    e.m_what = nullptr;
}

void python_error::restore() noexcept {
    check(m_type,
          "nanobind::python_error::restore(): error was already restored!");

    PyErr_Restore(m_type, m_value, m_traceback);
    m_type = m_value = m_traceback = nullptr;
}

#endif

const char *python_error::what() const noexcept {
    using detail::buf;

    // Return the existing error message if already computed once
    if (m_what)
        return m_what;

    gil_scoped_acquire acq;

    // Try again with GIL held
    if (m_what)
        return m_what;

#if PY_VERSION_HEX < 0x030C0000
    PyErr_NormalizeException(&m_type, &m_value, &m_traceback);
    check(m_type,
          "nanobind::python_error::what(): PyErr_NormalizeException() failed!");

    if (m_traceback) {
        if (PyException_SetTraceback(m_value, m_traceback) < 0)
            PyErr_Clear();
    }

    handle exc_type = m_type, exc_value = m_value;
#else
    handle exc_value = m_value, exc_type = exc_value.type();
#endif
    object exc_traceback = traceback();

#if defined(Py_LIMITED_API) || defined(PYPY_VERSION)
    object mod = module_::import_("traceback"),
           result = mod.attr("format_exception")(exc_type, exc_value, exc_traceback);
    m_what = detail::strdup_check(borrow<str>(str("\n").attr("join")(result)).c_str());
#else
    buf.clear();
    if (exc_traceback.is_valid()) {
        PyTracebackObject *to = (PyTracebackObject *) exc_traceback.ptr();

        // Get the deepest trace possible
        while (to->tb_next)
            to = to->tb_next;

        PyFrameObject *frame = to->tb_frame;
        Py_XINCREF(frame);

        std::vector<PyFrameObject *, detail::py_allocator<PyFrameObject *>> frames;

        while (frame) {
            frames.push_back(frame);
#if PY_VERSION_HEX >= 0x03090000
            frame = PyFrame_GetBack(frame);
#else
            frame = frame->f_back;
            Py_XINCREF(frame);
#endif
        }

        buf.put("Traceback (most recent call last):\n");
        for (auto it = frames.rbegin(); it != frames.rend(); ++it) {
            frame = *it;
#if PY_VERSION_HEX >= 0x03090000
            PyCodeObject *f_code = PyFrame_GetCode(frame);
#else
            PyCodeObject *f_code = frame->f_code;
#endif
            buf.put("  File \"");
            buf.put_dstr(borrow<str>(f_code->co_filename).c_str());
            buf.put("\", line ");
            buf.put_uint32(PyFrame_GetLineNumber(frame));
            buf.put(", in ");
            buf.put_dstr(borrow<str>(f_code->co_name).c_str());
            buf.put('\n');
#if PY_VERSION_HEX >= 0x03090000
            Py_DECREF(f_code);
#endif
            Py_DECREF(frame);
        }
    }

    if (exc_type.is_valid()) {
        object name = exc_type.attr("__name__");
        buf.put_dstr(borrow<str>(name).c_str());
        buf.put(": ");
    }

    if (exc_value.is_valid())
        buf.put_dstr(str(m_value).c_str());
    m_what = buf.copy();
#endif

    return m_what;
}

builtin_exception::builtin_exception(exception_type type, const char *what)
    : std::runtime_error(what ? what : ""), m_type(type) { }
builtin_exception::~builtin_exception() { }

NAMESPACE_BEGIN(detail)

void register_exception_translator(exception_translator t, void *payload) {
    nb_translator_seq *cur  = &internals->translators,
                      *next = new nb_translator_seq(*cur);
    cur->next = next;
    cur->payload = payload;
    cur->translator = t;
}

NB_CORE PyObject *exception_new(PyObject *scope, const char *name,
                                PyObject *base) {
    object modname;
    if (PyModule_Check(scope))
        modname = getattr(scope, "__name__", handle());
    else
        modname = getattr(scope, "__module__", handle());

    if (!modname.is_valid())
        raise("nanobind::detail::exception_new(): could not determine module "
              "name!");

    str combined =
        steal<str>(PyUnicode_FromFormat("%U.%s", modname.ptr(), name));

    object result = steal(PyErr_NewException(combined.c_str(), base, nullptr));
    check(result, "nanobind::detail::exception_new(): creation failed!");

    if (hasattr(scope, name))
        raise("nanobind::detail::exception_new(): an object of the same name "
              "already exists!");

    setattr(scope, name, result);
    return result.release().ptr();
}

NAMESPACE_END(detail)

static void chain_error_v(handle type, const char *fmt, va_list args) noexcept {
#if PY_VERSION_HEX >= 0x030C0000
    PyObject *value = PyErr_GetRaisedException();
#else
    PyObject *tp = nullptr, *value = nullptr, *traceback = nullptr;

    PyErr_Fetch(&tp, &value, &traceback);

    if (tp) {
        PyErr_NormalizeException(&tp, &value, &traceback);
        if (traceback) {
            PyException_SetTraceback(value, traceback);
            Py_DECREF(traceback);
        }

        Py_DECREF(tp);
        tp = traceback = nullptr;
    }
#endif

#if !defined(PYPY_VERSION)
    PyErr_FormatV(type.ptr(), fmt, args);
#else
    PyObject *exc_str = PyUnicode_FromFormatV(fmt, args);
    check(exc_str, "nanobind::detail::raise_from(): PyUnicode_FromFormatV() failed!");
    PyErr_SetObject(type.ptr(), exc_str);
    Py_DECREF(exc_str);
#endif

    if (!value)
        return;

    PyObject *value_2 = nullptr;
#if PY_VERSION_HEX >= 0x030C0000
    value_2 = PyErr_GetRaisedException();
#else
    PyErr_Fetch(&tp, &value_2, &traceback);
    PyErr_NormalizeException(&tp, &value_2, &traceback);
#endif

    Py_INCREF(value);
    PyException_SetCause(value_2, value); // steals
    PyException_SetContext(value_2, value); // steals

#if PY_VERSION_HEX >= 0x030C0000
    PyErr_SetRaisedException(value_2);
#else
    PyErr_Restore(tp, value_2, traceback);
#endif
}

void chain_error(handle type, const char *fmt, ...) noexcept {
    va_list args;
    va_start(args, fmt);
    chain_error_v(type, fmt, args);
    va_end(args);
}

void raise_from(python_error &e, handle type, const char *fmt, ...) {
    e.restore();

    va_list args;
    va_start(args, fmt);
    chain_error_v(type, fmt, args);
    va_end(args);

    detail::raise_python_error();
}

NAMESPACE_END(NB_NAMESPACE)
