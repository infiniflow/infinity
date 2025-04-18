import os
import threading


class RtnThread(threading.Thread):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._rtn = None
        self.exception_ = None

    def run(self):
        if self._target is None:
            raise RuntimeError("Should use python3")
        try:
            self._rtn = self._target(*self._args, **self._kwargs)
        except Exception as e:
            e.thread_id = self.ident
            self.exception_ = e

    def join(self, *args):
        super().join(*args)
        if self.exception_:
            raise self.exception_
        return self._rtn

# open toml file and 
# add "use_new_catalog = true" under [storage]
def make_new_config(filepath: str):
    file_dir, filename = filepath.rsplit("/", 1)
    new_filename = filename.split(".")[0] + "_new.toml"
    new_filepath = file_dir + "/" + new_filename

    with open(filepath, "r") as f:
        lines = f.readlines()
    with open(new_filepath, "w") as f:
        for line in lines:
            if line.startswith("[storage]"):
                f.write(line)
                f.write("use_new_catalog = true\n")
            else:
                f.write(line)
    return new_filepath

# remove the new config file
def remove_new_config(filepath: str):
    file_dir, filename = filepath.rsplit("/", 1)
    new_filename = filename.split(".")[0] + "_new.toml"
    new_filepath = file_dir + "/" + new_filename
    try:
        os.remove(new_filepath)
    except Exception as e:
        print(f"remove {new_filepath} failed: {e}")
    return new_filepath