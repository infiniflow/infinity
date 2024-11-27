import json
import logging
import platform
import shutil
import subprocess
import sys

import tomli
import tomli_w
from infinity_cluster import (
    InfinityRunner,
    InfinityCluster,
    MinioParams,
    convert_request_to_curl,
)
import os
from infinity.errors import ErrorCode
from infinity.common import InfinityException

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import http_network_util, infinity_http


class http_result:
    def __init__(self, error_code: int, json: json):
        self.error_code = error_code
        self.json_ = json

    def json(self):
        return self.json_


class mocked_http_network(http_network_util):
    def __init__(self, ns_name: str, logger: logging.Logger, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.ns_name = ns_name
        self.logger = logger

    def request_inner(self, url, method, header={}, data={}) -> http_result:
        logging.debug("url: " + url)

        cmd = convert_request_to_curl(method, header, data, url)
        cmd = f"ip netns exec {self.ns_name} {cmd}"
        self.logger.debug(f"cmd: {cmd}")
        try:
            result = subprocess.run(cmd, shell=True, capture_output=True, text=True, check=True)
        except subprocess.CalledProcessError as e:
            raise InfinityException(error_code=ErrorCode.INFINITY_IS_INITING, error_message=str(e))
        if len(result.stdout) == 0:
            return http_result(0, None)
        try:
            result = json.loads(result.stdout)
        except json.JSONDecodeError as e:
            raise Exception(f"Failed to parse response: {e}")
        error_code = result["error_code"]
        result.pop("error_code")
        return http_result(error_code, result)

    def raise_exception(self, resp):
        if resp.error_code != 0:
            self.logger.error(resp)
            raise InfinityException(
                error_code=resp.error_code, error_message=resp.json()["error_message"]
            )


class MockedInfinityRunner(InfinityRunner):
    def __init__(
        self,
        mock_ip: str,
        host_ip: str,
        ns_name: str,
        node_name: str,
        executable_path: str,
        config_path: str,
        logger: logging.Logger,
    ):
        self.mock_ip = mock_ip
        self.host_ip = host_ip
        self.ns_name = ns_name
        super().__init__(node_name, executable_path, config_path, logger=logger)

    def init(self, config_path: str | None):
        if self.process is not None:
            raise ValueError("Process is already initialized.")
        if config_path:
            self.config_path = config_path
            self.load_config()

        run_cmd = f"{self.executable_path} --config={self.config_path} 2>&1"
        run_cmd = f"ip netns exec {self.ns_name} {run_cmd}"
        self.process = subprocess.Popen(run_cmd, shell=True)
        self.check_start()

    def add_client(self, http_addr: str):
        net = mocked_http_network(self.ns_name, self.logger, http_addr)
        net.set_retry()
        self.client = infinity_http(net=net)

    def peer_uri(self):
        peer_port = self.network_config["peer_port"]
        return self.mock_ip, peer_port

    def load_config(self):
        if not os.path.basename(self.config_path).startswith("mock_"):
            config_dir, config_filename = os.path.split(self.config_path)
            mock_config_path = os.path.join(config_dir, f"mock_{config_filename}")
            shutil.copyfile(self.config_path, mock_config_path)
            self.config_path = mock_config_path
        with open(self.config_path, "rb") as f:
            config = tomli.load(f)
            obj_storage_config = config["storage"]["object_storage"]
            minio_url = obj_storage_config["url"]
            minio_ip, minio_port = minio_url.split(":")
            new_minio_url = f"{self.host_ip}:{minio_port}"
            obj_storage_config["url"] = new_minio_url

            network_config = config["network"]
            network_config["server_address"] = self.mock_ip
            network_config["peer_ip"] = self.mock_ip

            with open(self.config_path, "wb") as f:
                tomli_w.dump(config, f)
        super().load_config()


class MockInfinityCluster(InfinityCluster):
    def __init__(
        self,
        executable_path: str,
        *,
        minio_params: MinioParams = None,
        test_name: str = None,
    ):
        super().__init__(
            executable_path, minio_params=minio_params, test_name=test_name
        )
        self.ns_prefix = "ns"
        self.bridge_name = "br0"
        self.mock_ip_prefix = "17.0.0."
        self.mock_ip_mask = 24
        self.cur_ip_suffix = 1

    def __enter__(self):
        super().__enter__()
        self._check_prerequisites()
        self._prepare_bridge()
        return self

    def _clear_node_ns(self, node_name: str):
        cur_runner: MockedInfinityRunner = self.runners[node_name]
        ns_name = cur_runner.ns_name
        veth_name, veth_br_name = self.__veth_name_pair(ns_name)

        subprocess.run(
            f"ip netns exec {ns_name} ip link set {veth_name} down".split(),
            check=True,
        )
        subprocess.run(f"ip link set {veth_br_name} down".split(), check=True)
        subprocess.run(f"ip link delete {veth_br_name}".split(), check=True)
        subprocess.run(f"ip netns delete {ns_name}".split(), check=True)

    def clear(self):
        subprocess.run(f"ip link set {self.bridge_name} down".split(), check=True)
        subprocess.run(f"ip link delete {self.bridge_name}".split(), check=True)
        for node_name in self.runners.keys():
            self._clear_node_ns(node_name)
        super().clear()

    def add_node(self, node_name: str, config_path: str):
        if node_name in self.runners:
            raise ValueError(f"Node {node_name} already exists in the cluster.")
        ns_name = f"{self.ns_prefix}_{node_name}"
        mock_ip = self._connect_to_bridge(ns_name, ping=True)
        runner = MockedInfinityRunner(
            mock_ip,
            self.host_ip,
            ns_name,
            node_name,
            self.executable_path,
            config_path,
            self.logger,
        )
        self.runners[node_name] = runner

    def remove_node(self, node_name: str):
        self._clear_node_ns(node_name)
        super().remove_node(node_name)

    def disconnect(self, node_name: str):
        if node_name not in self.runners:
            raise ValueError(f"Node {node_name} not found in the cluster.")
        cur_runner: MockedInfinityRunner = self.runners[node_name]
        veth_name, veth_br_name = self.__veth_name_pair(cur_runner.ns_name)
        subprocess.run(f"ip link set {veth_br_name} down".split())
        subprocess.run(f"ip link set {veth_br_name} nomaster".split())

    def reconnect(self, node_name: str):
        if node_name not in self.runners:
            raise ValueError(f"Node {node_name} not found in the cluster.")
        cur_runner: MockedInfinityRunner = self.runners[node_name]
        veth_name, veth_br_name = self.__veth_name_pair(cur_runner.ns_name)
        subprocess.run(
            f"ip link set {veth_br_name} master {self.bridge_name}".split()
        )
        subprocess.run(f"ip link set {veth_br_name} up".split())

    def _next_mock_ip(self):
        mock_ip = f"{self.mock_ip_prefix}{self.cur_ip_suffix}/{self.mock_ip_mask}"
        self.cur_ip_suffix += 1
        return mock_ip

    def _check_prerequisites(self):
        if platform.system() != "Linux":
            self.logger.error("Network namespaces are only supported on Linux.")
            exit()

    def _prepare_bridge(self):
        subprocess.run(f"ip link set {self.bridge_name} down".split())
        subprocess.run(f"ip link delete {self.bridge_name}".split())
        subprocess.run(
            f"ip link add {self.bridge_name} type bridge".split(), check=True
        )
        mock_ip = self._next_mock_ip()
        subprocess.run(
            f"ip addr add {mock_ip} dev {self.bridge_name}".split(), check=True
        )
        self.host_ip, mask = mock_ip.split("/")
        subprocess.run(f"ip link set {self.bridge_name} up".split(), check=True)
        subprocess.run(
            f"iptables -A FORWARD -i {self.bridge_name} -j ACCEPT".split(),
            check=True,
        )
        # subprocess.run(f"sysctl -w net.ipv4.ip_forward=1".split(), check=True)

    def __veth_name_pair(self, ns_name: str):
        veth_name = f"{ns_name}_v"
        veth_br_name = f"{ns_name}_vbr"
        if len(veth_name) > 15 or len(veth_br_name) > 15:
            raise ValueError(f"veth name too long: {veth_name}, {veth_br_name}")
        return veth_name, veth_br_name

    def _connect_to_bridge(self, ns_name: str, ping: bool = False):
        subprocess.run(f"ip netns delete {ns_name}".split())
        subprocess.run(f"ip netns add {ns_name}".split(), check=True)
        veth_name, veth_br_name = self.__veth_name_pair(ns_name)
        subprocess.run(f"ip link delete {veth_name}".split())
        subprocess.run(f"ip link delete {veth_br_name}".split())
        subprocess.run(
            f"ip link add {veth_name} type veth peer name {veth_br_name}".split(),
            check=True,
        )
        subprocess.run(
            f"ip link set {veth_name} netns {ns_name}".split(), check=True
        )
        subprocess.run(
            f"ip netns exec {ns_name} ip link set lo up".split(), check=True
        )
        subprocess.run(
            f"ip link set {veth_br_name} master {self.bridge_name}".split()
        )
        mock_ip = self._next_mock_ip()
        self.logger.info(f"ns_name: {ns_name}, mock_ip: {mock_ip}")
        subprocess.run(
            f"ip netns exec {ns_name} ip addr add {mock_ip} dev {veth_name}".split(),
        )
        subprocess.run(
            f"ip netns exec {ns_name} ip link set {veth_name} up".split(),
            check=True,
        )
        subprocess.run(f"ip link set {veth_br_name} up".split(), check=True)

        if ping:
            if self.host_ip is not None:
                subprocess.run(
                    f"ip netns exec {ns_name} ping -c 1 {self.host_ip}".split(),
                    check=True,
                )
        mock_ip, mask = mock_ip.split("/")
        return mock_ip


if __name__ == "__main__":
    pass
