Options
=======

Aggregate types consumed by the parse and serialize functions.

ParseOptions
------------

Passed to :cpp:func:`polycpp::cookie::parseCookie` (and the ``parse``
alias). Only ``decode`` is currently overridable; the field is
default-constructed and uses ``decodeURIComponent`` when left empty.

.. doxygenstruct:: polycpp::cookie::ParseOptions
   :members:
   :undoc-members:

StringifyOptions
----------------

Passed to :cpp:func:`polycpp::cookie::stringifyCookie`. Mirrors
``ParseOptions`` with an ``encode`` callback.

.. doxygenstruct:: polycpp::cookie::StringifyOptions
   :members:
   :undoc-members:

SerializeOptions
----------------

Passed to :cpp:func:`polycpp::cookie::stringifySetCookie` and the
``serialize`` alias. Every RFC 6265 attribute that ``SetCookie``
understands has a matching field here — construct with designated
initialisers, not setters.

.. doxygenstruct:: polycpp::cookie::SerializeOptions
   :members:
   :undoc-members:
