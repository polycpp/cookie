#pragma once

/**
 * @file detail/cookie.hpp
 * @brief Inline implementation of cookie parsing and serialization.
 */

#include <polycpp/cookie/cookie.hpp>

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>

#include <polycpp/core/date.hpp>
#include <polycpp/core/error.hpp>
#include <polycpp/core/number.hpp>
#include <polycpp/core/uri.hpp>

// Pull in detail implementations for the polycpp types we use
#include <polycpp/core/detail/date/date.hpp>
#include <polycpp/core/detail/error/error.hpp>
#include <polycpp/core/detail/number/number.hpp>
#include <polycpp/core/detail/uri/uri.hpp>

namespace polycpp {
namespace cookie {
namespace detail {

// ============================================================================
// Character validation helpers
// ============================================================================

/**
 * @brief Check if a character is valid in a cookie name.
 *
 * Cookie name uses an extended range: [\u0021-\u003A\u003C\u003E-\u007E]
 * This is: ! through : (0x21-0x3A), < (0x3C), > through ~ (0x3E-0x7E).
 * Notably excludes: control chars, space, DEL, semicolon (;=0x3B), equals (==0x3D).
 */
inline bool isCookieNameChar(unsigned char c) {
    return (c >= 0x21 && c <= 0x3A) || c == 0x3C || (c >= 0x3E && c <= 0x7E);
}

/**
 * @brief Validate a cookie name string.
 * Must be non-empty and consist entirely of valid cookie name characters.
 */
inline bool isValidCookieName(const std::string& name) {
    if (name.empty()) return false;
    for (unsigned char c : name) {
        if (!isCookieNameChar(c)) return false;
    }
    return true;
}

/**
 * @brief Check if a character is valid in a cookie value.
 *
 * Cookie value range: [\u0021-\u003A\u003C-\u007E]
 * Same as name range but also allows = (0x3D).
 * Actually: 0x21 through 0x3A, 0x3C through 0x7E.
 */
inline bool isCookieValueChar(unsigned char c) {
    return (c >= 0x21 && c <= 0x3A) || (c >= 0x3C && c <= 0x7E);
}

/**
 * @brief Validate a cookie value string (after encoding).
 */
inline bool isValidCookieValue(const std::string& value) {
    for (unsigned char c : value) {
        if (!isCookieValueChar(c)) return false;
    }
    return true;
}

/**
 * @brief Validate a domain value per RFC 6265 / RFC 1034.
 *
 * Pattern: ([.]?[a-z0-9]([a-z0-9-]{0,61}[a-z0-9])?)([.][a-z0-9]([a-z0-9-]{0,61}[a-z0-9])?)*
 * Case-insensitive.
 */
inline bool isValidDomain(const std::string& domain) {
    if (domain.empty()) return false;

    size_t i = 0;
    size_t len = domain.size();

    // Optional leading dot
    if (domain[0] == '.') {
        i = 1;
    }

    // Must have at least one label after optional dot
    if (i >= len) return false;

    // Parse labels separated by '.'
    bool firstLabel = true;
    while (i < len) {
        if (!firstLabel) {
            // Expect a dot separator
            if (domain[i] != '.') return false;
            i++;
            if (i >= len) return false; // Trailing dot - invalid
        }
        firstLabel = false;

        // First char of label must be alphanumeric
        unsigned char c = static_cast<unsigned char>(domain[i]);
        if (!std::isalnum(c)) return false;

        size_t labelStart = i;
        i++;

        // Rest of label: alphanumeric or hyphen, max 63 chars total
        while (i < len && domain[i] != '.') {
            c = static_cast<unsigned char>(domain[i]);
            if (!std::isalnum(c) && c != '-') return false;
            i++;
        }

        size_t labelLen = i - labelStart;
        if (labelLen > 63) return false;

        // Last char of label (if > 1 char) must be alphanumeric (not hyphen)
        if (labelLen > 1) {
            c = static_cast<unsigned char>(domain[i - 1]);
            if (!std::isalnum(c)) return false;
        }
    }

    return true;
}

/**
 * @brief Validate a path value per RFC 6265.
 *
 * Path value: [\u0020-\u003A\u003D-\u007E]*
 * Printable ASCII excluding CTLs and semicolon.
 */
inline bool isValidPath(const std::string& path) {
    for (unsigned char c : path) {
        if (!((c >= 0x20 && c <= 0x3A) || (c >= 0x3D && c <= 0x7E))) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Validate a max-age string: must match /^-?\d+$/
 */
inline bool isValidMaxAge(const std::string& val) {
    if (val.empty()) return false;
    size_t i = 0;
    if (val[0] == '-') {
        i = 1;
        if (i >= val.size()) return false; // Just "-"
    }
    for (; i < val.size(); i++) {
        if (!std::isdigit(static_cast<unsigned char>(val[i]))) return false;
    }
    return true;
}

// ============================================================================
// Parsing helpers
// ============================================================================

/**
 * @brief Find the index of ';' in str starting from min, up to len.
 * Returns len if not found.
 */
inline size_t endIndex(const std::string& str, size_t min, size_t len) {
    auto pos = str.find(';', min);
    return (pos == std::string::npos) ? len : pos;
}

/**
 * @brief Find the index of '=' in str starting from min, up to max.
 * Returns std::string::npos if not found before max.
 */
inline size_t eqIndex(const std::string& str, size_t min, size_t max) {
    auto pos = str.find('=', min);
    return (pos < max) ? pos : std::string::npos;
}

/**
 * @brief Extract a trimmed value slice from str[min..max).
 * Trims leading and trailing whitespace (space and tab).
 */
inline std::string valueSlice(const std::string& str, size_t min, size_t max) {
    if (min == max) return "";

    size_t start = min;
    size_t end = max;

    // Trim leading whitespace
    while (start < end) {
        char c = str[start];
        if (c != ' ' && c != '\t') break;
        start++;
    }

    // Trim trailing whitespace
    while (end > start) {
        char c = str[end - 1];
        if (c != ' ' && c != '\t') break;
        end--;
    }

    return str.substr(start, end - start);
}

/**
 * @brief Default cookie value decoder.
 *
 * Uses decodeURIComponent with a fast-path skip when no '%' is present.
 * On decode errors, returns the raw string.
 */
inline std::string defaultDecode(const std::string& str) {
    if (str.find('%') == std::string::npos) return str;

    try {
        return polycpp::uri::decodeURIComponent(str);
    } catch (...) {
        return str;
    }
}

/**
 * @brief Default cookie value encoder.
 * Uses encodeURIComponent.
 */
inline std::string defaultEncode(const std::string& str) {
    return polycpp::uri::encodeURIComponent(str);
}

/**
 * @brief Convert a string to lowercase.
 */
inline std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

} // namespace detail

// ============================================================================
// parseCookie implementation
// ============================================================================

inline std::map<std::string, std::string> parseCookie(
    const std::string& str,
    const ParseOptions& options) {

    std::map<std::string, std::string> obj;
    const size_t len = str.size();

    // RFC 6265 sec 4.1.1: minimum valid cookie is "a=" (2 chars)
    if (len < 2) return obj;

    auto dec = options.decode ? options.decode : detail::defaultDecode;

    size_t index = 0;

    while (index < len) {
        auto eqIdx = detail::eqIndex(str, index, len);
        if (eqIdx == std::string::npos) break; // No more cookie pairs

        auto endIdx = detail::endIndex(str, index, len);

        if (eqIdx > endIdx) {
            // Backtrack on prior semicolon
            auto lastSemi = str.rfind(';', eqIdx - 1);
            index = (lastSemi == std::string::npos) ? 0 : lastSemi + 1;
            continue;
        }

        auto key = detail::valueSlice(str, index, eqIdx);

        // First-wins: only assign if key not already present
        if (obj.find(key) == obj.end()) {
            auto val = detail::valueSlice(str, eqIdx + 1, endIdx);
            obj[key] = dec(val);
        }

        index = endIdx + 1;
    }

    return obj;
}

// ============================================================================
// stringifyCookie implementation
// ============================================================================

inline std::string stringifyCookie(
    const std::map<std::string, std::string>& cookies,
    const StringifyOptions& options) {

    auto enc = options.encode ? options.encode : detail::defaultEncode;

    std::string str;
    bool first = true;

    for (const auto& [name, val] : cookies) {
        if (!detail::isValidCookieName(name)) {
            throw polycpp::TypeError("cookie name is invalid: " + name);
        }

        auto value = enc(val);

        if (!detail::isValidCookieValue(value)) {
            throw polycpp::TypeError("cookie val is invalid: " + val);
        }

        if (!first) str += "; ";
        str += name + "=" + value;
        first = false;
    }

    return str;
}

// ============================================================================
// parseSetCookie implementation
// ============================================================================

inline SetCookie parseSetCookie(
    const std::string& str,
    const ParseOptions& options) {

    auto dec = options.decode ? options.decode : detail::defaultDecode;

    const size_t len = str.size();
    const size_t firstEndIdx = detail::endIndex(str, 0, len);
    const auto firstEqIdx = detail::eqIndex(str, 0, firstEndIdx);

    SetCookie setCookie;
    if (firstEqIdx == std::string::npos) {
        setCookie.name = "";
        setCookie.value = dec(detail::valueSlice(str, 0, firstEndIdx));
    } else {
        setCookie.name = detail::valueSlice(str, 0, firstEqIdx);
        setCookie.value = dec(detail::valueSlice(str, firstEqIdx + 1, firstEndIdx));
    }

    // Parse attributes
    size_t index = firstEndIdx + 1;
    while (index < len) {
        const size_t attrEndIdx = detail::endIndex(str, index, len);
        const auto attrEqIdx = detail::eqIndex(str, index, attrEndIdx);

        auto attr = (attrEqIdx == std::string::npos)
            ? detail::valueSlice(str, index, attrEndIdx)
            : detail::valueSlice(str, index, attrEqIdx);

        std::optional<std::string> val;
        if (attrEqIdx != std::string::npos) {
            val = detail::valueSlice(str, attrEqIdx + 1, attrEndIdx);
        }

        auto attrLower = detail::toLower(attr);

        if (attrLower == "httponly") {
            setCookie.httpOnly = true;
        } else if (attrLower == "secure") {
            setCookie.secure = true;
        } else if (attrLower == "partitioned") {
            setCookie.partitioned = true;
        } else if (attrLower == "domain") {
            if (val) setCookie.domain = *val;
        } else if (attrLower == "path") {
            if (val) setCookie.path = *val;
        } else if (attrLower == "max-age") {
            if (val && detail::isValidMaxAge(*val)) {
                setCookie.maxAge = std::stoi(*val);
            }
        } else if (attrLower == "expires") {
            if (val && !val->empty()) {
                try {
                    double ms = polycpp::Date::parse(*val);
                    if (polycpp::Number::isFinite(ms)) {
                        auto tp = std::chrono::system_clock::time_point(
                            std::chrono::milliseconds(static_cast<int64_t>(ms)));
                        setCookie.expires = tp;
                    }
                } catch (...) {
                    // Invalid date - ignore
                }
            }
        } else if (attrLower == "priority") {
            if (val && !val->empty()) {
                auto pLower = detail::toLower(*val);
                if (pLower == "low" || pLower == "medium" || pLower == "high") {
                    setCookie.priority = pLower;
                }
            }
        } else if (attrLower == "samesite") {
            if (val && !val->empty()) {
                auto ssLower = detail::toLower(*val);
                if (ssLower == "lax" || ssLower == "strict" || ssLower == "none") {
                    setCookie.sameSite = ssLower;
                }
            }
        }

        index = attrEndIdx + 1;
    }

    return setCookie;
}

// ============================================================================
// stringifySetCookie implementations
// ============================================================================

namespace detail {

/**
 * @brief Core stringifySetCookie implementation used by both overloads.
 */
inline std::string stringifySetCookieImpl(
    const SetCookie& cookie,
    const std::function<std::string(const std::string&)>& enc) {

    if (!isValidCookieName(cookie.name)) {
        throw polycpp::TypeError("argument name is invalid: " + cookie.name);
    }

    std::string value = cookie.value.empty() ? "" : enc(cookie.value);

    if (!isValidCookieValue(value)) {
        throw polycpp::TypeError("argument val is invalid: " + cookie.value);
    }

    std::string str = cookie.name + "=" + value;

    // Max-Age
    if (cookie.maxAge.has_value()) {
        str += "; Max-Age=" + std::to_string(*cookie.maxAge);
    }

    // Domain
    if (cookie.domain.has_value() && !cookie.domain->empty()) {
        if (!isValidDomain(*cookie.domain)) {
            throw polycpp::TypeError("option domain is invalid: " + *cookie.domain);
        }
        str += "; Domain=" + *cookie.domain;
    }

    // Path
    if (cookie.path.has_value() && !cookie.path->empty()) {
        if (!isValidPath(*cookie.path)) {
            throw polycpp::TypeError("option path is invalid: " + *cookie.path);
        }
        str += "; Path=" + *cookie.path;
    }

    // Expires
    if (cookie.expires.has_value()) {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            cookie.expires->time_since_epoch()).count();
        polycpp::Date date(static_cast<double>(ms));
        auto utcStr = date.toUTCString();
        if (utcStr == "Invalid Date") {
            throw polycpp::TypeError("option expires is invalid");
        }
        str += "; Expires=" + utcStr;
    }

    // HttpOnly
    if (cookie.httpOnly) {
        str += "; HttpOnly";
    }

    // Secure
    if (cookie.secure) {
        str += "; Secure";
    }

    // Partitioned
    if (cookie.partitioned) {
        str += "; Partitioned";
    }

    // Priority
    if (cookie.priority.has_value()) {
        auto pLower = toLower(*cookie.priority);
        if (pLower == "low") {
            str += "; Priority=Low";
        } else if (pLower == "medium") {
            str += "; Priority=Medium";
        } else if (pLower == "high") {
            str += "; Priority=High";
        } else {
            throw polycpp::TypeError("option priority is invalid: " + *cookie.priority);
        }
    }

    // SameSite
    if (cookie.sameSite.has_value()) {
        auto ssLower = toLower(*cookie.sameSite);
        if (ssLower == "strict") {
            str += "; SameSite=Strict";
        } else if (ssLower == "lax") {
            str += "; SameSite=Lax";
        } else if (ssLower == "none") {
            str += "; SameSite=None";
        } else {
            throw polycpp::TypeError("option sameSite is invalid: " + *cookie.sameSite);
        }
    }

    return str;
}

} // namespace detail

inline std::string stringifySetCookie(
    const SetCookie& cookie,
    const StringifyOptions& options) {

    auto enc = options.encode ? options.encode : detail::defaultEncode;
    return detail::stringifySetCookieImpl(cookie, enc);
}

inline std::string stringifySetCookie(
    const std::string& name,
    const std::string& value,
    const SerializeOptions& options) {

    SetCookie cookie;
    cookie.name = name;
    cookie.value = value;
    cookie.maxAge = options.maxAge;
    cookie.expires = options.expires;
    cookie.domain = options.domain;
    cookie.path = options.path;
    cookie.httpOnly = options.httpOnly;
    cookie.secure = options.secure;
    cookie.partitioned = options.partitioned;
    cookie.priority = options.priority;
    cookie.sameSite = options.sameSite;

    auto enc = options.encode ? options.encode : detail::defaultEncode;
    return detail::stringifySetCookieImpl(cookie, enc);
}

} // namespace cookie
} // namespace polycpp
