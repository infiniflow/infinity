import sys
import re
import platform

def main():
    output = sys.stdin.read()

    print(output)

    print("-" * 50)

    time = int(re.search(r"\d+ tests? from \d+ test suites? ran. \((\d+) ms total\)", output).group(1))

    dwarf4 = any(["DWARF4" in arg for arg in sys.argv[1:]])
    dwarf5 = any(["DWARF5" in arg for arg in sys.argv[1:]])
    clang = any(["clang" in arg for arg in sys.argv[1:]])
    # Somehow -gdwarf-4 clang is fast after a completely unrelated PR? Weird. Something to do with static linking...?
    # https://github.com/jeremy-rifkin/cpptrace/pull/22
    expect_slow = False

    if platform.system() == "Windows":
        # For some reason SymInitialize is super slow on github's windows runner and it alone takes 250ms. Nothing we
        # can do about that.
        threshold = 350 # ms
    else:
        threshold = 100 # ms

    if expect_slow:
        if time > threshold:
            print(f"Success (expecting slow): Test program took {time} ms")
        else:
            print(f"Error (expecting slow): Test program took {time} ms")
            sys.exit(1)
    else:
        if time > threshold:
            print(f"Error: Test program took {time} ms")
            sys.exit(1)
        else:
            print(f"Success: Test program took {time} ms")


main()
