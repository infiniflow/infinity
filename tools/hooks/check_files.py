#!/usr/bin/env python3
"""The text-level checks behind the lefthook jobs (see lefthook.yml).

Modelled on RAGFlow's tools/hooks/check_files.py: one process per check, the
files to inspect come from the command line (lefthook passes the staged files)
or, when none are given, from "git diff --cached".

    check_files.py eof [--fix] [FILE ...]
    check_files.py trailing-whitespace [--fix] [FILE ...]
    check_files.py mixed-line-ending [--fix] [FILE ...]
    check_files.py merge-conflict [FILE ...]
    check_files.py yaml|json|case-conflict [FILE ...]

--fix rewrites the file (lefthook then stages it); without it the check only
reports and exits non-zero.

In check mode the line-level checks look at the lines the change **adds**, not
the whole file: a repository that already carries trailing whitespace must not
make every commit that touches such a file fail. The base is CHECK_FILES_BASE
when set (CI passes the pull request's base commit) and HEAD otherwise (a local
--all-files run then checks the uncommitted changes). Binary files (a NUL byte)
and files larger than MAX_FIX_BYTES are skipped: a fixture must never be
normalised byte for byte.
"""

from __future__ import annotations

import json
import os
import re
import subprocess
import sys
from pathlib import Path

# Larger files are data (CSV fixtures, parquet dumps), not source: normalising
# their line endings rewrites the fixture instead of fixing the commit.
MAX_FIX_BYTES = 5 * 1024 * 1024

# Git writes these at the start of a line. Anchoring them keeps ordinary code
# that merely mentions the markers (this file does) from matching.
MERGE_START = "<<<<<<< "
MERGE_SEP = "======="
MERGE_END = ">>>>>>> "

_TRAILING_WS = re.compile(r"[ \t]+$")


def _git_paths(*args: str) -> list[Path]:
    proc = subprocess.run(["git", *args, "-z"], check=True, capture_output=True)
    return [Path(os.fsdecode(p)) for p in proc.stdout.split(b"\0") if p]


def _staged_paths() -> list[Path]:
    return _git_paths("diff", "--cached", "--name-only", "--diff-filter=ACMR")


def _read(path: Path) -> bytes | None:
    """The file's bytes, or None when a text check must leave it alone."""
    try:
        if not path.is_file() or path.stat().st_size > MAX_FIX_BYTES:
            return None
        data = path.read_bytes()
    except OSError:
        return None
    return None if b"\0" in data else data


def _added_lines(paths: list[Path]) -> dict[str, list[str]] | None:
    """The lines this change adds, per file, or None when there is no base.

    One "git diff" for the whole run: a pull request can touch hundreds of
    files, and a process per file dominates the check.
    """
    base = os.environ.get("CHECK_FILES_BASE", "HEAD")
    if not base:
        return None
    proc = subprocess.run(
        ["git", "diff", "-U0", "--no-color", base, "--", *[str(p) for p in paths]],
        check=False,
        capture_output=True,
        text=True,
    )
    if proc.returncode != 0:
        return None
    added: dict[str, list[str]] = {}
    current: str | None = None
    for line in proc.stdout.split("\n"):
        if line.startswith("+++ b/"):
            current = line[len("+++ b/") :]
            added.setdefault(current, [])
        elif line.startswith(("+++ ", "--- ")):
            current = None
        elif line.startswith("+") and current is not None:
            added[current].append(line[1:])
    return added


def _report(errors: list[str]) -> int:
    for error in errors:
        print(error, file=sys.stderr)
    return 1 if errors else 0


def _fragment(line: str) -> str:
    """A diff line without the CR a CRLF file carries."""
    return line.removesuffix("\r")


def _base_bytes(path: Path) -> bytes | None:
    """The base revision's copy of "path", or None for a new file."""
    base = os.environ.get("CHECK_FILES_BASE", "HEAD")
    if not base:
        return None
    proc = subprocess.run(
        ["git", "show", f"{base}:{path}"],
        check=False,
        capture_output=True,
    )
    return proc.stdout if proc.returncode == 0 else None


def check_eof(paths: list[Path], fix: bool, added: dict[str, list[str]] | None) -> int:
    errors: list[str] = []
    for path in paths:
        data = _read(path)
        if data is None or not data or data.endswith(b"\n"):
            continue
        if fix:
            with path.open("ab") as handle:
                handle.write(b"\n")
            continue
        if added is not None:
            if not added.get(str(path)):
                continue
            # A new file must be well formed, and a change must not drop the
            # newline a file already had. A file that never had one keeps its
            # pre-existing state, like trailing whitespace on untouched lines.
            base = _base_bytes(path)
            if base is not None and not base.endswith(b"\n"):
                continue
        errors.append(f"missing trailing newline: {path}")
    return _report(errors)


def check_trailing_whitespace(
    paths: list[Path], fix: bool, added: dict[str, list[str]] | None
) -> int:
    errors: list[str] = []
    for path in paths:
        data = _read(path)
        if data is None:
            continue
        try:
            text = data.decode("utf-8")
        except UnicodeDecodeError:
            continue
        if fix:
            fixed = "\n".join(_TRAILING_WS.sub("", line) for line in text.split("\n"))
            if fixed != text:
                path.write_text(fixed, encoding="utf-8")
            continue
        if added is not None:
            lines = added.get(str(path))
            if not lines:
                continue
            if any(_TRAILING_WS.search(_fragment(line)) for line in lines):
                errors.append(f"trailing whitespace: {path}")
            continue
        if any(_TRAILING_WS.search(line) for line in text.split("\n")):
            errors.append(f"trailing whitespace: {path}")
    return _report(errors)


def check_mixed_line_ending(
    paths: list[Path], fix: bool, added: dict[str, list[str]] | None
) -> int:
    errors: list[str] = []
    for path in paths:
        data = _read(path)
        if data is None:
            continue
        if fix:
            if b"\r\n" in data:
                path.write_bytes(data.replace(b"\r\n", b"\n"))
            continue
        if added is not None:
            lines = added.get(str(path))
            if not lines:
                continue
            if any(line.endswith("\r") for line in lines):
                errors.append(f"CRLF line ending: {path}")
            continue
        if b"\r\n" in data:
            errors.append(f"CRLF line ending: {path}")
    return _report(errors)


def check_merge_conflict(
    paths: list[Path], fix: bool, added: dict[str, list[str]] | None
) -> int:
    del fix, added
    errors: list[str] = []
    for path in paths:
        data = _read(path)
        if data is None:
            continue
        try:
            lines = data.decode("utf-8").splitlines()
        except UnicodeDecodeError:
            continue
        start = any(line.startswith(MERGE_START) for line in lines)
        sep = any(line.rstrip() == MERGE_SEP for line in lines)
        end = any(line.startswith(MERGE_END) for line in lines)
        if start and sep and end:
            errors.append(f"merge conflict marker: {path}")
    return _report(errors)


def check_yaml(paths: list[Path], fix: bool, added: dict[str, list[str]] | None) -> int:
    del fix, added
    try:
        import yaml
    except ImportError:
        print(
            "check-yaml: PyYAML is not installed (python3-yaml); skipped",
            file=sys.stderr,
        )
        return 0
    errors: list[str] = []
    for path in paths:
        if path.suffix not in {".yaml", ".yml"} or not path.is_file():
            continue
        try:
            yaml.safe_load(path.read_text(encoding="utf-8"))
        except (OSError, UnicodeDecodeError, yaml.YAMLError) as exc:
            errors.append(f"invalid yaml: {path}: {exc}")
    return _report(errors)


def check_json(paths: list[Path], fix: bool, added: dict[str, list[str]] | None) -> int:
    del fix, added
    errors: list[str] = []
    for path in paths:
        if path.suffix != ".json" or not path.is_file():
            continue
        try:
            json.loads(path.read_text(encoding="utf-8"))
        except (OSError, UnicodeDecodeError, json.JSONDecodeError) as exc:
            errors.append(f"invalid json: {path}: {exc}")
    return _report(errors)


def check_case_conflict(
    paths: list[Path], fix: bool, added: dict[str, list[str]] | None
) -> int:
    """Two paths that differ only in case cannot coexist on macOS/Windows."""
    del fix, added
    seen: dict[str, Path] = {}
    errors: list[str] = []
    for path in paths:
        key = str(path).lower()
        other = seen.get(key)
        if other is not None and other != path:
            errors.append(f"case conflict: {other} vs {path}")
        else:
            seen[key] = path
    return _report(errors)


CHECKS = {
    "eof": check_eof,
    "trailing-whitespace": check_trailing_whitespace,
    "mixed-line-ending": check_mixed_line_ending,
    "merge-conflict": check_merge_conflict,
    "yaml": check_yaml,
    "json": check_json,
    "case-conflict": check_case_conflict,
}


def main(argv: list[str]) -> int:
    if not argv or argv[0] not in CHECKS:
        checks = "|".join(CHECKS)
        print(
            f"usage: {Path(__file__).name} <{checks}> [--fix] [FILE ...]",
            file=sys.stderr,
        )
        return 2
    check = argv[0]
    fix = "--fix" in argv[1:]
    files = [Path(arg) for arg in argv[1:] if arg != "--fix"]
    files = files or _staged_paths()
    added = None if fix else _added_lines(files)
    return CHECKS[check](files, fix, added)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
