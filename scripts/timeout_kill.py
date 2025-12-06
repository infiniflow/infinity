import logging
from subprocess import Popen, TimeoutExpired
import sys
import time
import os
import signal


def timeout_kill_pids(duration: int, pids: list[int]):
    # Send SIGTERM to all specified processes
    for pid in pids:
        try:
            os.kill(pid, signal.SIGTERM)
        except ProcessLookupError:
            pass

    start_time = time.time()
    end_time = start_time + duration

    alive_pids = set(pids)

    while True:
        # Check status of alive processes
        for pid in list(alive_pids):
            try:
                os.kill(pid, 0)
            except ProcessLookupError:
                print(f"Process {pid} has terminated.")
                alive_pids.remove(pid)

        if not alive_pids:
            print("All processes have terminated successfully.")
            return 0

        current_time = time.time()
        if current_time >= end_time:
            print("Some processes did not terminate in time. Sending SIGKILL.")
            for pid in alive_pids:
                try:
                    print(f"Force killing process {pid}")
                    os.kill(pid, signal.SIGKILL)
                except ProcessLookupError:
                    pass
            return 0

        print(f"Waiting for {len(alive_pids)} processes to exit...")
        time.sleep(1)


def timeout_kill(timeout: int, process: Popen[bytes], logger: logging.Logger = None):
    if logger is None:
        logger = logging.root
    process.terminate()
    try:
        ret = process.wait(timeout)
        logger.debug(f"process {process.pid} exited with code {ret}")
    except TimeoutExpired:
        logger.error(f"process {process.pid} timeout, killed.")
        ret = process.kill()
        raise


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <duration_in_seconds> [pids]".format(sys.argv[0]))
        sys.exit(1)
    duration = int(sys.argv[1])
    pids = [int(pid) for pid in sys.argv[2:]]
    sys.exit(timeout_kill_pids(duration, pids))
