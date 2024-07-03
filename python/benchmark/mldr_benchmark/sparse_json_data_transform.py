import os
import json
import struct
import argparse


def transform_sparse_json_to_data(sparse_json_file: str):
    dir_name = os.path.dirname(sparse_json_file)
    base_name = os.path.basename(sparse_json_file)
    base_suffix = base_name.split('.')[-1]
    without_suffix = base_name[:-(len(base_suffix) + 1)]
    sparse_data_file = os.path.join(dir_name, f'{without_suffix}.data')
    with open(sparse_json_file, 'r') as f:
        with open(sparse_data_file, 'wb') as g:
            sparse_json = json.load(f)
            total_num = len(sparse_json)
            g.write(struct.pack('<i', total_num))
            for one_dict in sparse_json:
                ll = len(one_dict)
                g.write(struct.pack('<i', ll))
                for p, v in one_dict.items():
                    intp = int(p)
                    g.write(struct.pack('<i', intp))
                    g.write(struct.pack('<f', v))
    return


def transform_data_to_sparse_json(sparse_data_file: str):
    dir_name = os.path.dirname(sparse_data_file)
    base_name = os.path.basename(sparse_data_file)
    base_suffix = base_name.split('.')[-1]
    without_suffix = base_name[:-(len(base_suffix) + 1)]
    sparse_json_file = os.path.join(dir_name, f'{without_suffix}.json')
    with open(sparse_data_file, 'rb') as f:
        with open(sparse_json_file, 'w') as g:
            total_num = struct.unpack('<i', f.read(4))[0]
            result = []
            for i in range(total_num):
                ll = struct.unpack('<i', f.read(4))[0]
                one_dict = {}
                for j in range(ll):
                    p = struct.unpack('<i', f.read(4))[0]
                    v = struct.unpack('<f', f.read(4))[0]
                    one_dict[str(p)] = v
                result.append(one_dict)
            json.dump(result, g)
    return


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--sparse_json_file", type=str, required=False, default=None)
    parser.add_argument("--sparse_data_file", type=str, required=False, default=None)
    args = parser.parse_args()
    if args.sparse_json_file is not None:
        transform_sparse_json_to_data(args.sparse_json_file)
    if args.sparse_data_file is not None:
        transform_data_to_sparse_json(args.sparse_data_file)
