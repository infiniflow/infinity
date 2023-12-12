import os
import time

from benchmark.test_benchmark_query import fvecs_read
from infinity.common import REMOTE_HOST
from infinity.remote_thrift.client import ThriftInfinityClient
from infinity.remote_thrift.query_builder import InfinityThriftQueryBuilder
from infinity.remote_thrift.table import RemoteTable


def query():
    thread_num = 1
    total_times = 10000

    print(">>> Query Benchmark Start <<<")
    print(f"Thread Num: {thread_num}, Times: {total_times}")
    start = time.time()

    sift_query_path = os.getcwd() + "/sift_1m/sift/query.fvecs"
    if not os.path.exists(sift_query_path):
        print(f"File: {sift_query_path} doesn't exist")
        return

    conn = ThriftInfinityClient(REMOTE_HOST)
    table = RemoteTable(conn, "default", "knn_benchmark")

    for idx, query_vec in enumerate(fvecs_read(sift_query_path)):
        query_builder = InfinityThriftQueryBuilder(table)
        query_builder.output(["*"])
        query_builder.knn('c1', query_vec, 'float', 'l2', 100)
        query_builder.to_df()
        if idx == total_times:
            assert idx == total_times
            break
    end = time.time()
    dur = end - start
    print(">>> Query Benchmark End <<<")
    qps = total_times / dur
    print(f"Total Times: {total_times}")
    print(f"Total Dur: {dur}")
    print(f"QPS: {qps}")


if __name__ == '__main__':
    import profile

    profile.run("query()", sort="tottime")
