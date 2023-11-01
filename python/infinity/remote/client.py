import grpc
import typing as tp
from python.infinity import infinity_pb2_grpc, infinity_pb2


class GrpcInfinityClient:
    db_name: str
    channel: grpc.Channel
    stub: infinity_pb2_grpc.InfinityServiceStub

    def __init__(self, url='localhost:50051'):
        self.url = url
        self.db_name = "default"
        self.channel = grpc.insecure_channel(url)
        self.stub = infinity_pb2_grpc.InfinityServiceStub(self.channel)
        self.stub.Connect(infinity_pb2.Empty())

    def create_database(self, db_name: str):
        response = self.stub.CreateDatabase(infinity_pb2.CreateDatabaseRequest(db_name=db_name, options=None))
        return response

    def drop_database(self, db_name: str):
        response = self.stub.DropDatabase(infinity_pb2.DropDatabaseRequest(db_name=db_name))
        return response

    def list_databases(self):
        response = self.stub.ListDatabase(infinity_pb2.ListDatabaseRequest())
        return response

    def describe_database(self, db_name: str):
        response = self.stub.DescribeDatabase(infinity_pb2.DescribeDatabaseRequest(db_name=db_name))
        return response

    def get_database(self, db_name: str):
        response = self.stub.GetDatabase(infinity_pb2.GetDatabaseRequest(db_name=db_name))
        return response

    def create_table(self, db_name: str, table_name: str, column_defs, options):
        return self.stub.CreateTable(infinity_pb2.CreateTableRequest(db_name=db_name, table_name=table_name,
                                                                     column_defs=column_defs, options=options))

    def drop_table(self, db_name: str, table_name: str):
        self.stub.DropTable(infinity_pb2.DropTableRequest(db_name=db_name, table_name=table_name))

    def list_tables(self, db_name: str):
        return self.stub.ListTable(infinity_pb2.ListTableRequest(db_name=db_name))

    def describe_table(self, db_name: str, table_name: str):
        return self.stub.DescribeTable(infinity_pb2.DescribeTableRequest(db_name=db_name, table_name=table_name))

    def get_table(self, db_name: str, table_name: str):
        return self.stub.GetTable(infinity_pb2.GetTableRequest(db_name=db_name, table_name=table_name))

    def create_index(self, db_name: str, table_name: str, index_name: str, column_names: list[str], method_type: str,
                     index_para_list: infinity_pb2.InitParameter, options):
        return self.stub.CreateIndex(infinity_pb2.CreateIndexRequest(db_name=db_name,
                                                                     table_name=table_name,
                                                                     index_name=index_name,
                                                                     column_names=column_names,
                                                                     method_type=method_type,
                                                                     index_para_list=index_para_list,
                                                                     options=options))

    def drop_index(self, db_name: str, table_name: str, index_name: str):
        return self.stub.DropIndex(infinity_pb2.DropIndexRequest(db_name=db_name,
                                                                 table_name=table_name,
                                                                 index_name=index_name))

    def insert(self, db_name: str, table_name: str, column_names: list[str], fields: list[infinity_pb2.Field]):
        return self.stub.Insert(infinity_pb2.InsertRequest(db_name=db_name,
                                                           table_name=table_name,
                                                           column_names=column_names,
                                                           fields=fields))

    def import_data(self, db_name: str, table_name: str, file_path: str, import_options):

        try:
            res = self.stub.Import(infinity_pb2.ImportRequest(db_name=db_name,
                                                              table_name=table_name,
                                                              file_path=file_path,
                                                              import_options=import_options))
            return res
        except Exception as e:
            print(e)
            return None

    def select(self, db_name: str, table_name: str, select_list, where_expr, group_by_list, limit_expr, offset_expr,
               search_expr):
        res = self.stub.Search(infinity_pb2.SelectStatement(db_name=db_name,
                                                            table_name=table_name,
                                                            select_list=select_list,
                                                            where_expr=where_expr,
                                                            group_by_list=group_by_list,
                                                            limit_expr=limit_expr,
                                                            offset_expr=offset_expr,
                                                            search_expr=search_expr
                                                            ))
        success = res.success
        if success:
            return res
        else:
            return None
