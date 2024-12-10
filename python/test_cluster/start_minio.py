import docker

class MinioParams:
    def __init__(self, minio_dir: str, minio_port: int):
        self.minio_dir = minio_dir
        self.minio_port = minio_port

def add_minio(minio_params: MinioParams):
    minio_image_name = "quay.io/minio/minio"

    minio_cmd = f'server /data --console-address ":{minio_params.minio_port}"'
    docker_client = docker.from_env()
    container_name = "minio_host"

    try:
        minio_container = docker_client.containers.get(container_name)
    except docker.errors.NotFound:
        minio_container = docker_client.containers.run(
            image=minio_image_name,
            name=container_name,
            detach=True,
            environment=[
                "MINIO_ROOT_PASSWORD=minioadmin",
                "MINIO_ROOT_USER=minioadmin",
            ],
            volumes=[f"{minio_params.minio_dir}:/data"],
            command=minio_cmd,
            network="host",
        )

if __name__ == "__main__":
    param = MinioParams("minio", 9001)
    add_minio(param)
