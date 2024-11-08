import threading


class RtnThread(threading.Thread):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._rtn = None

    def run(self):
        if self._target is None:
            raise RuntimeError("Should use python3")
        try:
            self._rtn = self._target(*self._args, **self._kwargs)
        except Exception as e:
            e.thread_id = self.ident

    def join(self, *args):
        super().join(*args)
        return self._rtn
