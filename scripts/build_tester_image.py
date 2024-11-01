import os
import subprocess


def image_exists(image_name):
    result = subprocess.run(
        ["sudo", "docker", "image", "inspect", image_name],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    return result.returncode == 0


def main():
    image_name = "infiniflow/infinity_tester:centos7_clang18"

    if image_exists(image_name):
        print(f"Image {image_name} already exists.")
        return

    if not os.path.exists("docker-27.3.1.tgz"):
        os.system(
            "wget https://download.docker.com/linux/static/stable/x86_64/docker-27.3.1.tgz"
        )
    else:
        print("docker-27.3.1.tgz already exists, skipping download.")

    if not os.path.exists("iproute2-6.9.0.tar.gz"):
        os.system(
            "wget https://mirrors.edge.kernel.org/pub/linux/utils/net/iproute2/iproute2-6.9.0.tar.gz"
        )
    else:
        print("iproute2-6.9.0.tar.gz already exists, skipping download.")
    os.system(
        "sudo docker build -f scripts/Dockerfile_infinity_tester_centos7 -t infiniflow/infinity_tester:centos7_clang18 ."
    )
    os.system("docker image ls")


if __name__ == "__main__":
    main()
