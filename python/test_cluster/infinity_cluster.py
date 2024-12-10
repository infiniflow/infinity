from abc import abstractmethod
import json
import logging
import subprocess
import time
import docker
import tomli
import sys
import os
import socket

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..")))
from scripts import timeout_kill

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import infinity_http, http_network_util


def convert_request_to_curl(method: str, header: dict, data: dict, url: str):
    cmd = "curl -sS --request {method} --url {url} {headers} --data '{data}'"
    method = method.upper()
    headers = " ".join([f"--header '{key}:{value}'" for key, value in header.items()])
    data = json.dumps(data)
    return cmd.format(method=method, headers=headers, data=data, url=url)


# https://stackoverflow.com/questions/2470971/fast-way-to-test-if-a-port-is-in-use-using-python
def is_port_in_use(port: int) -> bool:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        return s.connect_ex(("localhost", port)) == 0


class MinioParams:
    def __init__(self, minio_dir: str, minio_port: int, minio_console_port: int):
        self.minio_dir = minio_dir
        self.minio_port = minio_port
        self.minio_console_port = minio_console_port


class BaseInfinityRunner:

    def __init__(
        self,
        node_name: str,
        executable_path: str,
        config_path: str,
        init: bool = True,
        logger: logging.Logger = None,
    ):
        self.node_name = node_name
        self.executable_path = executable_path
        self.config_path = config_path
        if logger is None:
            self.logger = logging.root
        else:
            self.logger = logger
        self.load_config()
        http_ip, http_port = self.http_uri()
        self.add_client(f"http://{http_ip}:{http_port}/")
        if init:
            self.init(config_path)

    @abstractmethod
    def init(self, config_path: str | None):
        pass

    @abstractmethod
    def uninit(self):
        pass

    @abstractmethod
    def add_client(self, http_addr: str):
        pass

    def http_uri(self):
        http_ip = self.network_config["server_address"]
        http_port = self.network_config["http_port"]
        return http_ip, http_port

    def peer_uri(self):
        peer_ip = self.network_config["peer_ip"]
        peer_port = self.network_config["peer_port"]
        return peer_ip, peer_port

    @abstractmethod
    def load_config(self):
        pass


class InfinityRunner(BaseInfinityRunner):
    def __init__(
        self,
        node_name: str,
        executable_path: str,
        config_path: str,
        init=True,
        logger: logging.Logger = None,
    ):
        self.process = None
        super().__init__(node_name, executable_path, config_path, init, logger)

    def init(self, config_path: str | None):
        if self.process is not None:
            raise ValueError("Process is already initialized.")
        if config_path:
            self.config_path = config_path
            self.load_config()

        cmd = [self.executable_path, f"--config={self.config_path}"]
        self.logger.debug(f"Start process for node {self.node_name}: {cmd}")
        my_env = os.environ.copy()
        my_env["LD_PRELOAD"] = ""
        my_env["ASAN_OPTIONS"] = ""
        self.process = subprocess.Popen(cmd, shell=False, env=my_env)
        self.check_start()

    def check_start(self):
        timeout = 10  # Give the process a moment to start
        start_time = time.time()
        while time.time() - start_time < timeout:
            if self.process.poll() is None:
                break
            self.logger.debug(
                f"Waiting for process to start for node {self.node_name}, wait time: {time.time() - start_time}"
            )
            time.sleep(0.5)
        else:
            raise RuntimeError(
                f"Failed to start process for node {self.node_name}, return code: {self.process.returncode}"
            )
        self.logger.info(
            f"Launch {self.node_name} successfully. pid: {self.process.pid}"
        )

    def uninit(self):
        self.logger.info(f"Uninit node {self.node_name}")
        if self.process is None:
            return
        timeout = 60
        timeout_kill.timeout_kill(timeout, self.process, self.logger)

    def kill(self):
        self.logger.info(f"Kill node {self.node_name}")
        if self.process is None:
            return
        self.process.kill()

    def add_client(self, http_addr: str):
        net = http_network_util(http_addr)
        net.set_retry()
        self.client = infinity_http(net=net)

    def load_config(self):
        with open(self.config_path, "rb") as f:
            config = tomli.load(f)
            self.network_config = config["network"]


class InfinityCluster:
    def __init__(
        self, executable_path: str, *, minio_params: MinioParams, test_name: str = None
    ):
        self.executable_path = executable_path
        self.runners: dict[str, InfinityRunner] = {}
        self.leader_runner: InfinityRunner | None = None
        self.leader_name = None
        self.logger_name = test_name
        self.minio_params = minio_params

    def _log_filename(self):
        if self.logger_name is None:
            return "cluster.log"
        return f"{self.logger_name}.log"

    def init_logger(self, logger_name: str):
        logger = logging.getLogger(logger_name)
        logger.handlers.clear()
        logger.setLevel(logging.DEBUG)
        stream_handler = logging.StreamHandler()
        stream_handler.setLevel(logging.INFO)
        logger.addHandler(stream_handler)

        file_handler = logging.FileHandler(self._log_filename(), mode="w", delay=True)
        file_handler.setLevel(logging.DEBUG)
        logger.addHandler(file_handler)
        self.logger = logger

    def __enter__(self):
        self.init_logger(self.logger_name)
        self.add_minio(self.minio_params)
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.clear()
        if exc_type is not None:
            self.logger.error(f"Exception: {exc_val}")
        else:
            self.clear_log()

    def clear(self):
        # shutdown follower and learner first
        if self.leader_runner is not None:
            for runner_name in self.runners.keys():
                if runner_name != self.leader_name:
                    self.runners[runner_name].uninit()
            self.runners[self.leader_name].uninit()
        else:
            for runner in self.runners.values():
                runner.uninit()
        self.runners.clear()
        self.leader_runner = None

    def clear_log(self):
        log_file = self._log_filename()
        if os.path.exists(log_file):
            os.remove(log_file)

        # if self.minio_container is not None:
        #     self.minio_container.remove(force=True, v=True)

    def add_node(self, node_name: str, config_path: str, init=True):
        runner = InfinityRunner(
            node_name, self.executable_path, config_path, init, self.logger
        )
        if init:
            ip, port = runner.http_uri()
            timeout = 3
            start_time = time.time()
            while time.time() - start_time < timeout:
                if is_port_in_use(port):
                    break
                self.logger.debug(
                    f"Waiting for node {node_name} to open port {port} for {time.time() - start_time} seconds."
                )
                time.sleep(0.5)
            else:
                raise Exception
        if node_name in self.runners:
            raise ValueError(f"Node {node_name} already exists in the cluster.")
        self.runners[node_name] = runner

    def add_minio(self, minio_params: MinioParams):
        minio_image_name = "quay.io/minio/minio"

        minio_cmd = f'server /data --address ":{minio_params.minio_port}" --console-address ":{minio_params.minio_console_port}"'
        docker_client = docker.from_env()
        container_name = "minio_host"

        try:
            self.minio_container = docker_client.containers.get(container_name)
            self.minio_container.start()
            self.logger.debug(f"Minio container {container_name} already exists.")
        except docker.errors.NotFound:
            self.minio_container = docker_client.containers.run(
                image=minio_image_name,
                name=container_name,
                detach=True,
                environment=[
                    "MINIO_ROOT_PASSWORD=minioadmin",
                    "MINIO_ROOT_USER=minioadmin",
                ],
                volumes=[f"{minio_params.minio_dir}:/data"],
                command=minio_cmd,
                network="host",
            )
            self.logger.debug(f"Minio container {container_name} created.")

    def set_standalone(self, node_name: str):
        if self.leader_runner is not None and self.leader_name == node_name:
            self.leader_name = None
            self.leader_runner = None
        runner = self._get_runner(node_name)
        runner.client.set_role_standalone(node_name)
        self.logger.info(f"Set node {node_name} as standalone.")

    def set_admin(self, node_name: str):
        if self.leader_runner is not None and self.leader_name == node_name:
            self.leader_name = None
            self.leader_runner = None
        runner = self._get_runner(node_name)
        runner.client.set_role_admin()
        self.logger.info(f"Set node {node_name} as admin.")

    def set_leader(self, leader_name: str):
        if self.leader_runner is not None:
            raise ValueError(
                f"Leader {self.leader_runner.node_name} has already been initialized."
            )
        leader_runner = self._get_runner(leader_name)
        self.leader_name = leader_name
        self.leader_runner = leader_runner
        leader_runner.client.set_role_leader(leader_name)
        self.logger.info(f"Set node {leader_name} as leader.")

    def set_follower(self, follower_name: str):
        if self.leader_runner is None:
            raise ValueError("Leader has not been initialized.")
        follower_runner = self._get_runner(follower_name)
        leader_ip, leader_port = self.leader_addr()
        follower_runner.client.set_role_follower(
            follower_name, f"{leader_ip}:{leader_port}"
        )
        self.logger.info(f"Set node {follower_name} as follower.")

    def set_learner(self, learner_name: str):
        if self.leader_runner is None:
            raise ValueError("Learner has not been initialized.")
        learner_runner = self._get_runner(learner_name)
        leader_ip, leader_port = self.leader_addr()
        learner_runner.client.set_role_learner(
            learner_name, f"{leader_ip}:{leader_port}"
        )
        self.logger.info(f"Set node {learner_name} as learner.")

    def client(self, node_name: str) -> infinity_http | None:
        return self._get_runner(node_name).client

    def leader_addr(self):
        if self.leader_runner is None:
            raise ValueError("Leader runner is not initialized.")
        return self.leader_runner.peer_uri()

    def remove_node(self, node_name: str, kill: bool = False):
        runner = self._get_runner(node_name)
        if kill:
            runner.kill()
        else:
            runner.uninit()
        del self.runners[node_name]
        if self.leader_name is not None and self.leader_name == node_name:
            self.leader_name = None
            self.leader_runner = None
        if kill and self.leader_runner is not None:
            infinity_leader = self.leader_runner.client
            wait_timeout_sec = 10
            start_time = time.time()
            while time.time() - start_time < wait_timeout_sec:
                time.sleep(0.5)
                node_info = infinity_leader.show_node(node_name)

                if node_info.node_status != "alive":
                    break
            else:
                raise Exception(
                    f"Node {node_name} is still alive after {wait_timeout_sec} seconds."
                )
            infinity_leader.remove_node(node_name)
            self.logger.debug(
                f"Remove node {node_name} from the cluster because status is {node_info.node_status}."
            )

    def _get_runner(self, node_name: str) -> InfinityRunner:
        if node_name not in self.runners:
            raise ValueError(f"Node {node_name} not found in the cluster.")
        return self.runners[node_name]


if __name__ == "__main__":
    pass
