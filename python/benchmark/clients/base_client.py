import argparse
from abc import abstractmethod
from typing import Any, List, Optional, Dict, Union
from enum import Enum
import subprocess
import sys
import os
from urllib.parse import urlparse

class FilterType(str, Enum):
    VALUE_MATCH = "value_match"
    TEXT_MATCH = "text_match"
    RANGE = "range"
    GEO = "geo"

FieldValue = Union[str, int, float]
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
    def upload(self) :
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
    
    def parse(self, meta_conditions: Optional[Dict[str, List[Any]]]) -> Optional[Any]:
        """
        accept the conditions stored in a dictinary
        convert it into the representation used by database client(implemented in the specific client such as qdrant_client).

        The filter condition has the following structure (below is a full-text filter condition example):
        {
            "and": [
                {"text": {"text_match": {"text": "town"}}}
            ]
        }
        """
        if meta_conditions is None or 0 == len(meta_conditions):
            return None
        return self.build_condition(
            and_subfilters=self.create_condition_subfilters(meta_conditions.get("and")),
            or_subfilters=self.create_condition_subfilters(meta_conditions.get("or")),
        )

    def build_condition(
        self, and_subfilters: Optional[List[Any]], or_subfilters: Optional[List[Any]]
    ) -> Optional[Any]:
        raise NotImplementedError

    def create_condition_subfilters(self, entries) -> Optional[List[Any]]:
        if entries is None:
            return None

        output_filters = []
        for entry in entries:
            for field_name, field_filters in entry.items():
                for condition_type, value in field_filters.items():
                    condition = self.build_filter(
                        field_name, FilterType(condition_type), value
                    )
                    output_filters.append(condition)
        return output_filters

    def build_filter(
        self, field_name: str, filter_type: FilterType, criteria: Dict[str, Any]
    ):
        if FilterType.VALUE_MATCH == filter_type:
            return self.build_value_match_filter(
                field_name, value=criteria.get("value")
            )
        if FilterType.TEXT_MATCH == filter_type:
            return self.build_text_match_filter(
                field_name, text=criteria.get("text")
            )
        if FilterType.RANGE == filter_type:
            return self.build_range_filter(
                field_name,
                lt=criteria.get("lt"),
                gt=criteria.get("gt"),
                lte=criteria.get("lte"),
                gte=criteria.get("gte"),
            )
        if FilterType.GEO == filter_type:
            return self.build_geo_filter(
                field_name,
                lon=criteria.get("lon"),
                lat=criteria.get("lat"),
                radius=criteria.get("radius"),
            )
        raise NotImplementedError

    def build_exact_match_filter(self, field_name: str, value: FieldValue) -> Any:
        raise NotImplementedError

    def build_range_filter(
        self,
        field_name: str,
        lt: Optional[FieldValue],
        gt: Optional[FieldValue],
        lte: Optional[FieldValue],
        gte: Optional[FieldValue],
    ) -> Any:
        raise NotImplementedError

    def build_text_match_filter(
        self, field_name: str, text: str
    ) -> Any:
        raise NotImplementedError
