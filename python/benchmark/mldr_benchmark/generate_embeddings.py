"""
python generate_embeddings.py \
--languages ar de en es fr hi it ja ko pt ru th zh \
--embedding_save_dir ./corpus-embeddings \
--max_passage_length 8192 \
--batch_size 1 \
--fp16 True \
"""
import os
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
    languages: str = field(
        default="en",
        metadata={'help': 'Languages to evaluate. Avaliable languages: ar de en es fr hi it ja ko pt ru th zh',
                  "nargs": "+"}
    )
    embedding_save_dir: str = field(
        default='./corpus-embeddings',
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


def generate_dense_multivec(model: BGEM3FlagModel, corpus: datasets.Dataset, max_passage_length: int, batch_size: int):
    result_dict = model.encode(corpus["text"], batch_size=batch_size, max_length=max_passage_length, return_dense=True,
                               return_colbert_vecs=True)
    return result_dict


def save_result(embeddings_dict: dict, dense_save_file: str, multivec_save_file: str):
    dense_embeddings = embeddings_dict['dense_vecs']
    multivec_embeddings = embeddings_dict['colbert_vecs']
    with open(dense_save_file, 'wb') as f:
        for one_dense in tqdm(dense_embeddings, desc="Saving dense embeddings"):
            dim = one_dense.shape[-1]
            f.write(struct.pack('<i', dim))
            one_dense.astype('float32').tofile(f)
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
    model = get_model(model_args=model_args)
    print("==================================================")
    print("Start generating embedding with model BAAI/bge-m3:")

    print('Generate embedding of following languages: ', languages)
    for lang in languages:
        print("**************************************************")
        embedding_save_dir = os.path.join(eval_args.embedding_save_dir, 'bge-m3', lang)
        if not os.path.exists(embedding_save_dir):
            os.makedirs(embedding_save_dir)
        dense_save_file = os.path.join(embedding_save_dir, 'dense.fvecs')
        multivec_save_file = os.path.join(embedding_save_dir, 'multivec.data')
        if os.path.exists(dense_save_file) and os.path.exists(multivec_save_file) and not eval_args.overwrite:
            print(f'Embedding of {lang} already exists. Skip...')
            continue

        print(f"Start generating embedding of {lang} ...")
        corpus = load_corpus(lang)

        embeddings_dict = generate_dense_multivec(
            model=model,
            corpus=corpus,
            max_passage_length=eval_args.max_passage_length,
            batch_size=eval_args.batch_size
        )
        save_result(embeddings_dict, dense_save_file, multivec_save_file)

    print("==================================================")
    print("Finish generating embeddings with model BAAI/bge-m3.")


if __name__ == "__main__":
    main()
