from abc import abstractmethod
import json
import random
import string
import subprocess
import time
import docker
import tomli
import sys
import os

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


class MinioParams:
    def __init__(self, minio_dir: str, minio_port: int):
        self.minio_dir = minio_dir
        self.minio_port = minio_port


class BaseInfinityRunner:
    def __init__(self, node_name: str, executable_path: str, config_path: str):
        self.node_name = node_name
        self.executable_path = executable_path
        self.config_path = config_path
        self.load_config()
        http_ip, http_port = self.http_uri()
        self.add_client(f"http://{http_ip}:{http_port}/")

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

    def __init_cmd(self, send_f, timeout=10):
        t1 = time.time()
        while True:
            try:
                send_f()
            except Exception as e:
                print(e)
                time.sleep(1)
                if time.time() - t1 > timeout:
                    raise Exception("Timeout")
                continue
            break


class InfinityRunner(BaseInfinityRunner):
    def __init__(
        self, node_name: str, executable_path: str, config_path: str, init=True
    ):
        super().__init__(node_name, executable_path, config_path)
        self.process = None
        if init:
            self.init(config_path)

    def init(self, config_path: str | None):
        if self.process is not None:
            raise ValueError("Process is already initialized.")
        if config_path:
            self.config_path = config_path
            self.load_config()

        cmd = [self.executable_path, f"--config={self.config_path}"]
        my_env = os.environ.copy()
        my_env["LD_PRELOAD"] = ""
        my_env["ASAN_OPTIONS"] = ""
        self.process = subprocess.Popen(cmd, shell=False, env=my_env)
        time.sleep(1)  # Give the process a moment to start
        if self.process.poll() is not None:
            raise RuntimeError(
                f"Failed to start process for node {self.node_name}, return code: {self.process.returncode}"
            )
        print(f"Launch {self.node_name} successfully. pid: {self.process.pid}")

    def uninit(self):
        print(f"Uniting node {self.node_name}")
        if self.process is None:
            return
        timeout = 60
        timeout_kill.timeout_kill(timeout, self.process)

    def add_client(self, http_addr: str):
        self.client = infinity_http(net=http_network_util(http_addr))

    def load_config(self):
        with open(self.config_path, "rb") as f:
            config = tomli.load(f)
            self.network_config = config["network"]


class InfinityCluster:
    def __init__(self, executable_path: str, *, minio_params: MinioParams):
        self.executable_path = executable_path
        self.runners: dict[str, InfinityRunner] = {}
        self.leader_runner: InfinityRunner | None = None
        self.leader_name = None
        self.add_minio(minio_params)

    def clear(self):
        for runner in self.runners.values():
            runner.uninit()
        # if self.minio_container is not None:
        #     self.minio_container.remove(force=True, v=True)

    def add_node(self, node_name: str, config_path: str):
        runner = InfinityRunner(node_name, self.executable_path, config_path)
        if node_name in self.runners:
            raise ValueError(f"Node {node_name} already exists in the cluster.")
        self.runners[node_name] = runner

    def add_minio(self, minio_params: MinioParams):
        minio_image_name = "quay.io/minio/minio"

        minio_cmd = f'server /data --console-address ":{minio_params.minio_port}"'
        docker_client = docker.from_env()
        container_name = "minio_host"

        try:
            self.minio_container = docker_client.containers.get(container_name)
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

    def set_standalone(self, node_name: str):
        if self.leader_runner is not None and self.leader_name == node_name:
            self.leader_name = None
            self.leader_runner = None
        if node_name not in self.runners:
            raise ValueError(f"Node {node_name} not found in the runners.")
        runner = self.runners[node_name]
        runner.client.set_role_standalone(node_name)

    def set_admin(self, node_name: str):
        if self.leader_runner is not None and self.leader_name == node_name:
            self.leader_name = None
            self.leader_runner = None
        if node_name not in self.runners:
            raise ValueError(f"Node {node_name} not found in the runners.")
        runner = self.runners[node_name]
        runner.client.set_role_admin()

    def set_leader(self, leader_name: str):
        if self.leader_runner is not None:
            raise ValueError(
                f"Leader {self.leader_runner.node_name} has already been initialized."
            )
        if leader_name not in self.runners:
            raise ValueError(f"Leader {leader_name} not found in the runners.")
        leader_runner = self.runners[leader_name]
        self.leader_name = leader_name
        self.leader_runner = leader_runner
        leader_runner.client.set_role_leader(leader_name)

    def set_follower(self, follower_name: str):
        if follower_name not in self.runners:
            raise ValueError(f"Follower {follower_name} not found in the runners")
        if self.leader_runner is None:
            raise ValueError("Leader has not been initialized.")
        follower_runner = self.runners[follower_name]
        leader_ip, leader_port = self.leader_addr()
        follower_runner.client.set_role_follower(
            follower_name, f"{leader_ip}:{leader_port}"
        )

    def set_learner(self, learner_name: str):
        if learner_name not in self.runners:
            raise ValueError(f"Learner {learner_name} not found in the runners")
        if self.leader_runner is None:
            raise ValueError("Learner has not been initialized.")
        learner_runner = self.runners[learner_name]
        leader_ip, leader_port = self.leader_addr()
        learner_runner.client.set_role_learner(
            learner_name, f"{leader_ip}:{leader_port}"
        )

    def client(self, node_name: str) -> infinity_http | None:
        if node_name not in self.runners:
            raise ValueError(f"Node {node_name} not found in the runners.")
        return self.runners[node_name].client

    def leader_addr(self):
        if self.leader_runner is None:
            raise ValueError("Leader runner is not initialized.")
        return self.leader_runner.peer_uri()

    def remove_node(self, node_name: str):
        if node_name not in self.runners:
            raise ValueError(f"Node {node_name} not found in the cluster.")
        runner = self.runners[node_name]
        runner.uninit()
        del self.runners[node_name]


if __name__ == "__main__":
    pass
