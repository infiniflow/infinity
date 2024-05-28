import argparse
import sys
import sysconfig  # type: ignore

from . import __version__, include_dir, cmake_dir


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--version",
        action="version",
        version=__version__,
        help="Print the version number.",
    )
    parser.add_argument(
        "--include_dir",
        action="store_true",
        help="Print the path to the nanobind C++ header directory."
    )
    parser.add_argument(
        "--cmake_dir",
        action="store_true",
        help="Print the path to the nanobind CMake module directory."
    )
    args = parser.parse_args()
    if not sys.argv[1:]:
        parser.print_help()
    if args.include_dir:
        print(include_dir())
    if args.cmake_dir:
        print(cmake_dir())


if __name__ == "__main__":
    main()
