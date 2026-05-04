HTTP adapters
=============

Include ``<polycpp/cookie/http.hpp>`` when code already uses
``polycpp::http::Headers`` and should avoid manually extracting header
strings.

Cookie uses combined request-header semantics; Set-Cookie does not. Parse
request ``Cookie`` values from the combined header, and append one
``Set-Cookie`` header for each response cookie:

.. code-block:: cpp

   #include <polycpp/cookie/http.hpp>

   polycpp::http::Headers requestHeaders;
   requestHeaders.append("Cookie", "sid=abc");
   requestHeaders.append("Cookie", "theme=dark");

   auto cookies = polycpp::cookie::parseCookieHeader(requestHeaders);
   auto sid = cookies.find("sid");

   polycpp::http::Headers responseHeaders;
   polycpp::cookie::appendSetCookieHeader(responseHeaders, "sid", "def", {
       .path     = "/",
       .httpOnly = true,
       .secure   = true,
       .sameSite = "lax",
   });
   polycpp::cookie::appendSetCookieHeader(responseHeaders, "theme", "dark", {
       .path = "/",
   });

   auto setCookies = responseHeaders.getSetCookie();
   // setCookies contains two repeated Set-Cookie field values.

.. doxygenfunction:: polycpp::cookie::parseCookieHeader
.. doxygenfunction:: polycpp::cookie::setCookieHeader
.. doxygenfunction:: polycpp::cookie::parseSetCookieHeaders
.. doxygenfunction:: polycpp::cookie::appendSetCookieHeader(polycpp::http::Headers &, const SetCookie &, const StringifyOptions &)
.. doxygenfunction:: polycpp::cookie::appendSetCookieHeader(polycpp::http::Headers &, const std::string &, const std::string &, const SerializeOptions &)
