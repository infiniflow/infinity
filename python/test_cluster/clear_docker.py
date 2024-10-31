import docker
import shutil
import os
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Clear docker containers")
    parser.add_argument(
        "--docker",
        action="store_true",
        default=False,
    )

    use_docker = parser.parse_args().docker

    # sudo docker rm -f $(sudo docker ps -a -q -f name=minio);
    # sudo docker rm -f $(sudo docker ps -a -q -f name=infinity_build);
    # sudo docker network rm $(sudo docker network ls -q -f name=infinity_network)

    client = docker.from_env()

    if use_docker:
        for container in client.containers.list(all=True, filters={"name": "minio_docker"}):
            container.remove(force=True)
        for container in client.containers.list(
            all=True, filters={"name": "infinity_build"}
        ):
            container.remove(force=True)
        for network in client.networks.list(filters={"name": "infinity_network"}):
            network.remove()
    else:
        for container in client.containers.list(all=True, filters={"name": "minio_host"}):
            container.remove(force=True)

    dir_path = "./minio"
    if os.path.exists(dir_path) and os.path.isdir(dir_path):
        shutil.rmtree(dir_path)