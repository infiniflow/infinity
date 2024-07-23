from abc import ABC

import infinity.remote_thrift.infinity_thrift_rpc.ttypes as ttypes
import numpy as np
from infinity.db import Database
from infinity.errors import ErrorCode
from infinity.local_infinity.table import LocalTable
from infinity.remote_thrift.utils import check_valid_name, name_validity_check
from infinity.common import ConflictType, InfinityException
from infinity.local_infinity.utils import select_res_to_polars
from infinity.embedded_infinity_ext import ConflictType as LocalConflictType
from infinity.embedded_infinity_ext import WrapColumnDef, WrapDataType, LogicalType, ConstraintType, LiteralType, WrapConstantExpr, \
    EmbeddingDataType, WrapEmbeddingType, WrapSparseType, WrapIndexInfo

def get_constant_expr(column_info):
    # process constant expression
    default = None
    if "default" in column_info:
        default = column_info["default"]

    constant_expression = WrapConstantExpr()
    if default is None:
        constant_expression.literal_type = LiteralType.kNull
        return constant_expression
    else:
        if isinstance(default, str):
            constant_expression.literal_type = LiteralType.kString
            constant_expression.str_value = default

        elif isinstance(default, int):
            constant_expression.literal_type = LiteralType.kInteger
            constant_expression.i64_value = default

        elif isinstance(default, float) or isinstance(default, np.float32):
            constant_expression.literal_type = LiteralType.kDouble
            constant_expression.f64_value = default

        elif isinstance(default, list):
            if isinstance(default[0], int):
                constant_expression.literal_type = LiteralType.kIntegerArray
                constant_expression.i64_array_value = default
            elif isinstance(default[0], float):
                constant_expression.literal_type = LiteralType.kDoubleArray
                constant_expression.f64_array_value = default
        else:
            raise InfinityException(ErrorCode.INVALID_EXPRESSION, "Invalid constant expression")
        return constant_expression

def get_ordinary_info(column_info, column_defs, column_name, index):
    # "c1": {"type": "int", "constraints":["primary key", ...], "default": 1/"asdf"/[1,2]/...}
    # process column definition

    proto_column_def = WrapColumnDef()
    proto_column_def.id = index
    proto_column_def.column_name = column_name
    proto_column_def.column_type.logical_type = LogicalType.kInvalid

    for key, value in column_info.items():
        lower_key = key.lower()
        match lower_key:
            case "type":
                datatype = value.lower()
                column_big_info = [item.strip() for item in datatype.split(",")]
                column_big_info_first_str = column_big_info[0].lower()
                if column_big_info_first_str == "vector" or column_big_info_first_str == "tensor" or column_big_info_first_str == "tensorarray":
                    return get_embedding_info(column_info, column_defs, column_name, index)
                elif column_big_info_first_str == "sparse":
                    return get_sparse_info(column_info, column_defs, column_name, index)
                else:
                    pass

                proto_column_type = WrapDataType()
                match datatype:
                    case "int8":
                        proto_column_type.logical_type = LogicalType.kTinyInt
                    case "int16":
                        proto_column_type.logical_type = LogicalType.kSmallInt
                    case "int32" | "int" | "integer":
                        proto_column_type.logical_type = LogicalType.kInteger
                    case "int64":
                        proto_column_type.logical_type = LogicalType.kBigInt
                    case "int128":
                        proto_column_type.logical_type = LogicalType.kHugeInt
                    case "float" | "float32":
                        proto_column_type.logical_type = LogicalType.kFloat
                    case "double" | "float64":
                        proto_column_type.logical_type = LogicalType.kDouble
                    case "varchar":
                        proto_column_type.logical_type = LogicalType.kVarchar
                    case "bool":
                        proto_column_type.logical_type = LogicalType.kBoolean
                    case _:
                        raise InfinityException(ErrorCode.INVALID_DATA_TYPE, f"Unknown datatype: {datatype}")
                proto_column_def.column_type = proto_column_type

            case "constraints":
                # process constraints
                constraints = value
                for constraint in constraints:
                    constraint = constraint.lower()
                    match constraint:
                        case "null":
                            if ConstraintType.kNull not in proto_column_def.constraints:
                                proto_column_def.constraints.add(ConstraintType.kNull)
                            else:
                                raise InfinityException(ErrorCode.INVALID_CONSTRAINT_TYPE, f"Duplicated constraint: {constraint}")
                        case "not null":
                            if ConstraintType.kNotNull not in proto_column_def.constraints:
                                proto_column_def.constraints.add(ConstraintType.kNotNull)
                            else:
                                raise InfinityException(ErrorCode.INVALID_CONSTRAINT_TYPE, f"Duplicated constraint: {constraint}")
                        case "primary key":
                            if ConstraintType.kPrimaryKey not in proto_column_def.constraints:
                                proto_column_def.constraints.add(ConstraintType.kPrimaryKey)
                            else:
                                raise InfinityException(ErrorCode.INVALID_CONSTRAINT_TYPE, f"Duplicated constraint: {constraint}")
                        case "unique":
                            if ConstraintType.kUnique not in proto_column_def.constraints:
                                proto_column_def.constraints.add(ConstraintType.kUnique)
                            else:
                                raise InfinityException(ErrorCode.INVALID_CONSTRAINT_TYPE, f"Duplicated constraint: {constraint}")
                        case _:
                            raise InfinityException(ErrorCode.INVALID_CONSTRAINT_TYPE, f"Unknown constraint: {constraint}")

    if proto_column_def.column_type.logical_type is None:
        raise InfinityException(ErrorCode.NO_COLUMN_DEFINED, f"Column definition without data type")

    proto_column_def.constant_expr = get_constant_expr(column_info)

    column_defs.append(proto_column_def)


def get_embedding_info(column_info, column_defs, column_name, index):
    # "vector,1024,float32"
    column_big_info = [item.strip() for item in column_info["type"].split(",")]
    length = column_big_info[1]
    element_type = column_big_info[2]

    proto_column_def = WrapColumnDef()
    proto_column_def.id = index
    proto_column_def.column_name = column_name
    column_type = WrapDataType()

    if column_big_info[0] == "vector":
        column_type.logical_type = LogicalType.kEmbedding
    elif column_big_info[0] == "tensor":
        column_type.logical_type = LogicalType.kTensor
    elif column_big_info[0] == "tensorarray":
        column_type.logical_type = LogicalType.kTensorArray
    else:
        raise InfinityException(ErrorCode.FTS_INDEX_NOT_EXIST, f"Unknown column type: {column_big_info[0]}")

    embedding_type = WrapEmbeddingType()
    if element_type == "bit":
        embedding_type.element_type = EmbeddingDataType.kElemBit
    elif element_type == "float32" or element_type == "float":
        embedding_type.element_type = EmbeddingDataType.kElemFloat
    elif element_type == "float64" or element_type == "double":
        embedding_type.element_type = EmbeddingDataType.kElemDouble
    elif element_type == "int8":
        embedding_type.element_type = EmbeddingDataType.kElemInt8
    elif element_type == "int16":
        embedding_type.element_type = EmbeddingDataType.kElemInt16
    elif element_type == "int32" or element_type == "int":
        embedding_type.element_type = EmbeddingDataType.kElemInt32
    elif element_type == "int64":
        embedding_type.element_type = EmbeddingDataType.kElemInt64
    else:
        raise InfinityException(ErrorCode.INVALID_EMBEDDING_DATA_TYPE, f"Unknown element type: {element_type}")
    embedding_type.dimension = int(length)
    assert isinstance(embedding_type, WrapEmbeddingType)
    assert embedding_type.element_type is not None
    assert embedding_type.dimension is not None
    # physical_type = ttypes.PhysicalType()
    # physical_type.embedding_type = embedding_type
    column_type.embedding_type = embedding_type
    proto_column_def.column_type = column_type

    proto_column_def.constant_expr = get_constant_expr(column_info)

    column_defs.append(proto_column_def)

def get_sparse_info(column_info, column_defs, column_name, index):
    # "sparse,30000,float32,int16"
    column_big_info = [item.strip() for item in column_info["type"].split(",")]
    length = column_big_info[1]
    element_type = column_big_info[2]
    index_type = column_big_info[3]

    proto_column_def = WrapColumnDef()
    proto_column_def.id = index
    proto_column_def.column_name = column_name
    column_type = WrapDataType()

    if column_big_info[0] == "sparse":
        column_type.logical_type = LogicalType.kSparse
    else:
        raise InfinityException(ErrorCode.FTS_INDEX_NOT_EXIST, f"Unknown column type: {column_big_info[0]}")

    sparse_type = WrapSparseType()
    # embedding_type = WrapEmbeddingType()
    if element_type == "bit":
        sparse_type.element_type = EmbeddingDataType.kElemBit
    elif element_type == "float32" or element_type == "float":
        sparse_type.element_type = EmbeddingDataType.kElemFloat
    elif element_type == "float64" or element_type == "double":
        sparse_type.element_type = EmbeddingDataType.kElemDouble
    elif element_type == "int8":
        sparse_type.element_type = EmbeddingDataType.kElemInt8
    elif element_type == "int16":
        sparse_type.element_type = EmbeddingDataType.kElemInt16
    elif element_type == "int32" or element_type == "int":
        sparse_type.element_type = EmbeddingDataType.kElemInt32
    elif element_type == "int64":
        sparse_type.element_type = EmbeddingDataType.kElemInt64
    else:
        raise InfinityException(ErrorCode.INVALID_EMBEDDING_DATA_TYPE, f"Unknown element type: {element_type}")
    
    if index_type == "int8":
        sparse_type.index_type = EmbeddingDataType.kElemInt8
    elif index_type == "int16":
        sparse_type.index_type = EmbeddingDataType.kElemInt16
    elif index_type == "int32" or index_type == "int":
        sparse_type.index_type = EmbeddingDataType.kElemInt32
    elif index_type == "int64":
        sparse_type.index_type = EmbeddingDataType.kElemInt64
    else:
        raise InfinityException(ErrorCode.INVALID_EMBEDDING_DATA_TYPE, f"Unknown index type: {index_type}")
    sparse_type.dimension = int(length)

    column_type.sparse_type = sparse_type
    proto_column_def.column_type = column_type

    proto_column_def.constant_expr = get_constant_expr(column_info)

    column_defs.append(proto_column_def)

class LocalDatabase(Database, ABC):
    def __init__(self, conn, name: str):
        self._conn = conn
        self._db_name = name

    def create_table(self, table_name: str, columns_definition,
                     conflict_type: ConflictType = ConflictType.Error):
        # process column definitions
        """
        db_obj.create_table("my_table",
            {
                "c1": {
                    "type": "int",
                    "constraints":["primary key", ...],
                    "default"(optional): 1/"asdf"/[1,2]/...
                },
                "c2": {
                    "type":"vector,1024,float32",
                }
            }, None)
        """
        # to column_defs
        column_defs = []
        for index, (column_name, column_info) in enumerate(columns_definition.items()):
            check_valid_name(column_name, "Column")
            # column_big_info = [item.strip() for item in column_info["type"].split(",")]
            # column_big_info_first_str = column_big_info[0].lower()
            # if column_big_info_first_str == "vector" or column_big_info_first_str == "tensor" or column_big_info_first_str == "tensorarray":
            #     get_embedding_info(column_info, column_defs, column_name, index)
            # elif column_big_info_first_str == "sparse":
            #     get_sparse_info(column_info, column_defs, column_name, index)
            # else:  # numeric or varchar
            get_ordinary_info(column_info, column_defs, column_name, index)

        create_table_conflict: LocalConflictType
        if conflict_type == ConflictType.Error:
            create_table_conflict = LocalConflictType.kError
        elif conflict_type == ConflictType.Ignore:
            create_table_conflict = LocalConflictType.kIgnore
        elif conflict_type == ConflictType.Replace:
            create_table_conflict = LocalConflictType.kReplace
        else:
            raise InfinityException(ErrorCode.INVALID_CONFLICT_TYPE, f"Invalid conflict type")
        res = self._conn.create_table(db_name=self._db_name, table_name=table_name,
                                      column_defs=column_defs,
                                      conflict_type=create_table_conflict)

        if res.error_code == ErrorCode.OK:
            return LocalTable(self._conn, self._db_name, table_name)
        else:
            raise InfinityException(res.error_code, res.error_msg)

    @name_validity_check("table_name", "Table")
    def drop_table(self, table_name, conflict_type: ConflictType = ConflictType.Error):
        drop_table_conflict: LocalConflictType
        if conflict_type == ConflictType.Error:
            drop_table_conflict = LocalConflictType.kError
        elif conflict_type == ConflictType.Ignore:
            drop_table_conflict = LocalConflictType.kIgnore
        else:
            raise InfinityException(ErrorCode.INVALID_CONFLICT_TYPE, "nvalid conflict type")
        res = self._conn.drop_table(db_name=self._db_name, table_name=table_name, conflict_type=drop_table_conflict)
        if res.error_code == ErrorCode.OK or res.error_code == ErrorCode.TABLE_NOT_EXIST:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def list_tables(self):
        res = self._conn.list_tables(self._db_name)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    @name_validity_check("table_name", "Table")
    def show_table(self, table_name):
        res = self._conn.show_table(db_name=self._db_name, table_name=table_name)
        if res.error_code == ErrorCode.OK:
            return res
        else:
            raise InfinityException(res.error_code, res.error_msg)

    @name_validity_check("table_name", "Table")
    def show_columns(self, table_name):
        res = self._conn.show_columns(db_name=self._db_name, table_name=table_name)
        if res.error_code == ErrorCode.OK:
            return select_res_to_polars(res)
        else:
            raise InfinityException(res.error_code, res.error_msg)

    @name_validity_check("table_name", "Table")
    def get_table(self, table_name):
        res = self._conn.get_table(db_name=self._db_name, table_name=table_name)
        if res.error_code == ErrorCode.OK:
            return LocalTable(self._conn, self._db_name, table_name)
        else:
            raise InfinityException(res.error_code, res.error_msg)

    def show_tables(self):
        res = self._conn.show_tables(self._db_name)
        if res.error_code == ErrorCode.OK:
            return select_res_to_polars(res)
        else:
            raise InfinityException(res.error_code, res.error_msg)
