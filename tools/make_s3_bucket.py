from minio import Minio

client = Minio(
    endpoint = "127.0.0.1:9000",
    access_key="minioadmin",
    secret_key="minioadmin",
    secure = False,
)

# Create bucket.
client.make_bucket("infinity")