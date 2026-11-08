def test_helper_float32_to_chars(x: float) -> str:
    """Render a value the way the engine's `std::to_chars` does.

    The engine prints a vector element with `std::to_chars` (shortest
    round-tripping digits, general format), so `1.0f` comes out as `1` and
    `0.0f` as `0`. Python's `str()` keeps the `.0`; leaving it in makes a
    fixture disagree with the engine for a value that merely rounds to
    `1.0f` - a real possibility for 128k `np.random.random()` elements, and
    it fails the reference engine the same way.
    """
    text = str(x)
    if text.endswith(".0"):
        return text[:-2]
    return text


def format_float1(f: float) -> str:
    return test_helper_float32_to_chars(f)


def sparse_format_float(f: float) -> str:
    return format(f, "0.6f")

# print(format_float(0.12345678))
# print(format_float(0.12300000))
# print(format_float(0.00123456))
