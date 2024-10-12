
import argparse
import os
import time


# TEST_SEC = 3600 # 1 hour
TEST_SEC = 10 # run once

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Restart Test Continuously")
    parser.add_argument(
        "-t",
        "--test_sec",
        type=int,
        default=TEST_SEC,
        dest="test_sec",
    )
    parser.add_argument(
        "--infinity_path",
        type=str,
        default="./build/Debug/src/infinity",
        dest="infinity_path",
    )
    args = parser.parse_args()
    test_sec = args.test_sec
    infinity_path = args.infinity_path

    begin_time = time.time()
    test_i = 0

    test_fail = False
    while (time.time() - begin_time) < test_sec and not test_fail:
        print(f"Test {test_i}")
        try:
            os.system(f"python3 tools/run_restart_test.py --infinity_path={infinity_path} --slow=true")
        except Exception as e:
            print(e)
            test_fail = True
            raise e