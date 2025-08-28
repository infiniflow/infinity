import logging
import os
import subprocess
import psutil
import time
import infinity
from infinity.common import InfinityException
from infinity.errors import ErrorCode
from infinity.connection_pool import ConnectionPool

PYTEST_LOG_FILE = "restart_test.py.log"


class InfinityRunner:
    def __init__(self, infinity_path: str, config_path: str, *, logger=None):
        self.data_dir = "/var/infinity"
        self.default_config_path = config_path

        self.script_path = "./scripts/timeout_kill.sh"
        self.infinity_path = infinity_path

        if not os.access(self.infinity_path, os.X_OK):
            raise Exception(f"{self.infinity_path} is not executable.")

        self.i = 0
        if logger is not None:
            self.logger = logger
            return
        self.logger = logging.getLogger("infinity_runner")
        if not self.logger.handlers:
            self.logger.setLevel(logging.DEBUG)
            handler = logging.StreamHandler()
            handler.setLevel(logging.INFO)
            formatter = logging.Formatter(
                "%(asctime)s - %(name)s - %(levelname)s - %(message)s"
            )
            handler.setFormatter(formatter)
            self.logger.addHandler(handler)

            handler = logging.FileHandler(PYTEST_LOG_FILE, delay=True)
            handler.setLevel(logging.DEBUG)
            handler.setFormatter(formatter)
            self.logger.addHandler(handler)

    def clear(self):
        os.system(
            f"rm -rf {self.data_dir}/catalog {self.data_dir}/data {self.data_dir}/log {self.data_dir}/persistence {self.data_dir}/tmp {self.data_dir}/wal"
        )
        os.system("rm -rf restart_test.log.*")
        os.system(f"rm -rf {PYTEST_LOG_FILE}")
        self.logger.info(f"clear infinity data dir {self.data_dir}")
        self.i = 0

    def init(self, config_path: str | None = None):
        # init_timeout = 60
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
                # f"{self.script_path} {init_timeout} {' '.join(map(str, pids))}"
                f"kill -9 {' '.join(map(str, pids))}"
            )
            if ret != 0:
                raise Exception("An error occurred.")

        # unset env LD_PRELOAD, ASAN_OPTIONS
        my_env = os.environ.copy()
        my_env["LD_PRELOAD"] = ""
        my_env["ASAN_OPTIONS"] = ""
        self.process = subprocess.Popen(cmd, shell=True, env=my_env)
        self.i += 1
        self.logger.info(f"init infinity process: {self.process.pid}")

    def uninit(self, kill: bool = False, timeout: int = 60):
        if self.process is None:
            return

        pids = []
        for child in psutil.Process(self.process.pid).children(recursive=True):
            pids.append(child.pid)
        if len(pids) == 0:
            return
            # raise Exception("Cannot find infinity process.")
        #
        # if kill:
        #     os.system(f"kill -9 {' '.join(map(str, pids))}")
        #     time.sleep(1)
        #     while any(psutil.pid_exists(pid) for pid in pids):
        #         time.sleep(1)
        #     self.process = None
        #     return

        # ret = os.system(f"{self.script_path} {timeout} {' '.join(map(str, pids))}")
        self.logger.info(f"about to kill infinity processes: {pids}")
        ret = os.system(f"kill -9 {' '.join(map(str, pids))}")
        if ret != 0:
            raise Exception("An error occurred.")

        return_code = self.process.wait()
        self.logger.info(f"return code: {return_code}")
        self.process = None

    def connected(self):
        return self.process is not None

    def connect(self, uri: str):
        try_n = 15
        infinity_obj = None
        for i in range(try_n):
            try:
                if infinity_obj is None:
                    infinity_obj = infinity.connect(uri, self.logger)
                _ = infinity_obj.get_database("default_db")
                break
            except Exception as e:
                if infinity_obj is not None:
                    if isinstance(e, InfinityException):
                        if e.error_code == ErrorCode.INFINITY_IS_INITING:
                            self.logger.info("Connection: wait infinity starting")
                        else:
                            raise e
                    else:
                        raise e
                else:
                    self.logger.warn(str(e))
                sleep_time = 1 * (i + 1)
                time.sleep(sleep_time)
                current_time = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
                self.logger.info(
                    f"Connection: retrying connection attempt {i + 1} after {sleep_time} seconds, at {current_time}.")
        else:
            raise Exception(f"Connection: cannot connect to infinity after {try_n} retries.")
        self.logger.info("Connection: connected to infinity.")
        return infinity_obj

    def connect_pool(self, uri: str):
        try_n = 15
        infinity_pool = None
        for i in range(try_n):
            try:
                if infinity_pool is None:
                    infinity_pool = ConnectionPool(uri)
                infinity_obj = infinity_pool.get_conn()
                _ = infinity_obj.get_database("default_db")
                infinity_pool.release_conn(infinity_obj)
                break
            except Exception as e:
                if infinity_pool is not None:
                    if isinstance(e, InfinityException):
                        if e.error_code == ErrorCode.INFINITY_IS_INITING:
                            self.logger.info("Connection pool: wait infinity starting")
                        else:
                            raise e
                    else:
                        raise e
                else:
                    self.logger.warn(str(e))
                sleep_time = 1 * (i + 1)
                time.sleep(sleep_time)
                current_time = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
                self.logger.info(
                    f"Connection pool: retrying connection attempt {i + 1} after {sleep_time} seconds, at {current_time}.")
        else:
            raise Exception(f"Connection pool: cannot connect to infinity after {try_n} retries.")
        self.logger.info("Connection pool: connected to infinity.")
        return infinity_pool


'''
This decorator is used to run a test function with an infinity connection object.
'''


def infinity_runner_decorator_factory(
        config_path: str | None,
        uri: str,
        infinity_runner: InfinityRunner,
        *,
        shutdown_out: bool = False,
        kill: bool = False,
        terminate_timeout: int = 60,
        check_kill: bool = True,
):
    def decorator(f):
        def wrapper(*args, **kwargs):
            infinity_runner.init(config_path)
            infinity_obj = infinity_runner.connect(uri)
            try:
                return f(infinity_obj, *args, **kwargs)
            finally:
                try:
                    infinity_obj.disconnect()
                except Exception:
                    if not shutdown_out:
                        raise
                try:
                    infinity_runner.uninit(kill, terminate_timeout)
                except Exception:
                    if check_kill:
                        raise

        return wrapper

    return decorator


'''
This decorator is used to run a test function with an infinity ConnectionPool object.
'''


def infinity_runner_decorator_factory2(
        config_path: str | None,
        uri: str,
        infinity_runner: InfinityRunner,
        *,
        shutdown_out: bool = False,
        kill: bool = False,
        terminate_timeout: int = 60,
        check_kill: bool = True,
):
    def decorator(f):
        def wrapper(*args, **kwargs):
            infinity_runner.init(config_path)
            infinity_pool = infinity_runner.connect_pool(uri)
            try:
                return f(infinity_pool, *args, **kwargs)
            finally:
                try:
                    infinity_pool.destroy()
                except Exception:
                    if not shutdown_out:
                        raise
                try:
                    infinity_runner.uninit(kill, terminate_timeout)
                except Exception:
                    if check_kill:
                        raise

        return wrapper

    return decorator
