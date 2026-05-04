#include <polycpp/cookie/cookie.hpp>

#include "../tests/fixtures/top_site_fixtures.hpp"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

namespace {

std::uint64_t g_sink = 0;

std::string genCookies(int count) {
    std::string result;
    for (int i = 0; i < count; ++i) {
        if (!result.empty()) result += "; ";
        result += "foo" + std::to_string(i) + "=bar";
    }
    return result;
}

template <typename Fn>
void bench(const std::string& name, std::size_t iterations, Fn&& fn) {
    std::uint64_t localSink = 0;
    const auto start = std::chrono::steady_clock::now();

    for (std::size_t i = 0; i < iterations; ++i) {
        localSink += static_cast<std::uint64_t>(fn());
    }

    const auto stop = std::chrono::steady_clock::now();
    const auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(
        stop - start).count();
    g_sink += localSink;

    std::cout << name << '\t'
              << (static_cast<double>(nanos) / static_cast<double>(iterations))
              << " ns/op\n";
}

std::size_t parsedSetCookieSize(std::string_view header) {
    auto parsed = polycpp::cookie::parseSetCookie(std::string(header));
    return parsed.name.size() + parsed.value.size()
        + (parsed.maxAge.has_value() ? 1u : 0u)
        + (parsed.expires.has_value() ? 1u : 0u)
        + (parsed.domain.has_value() ? 1u : 0u)
        + (parsed.path.has_value() ? 1u : 0u)
        + (parsed.httpOnly ? 1u : 0u)
        + (parsed.secure ? 1u : 0u)
        + (parsed.partitioned ? 1u : 0u)
        + (parsed.priority.has_value() ? 1u : 0u)
        + (parsed.sameSite.has_value() ? 1u : 0u);
}

} // namespace

int main(int argc, char** argv) {
    std::size_t iterations = 10000;
    if (argc > 1) {
        iterations = std::strtoull(argv[1], nullptr, 10);
        if (iterations == 0) iterations = 1;
    }

    const auto duplicates = genCookies(2) + "; " + genCookies(2);
    const auto cookies10 = genCookies(10);
    const auto cookies100 = genCookies(100);

    bench("parseCookie empty", iterations, [] {
        return polycpp::cookie::parseCookie("").size();
    });
    bench("parseCookie simple", iterations, [] {
        return polycpp::cookie::parseCookie("foo=bar").size();
    });
    bench("parseCookie decode", iterations, [] {
        return polycpp::cookie::parseCookie("foo=hello%20there!").size();
    });
    bench("parseCookie unquote", iterations, [] {
        return polycpp::cookie::parseCookie("foo=\"foo bar\"").size();
    });
    bench("parseCookie duplicates", iterations, [&] {
        return polycpp::cookie::parseCookie(duplicates).size();
    });
    bench("parseCookie 10 cookies", iterations, [&] {
        return polycpp::cookie::parseCookie(cookies10).size();
    });
    bench("parseCookie 100 cookies", iterations, [&] {
        return polycpp::cookie::parseCookie(cookies100).size();
    });

    for (const auto& fixture : polycpp_cookie_test::kTopCookieFixtures) {
        bench("parseCookie top " + std::string(fixture.domain), iterations, [&] {
            return polycpp::cookie::parseCookie(std::string(fixture.header)).size();
        });
    }

    for (const auto& fixture : polycpp_cookie_test::kTopSetCookieFixtures) {
        bench("parseSetCookie top " + std::string(fixture.domain), iterations, [&] {
            return parsedSetCookieSize(fixture.header);
        });
    }

    if (g_sink == 0xFFFFFFFFFFFFFFFFull) {
        std::cerr << "unreachable sink value\n";
    }
}
