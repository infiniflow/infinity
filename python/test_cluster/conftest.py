def pytest_addoption(parser):
    parser.addoption(
        "--infinity_path",
        action="store",
        default="./build/Debug/src/infinity",
    )


def pytest_generate_tests(metafunc):
    if "infinity_path" in metafunc.fixturenames:
        infinity_path = metafunc.config.getoption("infinity_path")
        metafunc.parametrize("infinity_path", [infinity_path])
