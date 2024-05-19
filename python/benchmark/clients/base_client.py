import argparse
from abc import abstractmethod
from typing import Any
import subprocess
import os
import time
import logging


class BaseClient:
    """
    Base class for all clients(Qdrant, ES, infinity).
    mode is a string that corresponds to a JSON file's address in the configurations.
    Each client reads the required parameters from the JSON configuration file.
    """

    def __init__(
        self, conf_path: str, options: argparse.Namespace, drop_old: bool = True
    ) -> None:
        """
        The conf_path configuration file is parsed to extract the needed parameters, which are then all stored for use by other functions.
        """
        self.delta = 0
        self.running = True

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
        if ext == ".bz2":
            bz2_path = target_path + ".bz2"
            subprocess.run(["wget", "-O", bz2_path, url], check=True)
            subprocess.run(["bunzip2", bz2_path], check=True)
            extracted_path = os.path.splitext(bz2_path)[0]
            os.rename(extracted_path, target_path)
        else:
            subprocess.run(["wget", "-O", target_path, url], check=True)

    @abstractmethod
    def check_and_save_results(self, results: list[list[Any]]):
        """
        The correct results for queries are read from the mode configuration file to compare with the search results and calculate recall.
        Record the results (metrics to be measured) and save them in the results folder.
        """
        pass

    @abstractmethod
    def search_express(self, shared_counter, exit_event):
        """
        Search in express mode:
        - doesn't record per-query latency and result
        - call update_shared_counter regularly to update the shared counter and quit when exit_event is set
        """

    def search_express_outer(self, shared_counter, exit_event):
        try:
            self.search_express(shared_counter, exit_event)
        except KeyboardInterrupt:
            logging.info("Interrupted by user! Exiting...")
        except Exception as e:
            logging.error(e)
        finally:
            logging.info("subprocess exited")

    def update_shared_counter(self, shared_counter, exit_event):
        """
        update shared_counter (allocated from shared memory) regularly.
        set self.running to False when exit_event is set
        """
        self.delta += 1
        if self.delta >= 100:
            with shared_counter.get_lock():
                shared_counter.value += self.delta
                self.delta = 0
            self.running = not exit_event.is_set()
        return

    def run_experiment(self, args):
        """
        run experiment and save results.
        """
        if args.import_data:
            start_time = time.time()
            self.upload()
            finish_time = time.time()
            logging.info(f"upload finish, cost time = {finish_time - start_time}")
        elif args.query:
            results = self.search()
            self.check_and_save_results(results)
