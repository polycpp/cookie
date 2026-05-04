SetCookie
=========

The strongly-typed representation of a ``Set-Cookie`` header, including
every RFC 6265 / RFC 6265bis attribute polycpp recognises.

.. doxygenstruct:: polycpp::cookie::SetCookie
   :members:
   :undoc-members:

Attribute string values
-----------------------

``SetCookie::sameSite`` and ``SerializeOptions::sameSite`` accept the
validated strings ``"strict"``, ``"lax"``, and ``"none"``. Invalid strings
are ignored while parsing and throw ``polycpp::TypeError`` while serializing.
