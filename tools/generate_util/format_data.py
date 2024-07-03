def format_float1(f: float) -> str:
    return format(f, "0.7g")

def format_float(f: float) -> str:
    return format(f, "0.7f")

def sparse_format_float(f: float) -> str:
    return format(f, "0.6f")

# print(format_float(0.12345678))
# print(format_float(0.12300000))
# print(format_float(0.00123456))
