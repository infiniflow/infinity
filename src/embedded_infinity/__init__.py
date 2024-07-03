try:
    from infinity.embedded_infinity_ext import *
except ImportError as e:
    raise ImportError(f"Could not import the embedded_infinity_ext module: {e}")

