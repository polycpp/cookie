#pragma once

/**
 * @file http.hpp
 * @brief Adapters between cookie helpers and polycpp HTTP Headers.
 *
 * The core cookie API stays string-based to mirror npm cookie. This header
 * adds opt-in convenience helpers for callers already using polycpp::http.
 *
 * @since 1.0.0
 */

#include <polycpp/cookie/cookie.hpp>
#include <polycpp/http/headers.hpp>

#ifdef POLYCPP_HEADER_ONLY
#include <polycpp/http/detail/headers_impl.hpp>
#endif

#include <map>
#include <string>
#include <vector>

namespace polycpp {
namespace cookie {

/**
 * @brief Parse the combined Cookie header from a Headers object.
 *
 * Uses polycpp::http::Headers::getCombined("Cookie"), which joins multiple
 * Cookie header values with "; " using Node-compatible header semantics.
 *
 * @param headers Source HTTP headers.
 * @param options Optional parse options.
 * @return Parsed cookie name/value map, or an empty map when no Cookie header exists.
 *
 * @since 1.0.0
 */
inline std::map<std::string, std::string> parseCookieHeader(
    const polycpp::http::Headers& headers,
    const ParseOptions& options = {}) {
    auto value = headers.getCombined("Cookie");
    return value ? parseCookie(*value, options) : std::map<std::string, std::string>{};
}

/**
 * @brief Set the Cookie header from a cookie map.
 *
 * @param headers Destination HTTP headers.
 * @param cookies Cookie name/value map.
 * @param options Optional stringify options.
 *
 * @since 1.0.0
 */
inline void setCookieHeader(
    polycpp::http::Headers& headers,
    const std::map<std::string, std::string>& cookies,
    const StringifyOptions& options = {}) {
    headers.set("Cookie", stringifyCookie(cookies, options));
}

/**
 * @brief Parse all Set-Cookie header values from a Headers object.
 *
 * @param headers Source HTTP headers.
 * @param options Optional parse options.
 * @return Parsed SetCookie values in header insertion order.
 *
 * @since 1.0.0
 */
inline std::vector<SetCookie> parseSetCookieHeaders(
    const polycpp::http::Headers& headers,
    const ParseOptions& options = {}) {
    std::vector<SetCookie> cookies;
    for (const auto& value : headers.getSetCookie()) {
        cookies.push_back(parseSetCookie(value, options));
    }
    return cookies;
}

/**
 * @brief Append a Set-Cookie header from a SetCookie value.
 *
 * @param headers Destination HTTP headers.
 * @param cookie Cookie value to serialize.
 * @param options Optional stringify options.
 *
 * @since 1.0.0
 */
inline void appendSetCookieHeader(
    polycpp::http::Headers& headers,
    const SetCookie& cookie,
    const StringifyOptions& options = {}) {
    headers.append("Set-Cookie", stringifySetCookie(cookie, options));
}

/**
 * @brief Append a Set-Cookie header from name, value, and serialize options.
 *
 * @param headers Destination HTTP headers.
 * @param name Cookie name.
 * @param value Cookie value.
 * @param options Set-Cookie serialization options.
 *
 * @since 1.0.0
 */
inline void appendSetCookieHeader(
    polycpp::http::Headers& headers,
    const std::string& name,
    const std::string& value,
    const SerializeOptions& options = {}) {
    headers.append("Set-Cookie", stringifySetCookie(name, value, options));
}

} // namespace cookie
} // namespace polycpp
