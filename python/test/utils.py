import functools
import traceback


def trace_expected_exceptions(func):
    @functools.wraps(func)
    def wrapped_func(*args, **kwargs):
        try:
            func(*args, **kwargs)
        except:
            print('Expected exception in ' + func.__name__)
            traceback.print_exc()

    return wrapped_func

