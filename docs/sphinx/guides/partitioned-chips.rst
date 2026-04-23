Set a Partitioned (CHIPS) cookie
================================

**When to reach for this:** you embed your site as a third-party in an
iframe and still need session state without relying on third-party
cookies that browsers are phasing out. CHIPS partitions your cookie by
the top-level site so each embed gets its own jar.

.. code-block:: cpp

   std::string header = polycpp::cookie::serialize("embed_session", sessionId, {
       .httpOnly    = true,
       .secure      = true,
       .sameSite    = polycpp::cookie::SameSite::None,
       .partitioned = true,
       .path        = "/",
   });
   // embed_session=<id>; Path=/; HttpOnly; Secure; SameSite=None; Partitioned

Requirements the browser enforces:

- ``secure = true`` — partitioned cookies must be transmitted over
  HTTPS.
- ``sameSite = None`` — otherwise the cookie is not sent in a
  cross-site context, which defeats the whole purpose.

If the browser does not support CHIPS yet, the attribute is ignored and
the cookie falls back to a normal third-party cookie (which the browser
may still block). Build your flow to tolerate that — don't rely on the
partitioned cookie being present.
