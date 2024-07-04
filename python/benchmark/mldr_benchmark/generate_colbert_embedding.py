"""
python generate_colbert_embedding.py \
--begin_pos 0 \
--end_pos 200000 \
--languages ar de en es fr hi it ja ko pt ru th zh \
--embedding_save_dir ./corpus-embedding \
--max_passage_length 8192 \
--batch_size 1 \
--fp16 True \
"""
import os
import struct
import numpy as np
import datasets
from tqdm import tqdm
from dataclasses import dataclass, field
from transformers import HfArgumentParser
from mldr_common_tools import EvalArgs, check_languages, load_corpus


@dataclass
class ModelArgs:
    model: str = field(default="jina-colbert", metadata={'help': 'The model name of the model to use.'})
    fp16: bool = field(default=True, metadata={'help': 'Use fp16 in inference?'})


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


def get_model(model_args: ModelArgs):
    if model_args.model == "BAAI/bge-m3":
        from FlagEmbedding import BGEM3FlagModel
        return BGEM3FlagModel("BAAI/bge-m3", use_fp16=model_args.fp16)
    elif model_args.model == "jina-colbert":
        return FakeBGEM3FlagModelJinaColBERT()
    elif model_args.model == "colbertv2.0":
        return FakeBGEM3FlagModelColBERTV2()
    else:
        raise ValueError(f"Model {model_args.model} is not supported.")


def generate_multivec(model, corpus: datasets.Dataset, max_passage_length: int, batch_size: int,
                      begin_pos: int, end_pos: int):
    result_dict = model.encode(corpus["text"][begin_pos: end_pos], batch_size=batch_size, max_length=max_passage_length,
                               return_dense=False, return_sparse=False, return_colbert_vecs=True)
    return result_dict['colbert_vecs']


def save_result(multivec_embeddings, multivec_save_file: str):
    with open(multivec_save_file, 'wb') as f:
        for one_multivec in tqdm(multivec_embeddings, desc="Saving multivec embeddings"):
            l, dim = one_multivec.shape
            f.write(struct.pack('<i', l))
            for vec in one_multivec:
                f.write(struct.pack('<i', dim))
                vec.astype('float32').tofile(f)


def main():
    parser = HfArgumentParser([ModelArgs, EvalArgs])
    model_args, eval_args = parser.parse_args_into_dataclasses()
    model_args: ModelArgs
    eval_args: EvalArgs

    languages = check_languages(eval_args.languages)
    if model_args.model == "jina-colbert":
        if languages != ["en"]:
            raise ValueError(f"Model {model_args.model} only supports English language.")
    model = get_model(model_args=model_args)
    print("==================================================")
    print(f"Start generating colbert embedding with model: {model_args.model} ...")
    if model_args.model == "BAAI/bge-m3":
        save_dir_name = "bge-m3"
    elif model_args.model == "colbertv2.0":
        save_dir_name = "colbertv2"
    elif model_args.model == "jina-colbert":
        save_dir_name = "jina-colbert"
    else:
        raise ValueError(f"Model {model_args.model} is not supported.")
    print('Generate embedding of following languages: ', languages)
    for lang in languages:
        print("**************************************************")
        embedding_save_dir = os.path.join(eval_args.embedding_save_dir, save_dir_name, lang)
        if not os.path.exists(embedding_save_dir):
            os.makedirs(embedding_save_dir)
        colbert_save_file = os.path.join(embedding_save_dir, f'colbert-{eval_args.begin_pos}-{eval_args.end_pos}.data')
        if os.path.exists(colbert_save_file) and not eval_args.overwrite:
            print(f'Embedding of {lang} already exists. Skip...')
            continue

        print(f"Start generating embedding of {lang} ...")
        corpus = load_corpus(lang)

        colbert_embeddings = generate_multivec(model=model, corpus=corpus,
                                               max_passage_length=eval_args.max_passage_length,
                                               batch_size=eval_args.batch_size, begin_pos=eval_args.begin_pos,
                                               end_pos=eval_args.end_pos)
        save_result(colbert_embeddings, colbert_save_file)

    print("==================================================")
    print(f"Finish generating colbert embeddings with model: {model_args.model} ...")


if __name__ == "__main__":
    main()
