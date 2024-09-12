import threading
import time
import pytest
from infinity.connection_pool import ConnectionPool
from infinity_embedded.local_infinity.table import get_table


#Setting the number of concurrent transactions and test duration
NUM_TRANSACTIONS = 10
TEST_DURATION_SECONDS = 60

def transaction_routine(connection_pool, transaction_id, results):
    try:
        db_connection = connection_pool.get_conn()
        db = db_connection.get_database("default_db")
        table = get_table(db_connection, "default_db", "parallel_test_table")

        db.start_transaction()

        table.insert({"transaction_id": transaction_id, "value": transaction_id * 100})
        table.update(f"transaction_id = {transaction_id}", {"value": transaction_id * 100 + 50})
        result = table.query(f"transaction_id = {transaction_id}")

        if result['value'] == transaction_id * 100 + 50:
            results[transaction_id] = "Success"
        else:
            results[transaction_id] = f"data mismatch: expected {transaction_id * 100 + 50}, got {result['value']}"

        db.commit_transaction()

    except Exception as e:
        results[transaction_id] = f"fail, error is: {str(e)}"
    finally:
        connection_pool.release_conn(db_connection)

@pytest.mark.concurrency
def test_transaction_concurrency():

    connection_pool = ConnectionPool()

    results = {}

    threads = []
    for i in range(NUM_TRANSACTIONS):
        thread = threading.Thread(target=transaction_routine, args=(connection_pool, i, results))
        threads.append(thread)

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()
    
    print("transaction results:")
    for transaction_id, result in results.items():
        print(f"transaction {transaction_id}: {result}")
        
    assert all(res == "Success" for res in results.values()), "some transactions failed or data mismatches occurred"