.. nanobench documentation master file, created by
   sphinx-quickstart on Tue Jun  2 20:02:50 2020.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

=========
nanobench
=========

.. toctree::
   :maxdepth: 2
   :hidden:
   :caption: Getting Started

   tutorial


.. toctree::
   :maxdepth: 2
   :hidden:
   :caption: Comparison

   comparison


.. toctree::
   :maxdepth: 2
   :hidden:
   :caption: Reference

   reference
   genindex

.. toctree::
   :maxdepth: 2
   :hidden:
   :caption: About

   license
   CODE_OF_CONDUCT.md


.. |badge-release| image:: https://img.shields.io/github/release/martinus/nanobench.svg
   :target: https://github.com/martinus/nanobench/releases
   :alt: Release
.. |badge-license| image:: https://img.shields.io/github/license/martinus/nanobench.svg
   :target: https://raw.githubusercontent.com/martinus/nanobench/master/LICENSE
   :alt: License
.. |badge-travis| image:: https://travis-ci.com/martinus/nanobench.svg?branch=master
   :target: https://travis-ci.com/martinus/nanobench
   :alt: Travis CI Build Status
.. |badge-appveyor| image:: https://ci.appveyor.com/api/projects/status/github/martinus/nanobench?branch=master&svg=true
   :target: https://ci.appveyor.com/project/martinus/nanobench
   :alt: Appveyor Build Status
.. |badge-gitter| image:: https://badges.gitter.im/nanobench/community.svg
   :target: https://gitter.im/nanobench/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge
   :alt: Join the chat at https://gitter.im/nanobench/community

.. |download-button| raw:: html

   <div class="container">
      <div class="row">
         <div class="col text-center">
            <div class="btn-group">
               <button type="button" class="btn btn-success">Download</button>
               <button type="button" class="btn btn-success dropdown-toggle dropdown-toggle-split" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">
                  <span class="sr-only">Toggle Dropdown</span>
               </button>
               <div class="dropdown-menu">
                  <a class="dropdown-item" href="#">Action</a>
                  <a class="dropdown-item" href="#">Another action</a>
                  <a class="dropdown-item" href="#">Something else here</a>
                  <div class="dropdown-divider"></div>
                  <a class="dropdown-item" href="#">Separated link</a>
               </div>
            </div>
         </div>
      </div>
   </div>

   
|badge-release| |badge-license| |badge-travis| |badge-appveyor| |badge-gitter|

.. image:: nanobench-logo-small.svg
   :alt: I need a better logo. Currently I use a small bench. Nanobench. Ha ha.


.. hint::

   View and download `nanobench on Github <https://github.com/martinus/nanobench>`_.
   Get the latest :download:`release <https://github.com/martinus/nanobench/releases/latest>`.



``ankerl::nanobench`` is a platform independent microbenchmarking library for C++11/14/17/20.


.. literalinclude:: code/full_example_simple.cpp
    :language: c++
    :linenos:


The whole executable runs for ~60ms and prints


.. code-block:: text

   |               ns/op |                op/s |    err% |          ins/op |          cyc/op |    IPC |         bra/op |   miss% |     total | benchmark
   |--------------------:|--------------------:|--------:|----------------:|----------------:|-------:|---------------:|--------:|----------:|:----------
   |                7.52 |      132,948,239.79 |    1.1% |            6.65 |           24.07 |  0.276 |           1.00 |    8.9% |      0.00 | `some double ops`


Which github renders like

====================== ===================== ========= ================= ================= ======== ================ ========= =========== ====================
                ns/op                  op/s      err%            ins/op            cyc/op      IPC           bra/op     miss%       total   benchmark
====================== ===================== ========= ================= ================= ======== ================ ========= =========== ====================
                 7.52        132,948,239.79      1.1%              6.65             24.07    0.276             1.00      8.9%        0.00  ``some double ops``
====================== ===================== ========= ================= ================= ======== ================ ========= =========== ====================

The benchmarked code takes **7.52** nanoseconds to run, so **~133** million times per seconds. Measurements fluctuate by
**1.1%**. On average **6.65** instructions are executed in **24.07** CPU cycles, resulting in **0.276** instructions per
cycle. A **single** branch is in the code, which branch prediction missed in **8.9%** of the cases. Total runtime of
the benchmark with the name ``some double ops`` is **0.00**, so just a few milliseconds.

Design Goals
============

Ease of use
   Simple but powerful API, fast compile times, easy to integrate anywhere.
Fast
   Get accurate results as fast as possible
Accurate
   Get deterministic, repeatable, and accurate results that you can make sound decisions on.
Robust
   Be robust against outliers, warn if results are not reliable.

