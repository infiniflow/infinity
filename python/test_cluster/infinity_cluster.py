import platform
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

    def uninit(self):
        if self.process is None:
            return
        script_path = "./scripts/timeout_kill.sh"
        timeout = 60
        pids = []
        for child in psutil.Process(self.process.pid).children(recursive=True):
            pids.append(child.pid)
        if len(pids) == 0:
            raise Exception("Cannot find infinity process.")
        ret = os.system(f"bash {script_path} {timeout} {' '.join(map(str, pids))}")
        if ret != 0:
            raise Exception("An error occurred.")
        self.process = None

    def init_as_leader(self, config_path: str | None = None):
        self.init(config_path)
        http_ip, http_port = self.http_uri()
        self.client = infinity_http(f"http://{http_ip}:{http_port}/")
        time.sleep(1)
        self.client.set_role_leader(self.node_name)

    def init_as_follower(self, leader_addr: str, config_path: str | None = None):
        self.init(config_path)
        http_ip, http_port = self.http_uri()
        self.client = infinity_http(f"http://{http_ip}:{http_port}/")
        time.sleep(1)
        self.client.set_role_follower(self.node_name, leader_addr)

    def __load_config(self):
        with open(self.config_path, "rb") as f:
            config = tomli.load(f)
            self.network_config = config["network"]

    def http_uri(self):
        http_ip = self.network_config["server_address"]
        http_port = self.network_config["http_port"]
        return http_ip, http_port

    def peer_uri(self):
        peer_ip = self.network_config["peer_ip"]
        peer_port = self.network_config["peer_port"]
        return peer_ip, peer_port


class MockedInfinityRunner(InfinityRunner):
    def __init__(
        self, ns_name: str, node_name: str, executable_path: str, config_path: str
    ):
        super().__init__(node_name, executable_path, config_path)
        self.ns_name = ns_name

    def init(self, config_path: str | None):
        if self.process is not None:
            raise ValueError("Process is already initialized.")
        if config_path:
            self.config_path = config_path
            self.__load_config()

        run_cmd = f"{self.executable_path} --config={self.config_path} 2>&1"
        run_cmd = f"sudo ip netns exec {self.ns_name} {run_cmd}"
        self.process = subprocess.Popen(run_cmd, shell=True)


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


class MockInfinityCluster(InfinityCluster):
    def __init__(self, executable_path: str):
        super().__init__(executable_path)
        self.__check_prerequisites()
        self.ns_prefix = "ns"
        self.bridge_name = "br0"
        self.__prepare_ns()

    def __check_prerequisites(self):
        if platform.system() != "Linux":
            print("Network namespaces are only supported on Linux.")
            exit()
        # check if ip, brctl is installed
        subprocess.run("ip --version".split(), check=True)
        subprocess.run("brctl --version".split(), check=True)

    def __prepare_ns(self):
        subprocess.run("sudo ip netns delete ns*".split())
        subprocess.run(f"sudo ip link set {self.bridge_name} down".split())
        subprocess.run(f"sudo brctl delbr {self.bridge_name}".split())

    def add_node(self, node_name: str, config_path: str):
        ns_name = f"{self.ns_prefix}_{node_name}"
        self.__ns_connect(ns_name)
        runner = MockedInfinityRunner(
            ns_name, node_name, self.executable_path, config_path
        )
        if node_name in self.runners:
            raise ValueError(f"Node {node_name} already exists in the cluster.")
        self.runners[node_name] = runner

    def __ns_connect(self, ns_name: str):
        subprocess.run(f"sudo ip netns add {ns_name}".split(), check=True)
        subprocess.run(f"sudo brctl addbr {self.bridge_name}".split(), check=True)
        veth_name = f"veth-{ns_name}"
        veth_br_name = f"veth-{ns_name}-br"
        subprocess.run(
            f"sudo ip link add {veth_name} type veth peer name {veth_br_name}".split(),
            check=True,
        )
        subprocess.run(
            f"sudo ip link set {veth_name} netns {ns_name}".split(), check=True
        )
        subprocess.run(
            f"sudo brctl addif {self.bridge_name} {veth_br_name}".split(), check=True
        )
        subprocess.run(
            f"sudo ip netns exec {ns_name} ip link set {veth_name} up".split(),
            check=True,
        )
        subprocess.run(f"sudo ip link set {veth_br_name} up".split(), check=True)


if __name__ == "__main__":
    pass