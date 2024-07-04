import subprocess
import time
import sys
from concurrent.futures import ProcessPoolExecutor, as_completed

commands = [
    "python3 tools/run_pysdk_remote_infinity_test.py",
    "python3 tools/run_parallel_test.py",
    "python3 tools/run_http_api.py",
    "python3 tools/sqllogictest.py"
]

def run_command(command):
    start_time = time.time()
    result = subprocess.run(command, shell=True)
    end_time = time.time()
    elapsed_time = end_time - start_time
    if result.returncode != 0:
        raise RuntimeError(f"Command '{command}' failed with return code {result.returncode}")
    return command, elapsed_time

if __name__ == "__main__":
    print("Note: this script must be run under root directory of the project.")
    try:
        with ProcessPoolExecutor() as executor:
            futures = [executor.submit(run_command, cmd) for cmd in commands]
            for future in as_completed(futures):
                command, elapsed_time = future.result()
                print(f"Command '{command}' executed in {elapsed_time:.2f} seconds")
    except RuntimeError as e:
        print(e)
        sys.exit(-1)

