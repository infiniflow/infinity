import os


def generate_thrift():
    parent_dir = os.path.dirname(os.getcwd())
    python_dir = parent_dir + "/python/infinity/remote_thrift"
    cpp_dir = parent_dir + "/src/network/infinity_thrift"
    infinity_thrift_file = "infinity.thrift"
    os.system("thrift --version")
    os.system(f"thrift --out {python_dir} --gen py {infinity_thrift_file}")
    os.system(f"thrift -r --out {cpp_dir} --gen cpp:no_skeleton {infinity_thrift_file}")


if __name__ == '__main__':
    generate_thrift()
