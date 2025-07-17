import os
import time

import infinity.index as index
import pandas
import pytest
import random
from threading import Thread
from infinity.common import ConflictType
from infinity.errors import ErrorCode
from infinity.connection_pool import ConnectionPool
from threading import Lock

TEST_DATA_DIR = "/test/data/"
fulltext_file_path = os.getcwd() + TEST_DATA_DIR + "csv/enwiki_99.csv"
vector_file_path = os.getcwd() + TEST_DATA_DIR + "csv/pysdk_test_knn.csv"
kRunningTime = 30
kNumThread = 4
data_size = 100000
insert_delete_size = 100

# Global counters for snapshot operations
snapshot_stats = {
    'create_success': 0,
    'create_failed': 0,
    'restore_success': 0,
    'restore_failed': 0,
    'drop_success': 0,
    'drop_failed': 0
}
snapshot_stats_lock = Lock()

# Global counters for data operations
operation_stats = {
    'insert_count': 0,
    'delete_count': 0,
    'update_count': 0,
    'search_fulltext_count': 0,
    'search_vector_count': 0
}
operation_stats_lock = Lock()


class TestIndexParallel:
    # @pytest.mark.skip(reason="To pass benchmark, use wrong row count in knn scan")
    def test_chaos(self, get_infinity_connection_pool):
        # Reset statistics at the beginning of each test
        global snapshot_stats, operation_stats
        with snapshot_stats_lock:
            snapshot_stats = {
                'create_success': 0,
                'create_failed': 0,
                'restore_success': 0,
                'restore_failed': 0,
                'drop_success': 0,
                'drop_failed': 0
            }
        with operation_stats_lock:
            operation_stats = {
                'insert_count': 0,
                'delete_count': 0,
                'update_count': 0,
                'search_fulltext_count': 0,
                'search_vector_count': 0
            }
        
        data = read_out_data()
        connection_pool = get_infinity_connection_pool
        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")

        res = db_obj.drop_table("chaos_test", ConflictType.Ignore)
        assert res.error_code == ErrorCode.OK
        table_obj = db_obj.create_table("chaos_test", {
            "index": {"type": "int"}, "body": {"type": "varchar"}, "other_vector": {"type": "vector,4,float"}},
                                        ConflictType.Error)
        res = table_obj.create_index("body_index",
                                     index.IndexInfo("body",
                                                     index.IndexType.FullText))
        assert res.error_code == ErrorCode.OK
        res = table_obj.create_index("other_index",
                                     index.IndexInfo("other_vector", index.IndexType.Hnsw,{
                                         "M": "16",
                                         "ef_construction": "50",
                                         "metric": "l2"
                                     }), ConflictType.Error)
        assert res.error_code == ErrorCode.OK
        connection_pool.release_conn(infinity_obj)

        threads = []
        end_time = time.time() + kRunningTime
        print(f"Starting chaos test for {kRunningTime} seconds with {kNumThread} threads")
        for i in range(kNumThread):
            threads.append(Thread(target=random_exec, args=[connection_pool, data, end_time, i]))
        for i in range(len(threads)):
            threads[i].start()
        for i in range(len(threads)):
            threads[i].join()
        print(f"Chaos test completed in {time.time() - (end_time - kRunningTime):.1f} seconds")

        infinity_obj = connection_pool.get_conn()
        db_obj = infinity_obj.get_database("default_db")
        res = db_obj.drop_table("chaos_test", ConflictType.Error)
        
        # Clean up any snapshots created during the test
        try:
            snapshots_res = infinity_obj.list_snapshots()
            if snapshots_res.error_code == ErrorCode.OK:
                for snapshot in snapshots_res.snapshots:
                    if snapshot.name.startswith("chaos_snapshot_"):
                        infinity_obj.drop_snapshot(snapshot.name)
        except Exception as e:
            pass
        
        # Print final statistics
        print("\n" + "="*60)
        print("📊 SNAPSHOT OPERATION STATISTICS")
        print("="*60)
        print(f"Create Snapshot - Success: {snapshot_stats['create_success']}, Failed: {snapshot_stats['create_failed']}")
        print(f"Restore Snapshot - Success: {snapshot_stats['restore_success']}, Failed: {snapshot_stats['restore_failed']}")
        print(f"Drop Table - Success: {snapshot_stats['drop_success']}, Failed: {snapshot_stats['drop_failed']}")
        print(f"Combined Create+Drop Operations: {min(snapshot_stats['create_success'], snapshot_stats['drop_success'])} successful")
        
        # Calculate success rates
        total_create = snapshot_stats['create_success'] + snapshot_stats['create_failed']
        total_restore = snapshot_stats['restore_success'] + snapshot_stats['restore_failed']
        total_drop = snapshot_stats['drop_success'] + snapshot_stats['drop_failed']
        
        if total_create > 0:
            create_rate = (snapshot_stats['create_success'] / total_create) * 100
            print(f"Create Snapshot Success Rate: {create_rate:.1f}%")
        
        if total_restore > 0:
            restore_rate = (snapshot_stats['restore_success'] / total_restore) * 100
            print(f"Restore Snapshot Success Rate: {restore_rate:.1f}%")
        
        if total_drop > 0:
            drop_rate = (snapshot_stats['drop_success'] / total_drop) * 100
            print(f"Drop Table Success Rate: {drop_rate:.1f}%")
        
        print("\n" + "="*60)
        print("📊 DATA OPERATION STATISTICS")
        print("="*60)
        print(f"Insert Operations: {operation_stats['insert_count']}")
        print(f"Delete Operations: {operation_stats['delete_count']}")
        print(f"Update Operations: {operation_stats['update_count']}")
        print(f"Fulltext Search Operations: {operation_stats['search_fulltext_count']}")
        print(f"Vector Search Operations: {operation_stats['search_vector_count']}")
        
        total_operations = (operation_stats['insert_count'] + operation_stats['delete_count'] + 
                          operation_stats['update_count'] + operation_stats['search_fulltext_count'] + 
                          operation_stats['search_vector_count'])
        print(f"Total Data Operations: {total_operations}")
        
        print("="*60)
        
        connection_pool.release_conn(infinity_obj)


def read_out_data():
    column_names = ["doctitle", "docdate", "body"]
    df_fulltext = pandas.read_csv(fulltext_file_path, delimiter="\t", header=None, names=column_names)["body"]
    df_vector = pandas.DataFrame({"other_vector": [[0.0, 0.0, 0.0, 0.0], [1.1, 1.1, 1.1, 1.1], [2.2, 2.2, 2.2, 2.2],
                                                   [3.3, 3.3, 3.3, 3.3], [4.4, 4.4, 4.4, 4.4], [5.5, 5.5, 5.5, 5.5],
                                                   [6.6, 6.6, 6.6, 6.6], [7.7, 7.7, 7.7, 7.7], [8.8, 8.8, 8.8, 8.8],
                                                   [9.9, 9.9, 9.9, 9.9]]})
    print(df_vector)
    df_fulltext = pandas.concat([df_fulltext] * int(data_size / 10), ignore_index=True)
    df_vector = pandas.concat([df_vector] * int(data_size / 10), ignore_index=True)
    df_fulltext = df_fulltext.reset_index()
    df_vector = df_vector.reset_index()
    df = pandas.merge(df_vector, df_fulltext, on='index')
    df["body"] = df["body"].astype(str)
    print(df.dtypes)
    data = {key: list(value.values())
            for key, value in df.to_dict().items()}
    return data


def search_fulltext(table_obj):
    global operation_stats
    res, extra_result = table_obj.output(["index", "body", "other_vector", "_row_id", "_score"]).match_text(
        "body^5", "harmful chemical", 3).to_pl()
    with operation_stats_lock:
        operation_stats['search_fulltext_count'] += 1
    # Add sleep to allow more snapshot operations
    time.sleep(0.03)


def search_vector(table_obj):
    global operation_stats
    res, extra_result = table_obj.output(["*"]).match_dense("other_vector", [2] * 4, "float", "l2", 3).to_pl()
    with operation_stats_lock:
        operation_stats['search_vector_count'] += 1
    # Add sleep to allow more snapshot operations
    time.sleep(0.03)


def insert(table_obj, data):
    global operation_stats
    try:
        pos = random.randint(0, int(data_size / insert_delete_size) - 1)
        value = []
        for i in range(insert_delete_size):
            value.append({"index": data["index"][i + pos], "body": data["body"][i + pos],
                          "other_vector": data["other_vector"][i + pos]})
        table_obj.insert(value)
        with operation_stats_lock:
            operation_stats['insert_count'] += 1
        # Add sleep to allow more snapshot operations
        time.sleep(0.05)
    except Exception as e:
        pass


def delete(table_obj):
    global operation_stats
    pos = random.randint(0, int(data_size / insert_delete_size) - 1)
    try:
        table_obj.delete(f"index >= {pos} and index < {pos + insert_delete_size}")
        with operation_stats_lock:
            operation_stats['delete_count'] += 1
        # Add sleep to allow more snapshot operations
        time.sleep(0.05)
    except Exception as e:
        pass


def updata(table_obj):
    global operation_stats
    pos = random.randint(0, data_size - 1)
    try:
        table_obj.update(f"index = {pos}", [{"index": pos, "body": "infinity", "other_vector": [0.0, 0.0, 0.0, 0.0]}])
        with operation_stats_lock:
            operation_stats['update_count'] += 1
        # Add sleep to allow more snapshot operations
        time.sleep(0.05)
    except Exception as e:
        pass


def create_snapshot(db_obj, thread_id):
    """Create a snapshot of the chaos_test table"""
    global snapshot_stats
    try:
        snapshot_name = f"chaos_snapshot_{thread_id}_{random.randint(1, 1000)}"
        res = db_obj.create_table_snapshot(snapshot_name, "chaos_test")
        if res.error_code == ErrorCode.OK:
            with snapshot_stats_lock:
                snapshot_stats['create_success'] += 1
        else:
            with snapshot_stats_lock:
                snapshot_stats['create_failed'] += 1
    except Exception as e:
        with snapshot_stats_lock:
            snapshot_stats['create_failed'] += 1


def restore_snapshot(db_obj, thread_id):
    """Restore a table from snapshot"""
    global snapshot_stats
    try:
        # List available snapshots
        snapshots_res = db_obj.get_infinity_obj().list_snapshots()
        if snapshots_res.error_code == ErrorCode.OK and snapshots_res.snapshots:
            # Pick a random snapshot
            snapshot = random.choice(snapshots_res.snapshots)
            snapshot_name = snapshot.name
            
            # Drop the current table if it exists
            try:
                db_obj.drop_table("chaos_test", ConflictType.Ignore)
            except:
                pass
            
            # Restore from snapshot
            res = db_obj.restore_table_snapshot(snapshot_name)
            if res.error_code == ErrorCode.OK:
                with snapshot_stats_lock:
                    snapshot_stats['restore_success'] += 1
            else:
                with snapshot_stats_lock:
                    snapshot_stats['restore_failed'] += 1
        else:
            with snapshot_stats_lock:
                snapshot_stats['restore_failed'] += 1
    except Exception as e:
        with snapshot_stats_lock:
            snapshot_stats['restore_failed'] += 1


def drop_table(db_obj, thread_id):
    """Drop the chaos_test table"""
    global snapshot_stats
    try:
        res = db_obj.drop_table("chaos_test", ConflictType.Ignore)
        if res.error_code == ErrorCode.OK:
            with snapshot_stats_lock:
                snapshot_stats['drop_success'] += 1
        else:
            with snapshot_stats_lock:
                snapshot_stats['drop_failed'] += 1
    except Exception as e:
        with snapshot_stats_lock:
            snapshot_stats['drop_failed'] += 1


def random_exec(connection_pool: ConnectionPool, data, end_time, thread_id):
    infinity_obj = connection_pool.get_conn()
    db_obj = infinity_obj.get_database("default_db")
    
    # Track start time for timing logic
    start_time = time.time()
    test_duration = kRunningTime
    
    # Try to get the table, restore from snapshot if it doesn't exist
    
    
    while time.time() < end_time:
        # Calculate elapsed time
        elapsed_time = time.time() - start_time
        rand_v = random.randint(0, 5)

        
        try:
            try:
                table_obj = db_obj.get_table("chaos_test")
            except:
                # Table doesn't exist, try to restore from snapshot
                snapshots_res = infinity_obj.list_snapshots()
                if snapshots_res.error_code == ErrorCode.OK and snapshots_res.snapshots:
                    # Pick a random snapshot to restore from
                    snapshot = random.choice(snapshots_res.snapshots)
                    snapshot_name = snapshot.name
                    
                    # Restore from snapshot
                    res = db_obj.restore_table_snapshot(snapshot_name)
                    if res.error_code == ErrorCode.OK:
                        with snapshot_stats_lock:
                            snapshot_stats['restore_success'] += 1
                        table_obj = db_obj.get_table("chaos_test")
                    else:
                        with snapshot_stats_lock:
                            snapshot_stats['restore_failed'] += 1
                        connection_pool.release_conn(infinity_obj)
                        return

            if rand_v == 0:
                insert(table_obj, data)
                time.sleep(1)
            elif rand_v == 1:
                delete(table_obj)
                time.sleep(0.01)
            elif rand_v == 2:
                updata(table_obj)
                time.sleep(0.01)
            elif rand_v == 3:
                search_fulltext(table_obj)
                time.sleep(0.01)
            elif rand_v == 4:
                search_vector(table_obj)
                time.sleep(0.01)
            elif rand_v == 5:  # Only in first 10 seconds
                # Combined operation: create snapshot then drop table
                if(elapsed_time <= 20):
                    # First create snapshot
                    snapshot_name = f"chaos_snapshot_{thread_id}_{random.randint(1, 1000)}"
                    res = db_obj.create_table_snapshot(snapshot_name, "chaos_test")
                    if res.error_code == ErrorCode.OK:
                        with snapshot_stats_lock:
                            snapshot_stats['create_success'] += 1
                        # Only drop table if snapshot creation succeeded
                        drop_table(db_obj, thread_id)
                    else:
                        with snapshot_stats_lock:
                            snapshot_stats['create_failed'] += 1
                else:
                    drop_table(db_obj, thread_id)
        except Exception as e:
            # If any operation fails, try to get the table object again
            try:
                table_obj = db_obj.get_table("chaos_test")
            except:
                pass
        
        # Small delay to prevent overwhelming the system
        time.sleep(0.005)
    
    connection_pool.release_conn(infinity_obj)
