---
sidebar_position: 3
slug: /
---

# Deploy Infinity server

Infinity supports to deploy server for remote access.

## Prerequisites

- CPU: >= 4 cores, x86_64 with AVX2, or aarch64
- RAM: >= 16 GB
- Disk: >= 50 GB
- OS: Linux with glibc >=2.17

## Deploy Infinity database

### Deploy Infinity using Docker on Linux x86_64 and MacOS x86_64

```bash
sudo mkdir -p /var/infinity && sudo chown -R $USER /var/infinity
docker pull infiniflow/infinity:nightly
docker run -d --name infinity -v /var/infinity/:/var/infinity --ulimit nofile=500000:500000 --network=host infiniflow/infinity:nightly
```

### Deploy Infinity using binary package on Linux x86_64

You can download the binary package (deb, rpm, or tgz) for your respective host operating system from https://github.com/infiniflow/infinity/releases. The prebuilt packages are compatible with Linux distributions based on glibc 2.17 or later, for example, RHEL 7, Debian 8, Ubuntu 14.04.

Fedora/RHEL/CentOS/OpenSUSE
```bash
sudo rpm -i infinity-0.2.0-dev-x86_64.rpm
sudo systemctl start infinity
```

Ubuntu/Debian
```bash
sudo dpkg -i infinity-0.2.0-dev-x86_64.deb
sudo systemctl start infinity
```

## Connect to Infinity server

`infinity-sdk` requires Python 3.10+.

```bash
pip3 install infinity-sdk==0.2.0
```

```python
import infinity

# connect to infinity
infinity_obj = infinity.connect(infinity.NetworkAddress("ip_address", 23817)) # for local address, can use infinity.LOCAL_HOST
db = infinity_obj.get_database("default_db")
table = db.create_table("my_table", {"num": {"type": "integer"}, "body": {"type": "varchar"}, "vec": {"type": "vector, 4, float"}})
table.insert([{"num": 1, "body": "unnecessary and harmful", "vec": [1.0, 1.2, 0.8, 0.9]}])
table.insert([{"num": 2, "body": "Office for Harmful Blooms", "vec": [4.0, 4.2, 4.3, 4.5]}])
res = table.output(["*"]).knn("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 2).to_pl()
print(res)
```


> 💡 For more information about the Python API, see the [Python API Reference](../references/pysdk_api_reference.md).
> 💡 For a complete example, see the [hello_infinity.py](../../python/hello_infinity.py).
