#!/usr/bin/env python3
# script to check benchmark result in GitHub actions
# set input parameter 1 as test type, 2 as test result file path
# if the benchmark result is better than the standard, return 0, else return 1

import sys
import re


def get_results(fname):
    dic = {}

    with open(fname, 'r', encoding='utf-8') as file:
        content = file.read()

    lines = re.findall(r'thread :.*\n', content)
    for line in lines:
        k, v = re.search(r'(thread) : (\d+)', line).groups()
        key = f'{k}_{v}'
        dic[key] = int(re.search(r'QPS : (\d+)', line).group(1))

    return dic


def main():
    if len(sys.argv) != 3:
        print("Num of input para must equal 3")
        sys.exit(1)
    golden_fname, latest_fname = sys.argv[1], sys.argv[2]
    golden_qps = get_results(golden_fname)
    latest_qps = get_results(latest_fname)

    ok = True
    print('=' * 32)
    print(f"[{golden_fname}] to [{latest_fname}]")
    for key, value in golden_qps.items():
        if key in latest_qps:
            value_latest = latest_qps[key]
            diff = 100 * (value_latest - value) / value

            print(f"{key}, golden qps: {value}, latest qps: {value_latest}, difference percentage: {diff:.2f}%")
            if diff < -7:
                ok = False
    
    if not ok:
        print("Warning: benchmark result is unacceptable")
        sys.exit(1)
    

if __name__ == "__main__":
    main()
