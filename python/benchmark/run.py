import argparse
import os

from clients.elasticsearch_client import ElasticsearchClient
from clients.infinity_client import InfinityClient
from clients.qdrant_client import QdrantClient

ENGINES = ['infinity', 'qdrant', 'elasticsearch']
DATA_SETS = ['gist', 'sift', 'geonames']

def parse_args() -> argparse.Namespace:
    parser: argparse.ArgumentParser = argparse.ArgumentParser(description="Vector Database Benchmark")
    parser.add_argument('-e', '--engine', type=str, default='all', dest='engine')
    parser.add_argument('-m', '--mode', type=str, default='all', dest='mode')
    parser.add_argument('-t', '--threads', type=int, default=1, dest='threads')
    parser.add_argument('-r', '--rounds', type=int, default=5, dest='rounds')
    parser.add_argument('--hardware', type=str, default='8c_16g', dest='hardware')
    parser.add_argument('--limit-ram', type=str, default='16g', dest='limit_ram')
    parser.add_argument('--limit-cpu', type=int, default=8, dest='limit_cpu')
    return parser.parse_args()

def generate_config_paths(kwargs: argparse.Namespace) -> list[tuple[str, str]]:
    paths = []
    config_path_prefix = os.path.join(os.path.dirname(os.path.abspath(__file__)), "configs")

    def add_path(_engine: str, _mode: str):
        paths.append((os.path.join(config_path_prefix, f"{_engine}_{_mode}.json"), _engine))

    engines = ENGINES if kwargs.engine == 'all' else [kwargs.engine]
    modes = DATA_SETS if kwargs.mode == 'all' else [kwargs.mode]

    for engine in engines:
        for mode in modes:
            add_path(engine, mode)
    return paths

def get_client(engine: str, config: str, options: argparse.Namespace):
    if engine == 'qdrant':
        return QdrantClient(config, options)
    elif engine == 'elasticsearch':
        return ElasticsearchClient(config, options)
    elif engine == 'infinity':
        return InfinityClient(config, options)
    else:
        raise ValueError(f"Unknown engine: {engine}")

if __name__ == '__main__':
    args = parse_args()
    config_paths = generate_config_paths(args)

    for path, engine in config_paths:
        if not os.path.exists(path):
            print(f"qdrant does not supple full text search")
            continue
        print("Running", engine, "with", path)
        client = get_client(engine, path, args)
        client.run_experiment()
        print("Finished", engine, "with", path)
