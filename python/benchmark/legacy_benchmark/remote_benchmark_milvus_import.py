import argparse
import os
import time
import struct


from pymilvus import MilvusClient, DataType
from pymilvus import connections, db

from utils import SparseMatrix, csr_read_all

def import_data(
    data_set: str,
    block_size: int,
    compress: bool,
    opt_topk: int,
    bp_reorder: bool,
    remote: bool,
):
    current_path = os.getcwd()
    data_dir = current_path + "/test/data/benchmark/splade"
    print(f"Data Dir: {data_dir}")

    if data_set == "splade_small":
        import_path = data_dir + "/base_small.csr"
    elif data_set == "splade_1M":
        import_path = data_dir + "/base_1M.csr"
    elif data_set == "splade_full":
        import_path = data_dir + "/base_full.csr"
    else:
        raise Exception("Invalid data set")
    print(f"File Path: {import_path}")


    if remote:
        client = MilvusClient(
            uri="http://localhost:19530",
            token="root:Milvus"
        )

    if client.has_collection(collection_name="splade_benchmark_collection"):
        client.drop_collection(collection_name="splade_benchmark_collection")

    schema = MilvusClient.create_schema(
        auto_id=False,
        enable_dynamic_field=True,
        datatype=DataType.SPARSE_FLOAT_VECTOR,
    )
    schema.add_field(field_name="id", datatype=DataType.INT64, is_primary=True)
    schema.add_field(field_name="vector", datatype=DataType.SPARSE_FLOAT_VECTOR)
    index_params = client.prepare_index_params()
    #index_params.add_index(
    #    field_name="id",
    #)

    index_params.add_index(
        field_name="vector",
        index_name="sparse_inverted_index",
        index_type="SPARSE_WAND", # the type of index to be created. set to `SPARSE_INVERTED_INDEX` or `SPARSE_WAND`.
        metric_type="IP", # the metric type to be used for the index. Currently, only `IP` (Inner Product) is supported.
        params={"drop_ratio_build": 0.2}, # the ratio of small vector values to be dropped during indexi
    )

    client.create_collection(
        collection_name="splade_benchmark_collection",
        schema=schema,
        index_params=index_params,
    )

    assert os.path.exists(import_path)
    start = time.time()

    data_mat = csr_read_all(import_path)

    vectors = []
    count = 0
    for data_id in range(data_mat.nrow):
        indices, values = data_mat.at(data_id)

        sparse_embedding = []
        for i,j in zip(indices, values):
            sparse_embedding.append((i, j))


        vectors.append({"id": data_id, "vector": sparse_embedding})

        if len(vectors) == 1000:
            #print(vectors)
            client.insert(
                collection_name="splade_benchmark_collection",
                data=vectors,
            )
            vectors.clear()
            print(data_id)
        '''
        client.insert(collection_name="splade_benchmark_collection",
                      data=[{"id":data_id, "vector":sparse_embedding}],
                      )
        print(data_id)
        '''


    end = time.time()
    dur = end - start
    print(f"Import splade cost time: {dur} s")
    client.close()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Benchmark Infinity")
    parser.add_argument(
        "-d",
        "--data",
        type=str,
        default="splade_small",
        dest="data_set",
    )
    parser.add_argument("--block_size", type=int, default=8, dest="block_size")
    parser.add_argument("--compress", type=bool, default=True, dest="compress")
    parser.add_argument("--opt_topk", type=int, default=10, dest="opt_topk")
    parser.add_argument("--bp_reorder", type=bool, default=True, dest="bp_reorder")
    parser.add_argument("-R", "--remote", type=bool, default=True, dest="remote")

    args = parser.parse_args()

    import_data(
        args.data_set,
        args.block_size,
        args.compress,
        args.opt_topk,
        args.bp_reorder,
        args.remote,
    )