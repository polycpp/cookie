/**
 * @file test_cookie.cpp
 * @brief Tests for polycpp::cookie, ported from npm cookie test suite.
 */

#include <polycpp/cookie/detail/aggregator.hpp>

#include <gtest/gtest.h>

#include <chrono>
#include <map>
#include <string>

using namespace polycpp::cookie;

// ============================================================================
// parseCookie tests
// ============================================================================

TEST(ParseCookieTest, ParseBasicCookieString) {
    auto cookies = parseCookie("foo=bar");
    EXPECT_EQ(cookies["foo"], "bar");
}

TEST(ParseCookieTest, ParseNumericValue) {
    auto cookies = parseCookie("foo=123");
    EXPECT_EQ(cookies["foo"], "123");
}

TEST(ParseCookieTest, IgnoreOWS) {
    auto cookies = parseCookie("FOO    = bar;   baz  =   raz");
    EXPECT_EQ(cookies["FOO"], "bar");
    EXPECT_EQ(cookies["baz"], "raz");
}

TEST(ParseCookieTest, ReturnEmptyForEmptyString) {
    auto cookies = parseCookie("");
    EXPECT_TRUE(cookies.empty());
}

TEST(ParseCookieTest, ReturnEmptyForWhitespaceOnly) {
    auto cookies = parseCookie(" \t ");
    EXPECT_TRUE(cookies.empty());
}

TEST(ParseCookieTest, ParseEmptyValue) {
    auto cookies = parseCookie("foo=; bar=");
    EXPECT_EQ(cookies["foo"], "");
    EXPECT_EQ(cookies["bar"], "");
}

TEST(ParseCookieTest, ParseMinimumLength) {
    auto cookies = parseCookie("f=");
    EXPECT_EQ(cookies["f"], "");

    auto cookies2 = parseCookie("f=;b=");
    EXPECT_EQ(cookies2["f"], "");
    EXPECT_EQ(cookies2["b"], "");
}

TEST(ParseCookieTest, URLDecodeValues) {
    // Quoted value with special chars preserved literally
    auto cookies = parseCookie("foo=\"bar=123456789&name=Magic+Mouse\"");
    EXPECT_EQ(cookies["foo"], "\"bar=123456789&name=Magic+Mouse\"");

    // Percent-encoded values
    auto cookies2 = parseCookie("email=%20%22%2c%3b%2f");
    EXPECT_EQ(cookies2["email"], " \",;/");
}

TEST(ParseCookieTest, TrimWhitespace) {
    auto cookies = parseCookie("  foo  =  \"bar\"  ");
    EXPECT_EQ(cookies["foo"], "\"bar\"");

    auto cookies2 = parseCookie("  foo  =  bar  ;  fizz  =  buzz  ");
    EXPECT_EQ(cookies2["foo"], "bar");
    EXPECT_EQ(cookies2["fizz"], "buzz");

    auto cookies3 = parseCookie(" foo = \" a b c \" ");
    EXPECT_EQ(cookies3["foo"], "\" a b c \"");

    auto cookies4 = parseCookie(" = bar ");
    EXPECT_EQ(cookies4[""], "bar");

    auto cookies5 = parseCookie(" foo = ");
    EXPECT_EQ(cookies5["foo"], "");

    auto cookies6 = parseCookie("   =   ");
    EXPECT_EQ(cookies6[""], "");

    auto cookies7 = parseCookie("\tfoo\t=\tbar\t");
    EXPECT_EQ(cookies7["foo"], "bar");
}

TEST(ParseCookieTest, ReturnOriginalOnDecodeError) {
    auto cookies = parseCookie("foo=%1;bar=bar");
    EXPECT_EQ(cookies["foo"], "%1");
    EXPECT_EQ(cookies["bar"], "bar");
}

TEST(ParseCookieTest, IgnoreCookiesWithoutValue) {
    auto cookies = parseCookie("foo=bar;fizz  ;  buzz");
    EXPECT_EQ(cookies.size(), 1u);
    EXPECT_EQ(cookies["foo"], "bar");

    auto cookies2 = parseCookie("  fizz; foo=  bar");
    EXPECT_EQ(cookies2.size(), 1u);
    EXPECT_EQ(cookies2["foo"], "bar");
}

TEST(ParseCookieTest, IgnoreDuplicateCookies) {
    auto cookies = parseCookie("foo=%1;bar=bar;foo=boo");
    EXPECT_EQ(cookies["foo"], "%1");
    EXPECT_EQ(cookies["bar"], "bar");

    auto cookies2 = parseCookie("foo=false;bar=bar;foo=true");
    EXPECT_EQ(cookies2["foo"], "false");
    EXPECT_EQ(cookies2["bar"], "bar");

    auto cookies3 = parseCookie("foo=;bar=bar;foo=boo");
    EXPECT_EQ(cookies3["foo"], "");
    EXPECT_EQ(cookies3["bar"], "bar");
}

TEST(ParseCookieTest, CustomDecodeFunction) {
    ParseOptions opts;
    // Simple decoder that reverses the string
    opts.decode = [](const std::string& v) -> std::string {
        return std::string(v.rbegin(), v.rend());
    };
    auto cookies = parseCookie("foo=rab", opts);
    EXPECT_EQ(cookies["foo"], "bar");
}

// ============================================================================
// stringifyCookie tests
// ============================================================================

TEST(StringifyCookieTest, StringifyObject) {
    std::map<std::string, std::string> cookies = {{"key", "value"}};
    EXPECT_EQ(stringifyCookie(cookies), "key=value");
}

TEST(StringifyCookieTest, StringifyMultipleEntries) {
    std::map<std::string, std::string> cookies = {{"a", "1"}, {"b", "2"}};
    EXPECT_EQ(stringifyCookie(cookies), "a=1; b=2");
}

TEST(StringifyCookieTest, ErrorOnInvalidKeys) {
    std::map<std::string, std::string> cookies = {{"test=", ""}};
    EXPECT_THROW(stringifyCookie(cookies), polycpp::TypeError);
}

TEST(StringifyCookieTest, ErrorOnInvalidValues) {
    StringifyOptions opts;
    opts.encode = [](const std::string& x) { return x; };
    std::map<std::string, std::string> cookies = {{"test", ";"}};
    EXPECT_THROW(stringifyCookie(cookies, opts), polycpp::TypeError);
}

// ============================================================================
// parseSetCookie tests
// ============================================================================

TEST(ParseSetCookieTest, ParseBasicString) {
    auto sc = parseSetCookie("key=value");
    EXPECT_EQ(sc.name, "key");
    EXPECT_EQ(sc.value, "value");
}

TEST(ParseSetCookieTest, HandleEmptyValue) {
    auto sc = parseSetCookie("key=");
    EXPECT_EQ(sc.name, "key");
    EXPECT_EQ(sc.value, "");
}

TEST(ParseSetCookieTest, HandleMissingEqualsSign) {
    auto sc = parseSetCookie("value");
    EXPECT_EQ(sc.name, "");
    EXPECT_EQ(sc.value, "value");
}

TEST(ParseSetCookieTest, TrimWhitespace) {
    auto sc = parseSetCookie("  key  =  value  ");
    EXPECT_EQ(sc.name, "key");
    EXPECT_EQ(sc.value, "value");

    auto sc2 = parseSetCookie("\tkey\t=\tvalue\t");
    EXPECT_EQ(sc2.name, "key");
    EXPECT_EQ(sc2.value, "value");
}

TEST(ParseSetCookieTest, EmptyString) {
    auto sc = parseSetCookie("");
    EXPECT_EQ(sc.name, "");
    EXPECT_EQ(sc.value, "");
}

TEST(ParseSetCookieTest, SpacesOnly) {
    auto sc = parseSetCookie("   ");
    EXPECT_EQ(sc.name, "");
    EXPECT_EQ(sc.value, "");
}

TEST(ParseSetCookieTest, URLEncodedValues) {
    auto sc = parseSetCookie("key=value%20with%20spaces");
    EXPECT_EQ(sc.name, "key");
    EXPECT_EQ(sc.value, "value with spaces");
}

TEST(ParseSetCookieTest, MultipleEqualsInValue) {
    auto sc = parseSetCookie("key=value=with=equals");
    EXPECT_EQ(sc.name, "key");
    EXPECT_EQ(sc.value, "value=with=equals");
}

TEST(ParseSetCookieTest, IgnoreUnknownAttributes) {
    auto sc = parseSetCookie("key=value; UnknownAttr=somevalue; AnotherOne");
    EXPECT_EQ(sc.name, "key");
    EXPECT_EQ(sc.value, "value");
}

TEST(ParseSetCookieTest, BooleanAttributes) {
    auto sc = parseSetCookie("key=value; HttpOnly; Secure");
    EXPECT_EQ(sc.name, "key");
    EXPECT_EQ(sc.value, "value");
    EXPECT_TRUE(sc.httpOnly);
    EXPECT_TRUE(sc.secure);
}

TEST(ParseSetCookieTest, IgnoreValueOfBooleanAttributes) {
    auto sc = parseSetCookie("key=value; HttpOnly=true; Secure=false");
    EXPECT_TRUE(sc.httpOnly);
    EXPECT_TRUE(sc.secure);
}

TEST(ParseSetCookieTest, AttributesWithExtraSpaces) {
    auto sc = parseSetCookie("key=value;    HttpOnly   ;   Secure   ");
    EXPECT_TRUE(sc.httpOnly);
    EXPECT_TRUE(sc.secure);
}

TEST(ParseSetCookieTest, SkipEmptyAttributes) {
    auto sc = parseSetCookie("key=value;;; HttpOnly;;; Secure;;");
    EXPECT_TRUE(sc.httpOnly);
    EXPECT_TRUE(sc.secure);
}

TEST(ParseSetCookieTest, CustomDecodeFunction) {
    ParseOptions opts;
    opts.decode = [](const std::string& str) -> std::string {
        std::string result = str;
        size_t pos = 0;
        while ((pos = result.find('-', pos)) != std::string::npos) {
            result.replace(pos, 1, " ");
            pos++;
        }
        return result;
    };
    auto sc = parseSetCookie("key=value-with-dashes", opts);
    EXPECT_EQ(sc.name, "key");
    EXPECT_EQ(sc.value, "value with dashes");
}

TEST(ParseSetCookieTest, ParseValidExpires) {
    auto sc = parseSetCookie("key=value; Expires=Wed, 21 Oct 2015 07:28:00 GMT");
    EXPECT_EQ(sc.name, "key");
    EXPECT_EQ(sc.value, "value");
    EXPECT_TRUE(sc.expires.has_value());
}

TEST(ParseSetCookieTest, IgnoreInvalidExpires) {
    auto sc = parseSetCookie("key=value; Expires=InvalidDate");
    EXPECT_EQ(sc.name, "key");
    EXPECT_EQ(sc.value, "value");
    EXPECT_FALSE(sc.expires.has_value());
}

TEST(ParseSetCookieTest, ParseValidMaxAge) {
    auto sc = parseSetCookie("key=value; Max-Age=3600");
    EXPECT_EQ(sc.name, "key");
    EXPECT_EQ(sc.value, "value");
    EXPECT_TRUE(sc.maxAge.has_value());
    EXPECT_EQ(*sc.maxAge, 3600);
}

TEST(ParseSetCookieTest, IgnoreInvalidMaxAge) {
    auto sc = parseSetCookie("key=value; Max-Age=Invalid");
    EXPECT_FALSE(sc.maxAge.has_value());
}

TEST(ParseSetCookieTest, IgnorePartialDigitsMaxAge) {
    auto sc = parseSetCookie("key=value; Max-Age=123abc");
    EXPECT_FALSE(sc.maxAge.has_value());
}

TEST(ParseSetCookieTest, IgnoreDecimalMaxAge) {
    auto sc = parseSetCookie("key=value; Max-Age=1.5");
    EXPECT_FALSE(sc.maxAge.has_value());
}

TEST(ParseSetCookieTest, ParseNegativeMaxAge) {
    auto sc = parseSetCookie("key=value; Max-Age=-1");
    EXPECT_TRUE(sc.maxAge.has_value());
    EXPECT_EQ(*sc.maxAge, -1);
}

TEST(ParseSetCookieTest, ParseDomain) {
    auto sc = parseSetCookie("key=value; Domain=example.com");
    EXPECT_TRUE(sc.domain.has_value());
    EXPECT_EQ(*sc.domain, "example.com");
}

TEST(ParseSetCookieTest, ParsePath) {
    auto sc = parseSetCookie("key=value; Path=/some/path");
    EXPECT_TRUE(sc.path.has_value());
    EXPECT_EQ(*sc.path, "/some/path");
}

TEST(ParseSetCookieTest, ParseHttpOnly) {
    auto sc = parseSetCookie("key=value; HttpOnly");
    EXPECT_TRUE(sc.httpOnly);
}

TEST(ParseSetCookieTest, ParseSecure) {
    auto sc = parseSetCookie("key=value; Secure");
    EXPECT_TRUE(sc.secure);
}

TEST(ParseSetCookieTest, ParsePartitioned) {
    auto sc = parseSetCookie("key=value; Partitioned");
    EXPECT_TRUE(sc.partitioned);
}

TEST(ParseSetCookieTest, ParseSameSiteStrict) {
    auto sc = parseSetCookie("key=value; SameSite=Strict");
    EXPECT_TRUE(sc.sameSite.has_value());
    EXPECT_EQ(*sc.sameSite, "strict");

    auto sc2 = parseSetCookie("key=value; SameSite=strict");
    EXPECT_EQ(*sc2.sameSite, "strict");
}

TEST(ParseSetCookieTest, ParseSameSiteLax) {
    auto sc = parseSetCookie("key=value; SameSite=Lax");
    EXPECT_TRUE(sc.sameSite.has_value());
    EXPECT_EQ(*sc.sameSite, "lax");

    auto sc2 = parseSetCookie("key=value; SameSite=lax");
    EXPECT_EQ(*sc2.sameSite, "lax");
}

TEST(ParseSetCookieTest, ParseSameSiteNone) {
    auto sc = parseSetCookie("key=value; SameSite=None");
    EXPECT_TRUE(sc.sameSite.has_value());
    EXPECT_EQ(*sc.sameSite, "none");

    auto sc2 = parseSetCookie("key=value; SameSite=none");
    EXPECT_EQ(*sc2.sameSite, "none");
}

TEST(ParseSetCookieTest, IgnoreInvalidSameSite) {
    auto sc = parseSetCookie("key=value; SameSite=Invalid");
    EXPECT_FALSE(sc.sameSite.has_value());
}

TEST(ParseSetCookieTest, ParsePriorityLow) {
    auto sc = parseSetCookie("key=value; Priority=Low");
    EXPECT_TRUE(sc.priority.has_value());
    EXPECT_EQ(*sc.priority, "low");

    auto sc2 = parseSetCookie("key=value; Priority=low");
    EXPECT_EQ(*sc2.priority, "low");
}

TEST(ParseSetCookieTest, ParsePriorityMedium) {
    auto sc = parseSetCookie("key=value; Priority=Medium");
    EXPECT_TRUE(sc.priority.has_value());
    EXPECT_EQ(*sc.priority, "medium");

    auto sc2 = parseSetCookie("key=value; Priority=medium");
    EXPECT_EQ(*sc2.priority, "medium");
}

TEST(ParseSetCookieTest, ParsePriorityHigh) {
    auto sc = parseSetCookie("key=value; Priority=High");
    EXPECT_TRUE(sc.priority.has_value());
    EXPECT_EQ(*sc.priority, "high");

    auto sc2 = parseSetCookie("key=value; Priority=high");
    EXPECT_EQ(*sc2.priority, "high");
}

TEST(ParseSetCookieTest, IgnoreInvalidPriority) {
    auto sc = parseSetCookie("key=value; Priority=Invalid");
    EXPECT_FALSE(sc.priority.has_value());
}

// ============================================================================
// stringifySetCookie tests (SetCookie struct overload)
// ============================================================================

TEST(StringifySetCookieTest, SerializeNameAndValue) {
    EXPECT_EQ(stringifySetCookie("foo", "bar"), "foo=bar");
}

TEST(StringifySetCookieTest, URLEncodeValue) {
    EXPECT_EQ(stringifySetCookie("foo", "bar +baz"), "foo=bar%20%2Bbaz");
}

TEST(StringifySetCookieTest, SerializeEmptyValue) {
    EXPECT_EQ(stringifySetCookie("foo", ""), "foo=");
}

TEST(StringifySetCookieTest, SerializeSetCookieObject) {
    SetCookie sc;
    sc.name = "foo";
    sc.value = "bar +baz";
    EXPECT_EQ(stringifySetCookie(sc), "foo=bar%20%2Bbaz");
}

TEST(StringifySetCookieTest, SerializeWithCustomEncode) {
    SetCookie sc;
    sc.name = "foo";
    sc.value = "bar+baz";
    StringifyOptions opts;
    opts.encode = [](const std::string& x) { return x; };
    EXPECT_EQ(stringifySetCookie(sc, opts), "foo=bar+baz");
}

// Valid names test
TEST(StringifySetCookieTest, ValidNames) {
    std::vector<std::string> validNames = {
        "foo", "foo,bar", "foo!bar", "foo#bar", "foo$bar", "foo'bar",
        "foo*bar", "foo+bar", "foo-bar", "foo.bar", "foo^bar", "foo_bar",
        "foo`bar", "foo|bar", "foo~bar", "foo7bar", "foo/bar", "foo@bar",
        "foo[bar", "foo]bar", "foo:bar", "foo{bar", "foo}bar",
        "foo\"bar", "foo<bar", "foo>bar", "foo?bar", "foo\\bar"
    };
    for (const auto& name : validNames) {
        EXPECT_EQ(stringifySetCookie(name, "baz"), name + "=baz")
            << "Failed for name: " << name;
    }
}

// Invalid names test
TEST(StringifySetCookieTest, InvalidNames) {
    std::vector<std::string> invalidNames = {
        "foo\n", std::string("foo") + std::string(1, '\x80'),
        "foo=bar", "foo;bar", "foo bar", "foo\tbar"
    };
    for (const auto& name : invalidNames) {
        EXPECT_THROW(stringifySetCookie(name, "bar"), polycpp::TypeError)
            << "Should have thrown for name: " << name;
    }
}

// Domain tests
TEST(StringifySetCookieTest, ValidDomains) {
    std::vector<std::string> validDomains = {
        "example.com", "sub.example.com", ".example.com",
        "localhost", ".localhost", "my-site.org"
    };
    for (const auto& domain : validDomains) {
        SerializeOptions opts;
        opts.domain = domain;
        EXPECT_EQ(stringifySetCookie("foo", "bar", opts),
                  "foo=bar; Domain=" + domain)
            << "Failed for domain: " << domain;
    }
}

TEST(StringifySetCookieTest, InvalidDomains) {
    std::vector<std::string> invalidDomains = {
        std::string("example.com\n"),
        std::string("sub.example.com") + std::string(1, '\0'),
        "my site.org",
        "domain..com",
        "example.com; Path=/",
        "example.com /* inject a comment */"
    };
    for (const auto& domain : invalidDomains) {
        SerializeOptions opts;
        opts.domain = domain;
        EXPECT_THROW(stringifySetCookie("foo", "bar", opts), polycpp::TypeError)
            << "Should have thrown for domain: " << domain;
    }
}

// Encode option tests
TEST(StringifySetCookieTest, CustomEncoder) {
    SerializeOptions opts;
    opts.encode = [](const std::string& v) -> std::string {
        // Simple base64-like identity for testing
        return v;
    };

    // Values that are valid cookie octets when passed through identity encoder
    std::vector<std::string> validValues = {
        "foo=bar", "foo\"bar", "foo,bar", "foo\\bar", "foo$bar"
    };
    for (const auto& value : validValues) {
        opts.encode = [](const std::string& x) { return x; };
        EXPECT_EQ(stringifySetCookie("foo", value, opts), "foo=" + value)
            << "Failed for value: " << value;
    }
}

TEST(StringifySetCookieTest, InvalidEncodedValues) {
    SerializeOptions opts;
    opts.encode = [](const std::string& x) { return x; };

    std::vector<std::string> invalidValues = {
        "+\n", "foo bar", "foo\tbar", "foo;bar",
        std::string("foo") + std::string(1, '\x80')
    };
    for (const auto& value : invalidValues) {
        EXPECT_THROW(stringifySetCookie("foo", value, opts), polycpp::TypeError)
            << "Should have thrown for value: " << value;
    }
}

// Expires tests
TEST(StringifySetCookieTest, SetExpires) {
    SerializeOptions opts;
    // Dec 24, 2000 10:30:59.900 UTC = Date.UTC(2000, 11, 24, 10, 30, 59, 900)
    auto tp = std::chrono::system_clock::time_point(
        std::chrono::milliseconds(977653859900LL));
    opts.expires = tp;
    EXPECT_EQ(stringifySetCookie("foo", "bar", opts),
              "foo=bar; Expires=Sun, 24 Dec 2000 10:30:59 GMT");
}

// HttpOnly tests
TEST(StringifySetCookieTest, HttpOnlyTrue) {
    SerializeOptions opts;
    opts.httpOnly = true;
    EXPECT_EQ(stringifySetCookie("foo", "bar", opts), "foo=bar; HttpOnly");
}

TEST(StringifySetCookieTest, HttpOnlyFalse) {
    SerializeOptions opts;
    opts.httpOnly = false;
    EXPECT_EQ(stringifySetCookie("foo", "bar", opts), "foo=bar");
}

// MaxAge tests
TEST(StringifySetCookieTest, SetMaxAge) {
    SerializeOptions opts;
    opts.maxAge = 1000;
    EXPECT_EQ(stringifySetCookie("foo", "bar", opts), "foo=bar; Max-Age=1000");

    opts.maxAge = 0;
    EXPECT_EQ(stringifySetCookie("foo", "bar", opts), "foo=bar; Max-Age=0");
}

TEST(StringifySetCookieTest, MaxAgeUndefined) {
    SerializeOptions opts;
    EXPECT_EQ(stringifySetCookie("foo", "bar", opts), "foo=bar");
}

// Partitioned tests
TEST(StringifySetCookieTest, PartitionedTrue) {
    SerializeOptions opts;
    opts.partitioned = true;
    EXPECT_EQ(stringifySetCookie("foo", "bar", opts), "foo=bar; Partitioned");
}

TEST(StringifySetCookieTest, PartitionedFalse) {
    SerializeOptions opts;
    opts.partitioned = false;
    EXPECT_EQ(stringifySetCookie("foo", "bar", opts), "foo=bar");
}

// Path tests
TEST(StringifySetCookieTest, ValidPaths) {
    std::vector<std::string> validPaths = {
        "/", "/login", "/foo.bar/baz", "/foo-bar",
        "/foo=bar?baz", "/foo\"bar\"", "/../foo/bar",
        "../foo/", "./"
    };
    for (const auto& path : validPaths) {
        SerializeOptions opts;
        opts.path = path;
        EXPECT_EQ(stringifySetCookie("foo", "bar", opts),
                  "foo=bar; Path=" + path)
            << "Failed for path: " << path;
    }
}

TEST(StringifySetCookieTest, InvalidPaths) {
    std::vector<std::string> invalidPaths = {
        "/\n",
        std::string("/foo") + std::string(1, '\0'),
        std::string("/path/with\rnewline"),
        "/; Path=/sensitive-data",
    };
    for (const auto& path : invalidPaths) {
        SerializeOptions opts;
        opts.path = path;
        EXPECT_THROW(stringifySetCookie("foo", "bar", opts), polycpp::TypeError)
            << "Should have thrown for path: " << path;
    }
}

// Priority tests
TEST(StringifySetCookieTest, PriorityLow) {
    SerializeOptions opts;
    opts.priority = "low";
    EXPECT_EQ(stringifySetCookie("foo", "bar", opts), "foo=bar; Priority=Low");
}

TEST(StringifySetCookieTest, PriorityMedium) {
    SerializeOptions opts;
    opts.priority = "medium";
    EXPECT_EQ(stringifySetCookie("foo", "bar", opts), "foo=bar; Priority=Medium");
}

TEST(StringifySetCookieTest, PriorityHigh) {
    SerializeOptions opts;
    opts.priority = "high";
    EXPECT_EQ(stringifySetCookie("foo", "bar", opts), "foo=bar; Priority=High");
}

TEST(StringifySetCookieTest, PriorityCaseInsensitive) {
    SerializeOptions opts;
    opts.priority = "High";
    EXPECT_EQ(stringifySetCookie("foo", "bar", opts), "foo=bar; Priority=High");
}

TEST(StringifySetCookieTest, InvalidPriority) {
    SerializeOptions opts;
    opts.priority = "foo";
    EXPECT_THROW(stringifySetCookie("foo", "bar", opts), polycpp::TypeError);
}

// SameSite tests
TEST(StringifySetCookieTest, SameSiteStrict) {
    SerializeOptions opts;
    opts.sameSite = "strict";
    EXPECT_EQ(stringifySetCookie("foo", "bar", opts), "foo=bar; SameSite=Strict");
}

TEST(StringifySetCookieTest, SameSiteLax) {
    SerializeOptions opts;
    opts.sameSite = "lax";
    EXPECT_EQ(stringifySetCookie("foo", "bar", opts), "foo=bar; SameSite=Lax");
}

TEST(StringifySetCookieTest, SameSiteNone) {
    SerializeOptions opts;
    opts.sameSite = "none";
    EXPECT_EQ(stringifySetCookie("foo", "bar", opts), "foo=bar; SameSite=None");
}

TEST(StringifySetCookieTest, SameSiteCaseInsensitive) {
    SerializeOptions opts;
    opts.sameSite = "Lax";
    EXPECT_EQ(stringifySetCookie("foo", "bar", opts), "foo=bar; SameSite=Lax");
}

TEST(StringifySetCookieTest, InvalidSameSite) {
    SerializeOptions opts;
    opts.sameSite = "foo";
    EXPECT_THROW(stringifySetCookie("foo", "bar", opts), polycpp::TypeError);
}

// Secure tests
TEST(StringifySetCookieTest, SecureTrue) {
    SerializeOptions opts;
    opts.secure = true;
    EXPECT_EQ(stringifySetCookie("foo", "bar", opts), "foo=bar; Secure");
}

TEST(StringifySetCookieTest, SecureFalse) {
    SerializeOptions opts;
    opts.secure = false;
    EXPECT_EQ(stringifySetCookie("foo", "bar", opts), "foo=bar");
}

// ============================================================================
// Backward-compatible alias tests
// ============================================================================

TEST(AliasTest, ParseAlias) {
    auto cookies = parse("foo=bar");
    EXPECT_EQ(cookies["foo"], "bar");
}

TEST(AliasTest, SerializeAlias) {
    EXPECT_EQ(serialize("foo", "bar"), "foo=bar");
}

// ============================================================================
// Round-trip tests
// ============================================================================

TEST(RoundTripTest, ParseAndStringifyCookie) {
    std::map<std::string, std::string> original = {{"foo", "bar"}, {"baz", "qux"}};
    auto header = stringifyCookie(original);
    auto parsed = parseCookie(header);
    EXPECT_EQ(parsed["foo"], "bar");
    EXPECT_EQ(parsed["baz"], "qux");
}

TEST(RoundTripTest, ParseAndStringifySetCookie) {
    SetCookie sc;
    sc.name = "session";
    sc.value = "abc123";
    sc.httpOnly = true;
    sc.secure = true;
    sc.path = "/";
    sc.sameSite = "strict";
    sc.maxAge = 3600;

    auto header = stringifySetCookie(sc);
    auto parsed = parseSetCookie(header);

    EXPECT_EQ(parsed.name, "session");
    EXPECT_EQ(parsed.value, "abc123");
    EXPECT_TRUE(parsed.httpOnly);
    EXPECT_TRUE(parsed.secure);
    EXPECT_TRUE(parsed.path.has_value());
    EXPECT_EQ(*parsed.path, "/");
    EXPECT_TRUE(parsed.sameSite.has_value());
    EXPECT_EQ(*parsed.sameSite, "strict");
    EXPECT_TRUE(parsed.maxAge.has_value());
    EXPECT_EQ(*parsed.maxAge, 3600);
}

// ============================================================================
// Edge case tests
// ============================================================================

TEST(EdgeCaseTest, ParseCookieWithMultipleEquals) {
    // The value should include everything after the first =
    auto cookies = parseCookie("foo=bar=baz=qux");
    EXPECT_EQ(cookies["foo"], "bar=baz=qux");
}

TEST(EdgeCaseTest, ParseCookieSingleChar) {
    auto cookies = parseCookie("a=b");
    EXPECT_EQ(cookies["a"], "b");
}

TEST(EdgeCaseTest, StringifySetCookieAllOptions) {
    SerializeOptions opts;
    opts.maxAge = 3600;
    opts.domain = "example.com";
    opts.path = "/";
    opts.httpOnly = true;
    opts.secure = true;
    opts.partitioned = true;
    opts.priority = "high";
    opts.sameSite = "strict";

    auto result = stringifySetCookie("session", "abc", opts);
    EXPECT_NE(result.find("session="), std::string::npos);
    EXPECT_NE(result.find("Max-Age=3600"), std::string::npos);
    EXPECT_NE(result.find("Domain=example.com"), std::string::npos);
    EXPECT_NE(result.find("Path=/"), std::string::npos);
    EXPECT_NE(result.find("HttpOnly"), std::string::npos);
    EXPECT_NE(result.find("Secure"), std::string::npos);
    EXPECT_NE(result.find("Partitioned"), std::string::npos);
    EXPECT_NE(result.find("Priority=High"), std::string::npos);
    EXPECT_NE(result.find("SameSite=Strict"), std::string::npos);
}

TEST(EdgeCaseTest, ParseSetCookieAllAttributes) {
    auto sc = parseSetCookie(
        "session=abc; Max-Age=3600; Domain=example.com; Path=/; "
        "Expires=Sun, 24 Dec 2000 10:30:59 GMT; HttpOnly; Secure; "
        "Partitioned; Priority=High; SameSite=Strict");

    EXPECT_EQ(sc.name, "session");
    EXPECT_EQ(sc.value, "abc");
    EXPECT_TRUE(sc.maxAge.has_value());
    EXPECT_EQ(*sc.maxAge, 3600);
    EXPECT_TRUE(sc.domain.has_value());
    EXPECT_EQ(*sc.domain, "example.com");
    EXPECT_TRUE(sc.path.has_value());
    EXPECT_EQ(*sc.path, "/");
    EXPECT_TRUE(sc.expires.has_value());
    EXPECT_TRUE(sc.httpOnly);
    EXPECT_TRUE(sc.secure);
    EXPECT_TRUE(sc.partitioned);
    EXPECT_TRUE(sc.priority.has_value());
    EXPECT_EQ(*sc.priority, "high");
    EXPECT_TRUE(sc.sameSite.has_value());
    EXPECT_EQ(*sc.sameSite, "strict");
}
