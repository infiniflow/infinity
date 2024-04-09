import argparse
from abc import abstractmethod


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
    def upload(self) -> dict:
        """
        Upload data and build indexes (parameters are parsed by __init__).
        """
        pass

    @abstractmethod
    def search(self) -> list[int]:
        """
        Execute the corresponding query tasks (vector search, full-text search, hybrid search) based on the parsed parameters. 
        The function returns id list.
        """
        pass

    def check_and_save_results(self, results: list[int]):
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
