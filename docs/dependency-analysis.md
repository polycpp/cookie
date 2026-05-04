# Dependency and JavaScript API Analysis

- package: cookie
- package version: 1.1.1
- package root: `<repo>/.tmp/upstream/cookie`
- analyzer json: `.tmp/dependency-analysis.json`
- published npm artifact path: `<repo>/.tmp/npm-package`
- published npm artifact analyzed: yes; `dist/index.js` and `dist/index.d.ts` were manually inspected, and analyzer was attempted against the artifact before rerunning against release TypeScript source for source-level signals.
- include dev dependencies: no
- dependency source install used: npm production install with scripts disabled by the analyzer.
- companion root checked: `<companion workspace>`

## Package entry metadata

- main: `dist/index.js`
- module: none
- types: `dist/index.d.ts`
- exports: none
- bin: none
- missing declared entries in repo clone: manual source check found `dist/index.js` and `dist/index.d.ts` absent from the Git tag; the published npm artifact supplies both.
- TypeScript source files detected: 7 in the release tag.
- TypeScript declarations reviewed: `dist/index.d.ts` from the npm artifact.
- declaration-source decision: use the shipped `dist/index.d.ts` as public API evidence and `src/index.ts` as implementation behavior evidence.
- source-vs-published artifact decision: published artifact is the consumer runtime source of truth; release-tag TypeScript source is used for analyzer signals and upstream tests.

## Direct dependencies

- none detected

## Dependency ownership decisions

No direct runtime dependencies exist, so no companion dependency or private helper is required.

| Package | Kind | Requested | Installed | License | License evidence | License impact | License strategy | Affects repo license | Deps | Source files | Node API calls | JS API calls | Recommendation | Rationale |
|---|---|---|---|---|---|---|---|---|---:|---:|---:|---:|---|---|

## License impact summary

- upstream package license: MIT.
- repo license decision: keep MIT for the C++ companion.
- GPL/AGPL dependencies: none.
- LGPL/MPL dependencies: none.
- permissive dependencies requiring notices: upstream `cookie` MIT notice is already recorded in `THIRD_PARTY_LICENSES.md`.
- dev/test-only dependencies excluded from shipped artifacts: upstream dev dependencies are not linked or vendored.
- dependency license notices to add to `THIRD_PARTY_LICENSES.md`: none beyond the existing upstream cookie MIT notice.

## Transitive dependency summary

- none for runtime dependencies.

## Runtime API usage

### Target package

- entry points analyzed: `src/index.ts` by analyzer; `dist/index.js` and `dist/index.d.ts` manually inspected from the npm artifact.
- source files analyzed by analyzer: 1.
- source files manually inspected: `src/index.ts`, `dist/index.js`, `dist/index.d.ts`, `src/parse-cookie.spec.ts`, `src/stringify-cookie.spec.ts`, `src/parse-set-cookie.spec.ts`, and `src/stringify-set-cookie.spec.ts`.
- external imports seen from target: none in runtime source.

### Analyzer porting gates

- polycpp reuse hints consumed: none emitted; manual review selected `polycpp::uri`, `polycpp::Date`, `polycpp::Number`, and `polycpp::TypeError`.
- Node parity hints consumed: none emitted; manual Node parity audit recorded in `docs/research.md`.
- security hints consumed: analyzer reports `securitySensitive: false`; manual review treats header syntax validation as a bounded misuse-test surface.
- security-sensitive package: no for auth/crypto purposes; yes for malformed untrusted header inputs.
- polycpp capability snapshot consumed: 75bc07dfca6ac0aaca07c8748476246e8c18df74 on 2026-05-04.
- transport/listener capability hints consumed: no analyzer hints; manual review found TCP, IPC/path, native-handle, TLS, HTTP, and HTTPS listener primitives but no cookie listener surface.

### Node.js API usage

- none in the runtime entry point.
- upstream benchmark regeneration script uses `https` and `process`, but that script is not part of the package runtime.

### Node parity surface usage

- callbacks: synchronous encode/decode hooks only; mapped to `std::function`.
- Promise APIs: none.
- EventEmitter APIs: none.
- server/listener APIs: none.
- diagnostic/tracing APIs: none.
- streams: none.
- Buffer and binary data: no public runtime surface; upstream tests use `Buffer` inside custom callbacks only.
- URL/timer/process/filesystem APIs: none in runtime.
- crypto/compression/TLS/network/HTTP APIs: none in runtime.

### JavaScript API usage

- Analyzer reported `TypeError` (10), `Number.isFinite` (2), `Date` (1), `decodeURIComponent` (1), `Number.isInteger` (1), `Object.create` (1), and `Object.keys` (1).
- C++ mapping uses `polycpp::TypeError`, `polycpp::Number::isFinite`, `polycpp::Date`, `polycpp::uri::decodeURIComponent`, typed integer fields, and `std::map` iteration.
- `Object.create(null)` and JavaScript object key behavior are omitted/adapted in C++.

### Framework object boundary usage

- analyzer-reported target-package framework object accesses: none.
- analyzer-reported dependency framework object accesses: none.
- manual review decision: expose core pure functions over strings, maps, and typed structs, plus a separate optional adapter header for `polycpp::http::Headers`.

## Porting decisions

- Runtime dependencies: none; no companion dependency required.
- Published artifact: inspect `dist/` for shipped JS and declarations; source clone remains useful for tests and analyzer signals.
- Error mapping: upstream `TypeError` maps to `polycpp::TypeError` for serializer validation.
- Date mapping: upstream `Date` maps to `std::chrono::system_clock::time_point` publicly and `polycpp::Date` internally.
- JavaScript object model: null-prototype object and `undefined` values are intentionally not exposed as C++ runtime shapes.
- Request/response integration: not part of v0 because upstream accepts raw header strings rather than framework objects.

Each porting decision is consistent with the ecosystem reuse decisions recorded
in `docs/research.md`.

## Analyzer warnings

- Published artifact analyzer attempt emitted `cookie: no entry points found for cookie`; response: manually inspected `dist/index.js` and `dist/index.d.ts`, then reran analyzer on the `v1.1.1` release source to consume TypeScript source signals.
- Final `.tmp/dependency-analysis.json` over the release source emitted no warnings.
