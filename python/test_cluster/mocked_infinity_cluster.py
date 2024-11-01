import json
import logging
import platform
import subprocess
import sys
from infinity_cluster import (
    InfinityRunner,
    InfinityCluster,
    MinioParams,
    convert_request_to_curl,
)
import os

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import http_network_util, infinity_http


class mocked_http_network(http_network_util):
    def __init__(self, ns_name: str, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.ns_name = ns_name

    def request(self, url, method, header={}, data={}):
        if header is None:
            header = {}
        url = self.base_url + url
        logging.debug("url: " + url)

        cmd = convert_request_to_curl(method, header, data, url)
        cmd = f"sudo ip netns exec {self.ns_name} {cmd}"
        print(f"cmd: {cmd}")
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        return result.stdout

    def raise_exception(self, resp, expect={}):
        # todo: handle curl exception
        pass


class MockedInfinityRunner(InfinityRunner):
    def __init__(
        self,
        mock_ip: str,
        ns_name: str,
        node_name: str,
        executable_path: str,
        config_path: str,
    ):
        super().__init__(node_name, executable_path, config_path)
        self.mock_ip = mock_ip
        self.ns_name = ns_name

    def init(self, config_path: str | None):
        if self.process is not None:
            raise ValueError("Process is already initialized.")
        if config_path:
            self.config_path = config_path
            self.load_config()

        run_cmd = f"{self.executable_path} --config={self.config_path} 2>&1"
        run_cmd = f"sudo ip netns exec {self.ns_name} {run_cmd}"
        self.process = subprocess.Popen(run_cmd, shell=True)

    def add_client(self, http_addr: str):
        self.client = infinity_http(net=mocked_http_network(self.ns_name, http_addr))

    def peer_uri(self):
        peer_port = self.network_config["peer_port"]
        return self.mock_ip, peer_port


class MockInfinityCluster(InfinityCluster):
    def __init__(
        self,
        executable_path: str,
        *,
        minio_params: MinioParams = None,
        infinity_dir: str,
    ):
        super().__init__(executable_path, minio_params=minio_params, infinity_dir=infinity_dir)
        self.ns_prefix = "ns"
        self.bridge_name = "br0"
        self.mock_ip_prefix = "17.0.0."
        self.mock_ip_mask = 24
        self.cur_ip_suffix = 1
        self.first_mock_ip = None  # for ping test
        self.__check_prerequisites()
        self.__prepare_bridge()

    def clear(self):
        subprocess.run(f"sudo ip link set {self.bridge_name} down".split())
        subprocess.run(f"sudo brctl delbr {self.bridge_name}".split())
        for ns_name in self.runners:
            subprocess.run(f"sudo ip netns delete {ns_name}".split())
            veht_name, _ = self.__veth_name_pair(ns_name)
            subprocess.run(f"sudo ip link delete {veht_name}".split())

    def add_node(self, node_name: str, config_path: str):
        if node_name in self.runners:
            raise ValueError(f"Node {node_name} already exists in the cluster.")
        ns_name = f"{self.ns_prefix}_{node_name}"
        mock_ip = self.__connect_to_bridge(ns_name, ping=False)
        runner = MockedInfinityRunner(
            mock_ip, ns_name, node_name, self.executable_path, config_path
        )
        self.runners[node_name] = runner

    def remove_node(self, node_name: str):
        if node_name not in self.runners:
            raise ValueError(f"Node {node_name} not found in the cluster.")
        cur_runner: MockedInfinityRunner = self.runners[node_name]
        ns_name = cur_runner.ns_name
        veth_name, veth_br_name = self.__veth_name_pair(ns_name)

        subprocess.run(
            f"sudo ip netns exec {ns_name} ip link set {veth_name} down".split(),
            check=True,
        )
        subprocess.run(
            f"sudo brctl delif {self.bridge_name} {veth_br_name}".split(), check=True
        )
        subprocess.run(f"sudo ip link delete {veth_br_name}".split(), check=True)
        subprocess.run(f"sudo ip netns delete {ns_name}".split(), check=True)

        cur_runner.uninit()
        del self.runners[node_name]

    def disconnect(self, node_name: str):
        if node_name not in self.runners:
            raise ValueError(f"Node {node_name} not found in the cluster.")
        cur_runner: MockedInfinityRunner = self.runners[node_name]
        veth_name, veth_br_name = self.__veth_name_pair(cur_runner.ns_name)
        subprocess.run(f"sudo ip link set {veth_br_name} down".split())
        subprocess.run(f"sudo brctl delif {self.bridge_name} {veth_br_name}".split())

    def reconnect(self, node_name: str):
        if node_name not in self.runners:
            raise ValueError(f"Node {node_name} not found in the cluster.")
        cur_runner: MockedInfinityRunner = self.runners[node_name]
        veth_name, veth_br_name = self.__veth_name_pair(cur_runner.ns_name)
        subprocess.run(f"sudo brctl addif {self.bridge_name} {veth_br_name}".split())
        subprocess.run(f"sudo ip link set {veth_br_name} up".split())

    def __next_mock_ip(self):
        mock_ip = f"{self.mock_ip_prefix}{self.cur_ip_suffix}/{self.mock_ip_mask}"
        self.cur_ip_suffix += 1
        if self.first_mock_ip is None:
            self.first_mock_ip = mock_ip
        return mock_ip

    def __check_prerequisites(self):
        if platform.system() != "Linux":
            print("Network namespaces are only supported on Linux.")
            exit()

    def __prepare_bridge(self):
        subprocess.run(f"sudo ip link set {self.bridge_name} down".split())
        subprocess.run(f"sudo brctl delbr {self.bridge_name}".split())
        subprocess.run(f"sudo brctl addbr {self.bridge_name}".split(), check=True)
        subprocess.run(f"sudo ip link set {self.bridge_name} up".split(), check=True)
        subprocess.run(
            f"sudo iptables -A FORWARD -i {self.bridge_name} -j ACCEPT".split(),
            check=True,
        )

    def __veth_name_pair(self, ns_name: str):
        veth_name = f"{ns_name}_v"
        veth_br_name = f"{ns_name}_vbr"
        if len(veth_name) > 15 or len(veth_br_name) > 15:
            raise ValueError(f"veth name too long: {veth_name}, {veth_br_name}")
        return veth_name, veth_br_name

    def __connect_to_bridge(self, ns_name: str, ping: bool = False):
        subprocess.run(f"sudo ip netns delete {ns_name}".split())
        subprocess.run(f"sudo ip netns add {ns_name}".split(), check=True)
        subprocess.run(
            f"sudo ip netns exec {ns_name} ip link set lo up".split(), check=True
        )
        veth_name, veth_br_name = self.__veth_name_pair(ns_name)
        subprocess.run(f"sudo ip link delete {veth_name}".split())
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
        mock_ip = self.__next_mock_ip()
        print(f"ns_name: {ns_name}, mock_ip: {mock_ip}")
        subprocess.run(
            f"sudo ip netns exec {ns_name} ip addr add {mock_ip} dev {veth_name}".split(),
        )
        subprocess.run(
            f"sudo ip netns exec {ns_name} ip link set {veth_name} up".split(),
            check=True,
        )
        subprocess.run(f"sudo ip link set {veth_br_name} up".split(), check=True)

        if ping and self.first_mock_ip is not None:
            subprocess.run(
                f"sudo ip netns exec {ns_name} ping -c 3 {self.first_mock_ip}".split()
            )
        return mock_ip


if __name__ == "__main__":
    pass
