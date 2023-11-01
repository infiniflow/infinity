from .infinity import URI, InfinityConnection
from .remote.infinity import RemoteInfinityConnection


def connect(
        uri: URI,
) -> InfinityConnection:
    return RemoteInfinityConnection(uri)
