Send a cookie with ``SameSite=None`` safely
===========================================

**When to reach for this:** you need the cookie to travel on cross-site
or third-party requests (e.g., an iframe, a payment redirect, a CORS XHR with
credentials).

.. code-block:: cpp

   std::string header = polycpp::cookie::serialize("xs_token", token, {
       .path     = "/",
       .httpOnly = true,
       .secure   = true,                                   // REQUIRED
       .sameSite = "none",
   });

Browser policy, separate from library validation:

1. ``SameSite=None`` without ``Secure`` is rejected by current browsers.
   The library validates ``sameSite`` and ``secure`` independently and can
   emit that combination, so treat ``secure = true`` as an application
   requirement for browser delivery.

2. Pair ``SameSite=None`` with server-side CSRF defenses. ``SameSite``
   was your second line of defense; with ``None`` you're back to
   relying on origin checks and CSRF tokens.

3. Use ``SameSite=None`` for cookies that must be delivered in cross-site
   contexts. For first-party session cookies, ``Lax`` or ``Strict`` is
   usually a better default.

4. Consider whether :doc:`partitioned-chips` is a better fit. If the
   cookie only makes sense in a third-party embed, partitioning narrows
   where the browser stores and sends it.
