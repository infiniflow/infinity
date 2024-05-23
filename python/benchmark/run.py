import argparse
import os
import logging
import sys

from clients.elasticsearch_client import ElasticsearchClient
from clients.infinity_client import InfinityClient
from clients.qdrant_client import QdrantClient
from generate_queries import generate_query_txt

ENGINES = ["infinity", "qdrant", "elasticsearch"]
DATA_SETS = ["gist", "sift", "geonames", "enwiki"]


def parse_args() -> argparse.Namespace:
    parser: argparse.ArgumentParser = argparse.ArgumentParser(
        description="RAG Database Benchmark"
    )
    parser.add_argument(
        "--generate",
        action="store_true",
        dest="generate_queries",
        help="Generate fulltext queries based on the dataset",
    )
    parser.add_argument(
        "--import",
        action="store_true",
        dest="import_data",
        help="Import dataset into database engine",
    )
    parser.add_argument(
        "--query",
        type=int,
        default=0,
        dest="query",
        help="Run the query set only once using given number of clients with recording the result and latency. This is for result validation and latency analysis",
    )
    parser.add_argument(
        "--query-express",
        type=int,
        default=0,
        dest="query_express",
        help="Run the query set randomly using given number of clients without recording the result. This is for QPS measurement.",
    )
    parser.add_argument(
        "--engine",
        type=str,
        default="infinity",
        dest="engine",
        help="database engine to benchmark, one of: " + ", ".join(ENGINES),
    )
    parser.add_argument(
        "--dataset",
        type=str,
        default="enwiki",
        dest="dataset",
        help="dataset to benchmark, one of: " + ", ".join(DATA_SETS),
    )
    return parser.parse_args()


def get_client(engine: str, conf_path: str):
    if engine == "qdrant":
        return QdrantClient(conf_path)
    elif engine == "elasticsearch":
        return ElasticsearchClient(conf_path)
    elif engine == "infinity":
        return InfinityClient(conf_path)
    else:
        raise ValueError(f"Unknown engine: {engine}")


def main():
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)-15s %(levelname)-8s (%(process)d) %(message)s",
    )
    args = parse_args()
    conf_path = os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        "configs",
        f"{args.engine}_{args.dataset}.json",
    )

    if args.generate_queries:
        # TODO: Write a fixed path for the fulltext benchmark
        generate_query_txt(
            "datasets/enwiki/enwiki.csv",
            "datasets/enwiki/enwiki-terms.txt",
            "datasets/enwiki/operations.txt",
        )
        sys.exit(0)

    if not os.path.exists(conf_path):
        logging.error(f"config file doesn't exist: {conf_path}")
        return
    logging.info(
        f"Running engine {args.engine}, dataset {args.dataset}, config file {conf_path}"
    )
    client = get_client(args.engine, conf_path)
    client.run_experiment(args)
    logging.info(
        f"Done engine {args.engine}, dataset {args.dataset}, config file {conf_path}"
    )


if __name__ == "__main__":
    main()
