import subprocess


def run_command():
    count = 0
    i = 0
    while True:
        try:
            count += 1
            result = subprocess.run(
                ["sqllogictest", "./test/sql/dql/aggregate/test_simple_agg_big.slt"],
                check=True,
            )
            i += 1
            print(i)
        except subprocess.CalledProcessError:
            print(f"Command failed on the {count}th run.")
            break


if __name__ == "__main__":
    run_command()
