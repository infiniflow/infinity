import argparse
import json
import os
import subprocess
import sys
from typing import Any

from base_client import BaseClient


class InfinityClient(BaseClient):
    def __init__(self,
                 mode: str,
                 options: argparse.Namespace,
                 drop_old: bool = True) -> None:
        """
        The mode configuration file is parsed to extract the needed parameters, which are then all stored for use by other functions.
        """
        self.config = mode
        self.drop_old = drop_old
        self.threads = options.threads
        self.rounds = options.rounds
        self.hardware = options.hardware
        self.limit_ram = options.limit_ram
        self.limit_cpu = options.limit_cpu
        self._parse_json()
        self._download_data()

    def _parse_json(self):
        """
        Parse the JSON configuration file to extract the needed parameters.
        """
        with open(self.config) as f:
            json_obj = json.load(f)
            self.test_name = json_obj['name']
            self.dataset = json_obj['dataset']
            self.url = json_obj['url']
            self.server_host = json_obj['host']
            self.data_path = json_obj['data_path']
            self.query_path = json_obj['query_path']
            self.vector_index = json.loads(json_obj['vector_index'])

    def _download_data(self):
        """
        Download dataset and extract it into path.
        """
        if not os.path.exists(self.data_path) or not os.path.exists(self.query_path):
            subprocess.run(['wget', self.url], stdout=sys.stdout, stderr=sys.stderr)
            subprocess.run(['tar', '-zxvf', self.data_path, "-C", ], stdout=sys.stdout, stderr=sys.stderr)


    def upload(self):
        """
        Upload data and build indexes (parameters are parsed by __init__).
        """
        pass

    def search(self) -> list[list[Any]]:
        """
        Execute the corresponding query tasks (vector search, full-text search, hybrid search) based on the parsed parameters.
        The function returns id list.
        """
        pass

    def check_and_save_results(self, results: list[list[Any]]):
        """
        The correct results for queries are read from the mode configuration file to compare with the search results and calculate recall.
        Record the results (metrics to be measured) and save them in the results folder.
        """
        pass

    def run_experiment(self):
        """
        run experiment and save results.
        """
        self.upload()
        results = self.search()
        self.check_and_save_results(results)
