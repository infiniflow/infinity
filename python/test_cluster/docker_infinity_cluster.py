import logging
import os
import random
import string
import sys
import time
import docker
from infinity_cluster import BaseInfinityRunner, InfinityCluster

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import http_network_util, infinity_http
from mocked_infinity_cluster import convert_request_to_curl


class docker_http_network(http_network_util):
    def __init__(self, container, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.container = container
        pass

    def request(self, url, method, header={}, data={}):
        if header is None:
            header = {}
        url = self.base_url + url
        logging.debug("url: " + url)

        cmd = convert_request_to_curl(method, header, data, url)
        rtn = self.container.exec_run(cmd)
        return rtn

    def raise_exception(self, resp, expect={}):
        # todo: handle curl exception
        pass


class DockerInfinityRunner(BaseInfinityRunner):
    def __init__(
        self,
        container: docker.models.containers.Container,
        mock_ip: str,
        *args,
        **kwargs,
    ):
        super().__init__(*args, **kwargs)
        self.container = container
        self.mock_ip = mock_ip

    def init(self, config_path: str | None):
        if config_path:
            self.config_path = config_path
            self.load_config()
        run_cmds = " && ".join(
            [f"cd /infinity", f"{self.executable_path} -c {self.config_path} 2>&1"]
        )
        self.container.exec_run(run_cmds, detach=True)

    def uninit(self):
        timeout = 60
        run_cmds = " && ".join(
            [
                "cd /infinity",
                f"pid=pgrep -f {self.executable_path}",
                f"./infinity/scripts/timeout_kill {timeout} $pid",
            ]
        )
        self.container.exec_run(run_cmds)
        self.container.stop()
        self.container.remove(force=True, v=True)

    def add_client(self, http_addr: str):
        self.client = infinity_http(net=docker_http_network(self.container, http_addr))

    def peer_uri(self):
        peer_port = self.network_config["peer_port"]
        return self.mock_ip, peer_port


class DockerInfinityCluster(InfinityCluster):
    def __init__(
        self,
        executable_path: str,
        image_name: str = "infiniflow/infinity_builder:centos7_clang18",
    ):
        super().__init__(executable_path)
        self.docker_client = docker.from_env()
        self.image_name = image_name

        self.network = self.docker_client.networks.create(
            "infinity_network",
            driver="bridge",
        )

    def clear(self):
        for runner in self.runners.values():
            runner.uninit()
        self.network.remove()

    def add_node(self, node_name: str, config_path: str):
        if node_name in self.runners:
            raise ValueError(f"Node {node_name} already exists in the cluster.")
        container_name, cpus, tz = self.__init_docker_params()
        pwd = os.getcwd()
        container = self.docker_client.containers.run(
            image=self.image_name,
            name=container_name,
            detach=True,
            cpuset_cpus=f"0-{cpus - 1}",
            volumes=[f"{pwd}:/infinity", "/boot:/boot"],
            environment=[f"TZ={tz}"],
        )

        self.network.connect(container)
        info = self.docker_client.api.inspect_network(self.network.id)
        # print(info)
        mock_ip = info["Containers"][container.id]["IPv4Address"]

        runner = DockerInfinityRunner(
            container, mock_ip, node_name, self.executable_path, config_path
        )
        self.runners[node_name] = runner

    def remove_node(self, node_name: str):
        if node_name not in self.runners:
            raise ValueError(f"Node {node_name} not found in the cluster.")
        cur_runner: DockerInfinityRunner = self.runners[node_name]
        self.network.disconnect(cur_runner.container)
        cur_runner.uninit()
        del self.runners[node_name]

    def disconnect(self, node_name: str):
        if node_name not in self.runners:
            raise ValueError(f"Node {node_name} not found in the cluster.")
        cur_runner: DockerInfinityRunner = self.runners[node_name]
        self.network.disconnect(cur_runner.container)

    def reconnect(self, node_name: str):
        if node_name not in self.runners:
            raise ValueError(f"Node {node_name} not found in the cluster.")
        cur_runner: DockerInfinityRunner = self.runners[node_name]
        self.network.connect(cur_runner.container)
        info = self.docker_client.api.inspect_network(self.network.id)
        # print(info)
        mock_ip = info["Containers"][cur_runner.container.id]["IPv4Address"]
        cur_runner.mock_ip = mock_ip

    def __init_docker_params(self):
        container_name = f"infinity_build_{''.join(random.choices(string.ascii_lowercase + string.digits, k=8))}"
        cpus = os.cpu_count()
        tz = os.readlink("/etc/localtime").split("/zoneinfo/")[1]
        return container_name, cpus, tz


if __name__ == "__main__":
    pass
