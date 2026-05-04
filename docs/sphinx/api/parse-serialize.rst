Parse and serialize
===================

Cookie-header and Set-Cookie-header entry points. The functions in this
page are stateless — every call is independent.

.. doxygenfunction:: polycpp::cookie::parseCookie
.. doxygenfunction:: polycpp::cookie::stringifyCookie
.. doxygenfunction:: polycpp::cookie::parseSetCookie
.. doxygenfunction:: polycpp::cookie::stringifySetCookie(const SetCookie &, const StringifyOptions &)
.. doxygenfunction:: polycpp::cookie::stringifySetCookie(const std::string &, const std::string &, const SerializeOptions &)

Convenience aliases
-------------------

``parse`` and ``serialize`` are one-line aliases that match the
npm ``cookie`` package's top-level exports. They delegate to
``parseCookie`` and ``stringifySetCookie`` respectively.

.. doxygenfunction:: polycpp::cookie::parse
.. doxygenfunction:: polycpp::cookie::serialize
