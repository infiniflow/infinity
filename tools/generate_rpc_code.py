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
    infinity_proj_dir = os.getcwd()
    python_dir = infinity_proj_dir + "/python/infinity/remote_thrift"
    cpp_dir = infinity_proj_dir + "/src/network/infinity_thrift"
    create_dir([python_dir, cpp_dir])
    infinity_thrift_file = infinity_proj_dir + "/thrift/infinity.thrift"
    os.system("thrift --version")
    os.system(f"thrift --out {python_dir} --gen py {infinity_thrift_file}")
    os.system(f"thrift -r --out {cpp_dir} --gen cpp:no_skeleton {infinity_thrift_file}")


if __name__ == '__main__':
    generate_thrift()
