import subprocess
import time
import tomli
import sys
import os

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..")))
from scripts import timeout_kill

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

    def __del__(self):
        self.uninit()

    def init(self, config_path: str | None):
        if self.process is not None:
            raise ValueError("Process is already initialized.")
        if config_path:
            self.config_path = config_path
            self.__load_config()

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

    def init_as_standalone(self, config_path: str | None = None):
        self.init(config_path)
        http_ip, http_port = self.http_uri()
        print(f"add client: http://{http_ip}:{http_port}/")
        self.add_client(f"http://{http_ip}:{http_port}/")
        self.__init_cmd(lambda: self.client.set_role_standalone(self.node_name))

    def init_as_leader(self, config_path: str | None = None):
        self.init(config_path)
        http_ip, http_port = self.http_uri()
        self.add_client(f"http://{http_ip}:{http_port}/")
        self.__init_cmd(lambda: self.client.set_role_leader(self.node_name))

    def init_as_follower(self, leader_addr: str, config_path: str | None = None):
        self.init(config_path)
        http_ip, http_port = self.http_uri()
        self.add_client(f"http://{http_ip}:{http_port}/")
        self.__init_cmd(
            lambda: self.client.set_role_follower(self.node_name, leader_addr)
        )

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

    def __init_cmd(self, send_f, timeout=30):
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

    def init_standalone(self, node_name: str):
        if node_name not in self.runners:
            raise ValueError(f"Node {node_name} not found in the runners.")
        runner = self.runners[node_name]
        runner.init_as_standalone()

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
