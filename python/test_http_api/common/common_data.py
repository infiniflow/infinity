default_url = "http://localhost:23820/"
expected_status_code = 200

baseHeader = {
    'accept': 'application/json',
    'content-type': 'application/json',
}

baseData = {
    "create_option": "ignore_if_exists",
    "drop_option": "ignore_if_not_exists",
    "fields": {},
    "properties": {}
}

types = [
    "integer", "tinyint", "smallint", "bigint", "hugeint", "float",
    "double", "varchar", "boolean"
]

baseResponse = {
    "error_code": 0,
}

baseCreateOptions = {
    "kError": 'error',
    "kIgnore": "ignore_if_exists",
    "kReplace": "replace_if_exists"
}

baseDropOptions = {
    "kError": "error",
    "kIgnore": "ignore_if_not_exists",
}

tableDefaultData = {
    "fields":
        {
            "name":
                {
                    "type": "varchar",
                    "constraints": ["not null"]
                },
            "age":
                {
                    "type": "integer",
                    "constraints": ["not null"]
                },
            "score":
                {
                    "type": "integer",
                    "constraints": ["not null"]
                }
        },
    "properties":
        {
            "bloomfilter_columns":
                {
                    "age",
                    "score"
                }
        }
}
