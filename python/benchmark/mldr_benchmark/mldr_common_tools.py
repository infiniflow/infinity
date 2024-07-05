import json
import struct
import numpy as np
import datasets
from dataclasses import dataclass, field
from pyserini.output_writer import get_output_writer, OutputFormat


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


@dataclass
class QueryArgs:
    languages: str = field(default="en", metadata={
        'help': 'Languages to evaluate. Available languages: ar de en es fr hi it ja ko pt ru th zh', "nargs": "+"})
    query_result_dave_dir: str = field(default='./query-results', metadata={'help': 'Dir to save query result.'})
    query_types: str = field(default="bm25",
                             metadata={'help': 'Query method. Available methods: bm25, dense, sparse', "nargs": "+"})


def check_languages(languages):
    if isinstance(languages, str):
        languages = [languages]
    available_languages = ['ar', 'de', 'en', 'es', 'fr', 'hi', 'it', 'ja', 'ko', 'pt', 'ru', 'th', 'zh']
    for lang in languages:
        if lang not in available_languages:
            raise ValueError(f"Language `{lang}` is not supported. Available languages: {available_languages}")
    return languages


def check_query_types(query_types):
    if isinstance(query_types, str):
        query_types = [query_types]
    available_query_types = ['bm25', 'dense', 'sparse']
    for query_type in query_types:
        if query_type not in available_query_types:
            raise ValueError(
                f"Query type `{query_type}` is not supported. Available query types: {available_query_types}")
    return query_types


def load_corpus(lang: str):
    corpus = datasets.load_dataset('Shitao/MLDR', f'corpus-{lang}', split='corpus',
                                   download_config=datasets.DownloadConfig(resume_download=True))
    return corpus


def get_queries_and_qids(lang: str, streaming: bool = False):
    if streaming:
        dataset = datasets.load_dataset('Shitao/MLDR', lang, split='test', streaming=streaming)
    else:
        dataset = datasets.load_dataset('Shitao/MLDR', lang, split='test',
                                        download_config=datasets.DownloadConfig(resume_download=True))
    queries = []
    qids = []
    for data in dataset:
        qids.append(data['query_id'])
        queries.append(data['query'])
    return queries, qids


class FakeJScoredDoc:
    def __init__(self, docid: str, score: float):
        self.docid = docid
        self.score = score


# search_results: List[Tuple[str, List[JScoredDoc]]]
# here, JScoredDoc needs .docid, .score
def save_result(search_results, result_save_path: str, qids: list, max_hits: int):
    output_writer = get_output_writer(result_save_path, OutputFormat(OutputFormat.TREC.value), 'w',
                                      max_hits=max_hits, tag='Faiss', topics=qids,
                                      use_max_passage=False,
                                      max_passage_delimiter='#',
                                      max_passage_hits=1000)
    with output_writer:
        for topic, hits in search_results:
            output_writer.write(topic, hits)


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


def read_colbert_data_yield(file_path: str):
    with open(file_path, 'rb') as f:
        while True:
            try:
                tensor_len = struct.unpack('<i', f.read(4))[0]
                one_tensor = []
                for i in range(tensor_len):
                    dim = struct.unpack('<i', f.read(4))[0]
                    vec = struct.unpack('{}f'.format(dim), f.read(4 * dim))
                    assert dim == len(vec)
                    one_tensor.append(list(vec))
                yield one_tensor
            except struct.error:
                break


def get_all_part_begin_ends(total_row_count: int):
    result = []
    pos_now = 0
    while pos_now < total_row_count:
        new_pos = int(input("input part end position: "))
        if pos_now >= new_pos or new_pos > total_row_count:
            print("Invalid value. Input again.")
            continue
        result.append((pos_now, new_pos))
        pos_now = new_pos
    return result


def get_bit_array(float_array: list[list]):
    return [[1 if x > 0.0 else 0 for x in one_list] for one_list in float_array]

