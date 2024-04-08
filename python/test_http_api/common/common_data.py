
default_url = "http://localhost:23820/"
expected_status_code = 200

baseHeader = {
    'accept': 'application/json',
    'content-type': 'application/json',
}

baseResponse = {
    "error_code": 0,
}

baseData = {
    "create_option": "ignore_if_exists",
    "drop_option": "ignore_if_not_exists",
    "fields": {},
    "properties": {}
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


