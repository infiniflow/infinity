#!/usr/bin/env python3
import os
import re
import sys


def get_result(fname):
    dic = {}
    dic["file"] = fname

    with open(fname, 'r', encoding='utf-8') as file:
        content = file.read()
    dic['Build time'] = float(re.search(r'Create Index cost: ([\d.]+)s', content).group(1))

    lines = re.findall(r'thread :.*\n', content)

    for line in lines:
        k, v = re.search(r'(thread) : (\d+)', line).groups()
        key = f'{k}_{v}'
        value = {}
        k, v = re.search(r'(Average cost) : ([\d.]+) s', line).groups()
        value[k] = float(v)
        k, v = re.search(r'(QPS) : (\d+)', line).groups()
        value[k] = int(v)
        k, v = re.search(r'(Recall@10) : ([\d.]+)', line).groups()
        value[k] = float(v)

        dic[key] = value

    return dic


def main():
    args = sys.argv[1:]

    infos = []
    for fname in args:
        if os.path.exists(fname):
            infos.append(get_result(fname))

    infos = sorted(infos, key=lambda x: x['thread_1']['QPS'], reverse=True)

    for info in infos:
        print('=' * 32)
        for k, v in info.items():
            print(f'{k}: {v}')


if __name__ == "__main__":
    main()
