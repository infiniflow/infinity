.. cpp:namespace:: nanobind

.. _utilities:

Utilities
==========

.. _utilities_eval:

Evaluating Python expressions from strings
------------------------------------------

nanobind provides the :cpp:func:`eval` and :cpp:func:`exec` functions to
evaluate Python expressions and statements. The following example illustrates
how they can be used.

.. code-block:: cpp

    // At beginning of file
    #include <nanobind/eval.h>

    ...

    // Evaluate in scope of main module
    nb::object scope = nb::module_::import_("__main__").attr("__dict__");

    // Evaluate an isolated expression
    int result = nb::eval("my_variable + 10", scope).cast<int>();

    // Evaluate a sequence of statements
    nb::exec(
        "print('Hello')\n"
        "print('world!');",
        scope);

C++11 raw string literals are also supported and quite handy for this purpose.
The only requirement is that the first statement must be on a new line
following the raw string delimiter ``R"(``, ensuring all lines have common
leading indent:

.. code-block:: cpp

    nb::exec(R"(
        x = get_answer()
        if x == 42:
            print('Hello World!')
        else:
            print('Bye!')
        )", scope
    );

.. note::

    :cpp:func:`eval` accepts a template parameter that describes how the
    string/file should be interpreted. Possible choices include ``eval_expr``
    (isolated expression), ``eval_single_statement`` (a single statement,
    return value is always ``none``), and ``eval_statements`` (sequence of
    statements, return value is always ``none``). `eval` defaults to
    ``eval_expr`` and `exec` is just a shortcut for ``eval<eval_statements>``.
