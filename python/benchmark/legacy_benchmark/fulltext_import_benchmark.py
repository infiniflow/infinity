import argparse
import os
import time
import json

import infinity
from infinity import index
from infinity.common import LOCAL_HOST
from infinity.errors import ErrorCode
from infinity.common import ConflictType


def import_data(path):
    if not os.path.exists(path):
        raise Exception("Invalid data set")
    data = []
    with open(path, 'r', encoding="utf-8") as f:
        for line in f:
            data_line = json.loads(line)
            del data_line["metadata"]
            data.append(data_line)
    print("import data complete")
    return data[:10000]


def insert_data(db_obj, data):
    db_obj.drop_table("insert_benchmark")
    table_obj = db_obj.create_table("insert_benchmark",
                                    {"id": {"type": "varchar"}, "title": {"type": "varchar"},
                                     "text": {"type": "varchar"}}, ConflictType.Error)
    res = table_obj.create_index("text_index", index.IndexInfo("text", index.IndexType.FullText))
    assert res.error_code == ErrorCode.OK
    inserted_records_num = 0
    while inserted_records_num < len(data):
        value = []
        insert_num_this_round = 0
        while insert_num_this_round < 500 and insert_num_this_round + inserted_records_num < len(data):
            value.append(data[inserted_records_num + insert_num_this_round])
            insert_num_this_round += 1
        inserted_records_num += insert_num_this_round
        begin_time = time.time()
        table_obj.insert(value)
        print(f"insert {inserted_records_num} has been blocked {time.time() - begin_time} s")
    print(inserted_records_num)


def import_file(db_obj, path):
    db_obj.drop_table("import_file_benchmark")
    table_obj = db_obj.create_table("import_file_benchmark",
                                    {"id": {"type": "varchar"}, "title": {"type": "varchar"},
                                     "text": {"type": "varchar"}}, ConflictType.Error)
    assert table_obj
    res = table_obj.create_index("text_index", index.IndexInfo("text", index.IndexType.FullText))
    assert res.error_code == ErrorCode.OK
    table_obj.import_data(path, {'file_type': 'jsonl'})


if __name__ == '__main__':
    current_path = os.getcwd()
    parent_path = os.path.dirname(current_path)
    parent_path = os.path.dirname(parent_path)
    print(f"Current Path: {current_path}")
    parser = argparse.ArgumentParser(description="Benchmark Infinity")
    parser.add_argument(
        "-d",
        "--data",
        type=str,
        default='corpus.jsonl',
        dest="data_set",
    )
    data_dir = parent_path + "/test/data/benchmark/" + parser.parse_args().data_set
    print(f"Data Dir: {data_dir}")

    data = import_data(path=data_dir)

    infinity_obj = infinity.connect(LOCAL_HOST)
    assert infinity_obj
    db_obj = infinity_obj.get_database("default_db")
    assert db_obj

    start_time = time.time()
    # import_file(db_obj, data_dir)
    # print("import time cost: %.2f " % (time.time() - start_time))
    start_time = time.time()
    insert_data(db_obj, data)
    print("insert time cost: %.2f " % (time.time() - start_time))
