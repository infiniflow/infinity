# Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Helpers that build filter expressions."""

from __future__ import annotations

__all__ = ["quote_string_literal", "regex_filter"]


def quote_string_literal(value: str) -> str:
    """Quote a value as a string literal for a filter expression.

    A backslash is passed through unchanged, which is what a regular expression
    needs, so only the quote itself has to be doubled.
    """
    if not isinstance(value, str):
        raise TypeError(f"Expected a string, but got {type(value).__name__}")
    return "'" + value.replace("'", "''") + "'"


def regex_filter(column: str, pattern: str) -> str:
    """Build a ``regex(column, pattern)`` filter.

    The pattern is evaluated by RE2 on the rows the filter is applied to. When
    ``column`` has a full-text index built with a sparse gram analyzer
    (``sparsegram-3-12``, optionally ``-fold``), the server additionally
    narrows those rows with the literals the pattern proves mandatory before
    the regular expression runs, so the regular expression only sees
    candidates. The narrowing never removes a row the pattern would have
    matched, and a column without such an index keeps a plain scan.

    Example::

        table.filter(regex_filter("doc", r"colou?r of the (sky|sea)"))
    """
    if not isinstance(column, str) or not column:
        raise ValueError("column must be a non-empty string")
    return f"regex({column}, {quote_string_literal(pattern)})"
