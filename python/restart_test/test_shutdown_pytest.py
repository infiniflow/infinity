import subprocess
from common import common_values
from infinity_runner import InfinityRunner, infinity_runner_decorator_factory
import pytest
import os
import threading
import time
import random
import re


@pytest.mark.slow
@pytest.mark.skip(reason="skip ")
class TestShutDownPytest:
    test_i = 0
    log_prefix = "restart_test.log."
    finish_one = False

    def run_pytest_seperately(
        self, test_dir: str, pytest_mark="not complex and not slow"
    ):
        self.finish_one = False

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

            outfile = f"{self.log_prefix}{self.test_i}.log"
            with open(outfile, "w") as f:
                while True:
                    process = subprocess.Popen(cmd, shell=True, stdout=f, stderr=f)
                    process.wait()
                    if process.returncode == 0:
                        self.finish_one = True
                        break
                    yield test_name
                    self.finish_one = False

    @pytest.mark.parametrize(
        "pytest_mark",
        ["not complex and not slow"],
    )
    def test_shutdown_pytest(self, infinity_runner: InfinityRunner, pytest_mark: str):
        stop_interval = 10
        uri = common_values.TEST_LOCAL_HOST
        infinity_runner.clear()
        test_name = "test_pysdk"
        config = "conf/pytest_parallel_infinity_conf.toml"

        current_path = os.getcwd()
        test_dir = current_path + "/python/" + test_name

        gen = self.run_pytest_seperately(test_dir, pytest_mark=pytest_mark)

        decorator = infinity_runner_decorator_factory(
            config, uri, infinity_runner, shutdown_out=True
        )

        def shutdown_func():
            time.sleep(stop_interval)
            while True:
                if self.finish_one:
                    infinity_runner.uninit()
                    print("shutdown infinity")
                    return
                time.sleep(1)

        @decorator
        def part1(infinity_obj) -> bool:
            TRY_TIMES = 100
            for i in range(TRY_TIMES):
                try:
                    infinity_obj.get_database("default_db")
                    break
                except Exception as e:
                    infinity_runner.logger.debug(f"retry connect {i}")
                    time.sleep(0.5)
            else:
                raise Exception(
                    f"Cannot connect to infinity after {TRY_TIMES} retries."
                )

            t1 = threading.Thread(target=shutdown_func)
            t1.start()

            test_name = next(gen, None)
            if test_name is None:
                return False
            print(f"Stop at {test_name}")

            t1.join()

            return True

        while part1():
            pass
