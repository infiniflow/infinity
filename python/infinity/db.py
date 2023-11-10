from abc import ABC, abstractmethod


class Database(ABC):

    @abstractmethod
    def create_table(self, table_name, schema, options):
        pass  # implement create table logic here

    @abstractmethod
    def drop_table(self, table_name):
        pass  # implement drop table logic here

    @abstractmethod
    def list_tables(self, db_name):
        pass  # implement list tables logic here

    @abstractmethod
    def describe_table(self, table_name):
        pass  # implement describe table logic here

    @abstractmethod
    def get_table(self, table_name):
        pass  # implement get table logic here
