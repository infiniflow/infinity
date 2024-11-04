import os
import subprocess
import argparse
import subprocess


def image_exists(image_name):
    result = subprocess.run(
        ["sudo", "docker", "image", "inspect", image_name],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    return result.returncode == 0


def main(image_name: str, bin_path: str):
    # if image_exists(image_name):
    #     print(f"Image {image_name} already exists.")
    #     return

    # download_command = "wget -O docker-27.3.1.tgz https://download.docker.com/linux/static/stable/x86_64/docker-27.3.1.tgz"
    # subprocess.check_call(download_command, shell=True)
    # download_command = "wget -O iproute2-6.9.0.tar.gz https://mirrors.edge.kernel.org/pub/linux/utils/net/iproute2/iproute2-6.9.0.tar.gz"
    # subprocess.check_call(download_command, shell=True)
    subprocess.check_call(f"cp {bin_path}/docker-27.3.1.tgz .", shell=True)
    subprocess.check_call(f"cp {bin_path}/iproute2-6.9.0.tar.gz .", shell=True)

    print("Listing current directory contents:")
    for item in os.listdir("."):
        print(item)

    subprocess.check_call(
        f"sudo docker build -f scripts/Dockerfile_infinity_tester_centos7 -t {image_name} .",
        shell=True,
    )
    subprocess.check_call("docker image ls", shell=True)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--image_name", type=str, required=True)
    parser.add_argument("--bin_path", type=str, required=True)
    args = parser.parse_args()

    main(args.image_name, args.bin_path)
