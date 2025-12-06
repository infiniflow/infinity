import ast
import re
from enum import Enum
from numpy import dtype
from infinity.common import ConflictType
from infinity.table import ExplainType

default_url = "http://localhost:23820/"

baseHeader = {
    "accept": "application/json",
    "content-type": "application/json",
}

baseData = {
    "create_option": "ignore_if_exists",
    "drop_option": "ignore_if_not_exists",
    "fields": {},
    "properties": {},
}

types = [
    "integer",
    "tinyint",
    "smallint",
    "bigint",
    "hugeint",
    "float",
    "double",
    "varchar",
    "boolean",
]

functions = [
    "sqrt",
    "round",
    "ceil",
    "floor",
    "filter_text",
    "filter_fulltext",
    "or",
    "and",
    "not",
    "char_length",
    "sum",
    "min",
    "max",
    "count",
    "avg",
    "trunc",
    "datepart",
]

bool_functions = ["filter_text", "filter_fulltext", "or", "and", "not"]


def function_return_type(function_name, param_type):
    if function_name == "sqrt":
        return dtype("float64")
    elif (
        function_name == "round" or function_name == "ceil" or function_name == "floor"
    ):
        if (
            param_type == dtype("int8")
            or param_type == dtype("int16")
            or param_type == dtype("int32")
            or param_type == dtype("int64")
        ):
            return param_type
        else:
            return dtype("float64")
    elif (
        function_name == "filter_text"
        or function_name == "filter_fulltext"
        or function_name == "or"
        or function_name == "and"
        or function_name == "not"
    ):
        return dtype("bool")
    elif function_name == "trunc":
        return dtype("str_")
    elif function_name == "datepart":
        return dtype("int64")
    elif function_name == "char_length":
        return dtype("int32")
    elif function_name == "sum":
        if (
            param_type == dtype("int8")
            or param_type == dtype("int16")
            or param_type == dtype("int32")
            or param_type == dtype("int64")
        ):
            return dtype("int64")
        else:
            return dtype("float64")
    elif function_name == "min" or function_name == "max":
        return param_type
    elif function_name == "count":
        return dtype("int64")
    elif function_name == "avg":
        return dtype("float64")
    else:
        return param_type


unsupport_output = ["_similarity", "_row_id", "_score", "_distance"]

type_transfrom = {
    "int": "integer",
    "uint8": "uint8",
    "int8": "tinyint",
    "int16": "smallint",
    "int32": "integer",
    "integer": "integer",
    "int64": "bigint",  # hugeint unsupport
    "float16": "float16",
    "bfloat16": "bfloat16",
    "float": "float",
    "float32": "float",
    "float64": "double",
    "double": "double",
    "varchar": "varchar",
    "bool": "boolean",
    "INT": "INTEGER",
    "FLOAT": "FLOAT",
}


def type_to_dtype(type):
    match type.lower():
        case "bool":
            return dtype("bool")
        case "boolean":
            return dtype("bool")
        case "tinyint":
            return dtype("int8")
        case "smallint":
            return dtype("int16")
        case "integer":
            return dtype("int32")
        case "int":
            return dtype("int32")
        case "int32":
            return dtype("int32")
        case "bigint":
            return dtype("int64")
        case "float":
            return dtype("float32")
        case "float32":
            return dtype("float32")
        case "float64":
            return dtype("float64")
        case "float16":
            return dtype("float32")
        case "bfloat16":
            return dtype("float32")
        case "double":
            return dtype("float64")
        case "varchar":
            return dtype("str")
        case _:
            return object


def ExplainType_transfrom(explain_type):
    if explain_type == ExplainType.Ast:
        return "ast"
    elif explain_type == ExplainType.UnOpt:
        return "unopt"
    elif explain_type == ExplainType.Opt:
        return "opt"
    elif explain_type == ExplainType.Physical:
        return "physical"
    elif explain_type == ExplainType.Pipeline:
        return "pipeline"
    elif explain_type == ExplainType.Fragment:
        return "fragment"
    elif explain_type == ExplainType.Analyze:
        return "analyze"
    else:
        return "invalid"


def is_float(str):
    try:
        float(str)
        return True
    except Exception:
        return False


def is_list(str):
    try:
        result = ast.literal_eval(str)
        return isinstance(result, list)
    except Exception:
        return False


def is_bool(str):
    return str.lower() == "true" or str.lower() == "false"


def is_sparse(str_input):
    tmp = str_input.replace("[", "")
    tmp = tmp.replace("]", "")
    pairs = tmp.split(",")
    for pair in pairs:
        t = pair.split(":")
        if len(t) != 2:
            return False
        if not (t[0].isdigit() or is_float(t[0])):
            return False
        if not (t[1].isdigit() or is_float(t[1])):
            return False
    return True


def str2sparse(str_input):
    sparce_vec = {}
    tmp = str_input.replace("[", "")
    tmp = tmp.replace("]", "")
    pairs = tmp.split(",")
    for pair in pairs:
        t = pair.split(":")
        sparce_vec[str(eval(t[0]))] = eval(t[1])

    return sparce_vec


def is_date(str_input):
    if re.match(r"^\b\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01])\b$", str_input):
        return True
    return False


def is_time(str_input):
    if re.match(r"^([01]\d|2[0-3]):[0-5]\d:[0-5]\d$", str_input):
        return True
    return False


def is_datetime(str_input):
    if re.match(
        r"^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01]) ([01]\d|2[0-3]):[0-5]\d:[0-5]\d$",
        str_input,
    ):
        return True
    return False


index_type_transfrom = {
    1: "IVF",
    2: "HNSW",
    3: "FULLTEXT",
    4: "SECONDARY",
    5: "EMVB",
    6: "BMP",
}

baseResponse = {
    "error_code": 0,
}

baseCreateOptions = {
    ConflictType.Ignore: "ignore_if_exists",
    ConflictType.Error: "error",
    ConflictType.Replace: "replace_if_exists",
}

baseDropOptions = {
    ConflictType.Ignore: "ignore_if_not_exists",
    ConflictType.Error: "error",
}

tableDefaultData = {
    "fields": {
        "name": {"type": "varchar", "constraints": ["not null"]},
        "age": {"type": "integer", "constraints": ["not null"]},
        "score": {"type": "integer", "constraints": ["not null"]},
    },
    "properties": {"bloomfilter_columns": {"age", "score"}},
}


class literaltype(Enum):
    kBoolean = 0
    kDouble = 1
    kString = 2
    kInteger = 3
    kNull = 4
    kDate = 5
    kTime = 6
    kDateTime = 7
    kTimestamp = 8
    kIntegerArray = 9
    kDoubleArray = 10
    kSubArrayArray = 11
    kInterval = 12
    kLongSparseArray = 13
    kDoubleSparseArray = 14
    kEmptyArray = 15


type_to_literaltype = {
    "boolean": literaltype.kBoolean.value,
    "bool": literaltype.kBoolean.value,
    "tinyint": literaltype.kInteger.value,
    "int8": literaltype.kInteger.value,
    "smallint": literaltype.kInteger.value,
    "int16": literaltype.kInteger.value,
    "int": literaltype.kInteger.value,
    "int32": literaltype.kInteger.value,
    "integer": literaltype.kInteger.value,
    "bigint": literaltype.kInteger.value,
    "int64": literaltype.kInteger.value,
    "float": literaltype.kDouble.value,
    "float32": literaltype.kDouble.value,
    "double": literaltype.kDouble.value,
    "float64": literaltype.kDouble.value,
}

type_to_vector_literaltype = {
    "integer": literaltype.kIntegerArray.value,
    "float": literaltype.kDoubleArray.value,
    "double": literaltype.kDoubleArray.value,
}
