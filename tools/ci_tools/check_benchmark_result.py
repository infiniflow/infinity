#!/usr/bin/env python3
# script to check benchmark result in GitHub actions
# set input parameter 1 as test type, 2 as test result file path
# if the benchmark result is better than the standard, return 0, else return 1

import sys


def main():
    benchmark_bars = {"sift_1": 2255.7, "sift_4": 869230.0}
    benchmark_id = sys.argv[1]
    standard = benchmark_bars[benchmark_id]
    file_path = sys.argv[2]
    with open(file_path, 'r') as f:
        last_line = f.readlines()[-1]
    print()
    print("last line from log:", last_line)
    result = float(last_line.split(' ')[-1])
    print("average time:", result)
    print("required time:", standard)
    print()
    difference_percentage = 100 * (result - standard) / standard
    print("difference percentage: {:.2f}%".format(difference_percentage))
    print()
    if difference_percentage < 3:
        print("benchmark result is acceptable")
        sys.exit(0)
    else:
        print("benchmark result is unacceptable")
        sys.exit(1)


if __name__ == '__main__':
    main()
