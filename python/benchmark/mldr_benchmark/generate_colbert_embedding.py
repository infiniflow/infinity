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
from mldr_common_tools import EvalArgs, check_languages, load_corpus, get_colbert_model, save_colbert_list


@dataclass
class ModelArgs:
    colbert_model: str = field(default="jina-colbert", metadata={'help': 'The name of the colbert model to use.'})
    fp16: bool = field(default=True, metadata={'help': 'Use fp16 in inference?'})


def generate_multivec(model, corpus: datasets.Dataset, max_passage_length: int, batch_size: int,
                      begin_pos: int, end_pos: int):
    result_dict = model.encode(corpus["text"][begin_pos: end_pos], batch_size=batch_size, max_length=max_passage_length,
                               return_dense=False, return_sparse=False, return_colbert_vecs=True)
    return result_dict['colbert_vecs']


def main():
    parser = HfArgumentParser([ModelArgs, EvalArgs])
    model_args, eval_args = parser.parse_args_into_dataclasses()
    model_args: ModelArgs
    eval_args: EvalArgs

    languages = check_languages(eval_args.languages)
    if model_args.colbert_model == "jina-colbert":
        if languages != ["en"]:
            raise ValueError(f"Model {model_args.colbert_model} only supports English language.")
    model = get_colbert_model(model_args=model_args)
    print("==================================================")
    print(f"Start generating colbert embedding with model: {model_args.colbert_model} ...")
    if model_args.colbert_model == "BAAI/bge-m3":
        save_dir_name = "bge-m3"
    elif model_args.colbert_model == "colbertv2.0":
        save_dir_name = "colbertv2"
    elif model_args.colbert_model == "jina-colbert":
        save_dir_name = "jina-colbert"
    else:
        raise ValueError(f"Model {model_args.colbert_model} is not supported.")
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
        save_colbert_list(colbert_embeddings, colbert_save_file)

    print("==================================================")
    print(f"Finish generating colbert embeddings with model: {model_args.colbert_model} ...")


if __name__ == "__main__":
    main()
