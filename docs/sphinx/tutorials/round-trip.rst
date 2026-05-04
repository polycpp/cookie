Round-trip a ``Set-Cookie`` header
==================================

**You'll build:** a unit-test helper that proves
``stringifySetCookie(parseSetCookie(s)) == s`` for a representative
corpus of attributes — handy when you accept cookies from third parties
and want to be sure you aren't losing attributes on the way back out.

**You'll use:**
:cpp:func:`polycpp::cookie::parseSetCookie`,
:cpp:func:`polycpp::cookie::stringifySetCookie`,
:cpp:class:`polycpp::cookie::SetCookie`.

**Prerequisites:** a GoogleTest-enabled build (the repo ships with one).

Step 1 — parse once, inspect the attribute struct
-------------------------------------------------

.. code-block:: cpp

   #include <polycpp/cookie/cookie.hpp>
   using namespace polycpp::cookie;

   SetCookie parsed = parseSetCookie(
       "sid=abc; Domain=example.com; Path=/api; Max-Age=600; "
       "HttpOnly; Secure; SameSite=Strict");

   EXPECT_EQ(parsed.name, "sid");
   EXPECT_EQ(parsed.value, "abc");
   EXPECT_EQ(parsed.domain, "example.com");
   EXPECT_EQ(parsed.path,   "/api");
   EXPECT_EQ(parsed.maxAge, 600);
   EXPECT_TRUE(parsed.httpOnly);
   EXPECT_TRUE(parsed.secure);
   EXPECT_EQ(parsed.sameSite, "strict");

Every attribute is ``std::optional``-typed on ``SetCookie``, so a missing
``Expires`` is ``std::nullopt`` — not an empty string. That distinction
matters when you round-trip: serializing a ``nullopt`` attribute emits
nothing, serializing an empty string emits the attribute with an empty
value.

Step 2 — re-serialize
---------------------

.. code-block:: cpp

   std::string serialized = stringifySetCookie(parsed);
   EXPECT_EQ(serialized,
             "sid=abc; Domain=example.com; Path=/api; Max-Age=600; "
             "HttpOnly; Secure; SameSite=Strict");

Attribute order in the output follows the order the library emits
(documented on :cpp:func:`polycpp::cookie::stringifySetCookie`), not the
order of the input — so the round-trip compares canonical form, not raw
byte equality.

Step 3 — corpus-driven test
---------------------------

.. code-block:: cpp

   TEST(SetCookieRoundTrip, CanonicalForm) {
       const std::vector<std::string> corpus = {
           "a=b",
           "sid=xyz; HttpOnly",
           "sid=xyz; Path=/; HttpOnly; Secure; SameSite=Lax",
           "pref=dark; Max-Age=31536000; Path=/",
           "tracker=; Expires=Thu, 01 Jan 1970 00:00:00 GMT",
       };
       for (const auto& original : corpus) {
           auto parsed = parseSetCookie(original);
           auto reserialized = stringifySetCookie(parsed);
           EXPECT_EQ(reserialized, original)
               << "drift round-tripping: " << original;
       }
   }

If a test case drifts, either the input was non-canonical (the library
does not preserve stylistic variations like ``HTTPOnly`` vs.
``HttpOnly``) or the attribute is unsupported — check the
:cpp:class:`polycpp::cookie::SetCookie` class reference for the full
list.

What you learned
----------------

- ``parseSetCookie`` returns a strongly-typed :cpp:class:`SetCookie`,
  not a string map.
- Unset attributes are ``std::nullopt`` — use that to distinguish
  "absent" from "empty".
- Round-trips are canonical, not verbatim.
