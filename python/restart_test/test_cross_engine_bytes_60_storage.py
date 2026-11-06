"""L6 外部存储：对象目录与 S3。

`test_the_object_directory_is_shared_with_the_reference`（参考 `PersistenceManager`
的 `pm|object|` 打包对象，双向可读）与
`test_the_port_round_trips_through_object_storage`（端口的 minio 路径树往返，
需要 S3 端点，CI 不跑）。

这一层不比较两个引擎的桶内布局：端口不移植参考的对象键打包上传
（`FORMAT_EQUIVALENCE_TESTS.md` §6.5），只保证本地对象目录一致、端口自己往返可用。
"""

import os
import shutil
import time

import pytest
from bytes_common import (
    CPP_BIN,
    CRASH_COLUMNS,
    MINIO_ACCESS_KEY,
    MINIO_BUCKET,
    MINIO_SECRET_KEY,
    MINIO_URL,
    ROOT,
    RUST_BIN,
    _config,
    _spawn_and_run,
    _wait_port_free,
)
from infinity.common import ConflictType

from infinity import index

OBJECT_TABLE = "test_cross_engine_objects"
OBJECT_COLUMNS = {"id": {"type": "int"}, "title": {"type": "varchar"}, "score": {"type": "float"}}
OBJECT_ROWS = 100


def _object_dir_config(root, engine, reference_format):
    """`_config` with a persistence directory, plus the port's object-format key.

    Both engines must keep their files in the object directory; only the port
    knows `persistence_format` (the reference rejects unknown keys in
    `[storage]` with "Unrecognized config parameter"), so the key is written
    into the port's file alone.
    """
    text = _config(root, engine, 5432 if engine == "cpp" else 5433)
    text = text.replace(
        'persistence_dir = ""', f'persistence_dir = "{root}/persistence"'
    )
    if engine == "rust" and reference_format:
        text = text.replace(
            "[storage]", '[storage]\npersistence_format = "reference"', 1
        )
    return text


def _phase_object_seed(conn):
    db = conn.get_database("default_db")
    db.drop_table(OBJECT_TABLE, ConflictType.Ignore)
    t = db.create_table(OBJECT_TABLE, OBJECT_COLUMNS, ConflictType.Error)
    t.create_index("score_sec", index.IndexInfo("score", index.IndexType.Secondary))
    t.insert(
        [
            {"id": i, "title": f"doc-{i}", "score": i * 0.5}
            for i in range(OBJECT_ROWS)
        ]
    )
    conn.flush_data()
    time.sleep(2)


def _object_answers(conn):
    """Read through a block scan and through the index chunk."""
    t = conn.get_database("default_db").get_table(OBJECT_TABLE)

    def column(result, name):
        data, _, _ = result
        return [str(v) for v in data[name]]

    count = column(t.output(["count(*)"]).to_result(), "count(star)")
    ids = column(t.output(["id", "title"]).filter("id = 7").to_result(), "id")
    titles = column(t.output(["id", "title"]).filter("id = 7").to_result(), "title")
    scores = column(
        t.output(["id"]).filter("score >= 10.0 and score <= 12.0").to_result(), "id"
    )
    return count[0], list(zip(ids, titles)), scores


@pytest.mark.skipif(
    not os.access(CPP_BIN, os.X_OK), reason=f"{CPP_BIN} is not executable"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_object_directory_is_shared_with_the_reference():
    """Each engine reads the other's object directory, with `data/` emptied.

    The reference moves a persisted file into its object
    (`PersistenceManager::Persist`: an 8-byte-aligned part of a packed object,
    named by `pm|object|<path>`), and the port now writes the same records and
    reads them back on open. The `data/` tree is deleted between the two runs,
    so the reader can only answer by resolving the catalog's records against the
    object file - a scan (the block columns) and the secondary index chunk are
    both read this way.

    This is the **local** object layout, shared by both engines. The bucket
    transport is **not** aligned (see `docs/FORMAT_EQUIVALENCE_TESTS.md` §6.5):
    the reference uploads a packed object under a bare `obj_key` via
    `VirtualStore::UploadObject`, while the port mirrors a path tree
    (`data/`, `catalog/`, `persistence/`) as individual objects. The two do not
    read each other's buckets.
    """
    expected = {}
    for writer, reader, port in (("rust", "cpp", 5432), ("cpp", "rust", 5433)):
        root = os.path.join(ROOT, f"objects_{writer}")
        shutil.rmtree(root, ignore_errors=True)
        for sub in ("log", "tmp"):
            os.makedirs(os.path.join(root, sub), exist_ok=True)
        with open(os.path.join(root, "engine.toml"), "w") as handle:
            handle.write(_object_dir_config(root, writer, writer == "rust"))
        _spawn_and_run(writer, root, _phase_object_seed)

        object_dir = os.path.join(root, "persistence")
        objects = [
            name
            for name in os.listdir(object_dir)
            if name != "KEY_EMPTY" and not name.endswith((".sst", ".log"))
        ]
        assert objects, f"{writer}: no packed object was written"
        # The reference moves the files into the object; the port keeps its
        # `data/` as a local cache, so only the reference can be asked to leave
        # the directory empty.
        if writer == "cpp":
            assert not any(f for _, _, f in os.walk(os.path.join(root, "data"))), (
                "the reference left data files next to its object"
            )

        shutil.rmtree(os.path.join(root, "data"), ignore_errors=True)
        with open(os.path.join(root, "engine.toml"), "w") as handle:
            handle.write(_object_dir_config(root, reader, reader == "rust"))
        seen = []
        _spawn_and_run(reader, root, lambda conn: seen.append(_object_answers(conn)))
        count, by_id, by_score = seen[0]
        assert count == str(OBJECT_ROWS), f"{reader} read {count} rows from {writer}'s objects"
        assert by_id == [("7", "doc-7")], f"{reader}: {by_id}"
        assert by_score == ["20", "21", "22", "23", "24"], f"{reader}: {by_score}"
        expected[writer] = (count, by_id, by_score)
    assert expected["rust"] == expected["cpp"]


S3_TABLE = "test_cross_engine_object"


def _object_config(root, engine, pg_port, bucket):
    """`_config` plus the object-store section the reference reads."""
    base = _config(root, engine, pg_port).replace(
        "[storage]", '[storage]\nstorage_type = "minio"'
    )
    return base + f"""
[storage.object_storage]
url = "{MINIO_URL}"
bucket_name = "{bucket}"
access_key = "{MINIO_ACCESS_KEY}"
secret_key = "{MINIO_SECRET_KEY}"
enable_https = false
"""


def _write_object_config(root, engine, pg_port, bucket):
    for sub in ("log", "tmp"):
        os.makedirs(os.path.join(root, sub), exist_ok=True)
    with open(os.path.join(root, "engine.toml"), "w") as handle:
        handle.write(_object_config(root, engine, pg_port, bucket))


def _phase_object_write(conn):
    db = conn.get_database("default_db")
    db.drop_table(S3_TABLE, ConflictType.Ignore)
    t = db.create_table(S3_TABLE, CRASH_COLUMNS, ConflictType.Error)
    t.insert([{"id": i, "title": f"doc-{i}"} for i in range(200)])
    conn.flush_data()
    # The upload runs at the checkpoint, on the caller's thread, so returning
    # from `flush_data` means the bucket holds the tree.
    time.sleep(2)


@pytest.mark.skipif(
    not MINIO_URL, reason="set INFINITY_MINIO_URL to run the object-storage test"
)
@pytest.mark.skipif(
    not os.access(RUST_BIN, os.X_OK), reason=f"{RUST_BIN} is not executable"
)
def test_the_port_round_trips_through_object_storage():
    """Write with the bucket bound, wipe the local directory, read it back.

    The port uploads three trees at a checkpoint - `data/` (the C++-format
    blocks), `catalog/` and `persistence/` (the row store) - and only the first
    one used to come back, so a directory restored from the bucket had block
    files that belonged to no table.

    There is no cross-engine half to this, by decision: the port mirrors a
    path tree (`data/`, `catalog/`, `persistence/`) into the bucket as
    individual objects, while the reference uploads a packed object under a
    bare `obj_key` via `VirtualStore::UploadObject` (its object keys are not
    a path tree, and `VirtualStore::DownloadObject` has no caller in `src/` -
    measured, a reference directory whose local files are wiped starts with an
    empty catalog). Aligning the bucket transport is **not ported**; see
    `docs/FORMAT_EQUIVALENCE_TESTS.md` §6.5. This test only covers the port's
    own bucket round trip.
    """
    root = os.path.join(ROOT, "object_rust")
    # A bucket per run: a leftover from an earlier run would be restored into
    # the "empty" directory and the phases below would no longer describe a
    # write-then-restore round trip.
    bucket = f"{MINIO_BUCKET}-port-{int(time.time())}"
    _wait_port_free()
    shutil.rmtree(root, ignore_errors=True)
    _write_object_config(root, "rust", 5433, bucket)
    _spawn_and_run("rust", root, _phase_object_write)

    shutil.rmtree(root, ignore_errors=True)
    _write_object_config(root, "rust", 5433, bucket)

    seen = []

    def phase(conn):
        frame = (
            conn.get_database("default_db")
            .get_table(S3_TABLE)
            .output(["count(*)"])
            .to_pl()[0]
        )
        seen.append(int(frame.item()))

    _spawn_and_run("rust", root, phase)
    assert seen and seen[0] == 200, f"the restored directory holds {seen} rows"

    # A checkpoint *over the restored tree*, then another restore. This is the
    # path that used to lose data: the sync compared sizes only, and a rotated
    # RocksDB manifest leaves a 16-byte `CURRENT` whose content still changed,
    # so the new manifest was uploaded but the object naming the old one stayed
    # and the old manifest was deleted - an unopenable catalog.
    def write_more(conn):
        conn.get_database("default_db").get_table(S3_TABLE).insert(
            [{"id": 200 + i, "title": f"doc-{200 + i}"} for i in range(50)]
        )
        conn.flush_data()
        time.sleep(2)

    _spawn_and_run("rust", root, write_more)

    shutil.rmtree(root, ignore_errors=True)
    _write_object_config(root, "rust", 5433, bucket)
    _spawn_and_run("rust", root, phase)
    assert seen[-1] == 250, f"after the second checkpoint the bucket holds {seen} rows"
