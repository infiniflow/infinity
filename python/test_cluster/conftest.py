import pytest
from infinity_cluster import InfinityCluster
from mocked_infinity_cluster import MockInfinityCluster


def pytest_addoption(parser):
    parser.addoption(
        "--infinity_path",
        action="store",
        default="cmake-build-debug/src/infinity",
    )
    parser.addoption(
        "--use_sudo",
        action="store_true",
        default=False,
        help="Use sudo to run command",
    )


def pytest_generate_tests(metafunc):
    infinity_path = metafunc.config.getoption("infinity_path")

    use_sudo = metafunc.config.getoption("use_sudo")

    test_name = metafunc.function.__name__

    if "cluster" in metafunc.fixturenames:
        infinity_cluster = InfinityCluster(infinity_path, test_name=test_name)
        metafunc.parametrize("cluster", [infinity_cluster])
    elif "mock_cluster" in metafunc.fixturenames:
        mock_infinity_cluster = MockInfinityCluster(
            infinity_path,
            test_name=test_name,
            use_sudo=use_sudo,
        )
        metafunc.parametrize("mock_cluster", [mock_infinity_cluster])
