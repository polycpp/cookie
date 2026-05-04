# polycpp/cookie

A C++ port of [cookie](https://github.com/jshttp/cookie) for the [polycpp](https://github.com/enricohuang/polycpp) ecosystem.

HTTP cookie parsing and serialization for C++20, implementing RFC 6265.

Port version: `0.1.0`

Initial port based on upstream version: `1.1.1`

## Status

Implemented:

- `parseCookie` / `parse` for request `Cookie` headers
- `stringifyCookie` for request `Cookie` headers
- `parseSetCookie` for response `Set-Cookie` headers
- `stringifySetCookie` / `serialize` for response `Set-Cookie` headers
- `Max-Age`, `Expires`, `Domain`, `Path`, `HttpOnly`, `Secure`, `Partitioned`, `Priority`, and `SameSite`
- Custom value encode/decode callbacks

Known differences from upstream:

- Cookie objects use `std::map<std::string, std::string>`, so request-cookie serialization is key-sorted and cannot represent JavaScript `undefined` values.
- `sameSite` is represented as the strings `"strict"`, `"lax"`, or `"none"`; the JavaScript boolean shorthand is omitted.
- JavaScript package-loader details and null-prototype object behavior are not part of the C++ API.

## Prerequisites

- CMake 3.20+
- GCC 13+ or Clang 16+
- C++20 support required

## Building

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DPOLYCPP_COOKIE_BUILD_TESTS=ON
cmake --build build -j$(nproc)
```

## Running Tests

```bash
cd build && ctest --output-on-failure
```

## Usage

```cpp
#include <polycpp/cookie/cookie.hpp>

// Parse a Cookie header
auto cookies = polycpp::cookie::parseCookie("foo=bar; baz=qux");
// cookies["foo"] == "bar"
// cookies["baz"] == "qux"

// Serialize a Set-Cookie header
polycpp::cookie::SetCookie sc;
sc.name = "session";
sc.value = "abc123";
sc.httpOnly = true;
sc.secure = true;
sc.path = "/";
sc.sameSite = "strict";
auto header = polycpp::cookie::stringifySetCookie(sc);
// header == "session=abc123; Path=/; HttpOnly; Secure; SameSite=Strict"

// Stringify a Cookie request header
std::map<std::string, std::string> jar = {{"foo", "bar"}, {"baz", "qux"}};
auto cookieHeader = polycpp::cookie::stringifyCookie(jar);
// cookieHeader == "baz=qux; foo=bar"
```

## API

### `parseCookie(str, options?)` - Parse Cookie Header

Parses an HTTP `Cookie` header string into a map of name-value pairs.

### `stringifyCookie(cookies, options?)` - Stringify Cookie Header

Serializes a map of name-value pairs into an HTTP `Cookie` header string.

### `parseSetCookie(str, options?)` - Parse Set-Cookie Header

Parses an HTTP `Set-Cookie` header string into a `SetCookie` struct.

### `stringifySetCookie(cookie, options?)` - Stringify Set-Cookie Header

Serializes a `SetCookie` struct (or name + value + options) into an HTTP `Set-Cookie` header string.

## License

MIT License. See [LICENSE](LICENSE) for details.
