# infinity (Rust rewrite)

The Rust re-implementation of the Infinity engine, per [`../RUST_REWRITE_PLAN.md`](../RUST_REWRITE_PLAN.md).
This is a **clean-room rewrite** built milestone by milestone; the C++ engine
remains the reference oracle. Goals, methodology and the current gap list are in
[`docs/PORTING.md`](docs/PORTING.md).

## Layout (§4.1)

| crate | role | status |
|---|---|---|
| `infinity-common` | base types, errors | ✅ initial |
| `infinity-storage` | WAL + MVCC + `MetaStore`(RocksDB) | ✅ M1 kernel |
| `infinity-catalog` | metadata over `MetaStore` | ✅ minimal |
| `infinity-txn` | commit pipeline (WAL → KV) + recovery + concurrent-commit & crash-injection tests + **loom** model | ✅ minimal |
| `infinity-parser` | hand-written lexer + recursive-descent SQL (`CREATE/DROP/INSERT` (**multi-row**)/`SELECT` w/ `WHERE`/`JOIN…ON`/`GROUP BY`/`ORDER BY`/`LIMIT`, projection, aggregates) | ✅ M2 |
| `infinity-expression` | scalar `Expr`/`Value` + eval + aggregates | ✅ M2 |
| `infinity-planner` | logical plan (`Scan`/`Join`/`Filter`/`Projection`/`Aggregate`/`Sort`/`Limit`/`Values`) + optimizer (limit/predicate push-down, const folding) | ✅ M2 |
| `infinity-index` | secondary/low-cardinality index, vector (`Flat/HNSW/LVQ/RaBitQ/LSG/IVF/PLAID/SMVE/BMP`), full-text inverted index (BM25/phrase/BMW) and EMVB; wired into `CREATE INDEX` and the scan/fusion paths | ✅ M3 |
| `infinity-executor` | `Engine` + **Tokio per-edge-concurrent bounded pipeline** (join/aggregate/sort blocking) + `DELETE`/`UPDATE` (tombstones; update = delete+append) | ✅ minimal |
| `infinity-cli` | `infinity` binary (smoke/query/bench/import-csv/**conformance**/driver) | ✅ |
| `infinity-server` | Thrift `TBinaryProtocol` front-end (dispatcher, `Connect`/`Disconnect`/`ListDatabase`/`ShowTable`, columnar `SelectResponse`, `CreateTable`/`Insert`/`Delete`/`Update`/`Select` w/ projection+`WHERE`+`ORDER BY`+`LIMIT`); stateful TCP server; HTTP seed (`POST /query`); **PG wire** (startup + simple query); cross-verified by the authoritative Python codec | ✅ M4 |

## What is implemented (M0/M1)

- **WAL** — CRC-32/IEEE framed append log (`magic|size|checksum|txn_id|commit_ts|payload`)
  with torn-tail and checksum tolerance, `replay_since`, and checkpoint truncation.
- **MVCC** — timestamp-array `BlockVersion` (`created`/`deleted`), update = delete + append.
- **MetaStore** — trait + RocksDB impl (atomic batch, prefix scan, flush); the substrate the reference uses.
- **Catalog** — tables with persisted monotonic ids + name index.
- **Txn** — `WriteWAL(serial) → KV atomic write`, idempotent WAL replay on recovery.

## Build & test

```bash
cd rust_engine
cargo test --workspace
cargo run -p infinity-cli -- smoke /tmp/infinity-smoke
```

> The toolchain is pinned in `rust-toolchain.toml`: a dated **nightly**, because
> the posting codec decodes fastpfor's SIMD-BP128 blocks with `std::simd`
> (portable SIMD is still nightly-only; `std::arch` would need `unsafe`).
>
> The workspace `.cargo/config.toml` routes crates.io through a local proxy for
> environments without direct egress.

## Non-goals in this milestone

Query parsing/planning/execution, indexes, and the Thrift/HTTP/PG frontends are
tracked by milestones M2–M4 in the plan and are not yet implemented.
