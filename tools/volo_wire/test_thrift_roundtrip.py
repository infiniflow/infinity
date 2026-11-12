"""Wire transparency of pilota's reference-field ("&") handling.

thrift/infinity.thrift declares 17 fields as references, e.g.
struct ArrayType { 1: DataType & element_data_type }.  "&" is an Apache Thrift
language qualifier (C++ generates shared_ptr<T>); it is not a wire type.  The
pilota fork parses it and generates Option<Box<T>>.

This test proves that mapping does not move bytes: it encodes a
CreateTableRequest with the official thrift encoder (the python bindings the
SDK uses), decodes and re-encodes it with pilota (src/bin/roundtrip.rs), and
requires the two byte strings to be identical.

The message carries every physical type that involves a reference field:
ARRAY(INT) (ArrayType.element_data_type) next to a plain INT, an EMBEDDING and
a SPARSE column.
"""

import pathlib
import subprocess

import pytest
from infinity.remote_thrift.infinity_thrift_rpc import ttypes
from thrift.protocol import TBinaryProtocol
from thrift.transport import TTransport

ROOT = pathlib.Path(__file__).resolve().parents[2]
BIN = ROOT / "tools" / "volo_wire" / "target" / "debug" / "roundtrip"


def _encode(req):
    buf = TTransport.TMemoryBuffer()
    req.write(TBinaryProtocol.TBinaryProtocol(buf))
    return buf.getvalue()


def _col(cid, name, data_type):
    return ttypes.ColumnDef(id=cid, name=name, data_type=data_type, constraints=[])


def _request():
    array_type = ttypes.DataType(
        logic_type=ttypes.LogicType.Array,
        physical_type=ttypes.PhysicalType(
            array_type=ttypes.ArrayType(
                element_data_type=ttypes.DataType(
                    logic_type=ttypes.LogicType.Integer,
                    physical_type=ttypes.PhysicalType(number_type=ttypes.NumberType()),
                )
            )
        ),
    )
    embedding = ttypes.DataType(
        logic_type=ttypes.LogicType.Embedding,
        physical_type=ttypes.PhysicalType(
            embedding_type=ttypes.EmbeddingType(
                dimension=4, element_type=ttypes.ElementType.ElementFloat32
            )
        ),
    )
    sparse = ttypes.DataType(
        logic_type=ttypes.LogicType.Sparse,
        physical_type=ttypes.PhysicalType(
            sparse_type=ttypes.SparseType(
                dimension=1000,
                element_type=ttypes.ElementType.ElementFloat32,
                index_type=ttypes.ElementType.ElementInt32,
            )
        ),
    )
    return ttypes.CreateTableRequest(
        db_name="default_db",
        table_name="t",
        column_defs=[
            _col(0, "id", ttypes.DataType(logic_type=ttypes.LogicType.Integer)),
            _col(1, "a", array_type),
            _col(2, "v", embedding),
            _col(3, "s", sparse),
        ],
        session_id=1,
    )


@pytest.fixture(scope="module")
def roundtrip_bin():
    if not BIN.exists():
        pytest.skip(f"{BIN} missing; build it with: cargo build (in tools/volo_wire)")
    return BIN


def test_reference_field_roundtrip(roundtrip_bin):
    raw = _encode(_request())
    proc = subprocess.run([str(roundtrip_bin)], input=raw, capture_output=True)
    assert proc.returncode == 0, proc.stderr.decode()

    err = proc.stderr.decode()
    assert "columns=4" in err
    # The reference field must survive the decode: ArrayType.element_data_type
    # is Some(Integer) and not silently dropped to None.
    assert "array_elem=Some(LogicType(3))" in err

    assert proc.stdout == raw, (
        f"pilota re-encoded {len(proc.stdout)} B vs official {len(raw)} B\n"
        f"official={raw.hex()}\npilota={proc.stdout.hex()}"
    )
