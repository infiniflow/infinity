import json
import struct
import numpy as np
import datasets
from dataclasses import dataclass, field


@dataclass
class EvalArgs:
    begin_pos: int = field(metadata={'help': 'Begin position of the corpus to evaluate.'})
    end_pos: int = field(metadata={'help': 'End position of the corpus to evaluate.'})
    languages: str = field(default="en", metadata={
        'help': 'Languages to evaluate. Available languages: ar de en es fr hi it ja ko pt ru th zh', "nargs": "+"})
    embedding_save_dir: str = field(default='./corpus-embedding', metadata={
        'help': 'Dir to save embedding. Embedding will be saved to `embedding_save_dir/{encoder_name}/{lang}/`.'})
    max_passage_length: int = field(default=8192, metadata={'help': 'Max passage length.'})
    batch_size: int = field(default=1, metadata={'help': 'Inference batch size.'})
    overwrite: bool = field(default=False, metadata={'help': 'Whether to overwrite embedding'})


def check_languages(languages):
    if isinstance(languages, str):
        languages = [languages]
    available_languages = ['ar', 'de', 'en', 'es', 'fr', 'hi', 'it', 'ja', 'ko', 'pt', 'ru', 'th', 'zh']
    for lang in languages:
        if lang not in available_languages:
            raise ValueError(f"Language `{lang}` is not supported. Available languages: {available_languages}")
    return languages


def load_corpus(lang: str):
    corpus = datasets.load_dataset('Shitao/MLDR', f'corpus-{lang}', split='corpus',
                                   download_config=datasets.DownloadConfig(resume_download=True))
    return corpus


def ivecs_read(fname: str):
    a = np.fromfile(fname, dtype='int32')
    d = a[0]
    return a.reshape(-1, d + 1)[:, 1:].copy()


def fvecs_read(fname: str):
    return ivecs_read(fname).view('float32')


def fvecs_read_yield(filename):
    with open(filename, 'rb') as f:
        while True:
            try:
                dims = struct.unpack('i', f.read(4))[0]
                vec = struct.unpack('{}f'.format(dims), f.read(4 * dims))
                assert dims == len(vec)
                yield list(vec)
            except struct.error:
                break


# return list of dict[int, float]
def read_mldr_sparse_embedding_list_dict(file_path: str):
    result = None
    file_suffix = file_path.split('.')[-1]
    if file_suffix == 'json':
        with open(file_path, 'r') as f:
            result = []
            sparse_json = json.load(f)
            for one_dict in sparse_json:
                one_result = {}
                for p, v in one_dict.items():
                    one_result[int(p)] = v
                result.append(one_result)
    elif file_suffix == 'data':
        with open(file_path, 'rb') as f:
            result = []
            total_num = struct.unpack('<i', f.read(4))[0]
            for i in range(total_num):
                ll = struct.unpack('<i', f.read(4))[0]
                one_dict = {}
                for j in range(ll):
                    p = struct.unpack('<i', f.read(4))[0]
                    v = struct.unpack('<f', f.read(4))[0]
                    one_dict[p] = v
                result.append(one_dict)
    else:
        raise ValueError(f"Unsupported file: {file_path}")
    return result


# indices values format
def read_mldr_sparse_embedding(file_path: str):
    result = None
    file_suffix = file_path.split('.')[-1]
    if file_suffix == 'json':
        with open(file_path, 'r') as f:
            result = []
            sparse_json = json.load(f)
            for one_dict in sparse_json:
                one_result = {"indices": [], "values": []}
                for p, v in one_dict.items():
                    one_result["indices"].append(int(p))
                    one_result["values"].append(v)
                result.append(one_result)
    elif file_suffix == 'data':
        with open(file_path, 'rb') as f:
            result = []
            total_num = struct.unpack('<i', f.read(4))[0]
            for i in range(total_num):
                ll = struct.unpack('<i', f.read(4))[0]
                one_dict = {"indices": [], "values": []}
                for j in range(ll):
                    p = struct.unpack('<i', f.read(4))[0]
                    v = struct.unpack('<f', f.read(4))[0]
                    one_dict["indices"].append(p)
                    one_dict["values"].append(v)
                result.append(one_dict)
    else:
        raise ValueError(f"Unsupported file: {file_path}")
    return result


def read_mldr_sparse_embedding_yield(file_path: str):
    file_suffix = file_path.split('.')[-1]
    if file_suffix == 'json':
        with open(file_path, 'r') as f:
            sparse_json = json.load(f)
            for one_dict in sparse_json:
                one_result = {"indices": [], "values": []}
                for p, v in one_dict.items():
                    one_result["indices"].append(int(p))
                    one_result["values"].append(v)
                yield one_result
    elif file_suffix == 'data':
        with open(file_path, 'rb') as f:
            total_num = struct.unpack('<i', f.read(4))[0]
            for i in range(total_num):
                one_dict = {"indices": [], "values": []}
                ll = struct.unpack('<i', f.read(4))[0]
                for j in range(ll):
                    p = struct.unpack('<i', f.read(4))[0]
                    v = struct.unpack('<f', f.read(4))[0]
                    one_dict["indices"].append(p)
                    one_dict["values"].append(v)
                yield one_dict
    else:
        raise ValueError(f"Unsupported file: {file_path}")
