import infinity
import infinity.index as index
from infinity.common import REMOTE_HOST

infinity_obj = infinity.connect(REMOTE_HOST)

db = infinity_obj.get_database("default")

# Drop my_table if it already exists
db.drop_table("my_table", if_exists=True)
# Create a table named "my_table"
table = db.create_table("my_table", {"num": "integer", "body": "varchar", "vec": "vector, 4, float"}, None)

table.insert([{"num": 1, "body": "unnecessary and harmful", "vec": [1.0, 1.2, 0.8, 0.9]}])
table.insert([{"num": 2, "body": "Office for Harmful Blooms", "vec": [4.0, 4.2, 4.3, 4.5]}])
table.insert([{"num": 3, "body": "abc", "vec": [5.0, 5.2, 5.3, 5.5]}])

res = table.output(["*"]).knn("vec", [3.0, 2.8, 2.7, 3.1], "float", "ip", 2).to_pl()
print(res)