import os
import sys

sys.path.append(os.path.abspath("/home/weilongma/Project/infinity/build/cp311-cp311-linux_x86_64/src"))
# sys.path.append(os.path.abspath("/home/weilongma/Project/infinity/infinity_env/lib/python3.11/site-packages/infinity"))
print("sys path = ", sys.path)
try:
    # from embedded_infinity_ext import Infinity
    from embedded_infinity_ext import *
    # from embed_infinity import *

    # from infinity.embedded_infinity_ext import *
except ImportError as e:
    raise ImportError(f"Could not import the embedded_infinity_ext module: {e}")

