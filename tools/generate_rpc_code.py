import os
import subprocess


def get_path(executable):
    try:
        complete_path = subprocess.check_output(['which', executable])
        return complete_path.decode('utf-8').strip()
    except subprocess.CalledProcessError:
        return None


def create_dir(paths: list):
    for path in paths:
        if not os.path.exists(path):
            os.makedirs(path)


def fix_python_import_path(filename: str):
    # from  import infinity_brpc_pb2 as infinity__brpc__pb2
    # to    from . import infinity_brpc_pb2 as infinity__brpc__pb2
    # replace with your actual file path
    # Read the file
    with open(filename, 'r') as file:
        lines = file.readlines()

    for i in range(7 if len(lines) >= 7 else len(lines)):
        line = lines[i]
        words = line.split()
        if len(words) == 4 and words[0] == "import":
            import_module, infinity_brpc_pb2, alias, infinity__brpc__pb2 = words
            # Construct the new import line.
            new_line = f"from . {import_module} {infinity_brpc_pb2} {alias} {infinity__brpc__pb2}\n"
            lines[i] = new_line  # replace the line

        # Write back to the file
    with open(filename, 'w') as file:
        file.writelines(lines)


def generate_thrift():
    parent_dir = os.path.dirname(os.getcwd())
    python_dir = parent_dir + "/python/infinity/remote_thrift"
    cpp_dir = parent_dir + "/src/network/infinity_thrift"
    create_dir([python_dir, cpp_dir])
    infinity_thrift_file = python_dir + "/infinity_thrift_rpc/infinity.thrift"
    os.system("thrift --version")
    os.system(f"thrift --out {python_dir} --gen py {infinity_thrift_file}")
    os.system(f"thrift -r --out {cpp_dir} --gen cpp:no_skeleton {infinity_thrift_file}")


def generate_grpc():
    parent_dir = os.path.dirname(os.getcwd())
    python_dir = parent_dir + "/python/infinity/remote_grpc/grpc_pb"
    cpp_dir = parent_dir + "/src/network/infinity_grpc"
    create_dir([python_dir, cpp_dir])
    os.system("protoc --version")
    os.system("which protoc")
    # py
    os.system(
        f"python3 -m grpc_tools.protoc -I={python_dir} --python_out={python_dir} --grpc_python_out={python_dir} {python_dir}/infinity_grpc.proto")

    fix_python_import_path(python_dir + "/infinity_grpc_pb2_grpc.py")

    # cpp
    grpc_cpp_plugin_dir = get_path('grpc_cpp_plugin')
    match grpc_cpp_plugin_dir:
        case None:
            raise Exception(f"can't find the grpc_cpp_plugin")
        case _:
            os.system(f"/usr/bin/protoc -I={python_dir} --cpp_out={cpp_dir}  {python_dir}/infinity_grpc.proto")
            os.system(
                f"/usr/bin/protoc -I={python_dir}  --grpc_out={cpp_dir}  --plugin=protoc-gen-grpc={grpc_cpp_plugin_dir} {python_dir}/infinity_grpc.proto")


def generate_brpc():
    parent_dir = os.path.dirname(os.getcwd())
    python_dir = parent_dir + "/python/infinity/remote_brpc/brpc_pb"
    cpp_dir = parent_dir + "/src/network/infinity_brpc"
    create_dir([python_dir, cpp_dir])
    # py
    os.system(
        f"python3 -m grpc_tools.protoc -I={python_dir} --python_out={python_dir} --grpc_python_out={python_dir} {python_dir}/infinity_brpc.proto")

    fix_python_import_path(python_dir + "/infinity_brpc_pb2_grpc.py")
    # cpp

    if not os.path.exists(cpp_dir):
        os.makedirs(cpp_dir)

    os.system(f"/usr/bin/protoc -I={python_dir} --cpp_out={cpp_dir}  {python_dir}/infinity_brpc.proto")


if __name__ == '__main__':
    generate_thrift()
    generate_grpc()
    generate_brpc()
