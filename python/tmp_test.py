import infinity
import time
import json
import infinity.index as index
from infinity import NetworkAddress
from infinity.common import ConflictType

infinity_obj = infinity.connect(NetworkAddress("127.0.0.1", 23817))
geonames_file_path = "test/data/benchmark/geonames/geonames.json"
geonames_index_path = "test/data/benchmark/geonames/index.json"


db_obj = infinity_obj.get_database("default_db")
db_obj.drop_table("geonames")
table_obj = db_obj.create_table(
    "geonames",
    {
        "name": "varchar",
        "feature_class": "varchar",
        "cc2": "varchar",
        "timezone": "varchar",
        "dem": "varchar",
        "country_code": "varchar",
        "admin1_code": "varchar",
        "admin2_code": "varchar",
        "admin3_code": "varchar",
        "admin4_code": "varchar",
        "feature_code": "varchar",
        "alternatenames": "varchar",
        "asciiname": "varchar",
    },
    ConflictType.Error,
)
fields = [
    "name",
    "feature_class",
    "cc2",
    "timezone",
    "dem",
    "country_code",
    "admin1_code",
    "admin2_code",
    "admin3_code",
    "admin4_code",
    "feature_code",
    "alternatenames",
    "asciiname",
]
start = time.time()
with open(geonames_file_path, "r") as file:
    values = []
    for i, line in enumerate(file):
        if i != 0 and i % 500 == 0:
            table_obj.insert(values)
            values = []
        record = json.loads(line)
        value = {}
        for field in fields:
            value[field] = record.get(field, "233")
        values.append(value)
    if len(values):
        table_obj.insert(values)
    table_obj.create_index(
        "idx1",
        index.IndexInfo("name", index.IndexType.FullText, []),
    )
    table_obj.create_index(
        "idx2",
        index.IndexInfo("feature_class", index.IndexType.FullText, []),
    )
    table_obj.create_index(
        "idx3",
        index.IndexInfo("cc2", index.IndexType.FullText, []),
    )
    table_obj.create_index(
        "idx4",
        index.IndexInfo("timezone", index.IndexType.FullText, []),
    )
    table_obj.create_index(
        "idx5",
        index.IndexInfo("dem", index.IndexType.FullText, []),
    )
    table_obj.create_index(
        "idx12",
        index.IndexInfo("country_code", index.IndexType.FullText, []),
    )
    table_obj.create_index(
        "idx6",
        index.IndexInfo("admin1_code", index.IndexType.FullText, []),
    )
    table_obj.create_index(
        "idx7",
        index.IndexInfo("admin2_code", index.IndexType.FullText, []),
    )
    table_obj.create_index(
        "idx8",
        index.IndexInfo("admin3_code", index.IndexType.FullText, []),
    )
    table_obj.create_index(
        "idx9",
        index.IndexInfo("feature_code", index.IndexType.FullText, []),
    )
    table_obj.create_index(
        "idx10",
        index.IndexInfo("alternatenames", index.IndexType.FullText, []),
    )
    table_obj.create_index(
        "idx11",
        index.IndexInfo("asciiname", index.IndexType.FullText, []),
    )
    table_obj.create_index(
        "idx13",
        index.IndexInfo("admin4_code", index.IndexType.FullText, []),
    )
end = time.time()
print("Time to index geonames: ", end - start)

start = time.time()
table_obj.output(["name"]).match_text("name", "Sankt Georgen", 10).to_pl()
end = time.time()
