Send a cookie with ``SameSite=None`` safely
===========================================

**When to reach for this:** you need the cookie to travel on cross-site
requests (e.g., an iframe, a payment redirect, a CORS XHR with
credentials).

.. code-block:: cpp

   std::string header = polycpp::cookie::serialize("xs_token", token, {
       .httpOnly = true,
       .secure   = true,                                   // REQUIRED
       .sameSite = polycpp::cookie::SameSite::None,
       .path     = "/",
   });

Rules the library (and the browser) enforce:

1. ``SameSite=None`` without ``Secure`` is rejected by every current
   browser. The library happily emits it, but the browser will drop
   the cookie silently — which is hard to debug.

2. Pair ``SameSite=None`` with server-side CSRF defenses. ``SameSite``
   was your second line of defense; with ``None`` you're back to
   relying on origin checks and CSRF tokens.

3. Consider whether :doc:`partitioned-chips` is a better fit. If the
   cookie only makes sense in the third-party context, partitioning it
   is strictly safer than ``SameSite=None`` alone.
