Session cookie lifecycle
========================

A compact program that exercises the sign-in → refresh → sign-out flow
with the same ``SerializeOptions`` across all three phases. Run it with
``--help`` to see the CLI.

.. literalinclude:: ../../../examples/session_lifecycle.cpp
   :language: cpp
   :linenos:

Build and run:

.. code-block:: bash

   cmake -B build -G Ninja -DPOLYCPP_COOKIE_BUILD_EXAMPLES=ON
   cmake --build build --target session_lifecycle
   ./build/examples/session_lifecycle signin
   ./build/examples/session_lifecycle refresh
   ./build/examples/session_lifecycle signout
