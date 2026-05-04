# Divergences From Upstream

## Deferred Features

- none for the current public v0 API; broader top-site fixture coverage and benchmark parity are test/performance follow-ups rather than missing runtime API.

## Deliberate Behavior Changes

- `stringifyCookie` accepts `std::map<std::string, std::string>`, so output is key-sorted instead of JavaScript insertion order.
- JavaScript `undefined` cookie values are omitted from the C++ API because the typed map stores only strings.
- `sameSite` is represented as validated strings (`"strict"`, `"lax"`, `"none"`); upstream's boolean `true`/`false` shorthand is omitted.
- JavaScript object-prototype behavior from `Object.create(null)` is not modeled because C++ maps have no prototype chain.
- Serializer overloads accept typed C++ strings and structs instead of arbitrary JavaScript values coerced at runtime.

## Unsupported Runtime-Specific Features

- CommonJS module loading and npm package metadata are replaced by the CMake target `polycpp::cookie`.
- Upstream benchmark scripts and fixture-regeneration scripts are not shipped as C++ runtime APIs.
- Node-only `Buffer` usage inside upstream tests is not a cookie API surface; C++ callers can implement equivalent encode/decode callbacks.

## Audit findings (libgen catch-up)

| ID | Severity | Location | Description | Recommended classification |
|---|---|---|---|---|
| AF-2026-05-04-A | medium | `include/polycpp/cookie/detail/cookie.hpp:381` | `parseSetCookie` calls `std::stoi` for syntactically valid `Max-Age` strings; an oversized integer can throw instead of preserving upstream's tolerant parse behavior. | bug-fix-needed with regression test for oversized `Max-Age` |
