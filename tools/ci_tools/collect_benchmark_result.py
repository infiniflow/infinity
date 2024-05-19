#!/usr/bin/env python3
# script to check benchmark result in GitHub actions
# set input parameter 1 as test type, 2 as test result file path
# if the benchmark result is better than the standard, return 0, else return 1

import datetime
import sys


def main():
    output_file_path = sys.argv[1]
    with open(output_file_path, 'a') as of:
        tot_time = 0
        tot_count = 0
        for line in sys.stdin:
            x = line.split()
            if len(x) == 5 and x[0] == "Total" and x[1] == "cost" and x[2] == ":" and x[4] == "s":
                tot_time += float(x[3])
                tot_count += 1
        of.write(datetime.datetime.now().astimezone().isoformat() + " ")
        if tot_count == 0:
            of.write("Benchmark average time: INVALID s\n")
            sys.exit(1)
        else:
            of.write("Benchmark average time: {:.6f} s\n".format(tot_time / tot_count))
            sys.exit(0)


if __name__ == '__main__':
    main()
