import argparse
import os
import platform
import shutil
import subprocess
import sys
from colorama import Fore, Back, Style
from pathlib import Path

from util import *

sys.stdout.reconfigure(encoding='utf-8') # for windows gh runner

def build(runner: MatrixRunner):
    matrix = runner.current_config()

    if os.path.exists("build"):
        shutil.rmtree("build", ignore_errors=True)

    os.makedirs("build", exist_ok=True)
    os.chdir("build")

    if platform.system() != "Windows":
        succeeded = runner.run_command(
            "cmake",
            "..",
            "-GNinja",
            f"-DCMAKE_BUILD_TYPE={matrix['target']}",
            f"-DCMAKE_CXX_COMPILER={matrix['compiler']}",
            f"-DCMAKE_CXX_STANDARD={matrix['std']}",
            f"-DCPPTRACE_USE_EXTERNAL_LIBDWARF=On",
            f"-DCPPTRACE_USE_EXTERNAL_ZSTD=On",
            f"-DCPPTRACE_WERROR_BUILD=On",
            f"-D{matrix['unwind']}=On",
            f"-D{matrix['symbols']}=On",
            f"-D{matrix['demangle']}=On",
            "-DCPPTRACE_BACKTRACE_PATH=/usr/lib/gcc/x86_64-linux-gnu/10/include/backtrace.h",
        )
        if succeeded:
            succeeded = runner.run_command("ninja")
    else:
        args = [
            "cmake",
            "..",
            f"-DCMAKE_BUILD_TYPE={matrix['target']}",
            f"-DCMAKE_CXX_COMPILER={matrix['compiler']}",
            f"-DCMAKE_CXX_STANDARD={matrix['std']}",
            f"-DCPPTRACE_USE_EXTERNAL_LIBDWARF=On",
            f"-DCPPTRACE_USE_EXTERNAL_ZSTD=On",
            f"-DCPPTRACE_WERROR_BUILD=On",
            f"-D{matrix['unwind']}=On",
            f"-D{matrix['symbols']}=On",
            f"-D{matrix['demangle']}=On",
        ]
        if matrix["compiler"] == "g++":
            args.append("-GUnix Makefiles")
        succeeded = runner.run_command(*args)
        if succeeded:
            if matrix["compiler"] == "g++":
                succeeded = runner.run_command("make", "-j", "VERBOSE=1")
            else:
                succeeded = runner.run_command("msbuild", "cpptrace.sln")

    os.chdir("..")
    print()

    return succeeded

def run_linux_matrix(compilers: list):
    MatrixRunner(
        matrix = {
            "compiler": compilers,
            "target": ["Debug"],
            "std": ["11", "20"],
            "unwind": [
                # "CPPTRACE_UNWIND_WITH_UNWIND",
                # "CPPTRACE_UNWIND_WITH_EXECINFO",
                # "CPPTRACE_UNWIND_WITH_LIBUNWIND",
                "CPPTRACE_UNWIND_WITH_NOTHING",
            ],
            "symbols": [
                "CPPTRACE_GET_SYMBOLS_WITH_LIBBACKTRACE",
                # "CPPTRACE_GET_SYMBOLS_WITH_LIBDL",
                # "CPPTRACE_GET_SYMBOLS_WITH_LIBDWARF",
                # "CPPTRACE_GET_SYMBOLS_WITH_ADDR2LINE",
                "CPPTRACE_GET_SYMBOLS_WITH_NOTHING",
            ],
            "demangle": [
                # "CPPTRACE_DEMANGLE_WITH_CXXABI",
                "CPPTRACE_DEMANGLE_WITH_NOTHING",
            ],
        },
        exclude = []
    ).run(build)

def run_macos_matrix(compilers: list):
    MatrixRunner(
        matrix = {
            "compiler": compilers,
            "target": ["Debug"],
            "std": ["11", "20"],
            "unwind": [
                # "CPPTRACE_UNWIND_WITH_UNWIND",
                # "CPPTRACE_UNWIND_WITH_EXECINFO",
                "CPPTRACE_UNWIND_WITH_NOTHING",
            ],
            "symbols": [
                # "CPPTRACE_GET_SYMBOLS_WITH_LIBBACKTRACE",
                # "CPPTRACE_GET_SYMBOLS_WITH_LIBDL",
                # "CPPTRACE_GET_SYMBOLS_WITH_LIBDWARF",
                # "CPPTRACE_GET_SYMBOLS_WITH_ADDR2LINE",
                "CPPTRACE_GET_SYMBOLS_WITH_NOTHING",
            ],
            "demangle": [
                # "CPPTRACE_DEMANGLE_WITH_CXXABI",
                "CPPTRACE_DEMANGLE_WITH_NOTHING",
            ]
        },
        exclude = []
    ).run(build)

def run_windows_matrix(compilers: list):
    MatrixRunner(
        matrix = {
            "compiler": compilers,
            "target": ["Debug"],
            "std": ["11", "20"],
            "unwind": [
                # "CPPTRACE_UNWIND_WITH_WINAPI",
                # "CPPTRACE_UNWIND_WITH_DBGHELP",
                # "CPPTRACE_UNWIND_WITH_UNWIND",
                "CPPTRACE_UNWIND_WITH_NOTHING",
            ],
            "symbols": [
                # "CPPTRACE_GET_SYMBOLS_WITH_DBGHELP",
                # "CPPTRACE_GET_SYMBOLS_WITH_LIBDWARF",
                # "CPPTRACE_GET_SYMBOLS_WITH_ADDR2LINE",
                "CPPTRACE_GET_SYMBOLS_WITH_NOTHING",
            ],
            "demangle": [
                # "CPPTRACE_DEMANGLE_WITH_CXXABI",
                "CPPTRACE_DEMANGLE_WITH_NOTHING",
            ]
        },
        exclude = []
    ).run(build)

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
    args = parser.parse_args()

    if platform.system() == "Linux":
        compilers = []
        if args.clang or args.all:
            compilers.append("clang++-14")
        if args.gcc or args.all:
            compilers.append("g++-10")
        run_linux_matrix(compilers)
    if platform.system() == "Darwin":
        compilers = []
        if args.clang or args.all:
            compilers.append("clang++")
        if args.gcc or args.all:
            compilers.append("g++-12")
        run_macos_matrix(compilers)
    if platform.system() == "Windows":
        compilers = []
        if args.clang or args.all:
            compilers.append("clang++")
        if args.msvc or args.all:
            compilers.append("cl")
        if args.gcc or args.all:
            compilers.append("g++")
        run_windows_matrix(compilers)

main()
