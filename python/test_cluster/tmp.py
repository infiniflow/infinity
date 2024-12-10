import time

import pytest
from infinity_cluster import InfinityCluster, MinioParams
from numpy import dtype
import pandas as pd
import time
from infinity.errors import ErrorCode
from infinity.common import InfinityException
from infinity.common import ConflictType
from database_operations import do_some_operations, do_some_operations_cluster

if __name__ == "__main__" :
    minio = MinioParams(minio_dir, minio_port)
    cluster = InfinityCluster("cmake-build-debug/src/infinity");
