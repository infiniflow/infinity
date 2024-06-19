
import infinity
import infinity.index as index
from infinity.common import REMOTE_HOST, ConflictType

infinity_obj = infinity.connect(REMOTE_HOST)
db = infinity_obj.get_database("default_db")
table = db.get_table("t2")
res = table.output(["c2"]).filter("c2 = '458558@2'").to_pl()
print(res)