import argparse
import os
import logging
import sys
import time
import multiprocessing

from clients.elasticsearch_client import ElasticsearchClient
from clients.infinity_client import InfinityClient
from clients.qdrant_client import QdrantClient
from generate_query_json import generate_query_txt

ENGINES = ["infinity", "qdrant", "elasticsearch"]
DATA_SETS = ["gist", "sift", "geonames", "enwiki"]

WARM_UP_SECONDS = 60
REPORT_QPS_INTERVAL = 60


def parse_args() -> argparse.Namespace:
    parser: argparse.ArgumentParser = argparse.ArgumentParser(
        description="RAG Database Benchmark"
    )
    parser.add_argument(
        "--generate",
        action="store_true",
        dest="generate_terms",
        help="Generate fulltext queries based on the dataset",
    )
    parser.add_argument(
        "--import",
        action="store_true",
        dest="import_data",
        help="Import data set into database engine",
    )
    parser.add_argument(
        "--query",
        action="store_true",
        dest="query",
        help="Run single client to benchmark query latency",
    )
    parser.add_argument(
        "--query-express",
        type=int,
        default=0,
        dest="query_express",
        help="Run multiple clients in express mode to benchmark QPS",
    )
    parser.add_argument(
        "--engine",
        type=str,
        default="all",
        dest="engine",
        help="database engine to benchmark, one of: all, " + ", ".join(ENGINES),
    )
    parser.add_argument(
        "--dataset",
        type=str,
        default="all",
        dest="dataset",
        help="data set to benchmark, one of: all, " + ", ".join(DATA_SETS),
    )
    return parser.parse_args()


def generate_config_paths(kwargs: argparse.Namespace) -> list[tuple[str, str]]:
    paths = []
    config_path_prefix = os.path.join(
        os.path.dirname(os.path.abspath(__file__)), "configs"
    )
    engines = ENGINES if kwargs.engine == "all" else [kwargs.engine]
    datasets = DATA_SETS if kwargs.dataset == "all" else [kwargs.dataset]
    for engine in engines:
        for dataset in datasets:
            paths.append(
                (os.path.join(config_path_prefix, f"{engine}_{dataset}.json"), engine)
            )
    return paths


def get_client(engine: str, conf_path: str, options: argparse.Namespace):
    if engine == "qdrant":
        return QdrantClient(conf_path, options)
    elif engine == "elasticsearch":
        return ElasticsearchClient(conf_path, options)
    elif engine == "infinity":
        return InfinityClient(conf_path, options)
    else:
        raise ValueError(f"Unknown engine: {engine}")


def main():
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)-15s %(levelname)-8s (%(process)d) %(message)s",
    )
    args = parse_args()
    config_paths = generate_config_paths(args)

    if args.generate_terms:
        # TODO: Write a fixed path for the fulltext benchmark, expand or delete it for the general benchmark
        generate_query_txt(
            "datasets/enwiki/enwiki.csv",
            "datasets/enwiki/enwiki-terms.txt",
            "datasets/enwiki/operations.txt",
        )
        sys.exit(0)

    for conf_path, engine in config_paths:
        if not os.path.exists(conf_path):
            logging.info("qdrant does not support full text search")
            continue
        logging.info("Running {} with {}".format(engine, conf_path))
        if args.query_express >= 1:
            shared_counter = multiprocessing.Value("i", 0)
            exit_event = multiprocessing.Event()
            workers = []
            clients = []
            for i in range(args.query_express):
                client = get_client(engine, conf_path, args)
                clients.append(client)
                worker = multiprocessing.Process(
                    target=client.search_express_outer,
                    args=(shared_counter, exit_event),
                )
                worker.start()
                workers.append(worker)
            try:
                logging.info(f"Let database warm-up for {WARM_UP_SECONDS} seconds")
                time.sleep(WARM_UP_SECONDS)
                logging.info(
                    "Collecting statistics for 30 minutes. Print statistics so far every minute. Type Ctrl+C to quit."
                )
                shared_counter.value = 0
                start = time.time()
                start_str = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(start))
                counter_prev = 0
                for i in range(int(1800 / REPORT_QPS_INTERVAL)):
                    time.sleep(REPORT_QPS_INTERVAL)
                    now = time.time()
                    counter = shared_counter.value
                    avg_start = counter / (now - start)
                    avg_interval = (counter - counter_prev) / REPORT_QPS_INTERVAL
                    counter_prev = counter
                    logging.info(
                        f"average QPS since {start_str}: {avg_start}, average QPS of last interval:{avg_interval}"
                    )
            except KeyboardInterrupt:
                logging.info("Interrupted by user! Exiting...")
            except Exception as e:
                logging.error(e)
            finally:
                exit_event.set()
                for worker in workers:
                    worker.join()
        else:
            client = get_client(engine, conf_path, args)
            client.run_experiment(args)
        logging.info("Finished {} with {}".format(engine, conf_path))


if __name__ == "__main__":
    main()
