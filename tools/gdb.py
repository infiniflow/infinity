# gdb -ex "source /home/huikong/Code/work/infinity2/tmp/gdb.py"

import re
import gdb # type: ignore

# executable_path = "build/Debug/infinity"
executable_path = "/home/huikong/Code/work/infinity2/build/Debug/src/infinity"

log_path = "restart_test.log.1"

addr_list = []

with open(log_path, "r") as f:
    lines = f.readlines()
    for line in lines:
        # use regex to find such format [critical] number, path(addr), extract number, path and addr

        match = re.search(r"\[critical\] (\d+), (.+)\(\+(0x[0-9a-fA-F]+)\)", line)
        if match:
            number = match.group(1)
            path = match.group(2)
            addr = match.group(3)
            print(f"number: {number}, path: {path}, addr: {addr}")

            addr_list.append(addr)

    # use gdb info line
    gdb.execute(f"file {executable_path}")

    for addr in addr_list:
        try:
            # Use gdb to get information about the address
            result = gdb.execute(f"info line *{addr}", to_string=True)
            print(result)
        except gdb.error as e:
            print(f"Error processing address {addr}: {e}")
