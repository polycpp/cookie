Quickstart
==========

This page walks through a minimal cookie program end-to-end. Copy the
snippet, run it, then jump to :doc:`../tutorials/index` for task-oriented
walkthroughs or :doc:`../api/index` for the full reference.

We'll read an incoming ``Cookie`` header, set two cookies on the response
with different attributes, and print both headers to verify the
round-trip.

Full example
------------

.. code-block:: cpp

   #include <iostream>
   #include <polycpp/cookie/cookie.hpp>

   using namespace polycpp;

   int main() {
       // 1. Parse a Cookie header coming from a request.
       auto incoming = cookie::parse("session=abc123; theme=dark; lang=en-US");
       std::cout << "Parsed " << incoming.size() << " cookies\n";
       for (const auto& [name, value] : incoming) {
           std::cout << "  " << name << " = " << value << '\n';
       }

       // 2. Build a Set-Cookie header for a new session.
       std::string sessionHeader = cookie::serialize("session", "xyz789", {
           .maxAge   = 3600,                  // 1 hour
           .path     = "/",
           .httpOnly = true,
           .secure   = true,
           .sameSite = "strict",
       });
       std::cout << "\nSet-Cookie: " << sessionHeader << '\n';

       // 3. Build another Set-Cookie for a user preference.
       std::string themeHeader = cookie::serialize("theme", "dark", {
           .maxAge = 60 * 60 * 24 * 365,      // 1 year
           .path   = "/",
       });
       std::cout << "Set-Cookie: " << themeHeader << '\n';
   }

Compile it with the same CMake wiring from :doc:`installation`:

.. code-block:: bash

   cmake -B build -G Ninja
   cmake --build build
   ./build/my_app

Expected output:

.. code-block:: text

   Parsed 3 cookies
     lang = en-US
     session = abc123
     theme = dark

   Set-Cookie: session=xyz789; Max-Age=3600; Path=/; HttpOnly; Secure; SameSite=Strict
   Set-Cookie: theme=dark; Max-Age=31536000; Path=/

What just happened
------------------

1. ``cookie::parse`` took a raw request-side ``Cookie`` header and
   returned a ``std::map<std::string, std::string>``. Values are
   URL-decoded by default; pass your own ``decode`` callback in
   :cpp:class:`polycpp::cookie::ParseOptions` to override.

2. ``cookie::serialize`` built a ``Set-Cookie`` header string. The
   :cpp:class:`polycpp::cookie::SerializeOptions` aggregate lets you set
   every RFC 6265 attribute by designated initialiser — no builders, no
   chained setters.

3. ``sameSite`` accepts ``"strict"``, ``"lax"``, or ``"none"``. Use
   ``"none"`` only together with ``secure = true`` — browsers reject it
   otherwise.

Next steps
----------

- :doc:`../tutorials/index` — step-by-step walkthroughs of common tasks.
- :doc:`../guides/index` — short how-tos for specific problems.
- :doc:`../api/index` — every public type, function, and option.
- :doc:`../examples/index` — runnable programs you can drop into a sandbox.
