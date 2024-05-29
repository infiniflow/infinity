# from .embedded_infinity_ext import Infinity
# __init__.py

import os
import sys

# current_dir = os.path.dirname(os.path.abspath(__file__))
# build_dir = os.path.join(current_dir, '..', '..', 'build', 'cp311-cp311-linux_x86_64', 'src')
# sys.path.append(os.path.abspath(build_dir))
# print(os.path.abspath(build_dir))
sys.path.append(os.path.abspath("/home/weilongma/Project/infinity/build/cp311-cp311-linux_x86_64/src"))

try:
    from embedded_infinity_ext import Infinity
except ImportError as e:
    raise ImportError(f"Could not import the embedded_infinity_ext module: {e}")

