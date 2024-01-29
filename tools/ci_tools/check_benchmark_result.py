#!/usr/bin/env python3
# script to check benchmark result in GitHub actions
# set input parameter 1 as test type, 2 as test result file path
# if the benchmark result is better than the standard, return 0, else return 1

import sys


def main():
    benchmark_bars = {"sift_1": 1.897754, "sift_8": 0.332755}
    benchmark_id = sys.argv[1]
    standard = benchmark_bars[benchmark_id]
    file_path = sys.argv[2]
    with open(file_path, 'r') as f:
        last_line = f.readlines()[-1]
    print()
    print("last line from log:", last_line)
    result = float(last_line.split(' ')[-2])
    print("average time:", result, 's')
    print("required time:", standard, 's')
    print()
    difference_percentage = 100 * (result - standard) / standard
    print("difference percentage: {:.2f}%".format(difference_percentage))
    print()
    if difference_percentage < 5:
        print("benchmark result is acceptable")
        sys.exit(0)
    else:
        print("benchmark result is unacceptable")
        sys.exit(1)


if __name__ == '__main__':
    main()
