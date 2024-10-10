from abc import abstractmethod
import os
import subprocess
import os
import psutil
import time
import infinity


class InfinityRunner:
    def __init__(self, infinity_path: str):
        self.data_dir = "/var/infinity"
        self.default_config_path = "./conf/infinity_conf.toml"
        self.script_path = "./scripts/timeout_kill.sh"
        self.infinity_path = infinity_path
        self.i = 0

    def clear(self):
        os.system(
            f"rm -rf {self.data_dir}/data {self.data_dir}/log {self.data_dir}/persistence {self.data_dir}/tmp {self.data_dir}/wal"
        )
        os.system(f"rm -rf restart_test.log.*")
        print(f"clear {self.data_dir}")

    def init(self, config_path: str | None = None):
        init_timeout = 60
        if config_path is None:
            config_path = self.default_config_path
        cmd = f"{self.infinity_path} --config={config_path} > restart_test.log.{self.i} 2>&1"

        pids = [
            proc.pid
            for proc in psutil.process_iter(["pid", "name"])
            if "infinity" in proc.info["name"]
        ]
        if len(pids) > 0:
            ret = os.system(
                f"bash {self.script_path} {init_timeout} {' '.join(map(str, pids))}"
            )
            if ret != 0:
                raise Exception("An error occurred.")

        # unset env LD_PRELOAD, ASAN_OPTIONS
        my_env = os.environ.copy()
        my_env["LD_PRELOAD"] = ""
        my_env["ASAN_OPTIONS"] = ""
        self.process = subprocess.Popen(cmd, shell=True, env=my_env)
        self.i += 1

    def uninit(self):
        timeout = 60
        pids = []
        for child in psutil.Process(self.process.pid).children(recursive=True):
            pids.append(child.pid)
        if len(pids) == 0:
            raise Exception("Cannot find infinity process.")
        ret = os.system(f"bash {self.script_path} {timeout} {' '.join(map(str, pids))}")
        if ret != 0:
            raise Exception("An error occurred.")

    @staticmethod
    def connect(uri: str):
        try_n = 100
        time.sleep(0.5)
        for i in range(try_n):
            try:
                return infinity.connect(uri)
            except Exception as e:
                print(e)
                time.sleep(0.5)
                print(f"retry connect {i}")


def infinity_runner_decorator_factory(
    config_path: str | None, uri: str, infinity_runner: InfinityRunner
):
    def decorator(f):
        def wrapper(*args, **kwargs):
            infinity_runner.init(config_path)
            infinity_obj = InfinityRunner.connect(uri)
            try:
                f(infinity_obj, *args, **kwargs)
            finally:
                infinity_obj.disconnect()
                infinity_runner.uninit()

        return wrapper

    return decorator
