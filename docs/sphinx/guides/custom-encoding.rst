Use a custom encode/decode pair
===============================

**When to reach for this:** your cookie values contain characters your
reverse proxy rejects, or you're migrating from an app that uses a
non-standard encoding (e.g., hex, base64url).

The default encoder is ``encodeURIComponent`` — matches the npm
package. To override, supply ``encode`` / ``decode`` callbacks:

.. code-block:: cpp

   #include <polycpp/cookie/cookie.hpp>

   // Base64URL-style encoding so cookie values survive any proxy.
   auto encodeB64 = [](const std::string& v) { return myapp::base64url::encode(v); };
   auto decodeB64 = [](const std::string& v) { return myapp::base64url::decode(v); };

   std::string header = polycpp::cookie::serialize("data", "<raw-bytes>",
       {.encode = encodeB64});

   auto pairs = polycpp::cookie::parse(request.getHeader("Cookie").value_or(""),
                                       {.decode = decodeB64});

Keep the encode and decode halves paired. If a request arrives before
you've deployed the decoder, your app sees the encoded form and will
most likely crash downstream — validate at the boundary.

The encode/decode hooks run on every value, not every attribute. Attribute
values (``Domain``, ``Path``) are never passed through them.
