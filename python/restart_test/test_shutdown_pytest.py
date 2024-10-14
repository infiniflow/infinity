import subprocess
from common import common_values
from infinity_runner import InfinityRunner
import pytest
import os
import threading
import time
import random
import re


@pytest.mark.slow
@pytest.mark.skip(reason="skip")
class TestShutDownPytest:
    finish_one = False

    def run_pytest_seperately(
        self, test_dir: str, pytest_mark="not complex and not slow"
    ):
        cmd = f'pytest {test_dir} --collect-only -q -m "{pytest_mark}"'
        output = os.popen(cmd).read()
        lines = output.splitlines()
        random.shuffle(lines)

        # use regular expression to find "{filename}::{}::{testname}[{param}]"
        for line in lines:
            match = re.match(r"(.+?)::(.+?)::(.+?)\[.*\]", line)
            if not match:
                continue
            test_file, test_class, test_name = match.groups()
            print(
                f"Test file: {test_file}, test class: {test_class}, test name: {test_name}"
            )
            cmd = f"pytest {test_dir} -k {test_name} -x"  # exit after first failure
            print(f"Test cmd: {cmd}")

            while True:
                process = subprocess.Popen(cmd, shell=True)
                process.wait()
                if process.returncode == 0:
                    break
                yield test_file, test_name
                self.finish_one = False
            self.finish_one = True

    @pytest.mark.parametrize(
        "test_name",
        [
            "test_pysdk"
            # "parallel_test",  "test_http_api"
        ],
    )
    def test_shutdown_pytest(self, infinity_runner: InfinityRunner, test_name: str):
        stop_interval = 10
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()

        current_path = os.getcwd()
        test_dir = current_path + "/python/" + test_name

        gen = self.run_pytest_seperately(test_dir)

        def shutdown_func():
            time.sleep(stop_interval)
            while True:
                if self.finish_one:
                    infinity_runner.uninit()
                    print("shutdown infinity")
                    return
                time.sleep(1)

        while True:
            infinity_runner.init()
            infinity_obj = InfinityRunner.connect(uri)  # ensure infinity is started
            infinity_obj.disconnect()

            t1 = threading.Thread(target=shutdown_func)
            t1.start()

            x = next(gen, None)
            if x is None:
                break
            stop_testfile, stop_test_name = x
            print(f"Stop test: {stop_test_name}")

            t1.join()
