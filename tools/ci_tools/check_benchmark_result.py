#!/usr/bin/env python3
# script to check benchmark result in GitHub actions
# set input parameter 1 as test type, 2 as test result file path
# if the benchmark result is better than the standard, return 0, else return 1

import sys


def get_average_time(file_path):
    tot_time = 0
    tot_count = 0
    with open(file_path, "r") as f:
        for line in f:
            x = line.split()
            if (
                len(x) == 5
                and x[0] == "Total"
                and x[1] == "cost"
                and x[2] == ":"
                and x[4] == "s"
            ):
                tot_time += float(x[3])
                tot_count += 1
    if tot_count == 0:
        return -1.0
    return tot_time / tot_count


def main():
    golden_fp, latest_fp = sys.argv[1], sys.argv[2]
    golden_avg = get_average_time(golden_fp)
    if golden_avg < 0:
        print("Golden benchmark average time: INVALID")
        sys.exit(1)
    latest_avg = get_average_time(latest_fp)
    if latest_avg < 0:
        print("Latest benchmark average time: INVALID")
        sys.exit(1)
    difference_percentage = 100 * (latest_avg - golden_avg) / golden_avg
    print(
        "golden_avg: {}s, latest_avg: {}s, difference percentage: {:.2f}%".format(
            golden_avg, latest_avg, difference_percentage
        )
    )
    if difference_percentage < 5:
        print("benchmark result is acceptable")
        sys.exit(0)
    else:
        print("benchmark result is unacceptable")
        sys.exit(1)


if __name__ == "__main__":
    main()
