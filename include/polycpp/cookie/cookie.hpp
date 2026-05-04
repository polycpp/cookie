#pragma once

/**
 * @file cookie.hpp
 * @brief HTTP cookie parsing and serialization (RFC 6265).
 *
 * C++ port of npm cookie (https://github.com/jshttp/cookie).
 * Provides functions to parse Cookie headers, parse Set-Cookie headers,
 * stringify Cookie headers, and stringify Set-Cookie headers.
 *
 * @see https://www.npmjs.com/package/cookie
 * @see https://datatracker.ietf.org/doc/html/rfc6265
 * @since 0.1.0
 */

#include <chrono>
#include <functional>
#include <map>
#include <optional>
#include <string>

namespace polycpp {
namespace cookie {

/**
 * @brief Options for parsing cookie strings.
 *
 * @see https://github.com/jshttp/cookie#parsecookiestr-options
 * @since 0.1.0
 */
struct ParseOptions {
    /**
     * @brief Custom decode function for cookie values.
     *
     * If not provided, the default decoder uses decodeURIComponent with
     * a fallback to the raw string on decode errors.
     */
    std::function<std::string(const std::string&)> decode;
};

/**
 * @brief Options for stringifying cookie values.
 *
 * @see https://github.com/jshttp/cookie#stringifycookiecookies-options
 * @since 0.1.0
 */
struct StringifyOptions {
    /**
     * @brief Custom encode function for cookie values.
     *
     * If not provided, the default encoder uses encodeURIComponent.
     */
    std::function<std::string(const std::string&)> encode;
};

/**
 * @brief Represents a parsed or to-be-serialized Set-Cookie header.
 *
 * @see https://github.com/jshttp/cookie#setcookie
 * @since 0.1.0
 */
struct SetCookie {
    /** @brief Cookie name. */
    std::string name;

    /** @brief Cookie value. */
    std::string value;

    /**
     * @brief Max-Age attribute in seconds.
     * @see https://tools.ietf.org/html/rfc6265#section-5.2.2
     */
    std::optional<int> maxAge;

    /**
     * @brief Expires attribute as a time point.
     * @see https://tools.ietf.org/html/rfc6265#section-5.2.1
     */
    std::optional<std::chrono::system_clock::time_point> expires;

    /**
     * @brief Domain attribute.
     * @see https://tools.ietf.org/html/rfc6265#section-5.2.3
     */
    std::optional<std::string> domain;

    /**
     * @brief Path attribute.
     * @see https://tools.ietf.org/html/rfc6265#section-5.2.4
     */
    std::optional<std::string> path;

    /**
     * @brief HttpOnly flag.
     * @see https://tools.ietf.org/html/rfc6265#section-5.2.6
     */
    bool httpOnly = false;

    /**
     * @brief Secure flag.
     * @see https://tools.ietf.org/html/rfc6265#section-5.2.5
     */
    bool secure = false;

    /**
     * @brief Partitioned flag (CHIPS proposal).
     * @see https://tools.ietf.org/html/draft-cutler-httpbis-partitioned-cookies/
     */
    bool partitioned = false;

    /**
     * @brief Priority attribute ("low", "medium", "high").
     * @see https://tools.ietf.org/html/draft-west-cookie-priority-00#section-4.1
     */
    std::optional<std::string> priority;

    /**
     * @brief SameSite attribute ("strict", "lax", "none").
     * @see https://tools.ietf.org/html/draft-ietf-httpbis-rfc6265bis-09#section-5.4.7
     */
    std::optional<std::string> sameSite;
};

/**
 * @brief Serialization options for stringifySetCookie (backward-compatible overload).
 *
 * Combines StringifyOptions with all Set-Cookie attributes except name/value.
 *
 * @since 0.1.0
 */
struct SerializeOptions {
    /** @brief Custom encode function. */
    std::function<std::string(const std::string&)> encode;

    /** @brief Max-Age in seconds. */
    std::optional<int> maxAge;

    /** @brief Expires time point. */
    std::optional<std::chrono::system_clock::time_point> expires;

    /** @brief Domain attribute. */
    std::optional<std::string> domain;

    /** @brief Path attribute. */
    std::optional<std::string> path;

    /** @brief HttpOnly flag. */
    bool httpOnly = false;

    /** @brief Secure flag. */
    bool secure = false;

    /** @brief Partitioned flag. */
    bool partitioned = false;

    /** @brief Priority ("low", "medium", "high"). */
    std::optional<std::string> priority;

    /** @brief SameSite ("strict", "lax", "none"). */
    std::optional<std::string> sameSite;
};

/**
 * @brief Parse an HTTP Cookie header string into a map of name-value pairs.
 *
 * Parses the given cookie header string into a map where keys are cookie
 * names and values are cookie values. Implements first-wins semantics for
 * duplicate cookie names.
 *
 * @param str The Cookie header string to parse.
 * @param options Optional parse options (custom decode function).
 * @return Map of cookie name to cookie value.
 *
 * @par Example
 * @code{.cpp}
 *   auto cookies = polycpp::cookie::parseCookie("foo=bar; baz=qux");
 *   // cookies["foo"] == "bar"
 *   // cookies["baz"] == "qux"
 * @endcode
 *
 * @see https://github.com/jshttp/cookie#parsecookiestr-options
 * @since 0.1.0
 */
std::map<std::string, std::string> parseCookie(
    const std::string& str,
    const ParseOptions& options = {});

/**
 * @brief Stringify a map of cookies into an HTTP Cookie header string.
 *
 * @param cookies Map of cookie name to cookie value.
 * @param options Optional stringify options (custom encode function).
 * @return The Cookie header string.
 * @throws polycpp::TypeError if a cookie name or encoded value is invalid.
 *
 * @par Example
 * @code{.cpp}
 *   std::map<std::string, std::string> cookies = {{"foo", "bar"}, {"baz", "qux"}};
 *   auto str = polycpp::cookie::stringifyCookie(cookies);
 *   // str == "baz=qux; foo=bar"
 * @endcode
 *
 * @see https://github.com/jshttp/cookie#stringifycookiecookies-options
 * @since 0.1.0
 */
std::string stringifyCookie(
    const std::map<std::string, std::string>& cookies,
    const StringifyOptions& options = {});

/**
 * @brief Parse an HTTP Set-Cookie header string into a SetCookie struct.
 *
 * @param str The Set-Cookie header string to parse.
 * @param options Optional parse options (custom decode function).
 * @return Parsed SetCookie struct.
 *
 * @par Example
 * @code{.cpp}
 *   auto sc = polycpp::cookie::parseSetCookie("foo=bar; HttpOnly; Secure");
 *   // sc.name == "foo", sc.value == "bar", sc.httpOnly == true, sc.secure == true
 * @endcode
 *
 * @see https://github.com/jshttp/cookie#parsesetcookiestr-options
 * @since 0.1.0
 */
SetCookie parseSetCookie(
    const std::string& str,
    const ParseOptions& options = {});

/**
 * @brief Stringify a SetCookie struct into an HTTP Set-Cookie header string.
 *
 * @param cookie The SetCookie struct to serialize.
 * @param options Optional stringify options (custom encode function).
 * @return The Set-Cookie header string.
 * @throws polycpp::TypeError if cookie name, value, domain, path, maxAge,
 *         expires, priority, or sameSite is invalid.
 *
 * @par Example
 * @code{.cpp}
 *   polycpp::cookie::SetCookie sc;
 *   sc.name = "foo";
 *   sc.value = "bar";
 *   sc.httpOnly = true;
 *   auto str = polycpp::cookie::stringifySetCookie(sc);
 *   // str == "foo=bar; HttpOnly"
 * @endcode
 *
 * @see https://github.com/jshttp/cookie#stringifysetcookiecookie-options
 * @since 0.1.0
 */
std::string stringifySetCookie(
    const SetCookie& cookie,
    const StringifyOptions& options = {});

/**
 * @brief Stringify a Set-Cookie header from name, value, and options.
 *
 * Backward-compatible overload matching the npm cookie serialize() signature.
 *
 * @param name Cookie name.
 * @param value Cookie value.
 * @param options Serialization options including Set-Cookie attributes.
 * @return The Set-Cookie header string.
 * @throws polycpp::TypeError if any parameter is invalid.
 *
 * @par Example
 * @code{.cpp}
 *   polycpp::cookie::SerializeOptions opts;
 *   opts.httpOnly = true;
 *   opts.secure = true;
 *   auto str = polycpp::cookie::stringifySetCookie("foo", "bar", opts);
 *   // str == "foo=bar; HttpOnly; Secure"
 * @endcode
 *
 * @see https://github.com/jshttp/cookie#serializename-value-options
 * @since 0.1.0
 */
std::string stringifySetCookie(
    const std::string& name,
    const std::string& value,
    const SerializeOptions& options = {});

/**
 * @brief Backward-compatible alias for parseCookie.
 *
 * @param str The Cookie header string to parse.
 * @param options Optional parse options.
 * @return Map of cookie name to cookie value.
 *
 * @see parseCookie
 * @since 0.1.0
 */
inline std::map<std::string, std::string> parse(
    const std::string& str,
    const ParseOptions& options = {}) {
    return parseCookie(str, options);
}

/**
 * @brief Backward-compatible alias for name/value stringifySetCookie.
 *
 * @param name Cookie name.
 * @param value Cookie value.
 * @param options Serialization options including Set-Cookie attributes.
 * @return The Set-Cookie header string.
 *
 * @see stringifySetCookie
 * @since 0.1.0
 */
inline std::string serialize(
    const std::string& name,
    const std::string& value,
    const SerializeOptions& options = {}) {
    return stringifySetCookie(name, value, options);
}

} // namespace cookie
} // namespace polycpp

#include <polycpp/cookie/detail/cookie.hpp>
