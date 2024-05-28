# Ported from pybind11/tests/test_chrono.py

import test_chrono_ext as m

import time
import datetime
import sys

import pytest


def test_chrono_system_clock():
    # Get the time from both c++ and datetime
    date0 = datetime.datetime.today()
    date1 = m.test_chrono1()
    date2 = datetime.datetime.today()

    # The returned value should be a datetime
    assert isinstance(date1, datetime.datetime)

    # The numbers should vary by a very small amount (time it took to execute)
    diff_python = abs(date2 - date0)
    diff = abs(date1 - date2)

    # There should never be a days difference
    assert diff.days == 0

    # Since datetime.datetime.today() calls time.time(), and on some platforms
    # that has 1 second accuracy, we compare this way
    assert diff.seconds <= diff_python.seconds


def test_chrono_system_clock_roundtrip():
    date1 = datetime.datetime.today()

    # Roundtrip the time
    date2 = m.test_chrono2(date1)

    # The returned value should be a datetime
    assert isinstance(date2, datetime.datetime)

    # They should be identical (no information lost on roundtrip)
    diff = abs(date1 - date2)
    assert diff == datetime.timedelta(0)


def test_chrono_system_clock_roundtrip_date():
    date1 = datetime.date.today()

    # Roundtrip the time
    datetime2 = m.test_chrono2(date1)
    date2 = datetime2.date()
    time2 = datetime2.time()

    # The returned value should be a datetime
    assert isinstance(datetime2, datetime.datetime)
    assert isinstance(date2, datetime.date)
    assert isinstance(time2, datetime.time)

    # They should be identical (no information lost on roundtrip)
    diff = abs(date1 - date2)
    assert diff.days == 0
    assert diff.seconds == 0
    assert diff.microseconds == 0

    # Year, Month & Day should be the same after the round trip
    assert date1 == date2

    # There should be no time information
    assert time2.hour == 0
    assert time2.minute == 0
    assert time2.second == 0
    assert time2.microsecond == 0


SKIP_TZ_ENV_ON_WIN = pytest.mark.skipif(
    "sys.platform == 'win32'",
    reason="TZ environment variable only supported on POSIX"
)


@pytest.mark.parametrize(
    "time1",
    [
        datetime.datetime.today().time(),
        datetime.time(0, 0, 0),
        datetime.time(0, 0, 0, 1),
        datetime.time(0, 28, 45, 109827),
        datetime.time(0, 59, 59, 999999),
        datetime.time(1, 0, 0),
        datetime.time(5, 59, 59, 0),
        datetime.time(5, 59, 59, 1),
    ],
)
@pytest.mark.parametrize(
    "tz",
    [
        None,
        pytest.param("Europe/Brussels", marks=SKIP_TZ_ENV_ON_WIN),
        pytest.param("Asia/Pyongyang", marks=SKIP_TZ_ENV_ON_WIN),
        pytest.param("America/New_York", marks=SKIP_TZ_ENV_ON_WIN),
    ],
)
def test_chrono_system_clock_roundtrip_time(time1, tz, monkeypatch):
    if tz is not None:
        monkeypatch.setenv("TZ", f"/usr/share/zoneinfo/{tz}")

    # Roundtrip the time
    datetime2 = m.test_chrono2(time1)
    date2 = datetime2.date()
    time2 = datetime2.time()

    # The returned value should be a datetime
    assert isinstance(datetime2, datetime.datetime)
    assert isinstance(date2, datetime.date)
    assert isinstance(time2, datetime.time)

    # Hour, Minute, Second & Microsecond should be the same after the round trip
    assert time1 == time2

    # There should be no date information (i.e. date = python base date)
    assert date2.year == 1970
    assert date2.month == 1
    assert date2.day == 1


def test_chrono_duration_roundtrip():
    # Get the difference between two times (a timedelta)
    date1 = datetime.datetime.today()
    date2 = datetime.datetime.today()
    diff = date2 - date1

    # Make sure this is a timedelta
    assert isinstance(diff, datetime.timedelta)

    cpp_diff = m.test_chrono3(diff)

    assert cpp_diff == diff

    # Negative timedelta roundtrip
    diff = datetime.timedelta(microseconds=-1)
    cpp_diff = m.test_chrono3(diff)

    assert cpp_diff == diff


def test_chrono_duration_subtraction_equivalence():
    date1 = datetime.datetime.today()
    date2 = datetime.datetime.today()

    diff = date2 - date1
    cpp_diff = m.test_chrono4(date2, date1)

    assert cpp_diff == diff


def test_chrono_duration_subtraction_equivalence_date():
    date1 = datetime.date.today()
    date2 = datetime.date.today()

    diff = date2 - date1
    cpp_diff = m.test_chrono4(date2, date1)

    assert cpp_diff == diff


def test_chrono_steady_clock():
    time1 = m.test_chrono5()
    assert isinstance(time1, datetime.timedelta)


def test_chrono_steady_clock_roundtrip():
    time1 = datetime.timedelta(days=10, seconds=10, microseconds=100)
    time2 = m.test_chrono6(time1)

    assert isinstance(time2, datetime.timedelta)

    # They should be identical (no information lost on roundtrip)
    assert time1 == time2

    # Floating point conversion also works
    assert m.test_chrono6(time1.total_seconds()) == time1


def test_floating_point_duration():
    # Test using a floating point number in seconds
    time = m.test_chrono7(35.525123)

    assert isinstance(time, datetime.timedelta)

    assert time.seconds == 35
    assert 525122 <= time.microseconds <= 525123

    diff = m.test_chrono_float_diff(43.789012, 1.123456)
    assert diff.seconds == 42
    assert 665556 <= diff.microseconds <= 665557


def test_nano_timepoint():
    time = datetime.datetime.now()
    time1 = m.test_nano_timepoint(time, datetime.timedelta(seconds=60))
    assert time1 == time + datetime.timedelta(seconds=60)


def test_chrono_different_resolutions():
    resolutions = m.different_resolutions()
    time = datetime.datetime.now()
    resolutions.timestamp_h = time
    resolutions.timestamp_m = time
    resolutions.timestamp_s = time
    resolutions.timestamp_ms = time
    resolutions.timestamp_us = time

    assert time == resolutions.timestamp_us
    time = time.replace(microsecond=(time.microsecond // 1000) * 1000)
    assert time == resolutions.timestamp_ms
    time = time.replace(microsecond=0)
    assert time == resolutions.timestamp_s
    time = time.replace(second=0)
    assert time == resolutions.timestamp_m
    time = time.replace(minute=0)
    assert time == resolutions.timestamp_h


# Tests below this point are new in nanobind


def test_chrono_misc():
    from datetime import datetime, timedelta

    advance_datetime = m.test_nano_timepoint
    difference_between_datetimes = m.test_nano_timepoint_diff
    roundtrip_datetime = m.test_nano_timepoint_roundtrip

    d1 = datetime(2023, 4, 5, 12, 0, 0, 0)
    d2 = datetime(2023, 4, 5, 12, 30, 0, 123)
    # datetime -> time_point and duration -> timedelta conversion
    assert difference_between_datetimes(d1, d2) == d1 - d2
    assert difference_between_datetimes(d2, d1) == d2 - d1
    # date -> time_point conversion
    assert difference_between_datetimes(d2, d1.date()) == timedelta(
        hours=12, minutes=30, microseconds=123
    )
    # time -> time_point conversion
    assert difference_between_datetimes(d2.time(), d1.time()) == timedelta(
        minutes=30, microseconds=123
    )
    assert roundtrip_datetime(d1.time()) == datetime(1970, 1, 1, 12, 0, 0)
    for td in (
        timedelta(seconds=5),
        timedelta(microseconds=123),
        timedelta(days=1, seconds=10),
        timedelta(seconds=-5),
        timedelta(microseconds=-123),
        timedelta(days=-1, seconds=-10),
    ):
        # timedelta -> duration conversion
        assert advance_datetime(d1, td) == d1 + td
        # float -> duration conversion
        assert advance_datetime(d1, td.total_seconds()) == d1 + td
    # time_point -> datetime conversion
    assert roundtrip_datetime(d1) == d1
    assert roundtrip_datetime(d2) == d2


@pytest.mark.parametrize(
    "test_type,roundtrip_name",
    [
        (datetime.timedelta, 'test_chrono7'),
        (datetime.datetime, 'test_nano_timepoint_roundtrip'),
    ]
)
def test_chrono_invalid(test_type, roundtrip_name):
    roundtrip = getattr(m, roundtrip_name)

    # Can't pass None or an integer where a duration or timepoint is expected
    with pytest.raises(TypeError, match="incompatible function arguments"):
        roundtrip(None)
    with pytest.raises(TypeError, match="incompatible function arguments"):
        roundtrip(42)

    # Can't pass a duration where a timepoint is expected, or vice versa
    with pytest.raises(TypeError, match="incompatible function arguments"):
        if test_type is datetime.datetime:
            roundtrip(datetime.timedelta(seconds=5))
        else:
            roundtrip(datetime.datetime.now())

    # On the limited API we access timedelta/datetime objects via
    # regular attribute access; test that invalid results are handled
    # reasonably. On the full API we use Python's <datetime.h> header
    # so we'll always access the true C-level datetime slot and can't
    # be fooled by tricks like this. PyPy uses normal attribute access
    # and works like the limited API in this respect.

    class fake_type(test_type):
        @property
        def seconds(self):
            return self.override_value

        @property
        def second(self):
            return self.override_value

    if test_type is datetime.datetime:
        fake_val = fake_type.fromtimestamp(time.time())
        replace_overridden = lambda s: fake_val.replace(second=s)
    else:
        fake_val = fake_type(days=1, seconds=10, microseconds=123456)
        replace_overridden = lambda s: fake_type(
            days=1, seconds=s, microseconds=123456
        )

    for fake_result, errtype in (
        ("hi", "TypeError"),
        (0, None),
        (2**64, "Python int too large to convert to C long"),
        (2**32, "OverflowError"),
    ):
        fake_val.override_value = fake_result
        if not m.access_via_python:
            assert roundtrip(fake_val) == fake_val
        elif errtype is None:
            assert roundtrip(fake_val) == replace_overridden(fake_result)
        elif test_type is datetime.timedelta and sys.implementation.name == "pypy":
            # pypy's cpyext module converts timedelta to a C structure
            # before the nanobind function even gets called, producing
            # a different exception than the one we're testing below.
            # datetime still works as it doesn't have its attributes
            # converted but instead is implemented with Python
            # attribute accesses.
            pass
        else:
            from _pytest.unraisableexception import catch_unraisable_exception

            with catch_unraisable_exception() as cm:
                with pytest.raises(TypeError, match="incompatible function arguments"):
                    roundtrip(fake_val)
                assert cm.unraisable is not None
                assert errtype in repr(cm.unraisable.exc_value)
