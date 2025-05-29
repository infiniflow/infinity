import argparse
import os
import platform
import shutil
import subprocess
import sys
from typing import Tuple
from colorama import Fore, Back, Style

from util import *

sys.stdout.reconfigure(encoding='utf-8') # for windows gh runner

expected_dir = os.path.join(os.path.dirname(os.path.realpath(__file__)), "../test/expected/")

def get_c_compiler_counterpart(compiler: str) -> str:
    return compiler.replace("clang++", "clang").replace("g++", "gcc")

MAX_LINE_DIFF = 2

def similarity(name: str, target: List[str]) -> int:
    parts = name.split(".txt")[0].split(".")
    c = 0
    for part in parts:
        if part in target:
            c += 1
        else:
            return -1
    return c

def output_matches(raw_output: str, params: Tuple[str]):
    target = []

    if params[0].startswith("gcc") or params[0].startswith("g++"):
        target.append("gcc")
    elif params[0].startswith("clang"):
        target.append("clang")
    elif params[0].startswith("cl"):
        target.append("msvc")

    if platform.system() == "Windows":
        target.append("windows")
    elif platform.system() == "Darwin":
        target.append("macos")
    else:
        target.append("linux")

    other_configs = params[1:]
    for config in other_configs:
        assert "WITH_" in config
        target.append(config.split("WITH_")[1].lower())

    print(f"Searching for expected file best matching {target}")

    files = [f for f in os.listdir(expected_dir) if os.path.isfile(os.path.join(expected_dir, f))]
    if len(files) == 0:
        print(f"Error: No expected files to use (searching {expected_dir})")
        sys.exit(1)
    files = list(map(lambda f: (f, similarity(f, target)), files))
    m = max(files, key=lambda entry: entry[1])[1]
    if m <= 0:
        print(f"Error: Could not find match for {target} in {files}")
        sys.exit(1)
    files = [entry[0] for entry in files if entry[1] == m]
    if len(files) > 1:
        print(f"Error: Ambiguous expected file to use ({files})")
        sys.exit(1)

    file = files[0]
    print(f"Reading from {file}")

    with open(os.path.join(expected_dir, file), "r") as f:
        raw_expected = f.read()

    if raw_output.strip() == "":
        print(f"Error: No output from test")
        return False

    expected = [line.strip().split("||") for line in raw_expected.split("\n")]
    output = [line.strip().split("||") for line in raw_output.split("\n")]

    max_line_diff = 0

    errored = False

    try:
        for i, ((output_file, output_line, output_symbol), (expected_file, expected_line, expected_symbol)) in enumerate(zip(output, expected)):
            if output_file != expected_file:
                print(f"Error: File name mismatch on line {i + 1}, found \"{output_file}\" expected \"{expected_file}\"")
                errored = True
            if abs(int(output_line) - int(expected_line)) > max_line_diff:
                print(f"Error: File line mismatch on line {i + 1}, found {output_line} expected {expected_line}")
                errored = True
            if output_symbol != expected_symbol:
                print(f"Error: File symbol mismatch on line {i + 1}, found \"{output_symbol}\" expected \"{expected_symbol}\"")
                errored = True
            if expected_symbol == "main" or expected_symbol == "main()":
                break
    except ValueError:
        print("ValueError during output checking")
        errored = True

    if errored:
        print("Output:")
        print(raw_output)
        print("Expected:")
        print(raw_expected)

    return not errored

def run_test(runner: MatrixRunner, test_binary, params: Tuple[str]):
    def output_matcher(output: str):
        return output_matches(output, params)
    return runner.run_command(test_binary, output_matcher=output_matcher)

def build(runner: MatrixRunner):
    matrix = runner.current_config()
    if platform.system() != "Windows":
        args = [
            "cmake",
            "..",
            f"-DCMAKE_BUILD_TYPE={matrix['target']}",
            f"-DCMAKE_CXX_COMPILER={matrix['compiler']}",
            f"-DCMAKE_C_COMPILER={get_c_compiler_counterpart(matrix['compiler'])}",
            f"-DCMAKE_CXX_STANDARD={matrix['std']}",
            f"-DCPPTRACE_USE_EXTERNAL_LIBDWARF=On",
            f"-DCPPTRACE_USE_EXTERNAL_ZSTD=On",
            f"-DCPPTRACE_WERROR_BUILD=On",
            f"-D{matrix['unwind']}=On",
            f"-D{matrix['symbols']}=On",
            f"-D{matrix['demangle']}=On",
            "-DCPPTRACE_BACKTRACE_PATH=/usr/lib/gcc/x86_64-linux-gnu/10/include/backtrace.h",
            "-DCPPTRACE_BUILD_TESTING=On",
            "-DCPPTRACE_SKIP_UNIT=On",
            f"-DBUILD_SHARED_LIBS={matrix['shared']}"
        ]
        if matrix['symbols'] == "CPPTRACE_GET_SYMBOLS_WITH_LIBDL":
           args.append("-DCPPTRACE_BUILD_TEST_RDYNAMIC=On")
        succeeded = runner.run_command(*args)
        if succeeded:
            return runner.run_command("make", "-j")
    else:
        args = [
            "cmake",
            "..",
            f"-DCMAKE_BUILD_TYPE={matrix['target']}",
            f"-DCMAKE_CXX_COMPILER={matrix['compiler']}",
            f"-DCMAKE_C_COMPILER={get_c_compiler_counterpart(matrix['compiler'])}",
            f"-DCMAKE_CXX_STANDARD={matrix['std']}",
            f"-DCPPTRACE_USE_EXTERNAL_LIBDWARF=On",
            f"-DCPPTRACE_USE_EXTERNAL_ZSTD=On",
            f"-DCPPTRACE_WERROR_BUILD=On",
            f"-D{matrix['unwind']}=On",
            f"-D{matrix['symbols']}=On",
            f"-D{matrix['demangle']}=On",
            "-DCPPTRACE_BUILD_TESTING=On",
            "-DCPPTRACE_SKIP_UNIT=On",
            f"-DBUILD_SHARED_LIBS={matrix['shared']}"
        ]
        if matrix["compiler"] == "g++":
            args.append("-GUnix Makefiles")
        succeeded = runner.run_command(*args)
        if succeeded:
            if matrix["compiler"] == "g++":
                return runner.run_command("make", "-j")
            else:
                return runner.run_command("msbuild", "cpptrace.sln")
    return False

def build_full_or_auto(runner: MatrixRunner):
    matrix = runner.current_config()
    if platform.system() != "Windows":
        args = [
            "cmake",
            "..",
            f"-DCMAKE_BUILD_TYPE={matrix['target']}",
            f"-DCMAKE_CXX_COMPILER={matrix['compiler']}",
            f"-DCMAKE_C_COMPILER={get_c_compiler_counterpart(matrix['compiler'])}",
            f"-DCMAKE_CXX_STANDARD={matrix['std']}",
            f"-DCPPTRACE_USE_EXTERNAL_LIBDWARF=On",
            f"-DCPPTRACE_USE_EXTERNAL_ZSTD=On",
            f"-DCPPTRACE_WERROR_BUILD=On",
            f"-DCPPTRACE_BACKTRACE_PATH=/usr/lib/gcc/x86_64-linux-gnu/10/include/backtrace.h",
            "-DCPPTRACE_BUILD_TESTING=On",
            "-DCPPTRACE_SKIP_UNIT=On",
            f"-DBUILD_SHARED_LIBS={matrix['shared']}"
        ]
        if matrix["config"] != "":
            args.append(f"{matrix['config']}")
        succeeded = runner.run_command(*args)
        if succeeded:
            return runner.run_command("make", "-j")
    else:
        args = [
            "cmake",
            "..",
            f"-DCMAKE_BUILD_TYPE={matrix['target']}",
            f"-DCMAKE_CXX_COMPILER={matrix['compiler']}",
            f"-DCMAKE_C_COMPILER={get_c_compiler_counterpart(matrix['compiler'])}",
            f"-DCMAKE_CXX_STANDARD={matrix['std']}",
            f"-DCPPTRACE_USE_EXTERNAL_LIBDWARF=On",
            f"-DCPPTRACE_USE_EXTERNAL_ZSTD=On",
            f"-DCPPTRACE_WERROR_BUILD=On",
            "-DCPPTRACE_BUILD_TESTING=On",
            "-DCPPTRACE_SKIP_UNIT=On",
            f"-DBUILD_SHARED_LIBS={matrix['shared']}"
        ]
        if matrix["config"] != "":
            args.append(f"{matrix['config']}")
        if matrix["compiler"] == "g++":
            args.append("-GUnix Makefiles")
        succeeded = runner.run_command(*args)
        if succeeded:
            if matrix["compiler"] == "g++":
                return runner.run_command("make", "-j")
            else:
                return runner.run_command("msbuild", "cpptrace.sln")
    return False

def test(runner: MatrixRunner):
    matrix = runner.current_config()
    if platform.system() != "Windows":
        return run_test(
            runner,
            "./integration",
            (matrix["compiler"], matrix["unwind"], matrix["symbols"], matrix["demangle"])
        )
    else:
        if matrix["compiler"] == "g++":
            return run_test(
                runner,
                f".\\integration.exe",
                (matrix["compiler"], matrix["unwind"], matrix["symbols"], matrix["demangle"])
            )
        else:
            return run_test(
                runner,
                f".\\{matrix['target']}\\integration.exe",
                (matrix["compiler"], matrix["unwind"], matrix["symbols"], matrix["demangle"])
            )

def test_full_or_auto(runner: MatrixRunner):
    matrix = runner.current_config()
    if platform.system() != "Windows":
        return run_test(
            runner,
            "./integration",
            (matrix["compiler"],)
        )
    else:
        if matrix["compiler"] == "g++":
            return run_test(
                runner,
                f".\\integration.exe",
                (matrix["compiler"],)
            )
        else:
            return run_test(
                runner,
                f".\\{matrix['target']}\\integration.exe",
                (matrix["compiler"],)
            )

def build_and_test(runner: MatrixRunner):
    matrix = runner.current_config()

    if os.path.exists("build"):
        shutil.rmtree("build", ignore_errors=True)

    if not os.path.exists("build"):
        os.mkdir("build")
    os.chdir("build")

    good = False
    if build(runner):
        good = test(runner)

    os.chdir("..")
    print()

    return good

def build_and_test_full_or_auto(runner: MatrixRunner):
    matrix = runner.current_config()

    if os.path.exists("build"):
        shutil.rmtree("build", ignore_errors=True)

    if not os.path.exists("build"):
        os.mkdir("build")
    os.chdir("build")

    good = False
    if build_full_or_auto(runner):
        good = test_full_or_auto(runner)

    os.chdir("..")
    print()

    return good

def run_linux_matrix(compilers: list, shared: bool):
    MatrixRunner(
        matrix = {
            "compiler": compilers,
            "target": ["Debug"],
            "std": ["11", "20"],
            "unwind": [
                "CPPTRACE_UNWIND_WITH_EXECINFO",
                "CPPTRACE_UNWIND_WITH_UNWIND",
                "CPPTRACE_UNWIND_WITH_LIBUNWIND",
                #"CPPTRACE_UNWIND_WITH_NOTHING",
            ],
            "symbols": [
                # Disabled due to libbacktrace bug
                # "CPPTRACE_GET_SYMBOLS_WITH_LIBBACKTRACE",
                "CPPTRACE_GET_SYMBOLS_WITH_LIBDL",
                "CPPTRACE_GET_SYMBOLS_WITH_ADDR2LINE",
                "CPPTRACE_GET_SYMBOLS_WITH_LIBDWARF",
                #"CPPTRACE_GET_SYMBOLS_WITH_NOTHING",
            ],
            "demangle": [
                "CPPTRACE_DEMANGLE_WITH_CXXABI",
                #"CPPTRACE_DEMANGLE_WITH_NOTHING",
            ],
            "shared": ["On" if shared else "Off"]
        },
        exclude = []
    ).run(build_and_test)

def run_linux_default(compilers: list, shared: bool):
    MatrixRunner(
        matrix = {
            "compiler": compilers,
            "target": ["Debug"],
            "std": ["11", "20"],
            "config": [""],
            "shared": ["On" if shared else "Off"]
        },
        exclude = []
    ).run(build_and_test_full_or_auto)

def run_macos_matrix(compilers: list, shared: bool):
    MatrixRunner(
        matrix = {
            "compiler": compilers,
            "target": ["Debug"],
            "std": ["11", "20"],
            "unwind": [
                "CPPTRACE_UNWIND_WITH_EXECINFO",
                "CPPTRACE_UNWIND_WITH_UNWIND",
                #"CPPTRACE_UNWIND_WITH_NOTHING",
            ],
            "symbols": [
                #"CPPTRACE_GET_SYMBOLS_WITH_LIBBACKTRACE",
                "CPPTRACE_GET_SYMBOLS_WITH_LIBDL",
                "CPPTRACE_GET_SYMBOLS_WITH_ADDR2LINE",
                "CPPTRACE_GET_SYMBOLS_WITH_LIBDWARF",
                #"CPPTRACE_GET_SYMBOLS_WITH_NOTHING",
            ],
            "demangle": [
                "CPPTRACE_DEMANGLE_WITH_CXXABI",
                #"CPPTRACE_DEMANGLE_WITH_NOTHING",
            ],
            "shared": ["On" if shared else "Off"]
        },
        exclude = []
    ).run(build_and_test)

def run_macos_default(compilers: list, shared: bool):
    MatrixRunner(
        matrix = {
            "compiler": compilers,
            "target": ["Debug"],
            "std": ["11", "20"],
            "config": [""],
            "shared": ["On" if shared else "Off"]
        },
        exclude = []
    ).run(build_and_test_full_or_auto)

def run_windows_matrix(compilers: list, shared: bool):
    MatrixRunner(
        matrix = {
            "compiler": compilers,
            "target": ["Debug"],
            "std": ["11", "20"],
            "unwind": [
                "CPPTRACE_UNWIND_WITH_WINAPI",
                "CPPTRACE_UNWIND_WITH_DBGHELP",
                "CPPTRACE_UNWIND_WITH_UNWIND", # Broken on github actions for some reason
                #"CPPTRACE_UNWIND_WITH_NOTHING",
            ],
            "symbols": [
                "CPPTRACE_GET_SYMBOLS_WITH_DBGHELP",
                "CPPTRACE_GET_SYMBOLS_WITH_ADDR2LINE",
                "CPPTRACE_GET_SYMBOLS_WITH_LIBDWARF",
                #"CPPTRACE_GET_SYMBOLS_WITH_NOTHING",
            ],
            "demangle": [
                "CPPTRACE_DEMANGLE_WITH_CXXABI",
                "CPPTRACE_DEMANGLE_WITH_NOTHING",
            ],
            "shared": ["On" if shared else "Off"]
        },
        exclude = [
            {
                "demangle": "CPPTRACE_DEMANGLE_WITH_CXXABI",
                "compiler": "cl"
            },
            {
                "unwind": "CPPTRACE_UNWIND_WITH_UNWIND",
                "compiler": "cl"
            },
            {
                "unwind": "CPPTRACE_UNWIND_WITH_UNWIND",
                "compiler": "clang++"
            },
            {
                "symbols": "CPPTRACE_GET_SYMBOLS_WITH_ADDR2LINE",
                "compiler": "cl"
            },
            {
                "symbols": "CPPTRACE_GET_SYMBOLS_WITH_ADDR2LINE",
                "compiler": "clang++"
            },
            {
                "symbols": "CPPTRACE_GET_SYMBOLS_WITH_LIBDWARF",
                "compiler": "cl"
            },
            {
                "symbols": "CPPTRACE_GET_SYMBOLS_WITH_LIBDWARF",
                "compiler": "clang++"
            },
            {
                "symbols": "CPPTRACE_GET_SYMBOLS_WITH_DBGHELP",
                "compiler": "g++"
            },
            {
                "symbols": "CPPTRACE_GET_SYMBOLS_WITH_DBGHELP",
                "demangle": "CPPTRACE_DEMANGLE_WITH_CXXABI"
            },
            {
                "symbols": "CPPTRACE_GET_SYMBOLS_WITH_LIBDWARF",
                "demangle": "CPPTRACE_DEMANGLE_WITH_NOTHING"
            },
            {
                "symbols": "CPPTRACE_GET_SYMBOLS_WITH_ADDR2LINE",
                "demangle": "CPPTRACE_DEMANGLE_WITH_NOTHING"
            }
        ]
    ).run(build_and_test)

def run_windows_default(compilers: list, shared: bool):
    MatrixRunner(
        matrix = {
            "compiler": compilers,
            "target": ["Debug"],
            "std": ["11", "20"],
            "config": [""],
            "shared": ["On" if shared else "Off"]
        },
        exclude = []
    ).run(build_and_test_full_or_auto)

def main():
    parser = argparse.ArgumentParser(
        prog="Build in all configs",
        description="Try building the library in all possible configurations for the current host"
    )
    parser.add_argument(
        "--clang",
        action="store_true"
    )
    parser.add_argument(
        "--gcc",
        action="store_true"
    )
    parser.add_argument(
        "--msvc",
        action="store_true"
    )
    parser.add_argument(
        "--all",
        action="store_true"
    )
    parser.add_argument(
        "--shared",
        action="store_true"
    )
    parser.add_argument(
        "--default-config",
        action="store_true"
    )
    args = parser.parse_args()

    if platform.system() == "Linux":
        compilers = []
        if args.clang or args.all:
            compilers.append("clang++-14")
        if args.gcc or args.all:
            compilers.append("g++-10")
        if args.default_config:
            run_linux_default(compilers, args.shared)
        else:
            run_linux_matrix(compilers, args.shared)
    if platform.system() == "Darwin":
        compilers = []
        if args.clang or args.all:
            compilers.append("clang++")
        if args.gcc or args.all:
            compilers.append("g++-12")
        if args.default_config:
            run_macos_default(compilers, args.shared)
        else:
            run_macos_matrix(compilers, args.shared)
    if platform.system() == "Windows":
        compilers = []
        if args.clang or args.all:
            compilers.append("clang++")
        if args.msvc or args.all:
            compilers.append("cl")
        if args.gcc or args.all:
            compilers.append("g++")
        if args.default_config:
            run_windows_default(compilers, args.shared)
        else:
            run_windows_matrix(compilers, args.shared)

main()
