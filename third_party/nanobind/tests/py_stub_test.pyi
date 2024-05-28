from collections.abc import Callable
from typing import TypeVar, overload


class AClass:
    __annotations__: dict = {'STATIC_VAR' : int}

    STATIC_VAR: int = 5

    def __init__(self, x): ...

    def method(self, x: str): ...

    @staticmethod
    def static_method(x): ...

    @staticmethod
    def class_method(x): ...

    @overload
    def overloaded(self, x: int) -> None:
        """docstr 1"""

    @overload
    def overloaded(self, x: str) -> None:
        """docstr 2"""

    @overload
    def overloaded_2(self, x: int) -> None:
        """docstr 3"""

    @overload
    def overloaded_2(self, x: str) -> None: ...

C: int = 123

T = TypeVar("T")

def f1(a, b, c, /):
    """docstring"""

def f2(a=(3, (4, 5)), /, b=(1, 2), *, c=4):
    """docstring 2"""

def f3(*args, **kwargs): ...

def f4() -> Callable[[T], T]: ...
