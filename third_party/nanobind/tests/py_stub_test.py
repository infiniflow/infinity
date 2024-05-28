import sys

if sys.version_info < (3, 11, 0):
    # Work around limitations in typing.overload in Python<3.11
    import typing_extensions as typing
else:
    import typing

# Ignore a type and a function from elsewhere. These shouldn't be included in
# the stub by default
from os import PathLike, getcwd

del sys

C = 123
T = typing.TypeVar("T")

def f1(a, b, c, /):
    """docstring"""

def f2(a=(3,(4,5)), /, b=(1,2), *, c=4):
    """docstring 2"""

def f3(*args, **kwargs):
    pass

def f4() -> typing.Callable[[T], T]:
    return lambda x: x

class AClass:
    STATIC_VAR: int = 5

    def __init__(self, x):
        pass

    def method(self, x: str):
        pass

    @staticmethod
    def static_method(x):
        pass

    @classmethod
    def class_method(x):
        pass

    @typing.overload
    def overloaded(self, x: int) -> None:
        """docstr 1"""

    @typing.overload
    def overloaded(self, x: str) -> None:
        """docstr 2"""

    def overloaded(self, x):
        pass

    @typing.overload
    def overloaded_2(self, x: int) -> None: ...

    @typing.overload
    def overloaded_2(self, x: str) -> None: ...

    def overloaded_2(self, x):
        "docstr 3"

del typing
