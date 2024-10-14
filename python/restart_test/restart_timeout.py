import timeout_decorator

# Description: use timeout_decorator as backup solution if a test not finish in time

ONE_TEST_TIMEOUT = 60 * 60  # 1 hour


class TimeoutException(Exception):
    def __init__(self, name):
        super().__init__(name)
        print(f"Timeout decorator for {name}")
        self.timeout_name = name

    def __str__(self):
        return f"Timeout: run {self.timeout_name} exceed {ONE_TEST_TIMEOUT} seconds"


# wrap timeout_decorator with time and exception
def my_timeout(timeout_name):
    def wrapper(func):
        def inner(*args, **kwargs):
            try:
                return timeout_decorator.timeout(ONE_TEST_TIMEOUT, use_signals=False)(
                    func
                )(*args, **kwargs)
            except timeout_decorator.timeout_decorator.TimeoutError:
                raise TimeoutException(timeout_name)

        return inner

    return wrapper
