Delete a cookie
===============

**When to reach for this:** you want the browser to drop a cookie —
sign-out, user-opt-out, or consent revocation.

Two equivalent techniques: set ``Max-Age=0`` or set ``Expires`` to a
past date. ``Max-Age`` is preferred because it's a delta, not an absolute
time, so it can't be defeated by a skewed client clock.

.. code-block:: cpp

   // Preferred: Max-Age=0
   std::string clear = polycpp::cookie::serialize("sid", "", {
       .maxAge = 0,
       .path   = "/",
   });

   // Equivalent: Expires in the past
   std::string clearAlt = polycpp::cookie::serialize("sid", "", {
       .expires = std::chrono::system_clock::from_time_t(0),
       .path    = "/",
   });

**Match the original attributes.** The browser identifies a cookie by
``(name, domain, path)``. A deletion header that differs on ``Path`` or
``Domain`` creates a new, empty cookie instead of removing the old one.
If your sign-in set ``Path=/app``, your sign-out must too.

Verify with the browser devtools: after the response, the cookie should
disappear from the Application/Storage pane. If it's still there with
``value=""``, the attributes didn't match.
