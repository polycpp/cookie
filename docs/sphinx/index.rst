cookie
======

**HTTP cookie parse and serialize**

Parse ``Cookie`` headers, build ``Set-Cookie`` headers, and handle
attributes (Domain, Path, Expires, Max-Age, HttpOnly, Secure, SameSite,
Partitioned) with the same API shape as the npm ``cookie`` package.
Drop-in for request/response middleware that already speaks polycpp's
``http`` API.

.. code-block:: cpp

   #include <polycpp/cookie/cookie.hpp>
   using namespace polycpp;

   auto pairs = cookie::parse("session=abc123; theme=dark");
   // pairs["session"] == "abc123"

   std::string header = cookie::serialize("session", "abc123",
       {.httpOnly = true, .secure = true, .sameSite = "lax"});
   // header == "session=abc123; HttpOnly; Secure; SameSite=Lax"

.. grid:: 2

   .. grid-item-card:: Drop-in familiarity
      :margin: 1

      Mirrors the npm ``cookie`` package — ``parse(str, opts)``,
      ``serialize(name, value, opts)``, and a ``parseSetCookie`` /
      ``stringifySetCookie`` pair for server-side attribute handling.

   .. grid-item-card:: C++20 native
      :margin: 1

      A small compiled target with typed option structs and explicit
      validation exceptions.

   .. grid-item-card:: Tested
      :margin: 1

      Ported test corpus from the npm package and RFC 6265 — focused tests
      across attribute parsing, round-trips, and edge cases (empty values,
      custom encoding, and duplicate names).

   .. grid-item-card:: Plays well with polycpp
      :margin: 1

      Reuses polycpp URI, date, number, and error helpers while keeping the
      public API to ordinary C++ strings and aggregates.

Getting started
---------------

.. code-block:: cmake

   FetchContent_Declare(
       polycpp_cookie
       GIT_REPOSITORY https://github.com/polycpp/cookie.git
       GIT_TAG        master
   )
   FetchContent_MakeAvailable(polycpp_cookie)
   target_link_libraries(my_app PRIVATE polycpp::cookie)

:doc:`Installation <getting-started/installation>` · :doc:`Quickstart <getting-started/quickstart>` · :doc:`Tutorials <tutorials/index>` · :doc:`API reference <api/index>`

.. toctree::
   :hidden:
   :caption: Getting started

   getting-started/installation
   getting-started/quickstart

.. toctree::
   :hidden:
   :caption: Tutorials

   tutorials/index

.. toctree::
   :hidden:
   :caption: How-to guides

   guides/index

.. toctree::
   :hidden:
   :caption: API reference

   api/index

.. toctree::
   :hidden:
   :caption: Examples

   examples/index
