# Research

- package: cookie
- npm url: https://www.npmjs.com/package/cookie
- source url: https://github.com/jshttp/cookie.git
- upstream version basis: 1.1.1
- upstream revision analyzed: 1b89eec4e33256ac31962f4c0d6e711fff478803 (tag v1.1.1)
- upstream default branch: master
- license: MIT
- license evidence: package.json license field and upstream LICENSE text confirm MIT
- category: foundational HTTP utility parser/serializer

## Package purpose

`cookie` parses HTTP request `Cookie` headers and serializes both request-side
`Cookie` headers and response-side `Set-Cookie` headers. It is small, synchronous
compatibility infrastructure used by HTTP frameworks and middleware.

## Runtime assumptions

- browser: no direct browser runtime API; output is consumed by browsers through HTTP headers.
- node.js: upstream package targets Node.js >=18 but the public API is synchronous string processing.
- filesystem: none for runtime.
- network: none for runtime; callers own HTTP transport.
- crypto: none for runtime.
- terminal: none for runtime.

## Dependency summary

- package.json present: yes.
- package main: `dist/index.js` in the npm artifact.
- package types: `dist/index.d.ts` in the npm artifact.
- hard dependencies: none detected in package.json.
- peer dependencies: none detected in package.json.
- optional dependencies: none detected in package.json.
- package exports: none.
- package bin: none.
- dependency analysis report: `docs/dependency-analysis.md`.

## Upstream repo layout summary

- Clone path used for analysis: `<repo>/.tmp/upstream/cookie`.
- Published npm artifact used for runtime shape: `<repo>/.tmp/npm-package`.
- The release tag contains TypeScript source under `src/`, tests and benchmark files under `src/*.spec.ts` and `src/*.bench.ts`, and benchmark fixture JSON under `scripts/`.
- The published artifact contains `dist/index.js`, `dist/index.d.ts`, `README.md`, `LICENSE`, and `package.json`.
- The npm artifact was inspected because Git source does not ship `dist/`, while package metadata points consumers at `dist/index.js` and `dist/index.d.ts`.

## Entry points used by consumers

- `dist/index.js` in the npm artifact.
- `dist/index.d.ts` declaration source.
- TypeScript source reviewed: `src/index.ts`.
- TypeScript declarations inspected: `dist/index.d.ts` from the published npm artifact.

## Important files and why they matter

- `src/index.ts`: authoritative source for parser/serializer behavior, validation regular expressions, overloads, aliases, and default encode/decode behavior.
- `dist/index.js`: published runtime artifact consumed by CommonJS users.
- `dist/index.d.ts`: shipped public type contract; exposes `ParseOptions`, `StringifyOptions`, `SetCookie`, `SerializeOptions`, `Cookies`, and alias exports.
- `src/parse-cookie.spec.ts`: request-cookie parse compatibility cases, including duplicate names and decode fallback.
- `src/stringify-cookie.spec.ts`: request-cookie serialization cases, including `undefined` values and custom encoder errors.
- `src/parse-set-cookie.spec.ts`: response-cookie parse compatibility cases and real-world top-site snapshots.
- `src/stringify-set-cookie.spec.ts`: response-cookie serialization, validation, and attribute cases.
- `scripts/top-cookie.json` and `scripts/top-set-cookie.json`: upstream real-world fixture sources used by upstream snapshot tests.

## Files likely irrelevant to the C++ port

- `src/*.bench.ts` and `benchmarks/value-slice.bench.ts`: performance harnesses, not public API.
- `scripts/update-benchmark.js`: fixture regeneration and benchmark support that uses `https` and `process`; not runtime package behavior.
- `tsconfig*.json`, package scripts, size-limit metadata, and dev tooling entries: build infrastructure for npm, not C++ behavior.

## Test directories worth mining first

- `src/parse-cookie.spec.ts`: high-value request `Cookie` parser behavior.
- `src/stringify-cookie.spec.ts`: request `Cookie` serialization behavior.
- `src/parse-set-cookie.spec.ts`: `Set-Cookie` parser behavior and real-world fixture snapshots.
- `src/stringify-set-cookie.spec.ts`: `Set-Cookie` serializer validation behavior.
- `scripts/top-cookie.json` and `scripts/top-set-cookie.json`: larger compatibility corpus to add if parity coverage is expanded.

## Implementation risks discovered from the source layout

- Git source lacks `dist/`, so analyzing only the clone misses the shipped runtime and declaration files.
- Upstream `Cookies` values may be `undefined`; the current C++ map cannot represent that JavaScript value.
- Upstream `sameSite` accepts boolean `true` and `false`; the current C++ API accepts only validated strings.
- Upstream parser accepts syntactically numeric `Max-Age` values through JavaScript `Number`; the current C++ implementation narrows to `int`.
- The existing docs and examples previously referenced a nonexistent C++ `SameSite` enum; catch-up docs were updated to the actual string-based API.

## Companion repo alignment

- companion repos inspected: local `ini`, `content-type`, `qs`, and `yaml` clones, plus the shared companion pattern guide.
- CMake target and alias pattern: `polycpp_cookie` and `polycpp::cookie`, aligned with small utility companions.
- public header layout: `include/polycpp/cookie/cookie.hpp` is the public header; no top-level `include/polycpp/cookie.hpp` aggregator exists.
- detail/private header strategy: implementation split lives under `include/polycpp/cookie/detail/`; the public header includes the detail implementation after declarations so documented public-header users link successfully.
- aggregator header strategy: `include/polycpp/cookie/detail/aggregator.hpp` is retained as an internal include point for the compiled target and tests.
- examples strategy: two public-header examples are present and wired behind `POLYCPP_COOKIE_BUILD_EXAMPLES`.
- documentation site strategy: Doxygen/Breathe/Sphinx site exists; catch-up added `docs/build.py` and updated the workflow to use it.
- deliberate deviations from existing companions: no public top-level aggregator header; request-cookie output uses `std::map`, which imposes sorted order.

## Polycpp ecosystem reuse analysis

- polycpp core paths inspected: `<polycpp checkout>/include/polycpp`, especially `http/headers.hpp`, `http/http.hpp`, `https/https.hpp`, `net/net.hpp`, `tls/tls.hpp`, `io/*`, `stream/*`, `buffer/*`, `events/*`, `core/uri.hpp`, `core/date.hpp`, `core/number.hpp`, and `core/error.hpp`.
- polycpp capability snapshot: 75bc07dfca6ac0aaca07c8748476246e8c18df74 from `git -C <polycpp checkout> rev-parse HEAD` on 2026-05-04.
- transport/listener capability review: base polycpp provides `io::TcpAcceptor`, `io::PipeAcceptor`, `io::StreamAcceptor`, `net::Server` with TCP/path/native-handle listen modes, `tls::Server`, `tls::createServer`, `http::Server`, and `https::Server`; cookie does not expose listener behavior.
- polycpp core types/functions selected: `polycpp::uri::encodeURIComponent`, `polycpp::uri::decodeURIComponent`, `polycpp::Date::parse`, `polycpp::Date::toUTCString`, `polycpp::Number::isFinite`, and `polycpp::TypeError`.
- polycpp core types/functions rejected: `polycpp::http::Headers` is not used in v0 because upstream operates on raw header strings and does not mutate request/response objects; `polycpp::Buffer`, streams, events, timers, crypto, network, TLS, and HTTP server/client primitives are not runtime surfaces for this package.
- public polycpp interop review: public APIs remain pure string and typed aggregate functions so they can be used with any HTTP stack; callers can pass results to `polycpp::http::Headers` themselves.
- string policy: `std::string` is selected because cookie values are byte-oriented HTTP header text after caller-selected encoding; JavaScript UTF-16 code-unit semantics are not part of the public contract.
- JsonValue/Object/Array policy: no JSON interop is exposed because schemas are fixed and better represented by maps and structs.
- Date/time interop policy: public expiration uses `std::chrono::system_clock::time_point`; implementation reuses `polycpp::Date` for parse/UTC string parity.
- diagnostic/config object policy: option structs do not need `toObject()` or `toJSON()` adapters because they are not diagnostic payloads.
- toJSON/stringify policy: direct `polycpp::JSON::stringify` support is not expected for `SetCookie` or option structs in v0.
- companion libs inspected for reusable APIs: `content-type` and `qs` for parser/serializer API style, `ini` for small utility layout, `yaml` for API-rich header organization.
- companion libs selected for reuse: none.
- companion libs rejected or deferred: `content-type`, `qs`, `ini`, and `yaml` are convention references only; they do not own cookie parsing behavior.
- new local abstractions introduced: `ParseOptions`, `StringifyOptions`, `SerializeOptions`, and `SetCookie` are package-specific typed equivalents of upstream option/object shapes and do not overlap with base polycpp platform abstractions.
- reuse risks or integration gaps: the public API does not provide direct `polycpp::http::Headers` adapters; this is acceptable for v0 but remains a possible non-parity C++ extension. The public header now includes the detail implementation, which keeps usage simple but exposes more implementation code during compilation.

## Node parity surface audit

- callback APIs: none; encode/decode hooks are synchronous value callbacks represented as `std::function`.
- Promise APIs: none, because upstream is synchronous CPU-local parsing and formatting.
- EventEmitter APIs: none, because upstream exposes no event model.
- server/listener APIs: none, because upstream neither creates nor owns servers; base TCP, IPC/native handle, TLS, HTTP, and HTTPS listener primitives were revalidated before marking this not applicable.
- diagnostic/tracing APIs: none, because upstream exposes no diagnostics channel or trace hooks.
- stream APIs: none, because upstream processes complete header strings and exposes no stream transforms.
- Buffer and binary APIs: none in the public API; upstream tests use `Buffer` only inside a custom decode/encode callback.
- URL, timer, process, and filesystem APIs: none in runtime; benchmark regeneration script uses `process` and `https` outside package runtime.
- crypto, compression, TLS, network, and HTTP APIs: none in runtime; HTTP integration is caller-owned string/header usage.
- unsupported Node-specific APIs and audit reason: CommonJS module loading, null-prototype result objects, JavaScript `undefined`, and dynamic runtime value coercion are not meaningful as first-class C++ APIs.

## External SDK and native driver strategy

- upstream external services/protocols: not applicable because this package does not connect to an external service or implement a wire protocol.
- native SDKs/client libraries to use: not applicable because no external SDK boundary exists.
- SDKs/protocols explicitly not reimplemented: not applicable because cookie parsing/serialization is the package behavior.
- adapter/linking strategy: no SDK adapter; CMake links only `polycpp`.
- test environment needs: local CMake, GoogleTest, Doxygen, and Sphinx only.

## Compatibility foundation review

- downstream dependency role: foundational HTTP helper package used by web middleware and frameworks.
- native substitution risk: high enough to avoid substituting a generic cookie library; package-specific validation and option defaults are the compatibility contract.
- upstream implementation data to preserve: validation character ranges, decode fallback, duplicate first-wins behavior, attribute ordering, and parse/serialize option semantics.
- generated or vendored data plan: no runtime generated data is required; top-site JSON fixtures are upstream test inputs only and are not committed in v0.
- compatibility fixture strategy: existing `tests/test_cookie.cpp` ports representative upstream spec cases; future expansion should import top-site fixture snapshots into grouped compatibility tests.

## Security and fail-closed review

- security-sensitive behavior: user-controlled HTTP header parsing and header emission; no auth, crypto, or credential verification.
- trust boundary: callers may pass untrusted header strings and untrusted cookie names, values, domains, or paths.
- supported protocol or algorithm matrix: not applicable because the package has no protocol or algorithm negotiation.
- unsupported behavior and fail-closed policy: unsupported JavaScript runtime values are omitted at compile time; invalid serialized names, values, domains, paths, priority, and sameSite values throw `polycpp::TypeError`.
- result-set/framing drain policy, if protocol client: not applicable because there is no protocol client.
- binary payload type-mapping policy, if protocol client: not applicable because there is no protocol client or binary payload surface.
- stateful parser/session-state policy, if protocol client/server: not applicable because parsing is stateless.
- server/listener response writer matrix, if protocol server surface exists: not applicable because no server/listener surface exists.
- key, secret, credential, or user-controlled input handling: cookie values are opaque strings; callers own secret storage and signing. The library validates serialized header syntax but does not authenticate contents.
- misuse cases that must be tested: malformed percent escapes, duplicate names, invalid serialized names/values/domains/paths, invalid priority and sameSite values, and oversized `Max-Age` follow-up from the audit table.

## Core use cases

- Parse request `Cookie` header strings into name/value pairs.
- Serialize request cookie maps into `Cookie` header strings.
- Parse response `Set-Cookie` header strings into typed `SetCookie` structs.
- Serialize `SetCookie` structs or name/value/options triples into response header strings.
- Customize cookie value encoding and decoding.

## Key features to port first

- Already implemented in v0: parse, serialize, set-cookie parse/stringify, aliases, attribute validation, encode/decode hooks, and representative upstream compatibility tests.

## Features to defer

- none for public upstream v0 surfaces; larger top-site fixture coverage and benchmarks can be expanded later as testing or performance follow-up.

## Non-parity extension candidates

- Optional helpers that read/write `Set-Cookie` through `polycpp::http::Headers`.
- Optional strongly typed `SameSite` and `Priority` enum wrappers while retaining string compatibility.
- Larger fixture-driven compatibility corpus generated from upstream top-site JSON snapshots.

## v0 scope

- port version: 0.1.0
- versioning note: port version is independent from upstream versioning.
- supported APIs: `parseCookie`, `stringifyCookie`, `parseSetCookie`, `stringifySetCookie`, `parse`, `serialize`, `ParseOptions`, `StringifyOptions`, `SerializeOptions`, and `SetCookie`.
- unsupported APIs: JavaScript `undefined` cookie values, null-prototype object semantics, boolean `sameSite` shorthand, arbitrary runtime coercion beyond typed C++ parameters, CommonJS loader behavior, and benchmarks.
- dependency plan: no runtime npm dependencies; link only base `polycpp`.
- polycpp modules to use: `core/uri`, `core/date`, `core/number`, and `core/error`.
- missing polycpp primitives: none for v0; no socket, listener, stream, Buffer, timer, crypto, TLS, network, or HTTP primitive is required by the upstream public runtime API.
