# API Mapping

| Upstream symbol | C++ symbol | Status | Notes |
|---|---|---|---|
| `parseCookie(str, options?)` | `polycpp::cookie::parseCookie(const std::string&, const ParseOptions&)` | direct | Preserves first-wins duplicate behavior, whitespace trimming, percent-decoding fallback, and custom decoder hook. |
| `parse` alias | `polycpp::cookie::parse` | compatibility layer | Inline alias delegates to `parseCookie` for migration familiarity. |
| `stringifyCookie(cookie, options?)` | `polycpp::cookie::stringifyCookie(const std::map<std::string, std::string>&, const StringifyOptions&)` | adapted | Serializes key/value pairs with upstream validation and encoding; `std::map` gives sorted order and cannot represent JavaScript `undefined`. |
| `parseSetCookie(str, options?)` | `polycpp::cookie::parseSetCookie(const std::string&, const ParseOptions&)` | direct | Parses name/value plus supported attributes and ignores invalid parse-only attribute values like upstream. Oversized `Max-Age` is an audit finding. |
| `stringifySetCookie(cookie, options?)` | `polycpp::cookie::stringifySetCookie(const SetCookie&, const StringifyOptions&)` | adapted | Typed `SetCookie` struct maps the upstream object overload; unsupported dynamic value shapes are compile-time omissions. |
| `stringifySetCookie(name, val, options?)` | `polycpp::cookie::stringifySetCookie(const std::string&, const std::string&, const SerializeOptions&)` | direct | Name/value/options overload mirrors the main serialize call with typed options. |
| `serialize` alias | `polycpp::cookie::serialize` | compatibility layer | Inline alias delegates to name/value `stringifySetCookie`. |
| `ParseOptions.decode` | `polycpp::cookie::ParseOptions::decode` | direct | Synchronous custom decoder callback; default uses `decodeURIComponent` and returns raw input on decode errors. |
| `StringifyOptions.encode` | `polycpp::cookie::StringifyOptions::encode` and `SerializeOptions::encode` | direct | Synchronous custom encoder callback; encoded output is validated before emission. |
| `SetCookie` interface | `polycpp::cookie::SetCookie` | adapted | Typed aggregate with `std::optional` attributes and booleans for flag attributes. |
| `SerializeOptions` type | `polycpp::cookie::SerializeOptions` | adapted | Typed aggregate equivalent of the upstream serialize options without `name` and `value`. |
| `Cookies` record with optional values | `std::map<std::string, std::string>` | adapted | C++ result/input cannot represent `undefined`; omission is documented as a behavior difference. |
| `sameSite` boolean-or-string union | `std::optional<std::string> sameSite` | adapted | String values are supported; boolean shorthand is omitted from C++ v0. |
| `Object.create(null)` result object | no public C++ equivalent | omitted | JavaScript prototype-safety behavior is not meaningful for `std::map`. |
| CommonJS package exports | CMake target `polycpp::cookie` and public header `<polycpp/cookie/cookie.hpp>` | adapted | C++ packaging follows companion conventions rather than JS module loading. |

Status values:

- `direct`: same behavior with an idiomatic C++ spelling.
- `compatibility layer`: same user-facing behavior through a different implementation shape.
- `adapted`: preserves the upstream intent with a typed C++ API.
- `deferred`: planned future work for an upstream surface or explicitly accepted C++ extension that is intentionally not implemented yet.
- `omitted`: deliberately not part of this port.

## TypeScript Declaration Review

- Declaration source used: `dist/index.d.ts` from the published `cookie@1.1.1` npm artifact.
- Public APIs, overloads, options, callbacks, streams, or literal unions found only or most clearly in declarations: `Cookies` allows `undefined` values; `sameSite` allows boolean or string literals; `priority` is the closed string union `"low" | "medium" | "high"`; `stringifySetCookie` has object and name/value overloads.
- Declaration-only globals, caches, deprecated fields, or runtime-specific surfaces mapped as unsupported/not-applicable: none beyond CommonJS/TypeScript packaging; runtime aliases `parse` and `serialize` are implemented.

## Framework object boundary review

- Upstream reads or mutates framework/request/response/context objects: none.
- Upstream fields or methods read: none.
- Upstream fields or methods written: none.
- C++ adapter boundary: pure functions over `std::string`, `std::map`, `SetCookie`, and typed option structs; HTTP request/response integration remains with callers or future adapters.
- Partial mutation risk on validation failure: none because the API does not mutate external framework objects.

## Node parity surface review

- Callback APIs: encode/decode value callbacks map to typed `std::function` fields.
- Promise APIs: none.
- EventEmitter APIs: none.
- Server/listener APIs: none; current base polycpp listener capabilities were inspected and are not needed.
- Diagnostic/tracing APIs: none.
- Stream APIs: none.
- Buffer and binary APIs: no public binary surface; callback users can encode/decode their own bytes before passing strings.
- URL, timer, process, and filesystem APIs: none in runtime.
- Crypto, compression, TLS, network, and HTTP APIs: none in runtime; caller-owned HTTP stacks consume the returned header strings.
- Unsupported or non-meaningful Node-specific APIs and audit reason: CommonJS module loading, null-prototype objects, `undefined` values, dynamic arbitrary value coercion, and benchmark/update scripts are JavaScript runtime or tooling surfaces, not C++ library APIs.
