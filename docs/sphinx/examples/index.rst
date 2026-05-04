Examples
========

Self-contained programs exercising the main features of cookie. Each
example compiles against the public API only — no private headers, no
non-exported targets.

.. toctree::
   :maxdepth: 1

   parse-header
   session-lifecycle
   benchmarks

Running an example
------------------

From the repository root:

.. code-block:: bash

   cmake -B build -G Ninja -DPOLYCPP_COOKIE_BUILD_EXAMPLES=ON
   cmake --build build --target polycpp_cookie_example_parse_header
   printf '%s\n' 'a=1; b=two%20words' | ./build/examples/parse_header

For the session flow example, build ``polycpp_cookie_example_session_lifecycle``
and run ``./build/examples/session_lifecycle signin``.

Running the benchmark
---------------------

From the repository root:

.. code-block:: bash

   cmake -B build-bench -G Ninja -DCMAKE_BUILD_TYPE=Release -DPOLYCPP_COOKIE_BUILD_BENCHMARKS=ON
   cmake --build build-bench --target polycpp_cookie_benchmark
   ./build-bench/benchmarks/cookie_bench 10000

The optional argument is the iteration count. Output is one tab-separated
row per case with the benchmark name and average ``ns/op``. Use a Release
build for timing comparisons; Debug builds are only useful as smoke tests.
