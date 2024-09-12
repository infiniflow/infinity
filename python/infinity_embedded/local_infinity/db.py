from abc import ABC

from infinity_embedded.db import Database
from infinity_embedded.errors import ErrorCode
from infinity_embedded.local_infinity.table import LocalTable
from infinity_embedded.local_infinity.utils import check_valid_name, name_validity_check
from infinity_embedded.common import ConflictType, InfinityException
from infinity_embedded.local_infinity.utils import select_res_to_polars, get_local_constant_expr_from_python_value
from infinity_embedded.embedded_infinity_ext import ConflictType as LocalConflictType
from infinity_embedded.embedded_infinity_ext import WrapColumnDef, WrapDataType, LogicalType, ConstraintType, LiteralType, WrapConstantExpr, \
    EmbeddingDataType, WrapEmbeddingType, WrapSparseType, WrapIndexInfo

def get_constant_expr(column_info):
    # process constant expression
    default = None
    if "default" in column_info:
        default = column_info["default"]

    if default is None:
        constant_expression = WrapConstantExpr()
        constant_expression.literal_type = LiteralType.kNull
        return constant_expression
    else:
        return get_local_constant_expr_from_python_value(default)

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
                match column_big_info_first_str:
                    case "vector" | "multivector" | "tensor" | "tensorarray":
                        return get_embedding_info(column_info, column_defs, column_name, index)
                    case "sparse":
                        return get_sparse_info(column_info, column_defs, column_name, index)
                proto_column_type = WrapDataType()
                match datatype:
                    case "int8":
                        proto_column_type.logical_type = LogicalType.kTinyInt
                    case "int16":
                        proto_column_type.logical_type = LogicalType.kSmallInt
                    case "int32":
                        proto_column_type.logical_type = LogicalType.kInteger
                    case "int":
                        proto_column_type.logical_type = LogicalType.kInteger
                    case "integer":
                        proto_column_type.logical_type = LogicalType.kInteger
                    case "int64":
                        proto_column_type.logical_type = LogicalType.kBigInt
                    case "int128":
                        proto_column_type.logical_type = LogicalType.kHugeInt
                    case "float":
                        proto_column_type.logical_type = LogicalType.kFloat
                    case "float32":
                        proto_column_type.logical_type = LogicalType.kFloat
                    case "double":
                        proto_column_type.logical_type = LogicalType.kDouble
                    case "float64":
                        proto_column_type.logical_type = LogicalType.kDouble
                    case "float16":
                        proto_column_type.logical_type = LogicalType.kFloat16
                    case "bfloat16":
                        proto_column_type.logical_type = LogicalType.kBFloat16
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


def get_embedding_element_type(element_type):
    match element_type:
        case "bit":
            return EmbeddingDataType.kElemBit
        case "float32" | "float" | "f32":
            return EmbeddingDataType.kElemFloat
        case "float64" | "double" | "f64":
            return EmbeddingDataType.kElemDouble
        case "float16" | "f16":
            return EmbeddingDataType.kElemFloat16
        case "bfloat16" | "bf16":
            return EmbeddingDataType.kElemBFloat16
        case "uint8" | "u8":
            return EmbeddingDataType.kElemUInt8
        case "int8" | "i8":
            return EmbeddingDataType.kElemInt8
        case "int16" | "i16":
            return EmbeddingDataType.kElemInt16
        case "int32" | "int" | "i32":
            return EmbeddingDataType.kElemInt32
        case "int64" | "i64":
            return EmbeddingDataType.kElemInt64
        case _:
            raise InfinityException(ErrorCode.INVALID_EMBEDDING_DATA_TYPE, f"Unknown element type: {element_type}")


def get_embedding_info(column_info, column_defs, column_name, index):
    # "vector,1024,float32"
    column_big_info = [item.strip() for item in column_info["type"].split(",")]
    length = column_big_info[1]
    element_type = column_big_info[2]

    proto_column_def = WrapColumnDef()
    proto_column_def.id = index
    proto_column_def.column_name = column_name
    column_type = WrapDataType()

    match column_big_info[0]:
        case "vector":
            column_type.logical_type = LogicalType.kEmbedding
        case "multivector":
            column_type.logical_type = LogicalType.kMultiVector
        case "tensor":
            column_type.logical_type = LogicalType.kTensor
        case "tensorarray":
            column_type.logical_type = LogicalType.kTensorArray
        case _:
            raise InfinityException(ErrorCode.INVALID_DATA_TYPE, f"Unknown data type: {column_big_info[0]}")

    embedding_type = WrapEmbeddingType()
    embedding_type.element_type = get_embedding_element_type(element_type)
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
    sparse_type.element_type = get_embedding_element_type(element_type)
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
