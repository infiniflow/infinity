import argparse
import collect_thread_sanitizer_log


def run(filter: list[str], input: str, output: str) -> None:
    with open(input, "r") as f:
        lines = f.readlines()
        rets = collect_thread_sanitizer_log.filter_warning(lines, filter)
    with open(output, "w") as f2:
        f2.writelines(rets)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--filter", type=str, help="filter")
    parser.add_argument("--input", type=str, help="input")
    parser.add_argument("--output", type=str, help="output")

    args = parser.parse_args()
    filter = args.filter.split(",")
    run(filter, args.input, args.output)
