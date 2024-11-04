import os
import pytest
from infinity_cluster import InfinityCluster, MinioParams
from docker_infinity_cluster import DockerInfinityCluster
from mocked_infinity_cluster import MockInfinityCluster


def pytest_addoption(parser):
    parser.addoption(
        "--infinity_path",
        action="store",
        default="cmake-build-debug/src/infinity",
    )
    parser.addoption(
        "--minio_dir",
        action="store",
        default="minio",
    )
    parser.addoption(
        "--minio_port",
        action="store",
        default=9001,
    )
    parser.addoption(
        "--infinity_dir",
        action="store",
        required=True,
        help="Path to infinity directory. For local test, $pwd is ok",
    )
    parser.addoption("--docker", action="store_true", default=False)


def pytest_configure(config):
    config.addinivalue_line(
        "markers", "docker: mark test to run only when --docker option is provided"
    )


def pytest_collection_modifyitems(config, items):
    if config.getoption("--docker"):
        return  # do not skip docker test
    skip_docker = pytest.mark.skip(reason="need --docker option to run")
    for item in items:
        if "docker" in item.keywords:
            print(f"skip {item.name}")
            item.add_marker(skip_docker)


def pytest_generate_tests(metafunc):
    infinity_path = metafunc.config.getoption("infinity_path")
    minio_dir = metafunc.config.getoption("minio_dir")
    minio_port = metafunc.config.getoption("minio_port")
    minio_params = MinioParams(minio_dir, minio_port)

    infinity_dir = metafunc.config.getoption("infinity_dir")
    if len(infinity_dir) == 0:
        # raise ValueError("Please provide a valid infinity_dir")
        pass
    print("infinity_dir: ", infinity_dir)
    # print(metafunc.fixturenames)

    if "docker_cluster" in metafunc.fixturenames:
        # skip if docker is in option and the testcase is marked with docker
        if (
            not metafunc.config.getoption("--docker")
            and "docker" in metafunc.definition.keywords
        ):
            return

        print("Init DockerInfinityCluster")
        docker_infinity_cluster = DockerInfinityCluster(
            infinity_path, minio_params=minio_params, infinity_dir=infinity_dir
        )
        metafunc.parametrize("docker_cluster", [docker_infinity_cluster])
    elif "cluster" in metafunc.fixturenames:
        infinity_cluster = InfinityCluster(infinity_path, minio_params=minio_params)
        metafunc.parametrize("cluster", [infinity_cluster])
    elif "mock_cluster" in metafunc.fixturenames:
        mock_infinity_cluster = MockInfinityCluster(
            infinity_path, minio_params=minio_params
        )
        metafunc.parametrize("mock_cluster", [mock_infinity_cluster])
