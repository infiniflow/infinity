import json
import logging
import os
import random
import string
import sys
import time
from numpy import dtype
import pandas as pd
import tomli
import tomli_w
import docker
from infinity_cluster import (
    BaseInfinityRunner,
    InfinityCluster,
    MinioParams,
    convert_request_to_curl,
)
from mocked_infinity_cluster import convert_request_to_curl
import shutil

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)
from infinity_http import http_network_util, infinity_http


class docker_http_response:
    def __init__(self, json_output):
        self.json_output = json_output

    def json(self):
        return self.json_output


class docker_http_network(http_network_util):
    def __init__(self, container, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.container = container

    def request(self, url, method, header={}, data={}):
        if header is None:
            header = {}
        url = self.base_url + url
        logging.debug("url: " + url)

        cmd = convert_request_to_curl(method, header, data, url)
        print(cmd)
        exit_code, output = self.container.exec_run(cmd)
        print(output)
        assert exit_code is None or exit_code == 0
        try:
            return docker_http_response(json.loads(output))
        except json.JSONDecodeError as e:
            logging.error(f"Failed to decode JSON response: {e}")
            raise

    def raise_exception(self, resp, expect={}):
        # todo: handle curl exception
        pass


class DockerInfinityRunner(BaseInfinityRunner):
    def __init__(
        self,
        container: docker.models.containers.Container,
        mock_ip: str,
        minio_ip: str | None,
        *args,
        **kwargs,
    ):
        self.minio_ip = minio_ip
        self.container = container
        self.mock_ip = mock_ip
        super().__init__(*args, **kwargs)

    def init(self, config_path: str | None):
        if config_path:
            if self.config_path is not None and os.path.exists(self.config_path):
                os.remove(self.config_path)
            self.config_path = config_path
            self.load_config()
        run_cmds = " && ".join(
            [
                "cd /infinity",
                f"{self.executable_path} --config={self.config_path} 2>&1",
            ]
        )
        print(run_cmds)
        exit_code, output = self.container.exec_run(
            f"bash -c '{run_cmds}'", detach=True
        )
        print(f"init infinity: {output}")
        assert exit_code is None or exit_code == 0

    def uninit(self):
        timeout = 60
        run_cmds = " && ".join(
            [
                "cd /infinity",
                f"pid=$(pgrep -f infinity || true)",
                f"bash scripts/timeout_kill.sh {timeout} $pid",
            ]
        )
        print(run_cmds)
        exit_code, output = self.container.exec_run(f"bash -c '{run_cmds}'")
        print(f"uninit infinity: {output}")
        # assert exit_code is None or exit_code == 0
        # self.container.stop()
        # self.container.remove(force=True, v=True)

        if os.path.exists(self.config_path):
            os.remove(self.config_path)

    def add_client(self, http_addr: str):
        http_addr = http_addr.replace("http://", "")
        http_ip, http_port = http_addr.split(":")
        mock_addr = f"http://{self.mock_ip}:{http_port}"
        print(f"add client: {mock_addr}")
        self.client = infinity_http(net=docker_http_network(self.container, mock_addr))

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
            self.network_config = config["network"]

            if self.minio_ip is not None:
                minio_url = config["storage"]["object_storage"]["url"]
                minio_ip, minio_port = minio_url.split(":")

                new_minio_url = f"{self.minio_ip}:{minio_port}"
                config["storage"]["object_storage"]["url"] = new_minio_url

            config["network"]["server_address"] = self.mock_ip
            config["network"]["peer_ip"] = self.mock_ip

            with open(self.config_path, "wb") as f:
                tomli_w.dump(config, f)


class DockerInfinityCluster(InfinityCluster):
    def __init__(
        self,
        executable_path: str,
        *,
        minio_params: MinioParams = None,
        infinity_dir: str,
    ):
        docker_client = docker.from_env()
        network_name = "infinity_network"
        try:
            self.network = docker_client.networks.get(network_name)
        except docker.errors.NotFound:
            self.network = docker_client.networks.create(
                network_name,
                driver="bridge",
            )
        super().__init__(executable_path, minio_params=minio_params)
        self.infinity_dir = infinity_dir

        image_name = "infiniflow/infinity_builder:centos7_clang18"
        self.image_name = image_name

        if minio_params is not None:
            add = self.add_minio(minio_params)
            if add:
                self.network.connect(self.minio_container)
            info = docker_client.api.inspect_network(self.network.id)
            minio_ip = info["Containers"][self.minio_container.id]["IPv4Address"]
            minio_ip = minio_ip.split("/")[0]
            self.minio_ip = minio_ip
            self.minio_params = minio_params

    def add_minio(self, minio_params: MinioParams):
        minio_image_name = "quay.io/minio/minio"

        minio_cmd = f'server /data --console-address ":{minio_params.minio_port}"'
        docker_client = docker.from_env()
        kargs = {}
        container_name = "minio_docker"

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
                **kargs,
            )
            self.network.connect(self.minio_container)
        info = docker_client.api.inspect_network(self.network.id)
        minio_ip = info["Containers"][self.minio_container.id]["IPv4Address"]
        minio_ip = minio_ip.split("/")[0]
        self.minio_ip = minio_ip
        self.minio_params = minio_params

    def clear(self):
        super().clear()
        for runner in self.runners.values():
            runner.uninit()
        # self.network.remove()

    def add_node(self, node_name: str, config_path: str):
        if node_name in self.runners:
            raise ValueError(f"Node {node_name} already exists in the cluster.")
        container_name, cpus, tz = self.__init_docker_params()
        pwd = self.infinity_dir
        print(f"pwd: {pwd}")
        docker_client = docker.from_env()

        try:
            container = docker_client.containers.get(container_name)
        except docker.errors.NotFound:
            container = docker_client.containers.run(
                image=self.image_name,
                name=container_name,
                detach=True,
                cpuset_cpus=f"0-{cpus - 1}",
                volumes=["/boot:/boot"],
                mounts=[
                    docker.types.Mount(target="/infinity", source=pwd, type="bind")
                ],
                environment=[f"TZ={tz}"],
            )

        try:
            self.network.connect(container)
        except docker.errors.APIError as e:
            pass
        info = docker_client.api.inspect_network(self.network.id)
        # print(info)
        mock_ip = info["Containers"][container.id]["IPv4Address"]
        mock_ip = mock_ip.split("/")[0]

        runner = DockerInfinityRunner(
            container,
            mock_ip,
            self.minio_ip,
            node_name,
            self.executable_path,
            config_path,
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
        docker_client = docker.from_env()
        info = docker_client.api.inspect_network(self.network.id)
        # print(info)
        mock_ip = info["Containers"][cur_runner.container.id]["IPv4Address"]
        cur_runner.mock_ip = mock_ip

    def __init_docker_params(self):
        container_name = f"infinity_build_{len(self.runners)}"
        cpus = os.cpu_count()
        tz = os.readlink("/etc/localtime").split("/zoneinfo/")[1]
        return container_name, cpus, tz


if __name__ == "__main__":
    pass
