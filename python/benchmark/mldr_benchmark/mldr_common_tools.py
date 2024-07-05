import json
import struct
import numpy as np
from tqdm import tqdm
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
    with_colbert: bool = field(metadata={'help': 'Whether table contains colbert.'})
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
    available_query_types = ['bm25', 'dense', 'sparse', 'colbert']
    if query_types == 'all' or (len(query_types) == 1 and query_types[0] == 'all'):
        return available_query_types
    if isinstance(query_types, str):
        query_types = [query_types]
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


text_to_replace = "&|!+-–():\'\"?~^"
text_to_replace_with = " " * len(text_to_replace)
query_translation_table = str.maketrans(text_to_replace, text_to_replace_with)


def bm25_query_yield(queries: list[str], embedding_file: str):
    for query in queries:
        yield query.translate(query_translation_table)


def dense_query_yield(queries: list[str], embedding_file: str):
    return fvecs_read_yield(embedding_file)


def sparse_query_yield(queries: list[str], embedding_file: str):
    return read_mldr_sparse_embedding_yield(embedding_file)


def colbert_query_yield(queries: list[str], embedding_file: str):
    return read_colbert_data_yield(embedding_file)


query_yields = {'bm25': bm25_query_yield, 'dense': dense_query_yield, 'sparse': sparse_query_yield,
                'colbert': colbert_query_yield}


def apply_bm25(table, query_str: str, max_hits: int):
    return table.match('fulltext_col', query_str, f'topn={max_hits}')


def apply_dense(table, query_embedding, max_hits: int):
    return table.knn("dense_col", query_embedding, "float", "ip", max_hits)


def apply_sparse(table, query_embedding: dict, max_hits: int):
    return table.match_sparse("sparse_col", query_embedding, "ip", max_hits, {"alpha": "0.9", "beta": "0.5"})


def apply_colbert(table, query_tensor: list[list], max_hits: int):
    return table.match_tensor("colbert_col", query_tensor, 'float', 'maxsim',
                              f'topn={max_hits};emvb_threshold_first=0.3;emvb_threshold_final=0.4')


apply_funcs = {'bm25': apply_bm25, 'dense': apply_dense, 'sparse': apply_sparse, 'colbert': apply_colbert}


class FakeBGEM3FlagModelColBERTV2:
    def __init__(self):
        from colbert.modeling.checkpoint import Checkpoint
        from colbert.infra import ColBERTConfig
        self.ckpt = Checkpoint("colbert-ir/colbertv2.0", colbert_config=ColBERTConfig(root="experiments"))
        from langchain.text_splitter import RecursiveCharacterTextSplitter
        self.text_splitter = RecursiveCharacterTextSplitter(chunk_size=512, chunk_overlap=0, length_function=len,
                                                            is_separator_regex=False)

    def encode(self, text: list[str], batch_size: int, max_length: int, return_dense: bool, return_sparse: bool,
               return_colbert_vecs: bool):
        assert return_colbert_vecs is True and return_dense is False and return_sparse is False
        colbert_result = []
        for one_text in tqdm(text, desc="Generating ColBERT-v2.0 embeddings"):
            chunks = self.text_splitter.create_documents([one_text])
            subtext_tensor = [self.ckpt.docFromText([chunk.page_content], to_cpu=True)[0].numpy() for chunk in chunks]
            # now concatenate them together
            concatenated = np.concatenate(subtext_tensor)
            assert concatenated.ndim == 2 and concatenated.shape[1] == 128
            colbert_result.append(concatenated)
        return {'colbert_vecs': colbert_result}

    def encode_query(self, text: list[str]):
        colbert_result = []
        for one_text in tqdm(text, desc="Generating ColBERT-v2.0 query embeddings"):
            text_tensor = self.ckpt.queryFromText([one_text], to_cpu=True)[0].numpy()
            assert text_tensor.ndim == 2 and text_tensor.shape[1] == 128
            colbert_result.append(text_tensor)
        return {'colbert_vecs': colbert_result}


class FakeBGEM3FlagModelJinaColBERT:
    def __init__(self):
        from colbert.modeling.checkpoint import Checkpoint
        from colbert.infra import ColBERTConfig
        self.ckpt = Checkpoint("jinaai/jina-colbert-v1-en", colbert_config=ColBERTConfig(doc_maxlen=8192))

    def encode(self, text: list[str], batch_size: int, max_length: int, return_dense: bool, return_sparse: bool,
               return_colbert_vecs: bool):
        assert return_colbert_vecs is True and return_dense is False and return_sparse is False
        colbert_result = []
        for one_text in tqdm(text, desc="Generating jina-colbert-v1-en embeddings"):
            text_tensor = self.ckpt.docFromText([one_text], to_cpu=True)[0].numpy()
            assert text_tensor.ndim == 2 and text_tensor.shape[1] == 128
            colbert_result.append(text_tensor)
        return {'colbert_vecs': colbert_result}

    def encode_query(self, text: list[str]):
        colbert_result = []
        for one_text in tqdm(text, desc="Generating jina-colbert-v1-en query embeddings"):
            text_tensor = self.ckpt.queryFromText([one_text], to_cpu=True)[0].numpy()
            assert text_tensor.ndim == 2 and text_tensor.shape[1] == 128
            colbert_result.append(text_tensor)
        return {'colbert_vecs': colbert_result}


class FakeBGEM3FlagModel:
    def __init__(self, use_fp16: bool):
        from FlagEmbedding import BGEM3FlagModel
        self.bge_m3 = BGEM3FlagModel("BAAI/bge-m3", use_fp16=use_fp16)

    def encode(self, text: list[str], batch_size: int, max_length: int, return_dense: bool, return_sparse: bool,
               return_colbert_vecs: bool):
        assert return_colbert_vecs is True and return_dense is False and return_sparse is False
        return self.bge_m3.encode(text, batch_size=batch_size, max_length=max_length, return_dense=False,
                                  return_sparse=False, return_colbert_vecs=True)

    def encode_query(self, text: list[str]):
        return self.bge_m3.encode(text, return_dense=False, return_sparse=False, return_colbert_vecs=True)


def get_colbert_model(model_args):
    if model_args.colbert_model == "BAAI/bge-m3":
        return FakeBGEM3FlagModel(use_fp16=model_args.fp16)
    elif model_args.colbert_model == "jina-colbert":
        return FakeBGEM3FlagModelJinaColBERT()
    elif model_args.colbert_model == "colbertv2.0":
        return FakeBGEM3FlagModelColBERTV2()
    else:
        raise ValueError(f"Model {model_args.colbert_model} is not supported.")


def save_colbert_list(multivec_embeddings, multivec_save_file: str):
    with open(multivec_save_file, 'wb') as f:
        for one_multivec in tqdm(multivec_embeddings, desc="Saving multivec embeddings"):
            l, dim = one_multivec.shape
            f.write(struct.pack('<i', l))
            for vec in one_multivec:
                f.write(struct.pack('<i', dim))
                vec.astype('float32').tofile(f)
