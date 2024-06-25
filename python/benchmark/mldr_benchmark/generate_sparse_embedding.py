"""
python generate_sparse_embedding.py \
--begin_pos 0 \
--end_pos 200000 \
--encoder BAAI/bge-m3 \
--languages ar de en es fr hi it ja ko pt ru th zh \
--embedding_save_dir ./corpus-embedding \
--max_passage_length 8192 \
--batch_size 1 \
--fp16 True \
--pooling_method cls \
--normalize_embeddings True
"""
import os
import json
import struct
import datasets
import numpy as np
from tqdm import tqdm
from FlagEmbedding import BGEM3FlagModel
from dataclasses import dataclass, field
from transformers import HfArgumentParser


@dataclass
class ModelArgs:
    fp16: bool = field(
        default=True,
        metadata={'help': 'Use fp16 in inference?'}
    )


@dataclass
class EvalArgs:
    begin_pos: int = field(
        metadata={'help': 'Begin position of the corpus to evaluate.'}
    )
    end_pos: int = field(
        metadata={'help': 'End position of the corpus to evaluate.'}
    )
    languages: str = field(
        default="en",
        metadata={'help': 'Languages to evaluate. Avaliable languages: ar de en es fr hi it ja ko pt ru th zh',
                  "nargs": "+"}
    )
    embedding_save_dir: str = field(
        default='./corpus-embedding',
        metadata={
            'help': 'Dir to save embedding. Corpus embedding will be saved to `embedding_save_dir/{encoder_name}/{lang}/dense.fvecs`.'}
    )
    max_passage_length: int = field(
        default=8192,
        metadata={'help': 'Max passage length.'}
    )
    batch_size: int = field(
        default=1,
        metadata={'help': 'Inference batch size.'}
    )
    overwrite: bool = field(
        default=False,
        metadata={'help': 'Whether to overwrite embedding'}
    )


def get_model(model_args: ModelArgs):
    model = BGEM3FlagModel("BAAI/bge-m3", use_fp16=model_args.fp16)
    return model


def check_languages(languages):
    if isinstance(languages, str):
        languages = [languages]
    avaliable_languages = ['ar', 'de', 'en', 'es', 'fr', 'hi', 'it', 'ja', 'ko', 'pt', 'ru', 'th', 'zh']
    for lang in languages:
        if lang not in avaliable_languages:
            raise ValueError(f"Language `{lang}` is not supported. Avaliable languages: {avaliable_languages}")
    return languages


def load_corpus(lang: str):
    corpus = datasets.load_dataset('Shitao/MLDR', f'corpus-{lang}', split='corpus',
                                   download_config=datasets.DownloadConfig(resume_download=True))
    return corpus


def generate_sparse(model: BGEM3FlagModel, corpus: datasets.Dataset, max_passage_length: int, batch_size: int,
                    begin_pos: int, end_pos: int):
    result_dict = model.encode(corpus["text"][begin_pos: end_pos], batch_size=batch_size, max_length=max_passage_length,
                               return_dense=False, return_sparse=True, return_colbert_vecs=False)
    sparse_dict = result_dict['lexical_weights']
    result = []
    for one_dict in sparse_dict:
        one_result = {}
        for p, v in one_dict.items():
            one_result[p] = float(v)
        result.append(one_result)
    return result


def save_result(sparse_embeddings, sparse_save_file: str):
    with open(sparse_save_file, 'w') as f:
        json.dump(sparse_embeddings, f)


def main():
    parser = HfArgumentParser([ModelArgs, EvalArgs])
    model_args, eval_args = parser.parse_args_into_dataclasses()
    model_args: ModelArgs
    eval_args: EvalArgs

    languages = check_languages(eval_args.languages)
    model = get_model(model_args=model_args)
    print("==================================================")
    print("Start generating sparse embedding with model: BAAI/bge-m3")

    print('Generate embedding of following languages: ', languages)
    for lang in languages:
        print("**************************************************")
        embedding_save_dir = os.path.join(eval_args.embedding_save_dir, 'bge-m3', lang)
        if not os.path.exists(embedding_save_dir):
            os.makedirs(embedding_save_dir)
        sparse_save_file = os.path.join(embedding_save_dir, f'sparse-{eval_args.begin_pos}-{eval_args.end_pos}.json')
        if os.path.exists(sparse_save_file) and not eval_args.overwrite:
            print(f'Embedding of {lang} already exists. Skip...')
            continue

        print(f"Start generating embedding of {lang} ...")
        corpus = load_corpus(lang)

        sparse_embeddings = generate_sparse(
            model=model,
            corpus=corpus,
            max_passage_length=eval_args.max_passage_length,
            batch_size=eval_args.batch_size,
            begin_pos=eval_args.begin_pos,
            end_pos=eval_args.end_pos
        )
        save_result(sparse_embeddings, sparse_save_file)

    print("==================================================")
    print("Finish generating sparse embeddings with model: BAAI/bge-m3")


if __name__ == "__main__":
    main()
