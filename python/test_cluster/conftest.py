import os
import pytest
from infinity_cluster import InfinityCluster, MinioParams
from docker_infinity_cluster import DockerInfinityCluster
from mocked_infinity_cluster import MockInfinityCluster

@pytest.fixture(scope="function")
def skip_if_docker(request):
    if request.config.getoption("--docker"):
        pytest.skip("Skipping docker test")


@pytest.fixture(scope="function")
def skip_if_not_docker(request):
    if not request.config.getoption("--docker"):
        pytest.skip("Skipping not docker test")


def pytest_addoption(parser):
    parser.addoption(
        "--infinity_path",
        action="store",
        default="cmake-build-debug/src/infinity",
    )
    parser.addoption(
        "--docker",
        action="store_true",
        default=False,
        help="Run cluster test on docker",
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
        default="",
    )


def pytest_generate_tests(metafunc):
    infinity_path = metafunc.config.getoption("infinity_path")
    minio_dir = metafunc.config.getoption("minio_dir")
    minio_port = metafunc.config.getoption("minio_port")
    minio_params = MinioParams(minio_dir, minio_port)

    infinity_dir = metafunc.config.getoption("infinity_dir")
    if len(infinity_dir) == 0:
        infinity_dir = os.getcwd()
    print("infinity_dir: ", infinity_dir)
    # print(metafunc.fixturenames)

    if metafunc.config.getoption("--docker"):
        print("Docker argument is provided")
        if "docker_cluster" in metafunc.fixturenames:
            docker_infinity_cluster = DockerInfinityCluster(infinity_path, minio_params=minio_params, infinity_dir=infinity_dir)
            metafunc.parametrize("docker_cluster", [docker_infinity_cluster])
    else:
        print("Docker argument is not provided")
        if "cluster" in metafunc.fixturenames:
            infinity_cluster = InfinityCluster(infinity_path, minio_params=minio_params, infinity_dir=infinity_dir)
            metafunc.parametrize("cluster", [infinity_cluster])
        elif "mock_cluster" in metafunc.fixturenames:
            mock_infinity_cluster = MockInfinityCluster(infinity_path, minio_params=minio_params, infinity_dir=infinity_dir)
            metafunc.parametrize("mock_cluster", [mock_infinity_cluster])
