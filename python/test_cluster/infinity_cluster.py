import subprocess
import time
import tomli
import sys
import os
import psutil

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import infinity_http


class InfinityRunner:
    def __init__(self, node_name: str, executable_path: str, config_path: str):
        self.node_name = node_name
        self.executable_path = executable_path
        self.config_path = config_path
        self.__load_config()
        self.process = None
        self.client = None
        self.pids = []

    def __del__(self):
        self.uninit()

    def init(self, config_path: str | None):
        if self.process is not None:
            raise ValueError("Process is already initialized.")
        if config_path:
            self.config_path = config_path
            self.__load_config()

        run_cmd = f"{self.executable_path} --config={self.config_path} 2>&1"
        self.process = subprocess.Popen(run_cmd, shell=True)

        for child in psutil.Process(self.process.pid).children(recursive=True):
            self.pids.append(child.pid)
        if len(self.pids) == 0:
            raise Exception("Cannot find infinity process.")

    def uninit(self):
        if self.process is None:
            return
        script_path = "./scripts/timeout_kill.sh"
        timeout = 60

        ret = os.system(
            f"sudo bash {script_path} {timeout} {' '.join(map(str, self.pids))}"
        )
        if ret != 0:
            raise Exception("An error occurred.")
        self.process = None
        self.pids = []

    def init_as_leader(self, config_path: str | None = None):
        self.init(config_path)
        http_ip, http_port = self.http_uri()
        self.add_client(f"http://{http_ip}:{http_port}/")
        time.sleep(1)
        self.client.set_role_leader(self.node_name)

    def init_as_follower(self, leader_addr: str, config_path: str | None = None):
        self.init(config_path)
        http_ip, http_port = self.http_uri()
        self.add_client(f"http://{http_ip}:{http_port}/")
        time.sleep(1)
        self.client.set_role_follower(self.node_name, leader_addr)

    def add_client(self, http_addr: str):
        self.client = infinity_http(http_addr)

    def http_uri(self):
        http_ip = self.network_config["server_address"]
        http_port = self.network_config["http_port"]
        return http_ip, http_port

    def peer_uri(self):
        peer_ip = self.network_config["peer_ip"]
        peer_port = self.network_config["peer_port"]
        return peer_ip, peer_port

    def __load_config(self):
        with open(self.config_path, "rb") as f:
            config = tomli.load(f)
            self.network_config = config["network"]


class InfinityCluster:
    def __init__(self, executable_path: str):
        self.executable_path = executable_path
        self.runners: dict[str, InfinityRunner] = {}
        self.leader_runner: InfinityRunner | None = None

    def add_node(self, node_name: str, config_path: str):
        runner = InfinityRunner(node_name, self.executable_path, config_path)
        if node_name in self.runners:
            raise ValueError(f"Node {node_name} already exists in the cluster.")
        self.runners[node_name] = runner

    def init_leader(self, leader_name: str):
        if self.leader_runner is not None:
            raise ValueError(
                f"Leader {self.leader_runner.node_name} has already been initialized."
            )
        if leader_name not in self.runners:
            raise ValueError(f"Leader {leader_name} not found in the runners.")
        leader_runner = self.runners[leader_name]
        self.leader_runner = leader_runner
        leader_runner.init_as_leader()

    def init_follower(self, follower_name: str):
        if follower_name not in self.runners:
            raise ValueError(f"Follower {follower_name} not found in the runners")
        if self.leader_runner is None:
            raise ValueError("Leader has not been initialized.")
        follower_runner = self.runners[follower_name]
        leader_ip, leader_port = self.leader_addr()
        follower_runner.init_as_follower(f"{leader_ip}:{leader_port}")

    def client(self, node_name: str) -> infinity_http | None:
        if node_name not in self.runners:
            raise ValueError(f"Node {node_name} not found in the runners.")
        return self.runners[node_name]

    def leader_addr(self):
        if self.leader_runner is None:
            raise ValueError("Leader runner is not initialized.")
        return self.leader_runner.peer_uri()

    def remove(self, node_name: str):
        if node_name not in self.runners:
            raise ValueError(f"Node {node_name} not found in the cluster.")
        runner = self.runners[node_name]
        runner.uninit()
        del self.runners[node_name]


if __name__ == "__main__":
    pass
