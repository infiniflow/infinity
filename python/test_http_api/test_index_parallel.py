import os
import sys
import pytest
import time
import random
from httpapibase import HttpTest
from threading import Thread
from common.common_values import *
import infinity.index as index
import httputils
from infinity.errors import ErrorCode
from infinity.common import ConflictType


class TestIndexParallel(HttpTest):
    @pytest.mark.skip(reason="segfault")
    def test_http_fulltext_index_rw_parallel(self):
        def write_worker(data, file_path, end_time, thread_id):
            db_name = "default_db"
            table_name = "test_http_test_fulltext_index_parallel"
            self.show_database(db_name)

            while time.time() < end_time:
                operation = random.randint(0, 1)
                if operation == 0:
                    value = []
                    for i in range(len(data["doctitle"])):
                        value.append({"doctitle": data["doctitle"][i],
                                      "docdate": data["docdate"][i], "body": data["body"][i]})
                    self.insert(db_name, table_name, value)
                    # print(f"thread {thread_id}: insert complete")
                if operation == 1:
                    # print(f"thread {thread_id}: begin import")
                    self.import_data(file_path, {"delimiter": "\t"})
                    # print(f"thread {thread_id}: import complete")

        def read_worker(end_time):
            db_name = "default_db"
            table_name = "test_http_test_fulltext_index_parallel"
            while time.time() < end_time:
                self.select(db_name, table_name, ["doctitle", "docdate", "_row_id", "_score"], "", {
                    "match":
                        {
                            "fields": "body^5",
                            "query": "harmful chemical",
                            "operator": "topn=3"
                        }
                })
                time.sleep(0.1)

    def test_http_fulltext_index_update_delete_parallel(self):
        db_name = "default_db"
        table_name = "test_http_test_fulltext_index_parallel"
        self.create_database(db_name)
        self.create_table(db_name, table_name)

        data = {
            "doctitle": ["Document1", "Document2", "Document3"],
            "docdate": ["2024-01-01", "2024-02-01", "2024-03-01"],
            "body": ["chemical test data", "hazardous chemical", "safety chemical guidelines"]
        }

        for i in range(3):
            self.insert(db_name, table_name, [data])

        end_time = time.time() + 10  #run 10s

        def update_worker(end_time):
            while time.time() < end_time:
                update_data = {
                    "doctitle": ["Updated Document"],
                    "docdate": ["2024-04-01"],
                    "body": ["updated chemical data"] 
                }
                self.update(db_name, table_name, update_data, "_row_id=1")
                time.sleep(0.2)
        
        def delete_worker(end_time):
            while time.time() < end_time:
                self.delete(db_name, table_name, "_row_id=2")
                time.sleep(0.3)
        
        # create thread
        update_thread = Thread(target=update_worker, args=(end_time,))
        delete_thread = Thread(target=delete_worker, args=(end_time,))

        update_thread.start()
        delete_thread.start()

        update_thread.join()
        delete_thread.join()

        self.drop_database(db_name)

    def test_http_fulltext_index_conflict_resolution(self):
        db_name = "default_db"
        table_name = "test_http_test_fulltext_index_conflict"
        self.create_database(db_name)
        self.create_table(db_name, table_name)

        data1 = {
            "doctitle": "Conflict Document 1",
            "docdate": "2024-01-01",
            "body": "chemical data set 1"
        }

        data2 = {
            "doctitle": "Conflict Document 2",
            "docdate": "2024-01-02",
            "body": "chemical data set 2"
        }

        self.insert(db_name, table_name, [data1])
        self.insert(db_name, table_name, [data2])

        thread1 = Thread(target=self.insert, args=(db_name, table_name, [data1], ConflictType.IGNORE))
        thread2 = Thread(target=self.insert, args=(db_name, table_name, [data2], ConflictType.REPLACE))

        thread1.start()
        thread2.start()

        thread1.join()
        thread2.join()

        self.drop_database(db_name)

    def test_http_create_delete_index_parallel(self):
        db_name = "default_db"
        table_name = "test_http_test_index_parallel"
        self.create_database(db_name)
        self.create_table(db_name, table_name)

        
        end_time = time.time() + 10  # run 10s

        def create_index_worker(end_time, thread_id):
            while time.time() < end_time:
                index_name = f"index_{thread_id}_{int(time.time())}"
                self.create_index(db_name, table_name, index_name, {"column": "body"})
                time.sleep(0.1)

        def delete_index_worker(end_time, thread_id):
            while time.time() < end_time:
                index_name = f"index_{thread_id}_{int(time.time())}"
                self.drop_index(db_name, table_name, index_name)
                time.sleep(0.1)

        # create thread
        threads = []
        for i in range(5):
            t_create = Thread(target=create_index_worker, args=(end_time, i))
            t_delete = Thread(target=delete_index_worker, args=(end_time, i))
            threads.extend([t_create, t_delete])

        for t in threads:
            t.start()

        for t in threads:
            t.join()

        self.drop_database(db_name)

    def test_http_create_delete_index_parallel(self):
        db_name = "default_db"
        table_name = "test_http_test_index_parallel"
        self.create_database(db_name)
        self.create_table(db_name, table_name)

        end_time = time.time() + 10

        def create_index_worker(end_time, thread_id):
            while time.time() < end_time:
                index_name = f"index_{thread_id}_{int(time.time())}"
                self.create_index(db_name, table_name,  index_name, {"column": "body"})
                time.sleep(0.1)

        def delete_index_worker(end_time, thread_id):
            while time.time() < end_time:
                index_name = f"index_{thread_id}_{int(time.time())}"
                self.drop_index(db_name, table_name, index_name)
                time.sleep(0.1)

        threads = []
        for i in range(5):
            t_create = Thread(target=create_index_worker, args=(end_time, i))
            t_delete = Thread(target=delete_index_worker, args=(end_time, i))
            threads.extend([t_create, t_delete])

        for t in threads:
            t.start()

        for t in threads:
            t.join()

        self.drop_database(db_name)

    def test_http_bulk_insert_query_parallel(self):
        db_name = "default_db"
        table_name = "test_http_test_bulk_insert_query_parallel"
        self.create_database(db_name)
        self.create_table(db_name, table_name)

        data_batch = []
        for i in range(1000):
            data_batch.append({
                "doctitle": f"Document_{i}",
                "docdate": f"2024-01-{i%31 +1:02d}",
                "body": f"Sample body text {i}"
            })

        end_time = time.time() + 10  

        def bulk_insert_worker(end_time):
            while time.time() < end_time:
                self.insert(db_name, table_name, data_batch)
                time.sleep(0.5)

        def query_worker(end_time):
            while time.time() < end_time:
                query = f"SELECT * FROM {table_name} WHERE body LIKE '%Sample%' LIMIT 10"
                self.query_database(db_name, query, {
                    "error_code": 0
                })
                time.sleep(0.2)

        insert_thread = Thread(target=bulk_insert_worker, args=(end_time,))
        query_thread = Thread(target=query_worker, args=(end_time,))

        insert_thread.start()
        query_thread.start()

        insert_thread.join()
        query_thread.join()

        self.drop_database(db_name)
    
    def test_http_fulltext_index_build_delete_parallel(self):
        db_name = "default_db"
        table_name = "test_http_test_fulltext_index_build_delete_parallel"
        self.create_database(db_name)
        self.create_table(db_name, table_name)

        for i in range(100):
            data = {
                "doctitle": f"Doc_{i}",
                "docdate": f"2024-01-{i%31 +1:02d}",
                "body": f"Text content {i}"
            }
            self.insert(db_name, table_name, [data])

        end_time = time.time() + 10  # 运行10秒

        def build_index_worker(end_time, thread_id):
            while time.time() < end_time:
                index_name = f"fulltext_index_{thread_id}_{int(time.time())}"
                self.create_fulltext_index(db_name, table_name, index_name, {"column": "body"})
                time.sleep(0.3)

        def delete_index_worker(end_time, thread_id):
            while time.time() < end_time:
                index_name = f"fulltext_index_{thread_id}_{int(time.time())}"
                self.drop_index(db_name, table_name, index_name)
                time.sleep(0.3)

        threads = []
        for i in range(3):
            t_build = Thread(target=build_index_worker, args=(end_time, i))
            t_delete = Thread(target=delete_index_worker, args=(end_time, i))
            threads.extend([t_build, t_delete])

        for t in threads:
            t.start()

        for t in threads:
            t.join()

        self.drop_database(db_name)
    
    def test_http_index_performance_under_load(self):
        db_name = "default_db"
        table_name = "test_http_test_index_performance_under_load"
        self.create_database(db_name)
        self.create_table(db_name, table_name)

        index_name = "performance_index"
        self.create_index(db_name, table_name, index_name, {"column": "body"})

        data_batch = []
        for i in range(10000):
            data_batch.append({
                "doctitle": f"Document_{i}",
                "docdate": f"2024-01-{i%31 +1:02d}",
                "body": f"Performance test data {i}"
            })
        self.insert(db_name, table_name, data_batch)

        end_time = time.time() + 10  

        def query_worker(end_time, thread_id):
            while time.time() < end_time:
                self.select(db_name, table_name, ["doctitle", "docdate"], "", {
                    "match": {
                        "fields": "body",
                        "query": f"Performance test data {random.randint(0, 9999)}",
                        "operator": "topn=1"
                    }
                })
                time.sleep(0.1)

        threads = []
        for i in range(10):  
            t_query = Thread(target=query_worker, args=(end_time, i))
            threads.append(t_query)

        for t in threads:
            t.start()

        for t in threads:
            t.join()

        self.drop_database(db_name)

    def test_http_update_during_index_build(self):
        """
        Tests perform data updates during index building
        """
        db_name = "default_db"
        table_name = "test_http_test_update_during_index_build"
        self.create_database(db_name)
        self.create_table(db_name, table_name)

        data = {
            "doctitle": "Initial Document",
            "docdate": "2024-01-01",
            "body": "Initial content"
        }
        self.insert(db_name, table_name, [data])

        index_name = "concurrent_index"
        index_thread = Thread(target=self.create_index, args=(db_name, table_name, index_name, {"column": "body"}))
        index_thread.start()

        # Update data during index building
        for i in range(10):
            updated_data = {
                "doctitle": f"Updated Document {i}",
                "docdate": f"2024-01-{i+2:02d}",
                "body": f"Updated content {i}"
            }
            self.update(db_name, table_name, updated_data, "_row_id=1")
            time.sleep(0.5)

        index_thread.join()

        self.select(db_name, table_name, ["doctitle", "docdate"], "", {
            "match": {
                "fields": "body",
                "query": "Updated content 9",
                "operator": "topn=1"
            }
        }, {
            "error_code": 0,
            "results": [
                {
                    "doctitle": "Updated Document 9",
                    "docdate": "2024-01-11"
                }
            ]
        })

        self.drop_database(db_name)

    def test_http_index_operations_on_multiple_tables(self):
        """
        Tests concurrent index operations on different tables
        """
        db_name = "default_db"
        table_names = [f"test_table_{i}" for i in range(5)]
        self.create_database(db_name)
        for table_name in table_names:
            self.create_table(db_name, table_name)

        end_time = time.time() + 10  

        def index_worker(table_name, end_time):
            while time.time() < end_time:
                index_name = f"index_{int(time.time())}"
                self.create_index(db_name, table_name, index_name, {"column": "body"})
                time.sleep(0.2)
                self.drop_index(db_name, table_name, index_name)
                time.sleep(0.2)

        threads = []
        for table_name in table_names:
            t = Thread(target=index_worker, args=(table_name, end_time))
            threads.append(t)

        for t in threads:
            t.start()

        for t in threads:
            t.join()

        self.drop_database(db_name)
    
    def test_http_index_rebuild(self):
        """
        Test index reconstruction capabilities
        """
        db_name = "default_db"
        table_name = "test_http_test_index_rebuild"
        self.create_database(db_name)
        self.create_table(db_name, table_name)

        for i in range(100):
            data = {
                "doctitle": f"Doc_{i}",
                "docdate": f"2024-01-{i%31 +1:02d}",
                "body": f"Content {i}"
            }
            self.insert(db_name, table_name, [data])
        index_name = "rebuild_index"
        self.create_index(db_name, table_name, index_name, {"column": "body"})

        # the index is damaged or needs to be rebuilt
        self.corrupt_index(db_name, table_name, index_name)

        # rebuild index
        self.rebuild_index(db_name, table_name, index_name)

        self.select(db_name, table_name, ["doctitle"], "", {
            "match": {
                "fields": "body",
                "query": "Content 50",
                "operator": "topn=1"
            }
        }, {
            "error_code": 0,
            "results": [
                {
                    "doctitle": "Doc_50"
                }
            ]
        })

        self.drop_database(db_name)
