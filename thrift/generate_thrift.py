import os


def generate_thrift():
    parent_dir = os.path.dirname(os.getcwd())
    python_dir = parent_dir + "/python/infinity-thrift/remote"
    cpp_dir = parent_dir + "/src/network/thrift_cpp"
    infinity_dir = parent_dir + "/src/network/infinity_thrift"
    thrift_file ="tutorial.thrift"
    infinity_thrift_file = "infinity.thrift"
    os.system("thrift --version")
    # os.system(f"thrift --out {python_dir} --gen py {thrift_file}")
    os.system(f"thrift -r --out {cpp_dir} --gen cpp:no_skeleton {thrift_file}")
    os.system(f"thrift -r --out {infinity_dir} --gen cpp:no_skeleton {infinity_thrift_file}")


if __name__ == '__main__':
    generate_thrift()
