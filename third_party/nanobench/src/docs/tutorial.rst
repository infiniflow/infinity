Installation
============

Direct Inclusion
----------------

#. Download ``nanobench.h`` from the :download:`release <https://github.com/martinus/nanobench/releases/latest>`
   and make it available in your project. 

#. Create a .cpp file, e.g. ``nanobench.cpp``, where the bulk of nanobench is compiled.

   .. literalinclude:: ../test/app/nanobench.cpp
      :language: c++
      :linenos:
      :caption: nanobench.cpp

#. Compile e.g. with ``g++ -O3 -I../include -c nanobench.cpp``. This compiles
   the bulk of nanobench, and took 2.4 seconds on my machine. It needs to be compiled only once whenever you upgrade nanobench.


CMake Integration
-----------------

``nanobench`` can be integrated with CMake's `FetchContent <https://cmake.org/cmake/help/latest/module/FetchContent.html>`_ or as
a `git submodule <https://git-scm.com/book/en/v2/Git-Tools-Submodules>`_. Here is a full example how to this can be done:

.. literalinclude:: code/CMakeLists.txt
   :language: CMake
   :linenos:
   :caption: CMakeLists.txt

Usage
=====


#. Create the actual benchmark code, in ``full_example.cpp``:

   .. literalinclude:: code/full_example.cpp
      :language: c++
      :linenos:
      :caption: full_example.cpp

   The most important entry entry point is :cpp:class:`ankerl::nanobench::Bench`. It creates a benchmarking object,
   optionally configures it, and then runs the code to benchmark with :cpp:func:`run() <ankerl::nanobench::Bench::run()>`.

#. Compile and link the example with 

   .. code:: sh
   
      g++ -O3 -I../include nanobench.o full_example.cpp -o full_example
      
   This takes just 0.28 seconds on my machine.

#. Run ``./full_example``, which gives an output like this:

   .. code:: text

      |               ns/op |                op/s |    err% |          ins/op |          cyc/op |    IPC |         bra/op |   miss% |     total | benchmark
      |--------------------:|--------------------:|--------:|----------------:|----------------:|-------:|---------------:|--------:|----------:|:----------
      |                5.63 |      177,595,338.98 |    0.0% |            3.00 |           17.98 |  0.167 |           1.00 |    0.1% |      0.00 | `compare_exchange_strong`

   Which renders as

   ==================== ===================== ========= ================= ================= ======== ================ ========= =========== ==================
                  ns/op                  op/s      err%            ins/op            cyc/op      IPC           bra/op     miss%       total   benchmark
   ==================== ===================== ========= ================= ================= ======== ================ ========= =========== ==================
                   5.63        177,595,338.98      0.0%              3.00             17.98    0.167             1.00      0.1%        0.00   ``compare_exchange_strong``
   ==================== ===================== ========= ================= ================= ======== ================ ========= =========== ==================

   Which means that one ``x.compare_exchange_strong(y, 0);`` call takes
   5.63ns on my machine (wall-clock time), or ~178 million operations per second. Runtime
   fluctuates by around 0.0%, so the results are very stable. Each call
   required 3 instructions, which took ~18 CPU cycles. There was a single branch per call,
   with only 0.1% mispredicted. 

Nanobench does not come with a test runner, so you can easily use it with any framework you like.  In the remaining examples, I'm
using `doctest <https://github.com/onqtam/doctest>`_ as a unit test framework.

.. note::

   CPU statistics like instructions, cycles, branches, branch misses are only available on Linux, through
   `perf events <http://web.eece.maine.edu/~vweaver/projects/perf_events/>`_. On some systems you might need to 
   `change permissions <https://www.kernel.org/doc/html/latest/admin-guide/perf-security.html#unprivileged-users>`_
   through ``perf_event_paranoid`` or use ACL.


Examples
========


Something Fast
--------------

Let’s benchmarks how fast we can do ``x += x`` for ``uint64_t``:

.. literalinclude:: ../test/tutorial_fast_v1.cpp
   :language: c++
   :linenos:
   :caption: tutorial_fast_v1.cpp

After 0.2ms we get this output:

.. code-block:: text

   |               ns/op |                op/s |    err% |     total | benchmark
   |--------------------:|--------------------:|--------:|----------:|:----------
   |                   - |                   - |       - |         - | :boom: `++x` (iterations overflow. Maybe your code got optimized away?)

No data there! We only get ``:boom: iterations overflow.``.  The compiler could optimize ``x += x``
away because we never used the output. Thanks to ``doNotOptimizeAway``, this is easy to fix:

.. literalinclude:: ../test/tutorial_fast_v2.cpp
   :language: c++
   :linenos:
   :caption: tutorial_fast_v2.cpp
   :emphasize-lines: 7

This time the benchmark runs for 2.2ms and we actually get reasonable data:

.. code-block:: text

   |               ns/op |                op/s |    err% |          ins/op |          cyc/op |    IPC |         bra/op |   miss% |     total | benchmark
   |--------------------:|--------------------:|--------:|----------------:|----------------:|-------:|---------------:|--------:|----------:|:----------
   |                0.31 |    3,192,444,232.50 |    0.0% |            1.00 |            1.00 |  0.998 |           0.00 |    0.0% |      0.00 | `++x`

It's a very stable result. One run the op/s is 3,192 million/sec, the next time I execute it I get 3,168 million/sec. It always takes 
1.00 instructions per operation on my machine, and can do this in ~1 cycle.


Something Slow
--------------

Let's benchmark if sleeping for 100ms really takes 100ms.

.. literalinclude:: ../test/tutorial_slow_v1.cpp
   :language: c++
   :linenos:
   :caption: tutorial_slow_v1.cpp

After 1.1 seconds I get

.. code-block:: text

   |               ns/op |                op/s |    err% |          ins/op |          cyc/op |    IPC |         bra/op |   miss% |     total | benchmark
   |--------------------:|--------------------:|--------:|----------------:|----------------:|-------:|---------------:|--------:|----------:|:---------------------
   |      100,125,753.00 |                9.99 |    0.0% |           51.00 |        7,714.00 |  0.007 |          11.00 |   90.9% |      1.10 | `sleep 100ms, auto`


So we actually take 100.125ms instead of 100ms. Next time I run it, I get 100.141. Also a very stable result. Interestingly, sleep takes 51 instructions but 7,714 cycles - so we only got 0.007 instructions per cycle. That's extremely low, but expected of ``sleep``. It also required 11 branches, of which 90.9% were mispredicted on average.

If the extremely slow 1.1 second is too much for you, you can manually configure the number of evaluations (epochs):

.. literalinclude:: ../test/tutorial_slow_v2.cpp
   :language: c++
   :linenos:
   :caption: tutorial_slow_v2.cpp

.. code-block:: text

   |               ns/op |                op/s |    err% |          ins/op |          cyc/op |    IPC |         bra/op |   miss% |     total | benchmark
   |--------------------:|--------------------:|--------:|----------------:|----------------:|-------:|---------------:|--------:|----------:|:----------
   |      100,099,096.00 |                9.99 |    0.0% |           51.00 |        7,182.00 |  0.007 |          11.00 |   90.9% |      0.30 | `sleep 100ms`

This time it took only 0.3 seconds, but with only 3 evaluations instead of 11. The err% will be less meaningful, but since the benchmark is so stable it doesn't really matter.


Something Unstable
------------------

Let's create an extreme artificial test that's hard to benchmark, because runtime fluctuates randomly: Each iteration
randomly skip between 0-254 random numbers:

.. literalinclude:: ../test/tutorial_fluctuating_v1.cpp
   :language: c++
   :linenos:
   :caption: tutorial_fluctuating_v1.cpp

After 2.3ms, I get this result:

.. code-block:: text

   |               ns/op |                op/s |    err% |          ins/op |          cyc/op |    IPC |         bra/op |   miss% |     total | benchmark
   |--------------------:|--------------------:|--------:|----------------:|----------------:|-------:|---------------:|--------:|----------:|:----------
   |              334.12 |        2,992,911.53 |    6.3% |        3,486.44 |        1,068.67 |  3.262 |         287.86 |    0.7% |      0.00 | :wavy_dash: `random fluctuations` (Unstable with ~56.7 iters. Increase `minEpochIterations` to e.g. 567)

So on average each loop takes about 334.12ns, but we get a warning that the results are unstable. The median percentage error is 6.3% which is quite high, 

Let's use the suggestion and set the minimum number of iterations to 5000, and try again:

.. literalinclude:: ../test/tutorial_fluctuating_v2.cpp
   :language: c++
   :linenos:
   :caption: tutorial_fluctuating_v2.cpp
   :emphasize-lines: 9


The fluctuations are much better:

.. code-block:: text

   |               ns/op |                op/s |    err% |          ins/op |          cyc/op |    IPC |         bra/op |   miss% |     total | benchmark
   |--------------------:|--------------------:|--------:|----------------:|----------------:|-------:|---------------:|--------:|----------:|:----------
   |              277.31 |        3,606,106.48 |    0.7% |        3,531.75 |          885.18 |  3.990 |         291.59 |    0.7% |      0.00 | `random fluctuations`


The results are  more stable, with only 0.7% error.

.. _Tutorial Comparing Results:


Comparing Results
=================
To compare results, keep the `ankerl::nanobench::Bench` object around, enable `.relative(true)`, and `.run(...)` your benchmarks. All benchmarks will be automatically compared to the first one.


As an example, I have implemented a comparison of multiple random number generators.
Here several RNGs are compared to a baseline calculated from `std::default_random_engine`.
I factored out the general benchmarking code so it's easy to use for each of the random number generators:


.. literalinclude:: ../test/example_random_number_generators.cpp
   :language: c++
   :linenos:
   :caption: example_random_number_generators.cpp (excerpt)
   :lines: 309-



Runs for 60ms and prints this table:

.. code-block:: text

   | relative |         ns/uint64_t |          uint64_t/s |    err% |    ins/uint64_t |    cyc/uint64_t |    IPC |   bra/uint64_t |   miss% |     total | Random Number Generators
   |---------:|--------------------:|--------------------:|--------:|----------------:|----------------:|-------:|---------------:|--------:|----------:|:-------------------------
   |   100.0% |               35.87 |       27,881,924.28 |    2.3% |          127.80 |          114.61 |  1.115 |           9.77 |    3.7% |      0.00 | `std::default_random_engine`
   |   490.3% |                7.32 |      136,699,693.21 |    0.6% |           89.55 |           23.49 |  3.812 |           9.51 |    0.1% |      0.00 | `std::mt19937`
   | 1,767.4% |                2.03 |      492,786,582.33 |    0.6% |           24.38 |            6.48 |  3.761 |           1.26 |    0.6% |      0.00 | `std::mt19937_64`
   |    85.2% |               42.08 |       23,764,853.03 |    0.7% |          157.07 |          134.62 |  1.167 |          19.51 |    7.6% |      0.00 | `std::ranlux24_base`
   |   121.3% |               29.56 |       33,824,759.51 |    0.5% |           91.03 |           94.35 |  0.965 |          10.00 |    8.1% |      0.00 | `std::ranlux48_base`
   |    17.4% |              205.67 |        4,862,080.59 |    1.2% |          709.83 |          657.10 |  1.080 |         101.79 |   16.1% |      0.00 | `std::ranlux24_base`
   |     8.7% |              412.46 |        2,424,497.97 |    1.8% |        1,514.70 |        1,318.43 |  1.149 |         219.09 |   16.7% |      0.00 | `std::ranlux48`
   |    59.2% |               60.60 |       16,502,276.18 |    1.9% |          253.77 |          193.39 |  1.312 |          24.93 |    1.5% |      0.00 | `std::knuth_b`
   | 5,187.1% |                0.69 |    1,446,254,071.66 |    0.1% |            6.00 |            2.21 |  2.714 |           0.00 |    0.0% |      0.00 | `WyRng`
   | 1,431.7% |                2.51 |      399,177,833.54 |    0.0% |           21.00 |            8.01 |  2.621 |           0.00 |    0.0% |      0.00 | `NasamRng`
   | 2,629.9% |                1.36 |      733,279,957.30 |    0.1% |           13.00 |            4.36 |  2.982 |           0.00 |    0.0% |      0.00 | `Sfc4`
   | 3,815.7% |                0.94 |    1,063,889,655.17 |    0.0% |           11.00 |            3.01 |  3.661 |           0.00 |    0.0% |      0.00 | `RomuTrio`
   | 3,529.5% |                1.02 |      984,102,081.37 |    0.3% |            9.00 |            3.25 |  2.768 |           0.00 |    0.0% |      0.00 | `RomuDuo`
   | 4,580.4% |                0.78 |    1,277,113,402.06 |    0.0% |            7.00 |            2.50 |  2.797 |           0.00 |    0.0% |      0.00 | `RomuDuoJr`
   | 2,291.2% |                1.57 |      638,820,992.09 |    0.0% |           11.00 |            5.00 |  2.200 |           0.00 |    0.0% |      0.00 | `ankerl::nanobench::Rng`

It shows that :cpp:class:`ankerl::nanobench::Rng` is one of the fastest RNG, and has the least amount of
fluctuation. It takes only 1.57ns to generate a random ``uint64_t``, so ~638 million calls per
seconds are possible. To the left we show relative performance compared to ``std::default_random_engine``. 

.. note::

   Here pure runtime performance is not necessarily the best benchmark.
   Especially the fastest RNG's can be inlined and use instruction level parallelism
   to their advantage: they immediately return an old state, and while user code can
   already use that value, the next value is calculated in parallel. See the excellent paper
   at `romu-random <http://www.romu-random.org>`_ for details.


.. _asymptotic-complexity:

Asymptotic Complexity
=====================

It is possible to calculate asymptotic complexity (Big O) from multiple runs of a benchmark. Run the
benchmark with different complexity N, then nanobench can calculate the best fitting curve. 

The following example finds out the asymptotic complexity of ``std::set``'s ``find()``.

.. literalinclude:: ../test/tutorial_complexity_set.cpp
   :language: c++
   :linenos:
   :caption: tutorial_complexity_set.cpp

The loop runs the benchmark 10 times, with different set sizes from 10 to 10k.

.. note::
 
   Each of the 10 benchmark runs automatically scales the number of iterations so results are still
   fast and accurate. In total the whole test takes about 90ms.

The :cpp:class:`Bench <ankerl::nanobench::Bench>` object holds the benchmark results of the 10 benchmark runs. Each benchmark is recorded with a
different setting for :cpp:func:`complexityN <ankerl::nanobench::Bench::complexityN>`.

After the benchmark prints the benchmark results, we calculate & print the Big O of the most important complexity functions.
``std::cout << bench.complexityBigO() << std::endl;`` prints e.g. this markdown table:

.. code-block:: text

   |   coefficient |   err% | complexity
   |--------------:|-------:|------------
   |   6.66562e-09 |  29.1% | O(log n)
   |   1.47588e-11 |  58.3% | O(n)
   |   1.10742e-12 |  62.6% | O(n log n)
   |   5.15683e-08 |  63.8% | O(1)
   |   1.40387e-15 |  78.7% | O(n^2)
   |   1.32792e-19 |  85.7% | O(n^3)

The table is sorted, best fitting complexity function first. So
:math:`\mathcal{O}(\log{}n)` provides the best approximation for the complexity. Interestingly, in that case error compared to
:math:`\mathcal{O}(n)` is not very large, which can be an indication that even though the red-black tree should theoretically have
logarithmic complexity, in practices that is not perfectly the case.


.. _templating:

Rendering Mustache-like Templates
=================================

Nanobench comes with a powerful `Mustache <https://mustache.github.io/>`_-like template mechanism to process the benchmark
results into all kinds of formats. You can find a full description of all possible tags at :cpp:func:`ankerl::nanobench::render()`.

Several preconfigured format exist in the namespace ``ankerl::nanobench::templates``. Rendering these templates can be done
with either :cpp:func:`ankerl::nanobench::render()`, or directly with :cpp:func:`ankerl::nanobench::Bench::render()`.

The following example shows how to use the `CSV - Comma-Separated Values`_ template, without writing the standard output.

.. literalinclude:: ../test/tutorial_render_simple.cpp
   :language: c++
   :linenos:
   :emphasize-lines: 11,16
   :caption: tutorial_render_simple.cpp


In line 11 we call :cpp:func:`Bench::output() <ankerl::nanobench::Bench::output()>` with ``nullptr``, thus disabling the standard output.

After the benchmark we directly call :cpp:func:`Bench::render() <ankerl::nanobench::Bench::render()>` in line 16. Here we use the 
CSV template, and write the rendered output to ``std::cout``. When running, we get just the CSV output to the console which looks like this:

.. literalinclude:: _generated/tutorial_render_simple.txt
   :language: text

Nanobench comes with a few preconfigured templates, residing in the namespace ``ankerl::nanobench::templates``. To demonstrate what these templates can do,
here is a simple example that benchmarks two random generators ``std::mt19937_64`` and ``std::knuth_b`` and prints both the template and the rendered
output:

.. literalinclude:: ../test/tutorial_mustache.cpp
   :language: c++
   :linenos:

Nanobench allows to specify further context information, which may be accessed using ``{{context(name)}}`` where ``name`` names a variable defined in ``Bench::context()``.

.. literalinclude:: ../test/tutorial_context.cpp
   :language: c++
   :linenos:

.. _tutorial-template-csv:

CSV - Comma-Separated Values
----------------------------

The function :cpp:func:`ankerl::nanobench::templates::csv` provides this template:

.. literalinclude:: _generated/mustache.template.csv
   :language: text
   :linenos:

This generates a compact CSV file, where entries are separated by a semicolon `;`. Run with the example, I get this output:

.. literalinclude:: _generated/mustache.render.csv
   :language: text
   :linenos:

Rendered as CSV table:

.. csv-table::
   :file: _generated/mustache.render.csv
   :header-rows: 1
   :delim: ;

Note that the CSV template doesn't provide all the data that is available.



.. _tutorial-template-html:

HTML Box Plots
--------------

With the template :cpp:func:`ankerl::nanobench::templates::htmlBoxplot()` you get a `plotly <https://plotly.com/javascript/>`_ based HTML output which generates
a boxplot of the runtime. The template is rather simple.

.. literalinclude:: _generated/mustache.template.html
   :language: xml
   :linenos:

This generates a nice interactive boxplot, which gives a nice visual showcase of the runtime performance of the evaluated benchmarks. Each epoch is visualized as a dot,
and the boxplot itself shows median, percentiles, and outliers. You'll might want to increase the default number of epochs for an even better visualization result.

.. raw:: html
   :file: _generated/mustache.render.html


.. _tutorial-template-json:

JSON - JavaScript Object Notation
---------------------------------

The :cpp:func:`ankerl::nanobench::templates::json` template gives everything, all data that is available, from all runs. The template is therefore quite complex:

.. literalinclude:: _generated/mustache.template.json
   :language: text
   :linenos:

This also gives the data from each separate :cpp:func:`ankerl::nanobench::Bench::epochs()`, not just the accumulated data as in the CSV template.

.. literalinclude:: _generated/mustache.render.json
   :language: json
   :linenos:

.. _tutorial-template-pyperf:

pyperf - Python pyperf module Output
------------------------------------

`Pyperf <https://pyperf.readthedocs.io/en/latest/>`_ is a powerful tool for benchmarking and system tuning, and it can also analyze 
benchmark results. This template allows generation of output so it can be used for further analysis with pyperf.

.. note::

   Pyperf supports only a single benchmark result per generated output, so it is best to create a new
   ``Bench`` object for each benchmark.

The template looks like this. Note that it directly makes use of ``{{#measurement}}``, which is only possible when there is a single result in the benchmark.

.. literalinclude:: _generated/mustache.template.pyperf
   :language: text
   :linenos:

Here is an example that generates pyperf compatible output for a benchmark that shuffles a vector:

.. literalinclude:: ../test/example_pyperf.cpp
   :language: cpp
   :linenos:
   :caption: example_pyperf.cpp

This benchmark run creates the two files ``pyperf_shuffle_std.json`` and ``pyperf_shuffle_nanobench.json``.
Here are some of the analysis you can do:

Show Benchmark Statistics
~~~~~~~~~~~~~~~~~~~~~~~~~

Output from ``python3 -m pyperf stats pyperf_shuffle_std.json``:

.. literalinclude:: code/pyperf_stats.txt
   :language: text

Show a Histogram
~~~~~~~~~~~~~~~~

It's often interesting to see a histogram, especially to visually find out if there are outliers involved. 
Run ``python3 -m pyperf hist pyperf_shuffle_std.json`` produces this output

.. literalinclude:: code/pyperf_hist.txt
   :language: text


Compare Results
~~~~~~~~~~~~~~~

We have generated two results in the above examples, and we can compare them easily with ``python3 -m pyperf compare_to a.json b.json``:

.. literalinclude:: code/pyperf_compare_to.txt
   :language: text

For more information of pyperfs analysis capability, please see `pyperf - Analyze benchmark results <https://pyperf.readthedocs.io/en/latest/analyze.html>`_.
