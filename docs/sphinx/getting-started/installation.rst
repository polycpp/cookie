Installation
============

cookie targets C++20. On Linux it follows the base polycpp support
contract: GCC 11+ or Clang 15+ minimum, with GCC 13+ or Clang 16+
recommended. It depends only on the base
`polycpp <https://github.com/enricohuang/polycpp>`_ library.

CMake FetchContent (recommended)
--------------------------------

Add the library to your ``CMakeLists.txt``:

.. code-block:: cmake

   include(FetchContent)

   FetchContent_Declare(
       polycpp_cookie
       GIT_REPOSITORY https://github.com/polycpp/cookie.git
       GIT_TAG        v1.0.0
   )
   FetchContent_MakeAvailable(polycpp_cookie)

   add_executable(my_app main.cpp)
   target_link_libraries(my_app PRIVATE polycpp::cookie)

Pin ``GIT_TAG`` to a release tag or full commit SHA instead of ``master``.
A clean configure also fetches the transitive ``polycpp`` dependency at
the commit recorded by cookie's ``CMakeLists.txt``. If your environment
cannot fetch that repository directly, provide a local checkout with
``-DFETCHCONTENT_SOURCE_DIR_POLYCPP=/path/to/polycpp``.

If your application needs to choose the base polycpp revision itself,
declare ``polycpp`` before ``polycpp_cookie`` with the same FetchContent
name:

.. code-block:: cmake

   FetchContent_Declare(
       polycpp
       GIT_REPOSITORY https://github.com/enricohuang/polycpp.git
       GIT_TAG        7a076d7bbffaa16f6001e5c4fa3a0aead34ddb32
   )

Using a local clone
-------------------

If you already have cookie and polycpp checked out side by side, tell
CMake to use them instead of fetching from GitHub:

.. code-block:: bash

   cmake -B build -G Ninja \
       -DFETCHCONTENT_SOURCE_DIR_POLYCPP=/path/to/polycpp \
       -DFETCHCONTENT_SOURCE_DIR_POLYCPP_COOKIE=/path/to/cookie

Use this path when developing both repositories together or when your
environment cannot fetch the base polycpp repository directly.

Build options
-------------

``POLYCPP_COOKIE_BUILD_TESTS``
    Build the GoogleTest suite. Defaults to ``ON`` for standalone builds and
    ``OFF`` when consumed via FetchContent.

``POLYCPP_COOKIE_BUILD_EXAMPLES``
    Build the runnable example programs. Defaults to ``OFF``.

``POLYCPP_COOKIE_BUILD_BENCHMARKS``
    Build the dependency-free parse benchmark executable. Defaults to ``OFF``.

``POLYCPP_IO``
    ``asio`` (default) or ``libuv`` — inherited from polycpp. Android
    defaults to ``libuv`` when the option is not set.

``POLYCPP_SSL_BACKEND``
    ``openssl`` (default) or ``boringssl``. Android defaults to
    ``boringssl`` when the option is not set.

``POLYCPP_UNICODE``
    ``auto`` (default), ``icu``, or ``builtin``. ``auto`` uses system ICU
    when available, then libunistring/iconv when available, and finally the
    built-in ASCII-only fallback.

Verifying the install
---------------------

.. code-block:: bash

   cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
   cmake --build build
   ctest --test-dir build --output-on-failure

All tests should pass on a supported toolchain — if they do not, open an
issue on the `repository <https://github.com/polycpp/cookie/issues>`_
with the compiler version and the failing test name.
