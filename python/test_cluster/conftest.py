import pytest


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
        default="./build/Debug/src/infinity",
    )
    parser.addoption(
        "--docker",
        action="store_true",
        default=False,
        help="Run cluster test on docker",
    )


def pytest_generate_tests(metafunc):
    if "infinity_path" in metafunc.fixturenames:
        infinity_path = metafunc.config.getoption("infinity_path")
        metafunc.parametrize("infinity_path", [infinity_path])
