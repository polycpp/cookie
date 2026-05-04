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

      Ported test corpus from the npm package and RFC 6265, including
      upstream top-site fixtures, attribute parsing, round-trips, and edge
      cases (empty values, custom encoding, and duplicate names).

   .. grid-item-card:: Plays well with polycpp
      :margin: 1

      Reuses polycpp URI, date, number, and error helpers, with optional
      ``polycpp::http::Headers`` adapters for applications already using
      polycpp HTTP types.

Getting started
---------------

.. code-block:: cmake

   FetchContent_Declare(
       polycpp_cookie
       GIT_REPOSITORY https://github.com/polycpp/cookie.git
       GIT_TAG        v1.0.0
   )
   FetchContent_MakeAvailable(polycpp_cookie)
   target_link_libraries(my_app PRIVATE polycpp::cookie)

Pin ``GIT_TAG`` to a release tag or commit SHA you have tested; avoid
floating branch names in application builds.

Version and compatibility
-------------------------

cookie ``1.0.0`` is based on the public API of upstream npm
``cookie@1.1.1``. The main C++ differences are intentional:

- request-cookie containers are ``std::map<std::string, std::string>``, so
  ``stringifyCookie`` output is key-sorted and JavaScript ``undefined``
  values are not representable;
- ``SameSite`` accepts string values only, not the JavaScript boolean
  shorthand;
- Set-Cookie entry points use typed C++ overloads and option structs
  instead of arbitrary runtime value coercion;
- parsing is tolerant of invalid parse-only attribute values, while
  serialization validates emitted names, values, and attributes and throws
  ``polycpp::TypeError`` for invalid output.

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
