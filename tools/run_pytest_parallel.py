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
    result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    end_time = time.time()
    elapsed_time = end_time - start_time
    if result.returncode != 0:
        raise RuntimeError(f"Command '{command}' failed with return code {result.returncode}\nOutput:\n{result.stdout}\nError:\n{result.stderr}")
    return command, elapsed_time, result.stdout, result.stderr

if __name__ == "__main__":
    print("Note: this script must be run under root directory of the project.")
    command_failed = False
    for command in commands:
        try:
            command, elapsed_time, stdout, stderr = run_command(command)
            print(f"Command '{command}' executed in {elapsed_time:.2f} seconds")
            print(f"Command '{command}' output:\n{stdout}")
            if stderr:
                print(f"Command '{command}' error:\n{stderr}")
                command_failed = True
        except RuntimeError as e:
            print(e)
            command_failed = True

    if command_failed:
        sys.exit(-1)
