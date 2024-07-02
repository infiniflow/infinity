import os
import platform
import subprocess
from pprint import pprint
from dataclasses import dataclass, field
from transformers import HfArgumentParser
from pyserini.util import download_evaluation_script
from mldr_common_tools import check_languages


@dataclass
class EvalArgs:
    languages: str = field(default="en", metadata={
        'help': 'Languages to evaluate. Avaliable languages: ar de en es fr hi it ja ko pt ru th zh', "nargs": "+"})
    query_result_dave_dir: str = field(default='./query-results', metadata={'help': 'Dir to save query result.'})
    qrels_dir: str = field(default='./qrels', metadata={'help': 'Dir to qrels.'})
    metrics: str = field(default="ndcg@10",
                         metadata={'help': 'Metrics to evaluate. Avaliable metrics: ndcg@k, recall@k', "nargs": "+"})


all_query_types = ['bm25', 'dense', 'sparse']


def map_metric(metric: str):
    metric, k = metric.split('@')
    if metric.lower() == 'ndcg':
        return k, f'ndcg_cut.{k}'
    elif metric.lower() == 'recall':
        return k, f'recall.{k}'
    else:
        raise ValueError(f"Unkown metric: {metric}")


def evaluate(script_path, qrels_path, query_result_path, metrics: list):
    cmd_prefix = ['java', '-jar', script_path]

    results = {}
    for metric in metrics:
        k, mapped_metric = map_metric(metric)
        args = ['-c', '-M', str(k), '-m', mapped_metric, qrels_path, query_result_path]
        cmd = cmd_prefix + args

        # print(f'Running command: {cmd}')
        shell = platform.system() == "Windows"
        process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=shell)
        stdout, stderr = process.communicate()
        if stderr:
            print(stderr.decode("utf-8"))
        result_str = stdout.decode("utf-8")
        try:
            results[metric] = float(result_str.split(' ')[-1].split('\t')[-1])
        except:
            results[metric] = result_str
    return results


def main():
    parser = HfArgumentParser([EvalArgs])
    eval_args = parser.parse_args_into_dataclasses()[0]
    eval_args: EvalArgs
    query_result_dave_dir = eval_args.query_result_dave_dir
    languages = check_languages(eval_args.languages)
    metrics = eval_args.metrics if isinstance(eval_args.metrics, list) else [eval_args.metrics]
    script_path = download_evaluation_script('trec_eval')
    for lang in languages:
        print("*****************************")
        print(f"Start evaluating {lang} ...")
        qrels_path = os.path.join(eval_args.qrels_dir, f"qrels.mldr-v1.0-{lang}-test.tsv")
        for query_type in all_query_types:
            query_result_path = os.path.join(query_result_dave_dir, f"{lang}_{query_type}.txt")
            if not os.path.exists(query_result_path):
                print(f"Skip {query_result_path}, not found.")
                continue
            print(f"Start evaluating {query_result_path} ...")
            result = evaluate(script_path, qrels_path, query_result_path, metrics)
            pprint(result)
    print("==================================================")
    print("Finish generating evaluation results.")


if __name__ == "__main__":
    main()
