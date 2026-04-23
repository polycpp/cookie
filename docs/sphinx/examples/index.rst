Examples
========

Self-contained programs exercising the main features of cookie. Each
example compiles against the public API only — no private headers, no
non-exported targets.

.. toctree::
   :maxdepth: 1

   parse-header
   session-lifecycle

Running an example
------------------

From the repository root:

.. code-block:: bash

   cmake -B build -G Ninja
   cmake --build build --target <example_name>
   ./build/examples/<example_name>

Examples are only built when ``POLYCPP_COOKIE_BUILD_EXAMPLES=ON`` is passed to CMake.
