HTTP adapters
=============

Include ``<polycpp/cookie/http.hpp>`` when code already uses
``polycpp::http::Headers`` and should avoid manually extracting header
strings.

.. doxygenfunction:: polycpp::cookie::parseCookieHeader
.. doxygenfunction:: polycpp::cookie::setCookieHeader
.. doxygenfunction:: polycpp::cookie::parseSetCookieHeaders
.. doxygenfunction:: polycpp::cookie::appendSetCookieHeader(polycpp::http::Headers&, const SetCookie&, const StringifyOptions&)
.. doxygenfunction:: polycpp::cookie::appendSetCookieHeader(polycpp::http::Headers&, const std::string&, const std::string&, const SerializeOptions&)
