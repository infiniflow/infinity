import argparse
from abc import abstractmethod
from typing import Any, List, Optional, Dict, Union
from enum import Enum
import subprocess
import sys
import os
from urllib.parse import urlparse
import time


class BaseClient:
    """
    Base class for all clients(Qdrant, ES, infinity).
    mode is a string that corresponds to a JSON file's address in the configurations. 
    Each client reads the required parameters from the JSON configuration file.
    """

    @abstractmethod
    def __init__(self,
                 mode: str,
                 options: argparse.Namespace,
                 drop_old: bool = True) -> None:
        """
        The mode configuration file is parsed to extract the needed parameters, which are then all stored for use by other functions.
        """
        pass

    @abstractmethod
    def upload(self):
        """
        Upload data and build indexes (parameters are parsed by __init__).
        """
        pass

    @abstractmethod
    def search(self) -> list[list[Any]]:
        """
        Execute the corresponding query tasks (vector search, full-text search, hybrid search) based on the parsed parameters. 
        The function returns id list.
        """
        pass

    def download_data(self, url, target_path):
        """
        Download dataset and extract it into path.
        """
        _, ext = os.path.splitext(url)
        if ext == '.bz2':
            bz2_path = target_path + '.bz2'
            subprocess.run(['wget', '-O', bz2_path, url], check=True)
            subprocess.run(['bunzip2', bz2_path], check=True)
            extracted_path = os.path.splitext(bz2_path)[0]
            os.rename(extracted_path, target_path)
        else:
            subprocess.run(['wget', '-O', target_path, url], check=True)

    @abstractmethod
    def check_and_save_results(self, results: list[list[Any]]):
        """
        The correct results for queries are read from the mode configuration file to compare with the search results and calculate recall.
        Record the results (metrics to be measured) and save them in the results folder.
        """
        pass

    def run_experiment(self, args):
        """
        run experiment and save results.
        """
        if args.import_data:
            start_time = time.time()
            self.upload()
            finish_time = time.time()
            print(f"upload finish, cost time = {finish_time - start_time}")
        if args.query:
            results = self.search()
            self.check_and_save_results(results)
