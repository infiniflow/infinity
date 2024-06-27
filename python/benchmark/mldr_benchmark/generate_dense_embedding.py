"""
python generate_dense_embedding.py \
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
import datasets
import numpy as np
from tqdm import tqdm
from FlagEmbedding import FlagModel
from dataclasses import dataclass, field
from transformers import HfArgumentParser
from mldr_common_tools import EvalArgs, check_languages, load_corpus


@dataclass
class ModelArgs:
    encoder: str = field(default="BAAI/bge-m3", metadata={'help': 'Name or path of encoder'})
    fp16: bool = field(default=True, metadata={'help': 'Use fp16 in inference?'})
    pooling_method: str = field(default='cls', metadata={'help': "Pooling method. Avaliable methods: 'cls', 'mean'"})
    normalize_embeddings: bool = field(default=True, metadata={'help': "Normalize embeddings or not"})


def get_model(model_args: ModelArgs):
    model = FlagModel(model_args.encoder, pooling_method=model_args.pooling_method,
                      normalize_embeddings=model_args.normalize_embeddings, use_fp16=model_args.fp16)
    return model


def generate_dense(model: FlagModel, corpus: datasets.Dataset, max_passage_length: int, batch_size: int, begin_pos: int,
                   end_pos: int):
    dense_embeddings = model.encode_corpus(corpus["text"][begin_pos: end_pos], batch_size=batch_size,
                                           max_length=max_passage_length)
    dense_embeddings = dense_embeddings.astype(np.float32)
    return dense_embeddings


def save_result(dense_embeddings, dense_save_file: str):
    with open(dense_save_file, 'wb') as f:
        for one_dense in tqdm(dense_embeddings, desc="Saving dense embeddings"):
            dim = one_dense.shape[-1]
            f.write(struct.pack('<i', dim))
            one_dense.astype('float32').tofile(f)


def main():
    parser = HfArgumentParser([ModelArgs, EvalArgs])
    model_args, eval_args = parser.parse_args_into_dataclasses()
    model_args: ModelArgs
    eval_args: EvalArgs

    languages = check_languages(eval_args.languages)

    if model_args.encoder[-1] == '/':
        model_args.encoder = model_args.encoder[:-1]

    model = get_model(model_args=model_args)

    encoder = model_args.encoder
    if os.path.basename(encoder).startswith('checkpoint-'):
        encoder = os.path.dirname(encoder) + '_' + os.path.basename(encoder)

    print("==================================================")
    print("Start generating embedding with model:")
    print(model_args.encoder)

    print('Generate embedding of following languages: ', languages)
    for lang in languages:
        print("**************************************************")
        embedding_save_dir = os.path.join(eval_args.embedding_save_dir, os.path.basename(encoder), lang)
        if not os.path.exists(embedding_save_dir):
            os.makedirs(embedding_save_dir)
        dense_save_file = os.path.join(embedding_save_dir, f'dense-{eval_args.begin_pos}-{eval_args.end_pos}.fvecs')
        if os.path.exists(dense_save_file) and not eval_args.overwrite:
            print(f'Embedding of {lang} already exists. Skip...')
            continue

        print(f"Start generating embedding of {lang} ...")
        corpus = load_corpus(lang)

        dense_embeddings = generate_dense(model=model, corpus=corpus, max_passage_length=eval_args.max_passage_length,
                                          batch_size=eval_args.batch_size, begin_pos=eval_args.begin_pos,
                                          end_pos=eval_args.end_pos)
        save_result(dense_embeddings, dense_save_file)

    print("==================================================")
    print("Finish generating embeddings with model:")
    print(model_args.encoder)


if __name__ == "__main__":
    main()
