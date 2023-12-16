import os


def run():
    os.system("cd test")
    os.system("python3 -m pytest test")


if __name__ == '__main__':
    run()
