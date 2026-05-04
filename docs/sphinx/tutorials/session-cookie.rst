Issue a secure session cookie
=============================

**You'll build:** a small HTTP handler that hands out a short-lived,
opaque server-side session id and invalidates it on sign-out.

**You'll use:** :cpp:func:`polycpp::cookie::serialize`,
:cpp:class:`polycpp::cookie::SerializeOptions`, and the ``sameSite``
option.

**Prerequisites:** installed and linking polycpp and ``polycpp::cookie``.
See :doc:`../getting-started/installation` if not.

Step 1 — pick attributes that match your threat model
-----------------------------------------------------

For a session cookie on a TLS-only site, the sane defaults are:

.. code-block:: cpp

   polycpp::cookie::SerializeOptions sessionOpts{
       .maxAge   = 60 * 60 * 2,                     // 2-hour idle lifetime
       .path     = "/",
       .httpOnly = true,                            // block document.cookie access
       .secure   = true,                            // never sent over plain HTTP
       .sameSite = "lax",                           // allow top-level cross-site GET
   };

These align with the OWASP session-management cheat sheet. Bump
``sameSite`` to ``Strict`` if your site has no cross-origin navigations
that need the cookie (SSO redirects are the common counter-example).

Step 2 — mint the cookie on sign-in
-----------------------------------

After you validate credentials, generate a server-side session id and
put it in the cookie value:

.. code-block:: cpp

   std::string sessionId = myapp::newSessionId();  // e.g. 32-byte base64url
   myapp::sessions::create(sessionId, userId);

   std::string setCookie = polycpp::cookie::serialize("sid", sessionId, sessionOpts);
   response.setHeader("Set-Cookie", setCookie);

Never put secrets directly into the cookie value — store the session
server-side and let the cookie hold only the opaque id.

Step 3 — consume the cookie on subsequent requests
--------------------------------------------------

Parse the ``Cookie`` header with :cpp:func:`polycpp::cookie::parse`:

.. code-block:: cpp

   auto cookies = polycpp::cookie::parse(request.getHeader("Cookie").value_or(""));
   auto it = cookies.find("sid");
   if (it == cookies.end()) {
       response.writeHead(401);
       response.end("sign in, please");
       return;
   }
   auto userId = myapp::sessions::resolve(it->second);

If ``it->second`` doesn't resolve to an active session, treat it as if
the cookie weren't sent at all. Don't echo it back with a ``Set-Cookie``
— let the browser keep the stale id until it expires naturally.

Step 4 — expire on sign-out
---------------------------

To invalidate a cookie, re-issue it with ``maxAge = 0``:

.. code-block:: cpp

   auto opts = sessionOpts;
   opts.maxAge = 0;
   response.setHeader("Set-Cookie",
                      polycpp::cookie::serialize("sid", "", opts));

The ``Max-Age=0`` attribute tells the browser to drop the cookie
immediately. Match the original ``Path`` and ``Domain`` because those
attributes identify which browser cookie is being deleted. Reusing the
rest of ``sessionOpts`` keeps the deletion header consistent, but
``Secure``, ``HttpOnly``, and ``SameSite`` do not create separate cookie
identities.

What you learned
----------------

- ``SerializeOptions`` is a plain aggregate — construct it with
  designated initialisers, not setters.
- The sign-out flow reuses the sign-in attributes with ``maxAge = 0``.
- Cookie content is an opaque id; authentication state lives on the
  server.
- If you choose signed stateless cookie values instead, sign before
  calling ``serialize`` and verify after ``parse``; cookie only handles
  header syntax.

Next: :doc:`round-trip` shows how to verify parse/serialize symmetry in
a unit test.
