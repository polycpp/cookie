Benchmarks
==========

The optional benchmark executable measures parser throughput for small
headers, duplicate-name request cookies, generated multi-cookie headers,
and top-site compatibility fixtures.

Build and run from the repository root:

.. code-block:: bash

   cmake -B build-bench -G Ninja -DCMAKE_BUILD_TYPE=Release -DPOLYCPP_COOKIE_BUILD_BENCHMARKS=ON
   cmake --build build-bench --target polycpp_cookie_benchmark
   ./build-bench/benchmarks/cookie_bench 10000

The numeric argument is optional and defaults to ``10000`` iterations.
Each output row is tab-separated and reports the case name followed by
average nanoseconds per operation:

.. code-block:: text

   parseCookie empty        1234.5 ns/op
   parseCookie simple       1456.7 ns/op
   parseCookie 100 cookies  240000 ns/op
   parseSetCookie top youtube.com       8200.1 ns/op

Exact timings depend on compiler, optimization level, hardware, and
selected polycpp dependency options. Use a Release build for timing
comparisons; Debug builds are only useful as smoke tests.
