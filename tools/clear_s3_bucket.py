from minio import Minio

client = Minio(
    endpoint = "192.168.200.165:9000",
    access_key="minioadmin",
    secret_key="minioadmin",
    secure = False,
)


objects = client.list_objects("infinity", recursive=True)
for obj in objects:
    print(obj.object_name)
    client.remove_object("infinity", obj.object_name)
