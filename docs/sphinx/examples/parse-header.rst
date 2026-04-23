Parse a request ``Cookie`` header
=================================

A minimal CLI that reads one line on stdin (the value of a ``Cookie``
header) and prints each cookie on its own line. Useful for piping
through logs.

.. literalinclude:: ../../../examples/parse_header.cpp
   :language: cpp
   :linenos:

Build and run:

.. code-block:: bash

   cmake -B build -G Ninja -DPOLYCPP_COOKIE_BUILD_EXAMPLES=ON
   cmake --build build --target parse_header
   echo 'session=abc; theme=dark; lang=en' | ./build/examples/parse_header
