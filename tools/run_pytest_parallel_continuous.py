from concurrent import futures
import os
from run_pytest_parallel import commands, run_command
import time
import argparse


LOG_PATH = "/var/infinity/log/infinity.log"
TEST_SEC = 900


def clear_infinity_log():
    if os.path.exists(infinity_log_path):
        os.remove(infinity_log_path)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Pytest Parallel Continuous")
    parser.add_argument(
        "-t",
        "--test_sec",
        type=int,
        default=TEST_SEC,
        dest="test_sec",
    )
    parser.add_argument(
        "-l",
        "--log_path",
        type=str,
        default=LOG_PATH,
        dest="log_path",
    )
    args = parser.parse_args()
    test_sec = args.test_sec
    infinity_log_path = args.log_path

    command_failed = False
    begin_time = time.time()
    try:
        with futures.ProcessPoolExecutor() as executor:
            while not command_failed and time.time() - begin_time < test_sec:
                futs = [executor.submit(run_command, cmd) for cmd in commands]
                for fut in futures.as_completed(futs):
                    try:
                        command, elapsed_time, stdout, stderr = fut.result()
                        print(
                            f"Command '{command}' executed in {elapsed_time:.2f} seconds"
                        )
                        print(f"Command '{command}' output:\n{stdout}")
                        if stderr:
                            print(f"Command '{command}' error:\n{stderr}")
                            command_failed = True
                    except RuntimeError as e:
                        print(e)
                        command_failed = True
                clear_infinity_log()

    except Exception as e:
        print(e)
        command_failed = True
    if command_failed:
        exit(-1)
