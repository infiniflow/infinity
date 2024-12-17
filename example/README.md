# Examples

## Python SDK examples

The Python scripts in the current directory provides the examples of the infinity SDK API. Before running these scripts, please ensure that the correct version of infinity SDK is installed. If you are using the server version of Infinity, you can use the following command to install it

```shell
pip install infinity-sdk==0.5.0.dev7
```

To use Infinity locally as a Python module, install the Infinity Embedded SDK:

```shell
pip install infinity-embedded-sdk==0.5.0.dev7
```

Once the installation completes, run some simple examples:

```shell
python simple_example.py
```

## HTTP examples

We provide several SHELL scripts in the **/example/http** directory, and you need to install `curl` to run these scripts. For Debian or Ubuntu users, use the following command:

```shell
sudo apt install curl
```

The HTTP API only works for the server version of Infinity. Once the Infinity server is started, users can run these scripts using commands similar to the following

```shell
bash http/create_list_show_database.sh
```