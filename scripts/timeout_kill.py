from subprocess import Popen
import sys
import time
import os
import signal


def timeout_kill(duration: int, pids: list[int]):
    # Send SIGTERM to all specified processes
    for pid in pids:
        try:
            os.kill(pid, signal.SIGTERM)
        except ProcessLookupError:
            pass

    start_time = time.time()
    end_time = start_time + duration

    while True:
        all_dead = True
        for pid in pids:
            try:
                os.kill(pid, 0)
            except ProcessLookupError:
                print(f"Process {pid} has terminated.")
                continue
            else:
                print(f"Process {pid} is still running.")
                all_dead = False
                break

        if all_dead:
            print("All processes have terminated successfully.")
            sys.exit(0)

        current_time = time.time()
        if current_time >= end_time:
            print("Some processes did not terminate in time. Sending SIGKILL.")
            for pid in pids:
                try:
                    os.kill(pid, signal.SIGKILL)
                except ProcessLookupError:
                    pass
            sys.exit(2)

        time.sleep(1)


def timeout_kill(timeout: int, process: Popen[bytes]):
    process.terminate()
    if process.wait(timeout) is None:
        process.kill()
        process.wait()
        print(f"process {process.pid} timeout, killed.")
        exit(1)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: {} <duration_in_seconds> [pids]".format(sys.argv[0]))
        sys.exit(1)
    duration = int(sys.argv[1])
    pids = [int(pid) for pid in sys.argv[2:]]
    timeout_kill(duration, pids)
