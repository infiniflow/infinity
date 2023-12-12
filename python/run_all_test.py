import os


def run():
    os.system("cd test")
    os.system(f"python -m pytest test")


if __name__ == '__main__':
    run()
