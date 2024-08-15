from infinity_runner import InfinityRunner


def pytest_addoption(parser):
    parser.addoption(
        "--infinity_path",
        action="store",
        default="./build/Debug/src/infinity",
    )
    parser.addoption(
        "--builder_container",
        action="store",
    )


def pytest_generate_tests(metafunc):
    if "infinity_runner" in metafunc.fixturenames:
        infinity_path = metafunc.config.getoption("infinity_path")
        runner = InfinityRunner(infinity_path)
        metafunc.parametrize("infinity_runner", [runner])
        pass
