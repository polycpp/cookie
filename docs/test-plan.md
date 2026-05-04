# Test Plan

## Unit tests

- Request `Cookie` parsing: empty input, whitespace trimming, percent-decoding, decode fallback, duplicate first-wins behavior, missing-value segments, and empty values.
- Request `Cookie` serialization: key/value validation, default encoding, custom encoder, empty maps, and multi-entry output.
- `Set-Cookie` parsing: name/value splitting, unknown attributes, boolean attributes, `Max-Age`, `Expires`, `Domain`, `Path`, `Priority`, `SameSite`, and `Partitioned`.
- `Set-Cookie` serialization: validation failures, attribute output order, custom encoder, flag attributes, optional attributes, and aliases.
- Regression follow-up: oversized `Max-Age` parse should not throw and should match the chosen documented behavior.

## Integration tests

- Round-trip request cookie parse/stringify for representative maps.
- Round-trip `Set-Cookie` parse/stringify for canonical attribute combinations.
- Public-header examples under `examples/parse_header.cpp` and `examples/session_lifecycle.cpp` compile when `POLYCPP_COOKIE_BUILD_EXAMPLES=ON`.
- README usage snippets mirror public API calls covered by tests and examples.

## Compatibility tests adapted from upstream

- upstream compatibility layout: upstream has TypeScript spec files under `src/*.spec.ts`; current local coverage is aggregated in `tests/test_cookie.cpp`.
- upstream-to-local coverage map:
  - `src/parse-cookie.spec.ts` -> `tests/test_cookie.cpp` `ParseCookieTest.*` and `AliasTest.ParseAlias`.
  - `src/stringify-cookie.spec.ts` -> `tests/test_cookie.cpp` `StringifyCookieTest.*` and `RoundTripTest.ParseAndStringifyCookie`.
  - `src/parse-set-cookie.spec.ts` -> `tests/test_cookie.cpp` `ParseSetCookieTest.*`.
  - `src/stringify-set-cookie.spec.ts` -> `tests/test_cookie.cpp` `StringifySetCookieTest.*`, `AliasTest.SerializeAlias`, and `RoundTripTest.ParseAndStringifySetCookie`.
- omitted upstream cases:
  - `scripts/top-cookie.json` and `scripts/top-set-cookie.json` snapshot suites are not imported yet because v0 uses representative compatibility cases; they are a follow-up for broader fixture coverage.
  - upstream `undefined` value stringify cases are not applicable to `std::map<std::string, std::string>`.
  - upstream boolean `sameSite` shorthand cases are omitted because C++ v0 uses string values.
  - upstream invalid numeric runtime type cases for `maxAge` are compile-time-only in C++ because `SerializeOptions::maxAge` is `std::optional<int>`.

## Security and fail-closed tests

- Existing tests cover invalid names, invalid encoded values, invalid domains, invalid paths, invalid priority, invalid sameSite, malformed percent escapes, and duplicate cookie names.
- Add a regression test for oversized `Max-Age` parse behavior before resolving audit finding AF-2026-05-04-A.
- Service-backed security tests are not applicable because the package does not authenticate, encrypt, sign, or connect to external services.

## Protocol/client tests

not applicable because `cookie` is a synchronous header parser/serializer and has no database, cache, queue, cloud-service, wire-protocol, transport, auth, or listener surface.

- service-backed e2e matrix: not applicable because there is no service.
- transport matrix, including TLS/compression decisions: not applicable because callers own HTTP transport.
- auth or credential flow matrix: not applicable because cookie values are opaque strings.
- malformed packet / unsupported mode tests: not applicable because there are no packets or protocol modes.
- binary payload type-mapping tests: not applicable because the public API is string-only.
- stateful parser/session-state tests: not applicable because parsing is stateless.
- server/listener response writer loopback tests: not applicable because no listener surface exists.
- multi-result or unread-packet drain behavior: not applicable because there is no protocol client.
- pool/session lifecycle tests: not applicable because there is no connection pool or session object.

## Release-blocking behaviors

- Baseline readiness must pass after scaffold catch-up.
- Strict readiness must pass with planning docs filled and private GitHub repo verified.
- CMake configure/build and CTest must pass with local `polycpp`.
- Example targets must compile with public headers.
- `python3 docs/build.py` must pass without Sphinx warnings.
- `scripts/check-port-validation.py --run-docs-build` must pass.
- Audit finding AF-2026-05-04-A should be triaged before production-quality release claims.

## Current validation

- 2026-05-04: `python3 <libgen>/scripts/check-port-readiness.py --baseline <repo>` -> passed.
- 2026-05-04: `python3 <libgen>/scripts/check-port-readiness.py --strict <repo>` -> passed.
- 2026-05-04: `cmake -B build-libgen-catchup -G Ninja -DCMAKE_BUILD_TYPE=Debug -DFETCHCONTENT_SOURCE_DIR_POLYCPP=<polycpp checkout> -DPOLYCPP_COOKIE_BUILD_TESTS=ON -DPOLYCPP_COOKIE_BUILD_EXAMPLES=ON` -> configured successfully. CMake warned that GCC 11.4.0 has known polycpp issues and recommends GCC 12+.
- 2026-05-04: `cmake --build build-libgen-catchup --target polycpp_cookie_test_cookie polycpp_cookie_example_parse_header polycpp_cookie_example_session_lifecycle -j2` -> passed.
- 2026-05-04: `ctest --test-dir build-libgen-catchup --output-on-failure` -> 91/91 tests passed.
- 2026-05-04: `printf 'a=1; b=two%20words\n' | ./build-libgen-catchup/examples/parse_header` -> printed decoded cookie pairs.
- 2026-05-04: `./build-libgen-catchup/examples/session_lifecycle signin && ./build-libgen-catchup/examples/session_lifecycle refresh && ./build-libgen-catchup/examples/session_lifecycle signout` -> printed expected `Set-Cookie` lifecycle headers.
- 2026-05-04: `python3 docs/build.py` -> Doxygen and Sphinx completed successfully with `-W --keep-going`.
- 2026-05-04: `python3 <libgen>/scripts/check-port-validation.py --run-docs-build <repo>` -> passed.
