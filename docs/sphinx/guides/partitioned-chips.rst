Set a Partitioned (CHIPS) cookie
================================

**When to reach for this:** you embed your site as a third-party in an
iframe and still need session state without relying on third-party
cookies that many browsers, modes, or user settings block or restrict.
CHIPS partitions your cookie by the top-level site so each embed gets
its own jar.

.. code-block:: cpp

   std::string header = polycpp::cookie::serialize("embed_session", sessionId, {
       .path        = "/",
       .httpOnly    = true,
       .secure      = true,
       .partitioned = true,
       .sameSite    = "none",
   });
   // embed_session=<id>; Path=/; HttpOnly; Secure; Partitioned; SameSite=None

What the library validates:

- ``partitioned`` is a boolean flag. When true, it emits
  ``Partitioned``.
- ``sameSite`` accepts ``strict``, ``lax``, or ``none`` and is validated
  independently.
- The serializer does not enforce browser cross-attribute policy.

Browser policy for third-party CHIPS delivery:

- ``secure = true`` — partitioned cookies must be transmitted over
  HTTPS.
- ``sameSite = None`` — needed when this cookie must be sent in a
  third-party context.

If the browser does not support CHIPS yet, the attribute is ignored and
the cookie falls back to a normal unpartitioned third-party cookie
(which the browser may still block). Build your flow to tolerate that —
don't rely on the partitioned cookie being present.
