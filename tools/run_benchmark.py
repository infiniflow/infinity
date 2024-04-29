import argparse
import itertools
import os
import subprocess
import sys
import threading
import time


def benchmark_test(benchmark_dir: str):
    print("benchmark path is {}".format(benchmark_dir))
    # run test
    all_benchmark_cmds = [[f'{benchmark_dir}blocking_scheduler/blocking_scheduler_benchmark'], 
                          [f'{benchmark_dir}csv/csv_benchmark'], 
                          [f'{benchmark_dir}polling_scheduler/polling_scheduler_benchmark'], 
                          ["echo", "'1 1 centroids'", "|", f'{benchmark_dir}embedding/ann_ivfflat_benchmark'], 
                          [f'{benchmark_dir}embedding/hnsw_benchmark2'], 
                          [f'{benchmark_dir}local_infinity/infinity_benchmark'],
                          [f'{benchmark_dir}local_infinity/fulltext_benchmark'],
                          ]
    
    run_benchmark_cmds = [[f'{benchmark_dir}blocking_scheduler/blocking_scheduler_benchmark'], 
                          [f'{benchmark_dir}polling_scheduler/polling_scheduler_benchmark'], 
                          ["echo", "'1 1 centroids'", "|", f'{benchmark_dir}embedding/ann_ivfflat_benchmark'], 
                          [f'{benchmark_dir}embedding/hnsw_benchmark2'],
                          [f'{benchmark_dir}local_infinity/fulltext_benchmark'],
                          ]
    
    for i in range(len(run_benchmark_cmds)):
        process = subprocess.Popen(
            run_benchmark_cmds[i],
            stdout=sys.stdout,
            stderr=sys.stderr,
            universal_newlines=True,
        )
        process.wait()
        if process.returncode != 0:
            raise Exception(f"An error occurred: {process.stderr}")
        
    print("benchmark test finished.")



if __name__ == "__main__":
    print("Note: this script must be run under root directory of the project.")
    current_path = os.getcwd()
    benchmark_dir = current_path + "/cmake-build-release/benchmark/"

    print("Start cpp benchmark...")
    start = time.time()
    try:
        benchmark_test(benchmark_dir)
    except Exception as e:
        print(e)
        sys.exit(-1)
    end = time.time()
    print("Test finished.")
    print("Time cost: {}s".format(end - start))