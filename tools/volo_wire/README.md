# volo_wire

Wire-parity harness: a [volo-thrift](https://github.com/cloudwego/volo) server
that answers Select with canned values, so the reply can be compared byte for
byte with the hand-written Thrift codec in rust_engine/crates/server.

The point is protocol compatibility, not features: both the go and python SDKs
use TBufferedTransport (unframed binary Thrift), while volo's default codec is
TTHeader<Framed<Binary>>. src/main.rs therefore installs
DefaultMakeCodec::new(MakeThriftCodec::new()).

volo.yml points at the repository's unmodified thrift/infinity.thrift; the 17
reference fields need the pilota fork branch pinned in Cargo.toml
(yuzhichang/pilota, branch fix/thrift-reference-field, upstream PR
cloudwego/pilota#379).

## Run

    cargo build                                          # in this directory
    uv run pytest tools/volo_wire/test_wire_parity.py    # from the repo root

The test starts a real infinity server, creates each table through the python
SDK, replays the same SelectRequest to both servers and asserts the replies
are identical. Volo encodes a ColumnField's column_vectors as opaque binary,
so what this checks is the envelope: column_type / column_name / bitmasks
encoding, struct field order, and the empty-vs-absent optional fields.

Cases are listed in CASES in the test and must match the arms of
select_response() in src/svc.rs:

| VOLO_WIRE_CASE | columns |
|---|---|
| int_varchar | INT, VARCHAR, last row NULL |
| int_varchar_no_null | INT, VARCHAR, no NULL (no bitmask list) |
| float_embedding | INT, FLOAT, EMBEDDING(FLOAT,4) |
| double_timestamp | INT, DOUBLE, TIMESTAMP |
| date_time_json | INT, DATE, TIME, DATETIME, JSON |
| array_int | INT, ARRAY(INT) |
| sparse_float | INT, SPARSE(1000,FLOAT,INT32) |
| tensor_float | INT, TENSOR(FLOAT,4) |
| multivector_float | INT, MULTIVECTOR(FLOAT,4) |

VOLO_WIRE_ADDR selects the listen address (default 127.0.0.1:28317).

## Adding a case

1. Add the DDL/rows to CASES in test_wire_parity.py.
2. Add a matching arm in src/svc.rs. Column vectors are written in hex (see
   the hex helper); run the test once and copy the rust reply's bytes into the
   arm if it differs - the failure message prints both hex dumps.
3. cargo build, rerun.

## Reference fields ("&")

thrift/infinity.thrift has 17 reference fields. pilota parses them into
Option<Box<T>>; the wire format has no notion of "&", so this is only a Rust
type-level mapping.

`target/debug/roundtrip` decodes a CreateTableRequest from stdin with pilota and
re-encodes it to stdout. `test_thrift_roundtrip.py` feeds it bytes produced by the
official thrift encoder (ARRAY(INT) exercises `ArrayType.element_data_type`, plus
EMBEDDING and SPARSE columns) and asserts the output is identical (268 B). That is
the evidence that the fork does not move bytes on the wire.

## Size bounds

The Cargo.toml patch also takes `pilota` from the fork, which bounds container
sizes before the generated decode reserves a Vec for them (negative header ->
NEGATIVE_SIZE, configurable element cap -> SIZE_LIMIT, and, for the buffered
protocols, the element count must fit in the remaining bytes). That is
decode-side only, so accepted frames keep their bytes: the ten tests below
run against the patched crate. Apache Thrift has the same checks
(TBinaryProtocol::readListBegin, Protocol::checkReadBytesAvailable); Infinity
just never enabled setContainerSizeLimit.
