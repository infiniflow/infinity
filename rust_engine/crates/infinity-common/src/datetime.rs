//! Date/time kernels ported from the C++ parser types.
//!
//! C++: `src/parser/type/datetime/{date_type,interval_type}.cpp`.

use crate::types::{DateT, IntervalT, TimeUnit};
use crate::Error;

const DAYS_PER_YEAR_LEAP: i32 = 146097;
const EPOCH_YEAR: i32 = 1970;
const DAY_HOUR: i32 = 24;
const DAY_MINUTE: i32 = 24 * 60;
const DAY_SECOND: i32 = 24 * 60 * 60;

fn is_leap_year(year: i32) -> bool {
    (year % 4 == 0 && year % 100 != 0) || year % 400 == 0
}

fn days_in_month(year: i32, month: i32) -> i32 {
    match month {
        1 | 3 | 5 | 7 | 8 | 10 | 12 => 31,
        4 | 6 | 9 | 11 => 30,
        2 if is_leap_year(year) => 29,
        2 => 28,
        _ => 0,
    }
}

fn is_date_valid(year: i32, month: i32, day: i32) -> bool {
    (1..=12).contains(&month) && day >= 1 && day <= days_in_month(year, month)
}

/// Proleptic-Gregorian `(y, m, d)` -> days since 1970-01-01.
pub fn ymd_to_date(year: i32, month: i32, day: i32) -> Option<DateT> {
    if !is_date_valid(year, month, day) {
        return None;
    }
    let y = year as i64 - i64::from(month <= 2);
    let era = if y >= 0 { y } else { y - 399 } / 400;
    let yoe = y - era * 400;
    let mp = i64::from((month + 9) % 12);
    let doy = (153 * mp + 2) / 5 + i64::from(day) - 1;
    let doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    let days = era * 146097 + doe - 719_468;
    Some(DateT::new(days as i32))
}

/// Days since 1970-01-01 -> proleptic-Gregorian `(y, m, d)`.
pub fn date_to_ymd(date: DateT) -> (i32, i32, i32) {
    let z = i64::from(date.value) + 719_468;
    let era = if z >= 0 { z } else { z - 146_096 } / 146_097;
    let doe = z - era * 146_097;
    let yoe = (doe - doe / 1460 + doe / 36_524 - doe / 146_096) / 365;
    let y = yoe + era * 400;
    let doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    let mp = (5 * doy + 2) / 153;
    let d = doy - (153 * mp + 2) / 5 + 1;
    let m = if mp < 10 { mp + 3 } else { mp - 9 };
    ((y + i64::from(m <= 2)) as i32, m as i32, d as i32)
}

impl DateT {
    /// C++ `DateType::ConvertFromString` + `FromString`: `YYYY-MM-DD` or
    /// `YYYY/MM/DD`, with an optional leading `-` for negative years.
    pub fn from_string(&mut self, text: &str) -> crate::Result<()> {
        let bytes = text.as_bytes();
        let mut pos = 0usize;
        while pos < bytes.len() && bytes[pos].is_ascii_whitespace() {
            pos += 1;
        }

        let negative_year = bytes.get(pos) == Some(&b'-');
        if negative_year {
            pos += 1;
        }

        let mut year: i32 = 0;
        let start = pos;
        while pos < bytes.len() && bytes[pos].is_ascii_digit() {
            year = year * 10 + i32::from(bytes[pos] - b'0');
            pos += 1;
            if year > 9999 {
                return Err(invalid_date());
            }
        }
        if pos == start {
            return Err(invalid_date());
        }
        if negative_year {
            year = -year;
        }
        if !matches!(bytes.get(pos), Some(b'-') | Some(b'/')) {
            return Err(invalid_date());
        }
        pos += 1;

        let mut month: i32 = 0;
        let start = pos;
        while pos < bytes.len() && bytes[pos].is_ascii_digit() {
            month = month * 10 + i32::from(bytes[pos] - b'0');
            pos += 1;
            if month > 12 {
                return Err(invalid_date());
            }
        }
        if pos == start {
            return Err(invalid_date());
        }
        if !matches!(bytes.get(pos), Some(b'-') | Some(b'/')) {
            return Err(invalid_date());
        }
        pos += 1;

        let mut day: i32 = 0;
        let start = pos;
        while pos < bytes.len() && bytes[pos].is_ascii_digit() {
            day = day * 10 + i32::from(bytes[pos] - b'0');
            pos += 1;
        }
        if pos == start {
            return Err(invalid_date());
        }
        // The C++ parser stops at the first non-digit and ignores the rest.
        let _ = pos;

        match ymd_to_date(year, month, day) {
            Some(date) => {
                *self = date;
                Ok(())
            }
            None => Err(invalid_date()),
        }
    }

    /// C++ `DateType::ToString`.
    pub fn to_string_value(self) -> String {
        let (year, month, day) = date_to_ymd(self);
        format!("{year:04}-{month:02}-{day:02}")
    }

    /// C++ `DateType::Add`.
    #[allow(clippy::should_implement_trait)]
    pub fn add(self, interval: IntervalT) -> crate::Result<DateT> {
        let (year, month, day) = date_to_ymd(self);
        match interval.unit {
            TimeUnit::Year => {
                ymd_to_date(year + interval.value, month, day).ok_or_else(invalid_date)
            }
            TimeUnit::Month => {
                let mut year = year;
                let mut month = month + interval.value;
                if month > 12 {
                    year += (month - 1) / 12;
                    month = 1 + (month - 1) % 12;
                } else if month < 1 {
                    year += (month - 12) / 12;
                    month = 12 + (month - 12) % 12;
                }
                let day = day.min(days_in_month(year, month));
                ymd_to_date(year, month, day).ok_or_else(invalid_date)
            }
            TimeUnit::Day => Ok(DateT::new(self.value + interval.value)),
            TimeUnit::Hour => Ok(DateT::new(self.value + interval.value / DAY_HOUR)),
            TimeUnit::Minute => Ok(DateT::new(self.value + interval.value / DAY_MINUTE)),
            TimeUnit::Second => Ok(DateT::new(self.value + interval.value / DAY_SECOND)),
            TimeUnit::Invalid => Err(Error::Invalid("parser: Invalid interval unit.".into())),
        }
    }

    /// C++ `DateType::Subtract`.
    pub fn subtract(self, interval: IntervalT) -> crate::Result<DateT> {
        self.add(IntervalT::with_unit(-interval.value, interval.unit))
    }

    /// C++ `DateType::GetDatePart`.
    pub fn get_date_part(self, unit: TimeUnit) -> crate::Result<i64> {
        let (year, month, day) = date_to_ymd(self);
        match unit {
            TimeUnit::Year => Ok(i64::from(year)),
            TimeUnit::Month => Ok(i64::from(month)),
            TimeUnit::Day => Ok(i64::from(day)),
            TimeUnit::Hour => Err(Error::Invalid(
                "parser: Can't extract hour from date".into(),
            )),
            TimeUnit::Minute => Err(Error::Invalid(
                "parser: Can't extract minute from date".into(),
            )),
            TimeUnit::Second => Err(Error::Invalid(
                "parser: Can't extract second from date".into(),
            )),
            TimeUnit::Invalid => Err(Error::Invalid("parser: Invalid time unit".into())),
        }
    }
}

fn invalid_date() -> Error {
    Error::Invalid("parser: Invalid date format (YYYY-MM-DD or YYYY/MM/DD).".into())
}

// Silence the unused-constant warning while keeping the C++ constants visible.
const _: i32 = DAYS_PER_YEAR_LEAP + EPOCH_YEAR;

#[cfg(test)]
mod tests {
    use super::*;

    // C++: src/unit_test/parser/type/datetime/date_type_ut.cpp:TestFromString
    #[test]
    fn test_from_string() {
        let mut date = DateT::default();
        for good in [
            "2020-01-30",
            "4535-02-11",
            "2002-05-26",
            "-1587-8-1",
            "-4532/1/23",
            "2024/9/4",
            "2024-9/4",
            "2024/9-4",
        ] {
            assert!(date.from_string(good).is_ok(), "{good}");
        }
        for bad in [
            "2018/2/29",
            "20-1233/45",
            "-12354--56f1--ade",
            "1234@56.789",
            "qwlssmabz",
        ] {
            assert!(date.from_string(bad).is_err(), "{bad}");
        }
    }

    // C++: src/unit_test/parser/type/datetime/date_type_ut.cpp:TestAddSubstract
    #[test]
    fn test_add_subtract() {
        let mut date = DateT::default();
        date.from_string("2020-01-30").unwrap();
        assert_eq!(date.to_string_value(), "2020-01-30");

        let month = IntervalT::with_unit(1, TimeUnit::Month);
        assert_eq!(date.add(month).unwrap().to_string_value(), "2020-02-29");
        // Adding twice to the same input stays idempotent for month clamping.
        assert_eq!(date.add(month).unwrap().to_string_value(), "2020-02-29");

        let year = IntervalT::with_unit(1, TimeUnit::Year);
        assert_eq!(date.add(year).unwrap().to_string_value(), "2021-01-30");

        let months = IntervalT::with_unit(25, TimeUnit::Month);
        assert_eq!(date.add(months).unwrap().to_string_value(), "2022-02-28");

        let back = IntervalT::with_unit(23, TimeUnit::Month);
        assert_eq!(date.subtract(back).unwrap().to_string_value(), "2018-02-28");
    }

    // C++: src/unit_test/parser/type/datetime/date_type_ut.cpp:TestNegativeYears
    #[test]
    fn test_negative_years() {
        let mut date = DateT::default();
        date.from_string("-1/5/4").unwrap();
        assert_eq!(date.to_string_value(), "-001-05-04");
        assert_eq!(date.get_date_part(TimeUnit::Year).unwrap(), -1);
        assert_eq!(date.get_date_part(TimeUnit::Month).unwrap(), 5);
        assert_eq!(date.get_date_part(TimeUnit::Day).unwrap(), 4);

        let year = IntervalT::with_unit(3, TimeUnit::Year);
        let shifted = date.add(year).unwrap();
        assert_eq!(shifted.to_string_value(), "0002-05-04");
        assert_eq!(
            shifted.subtract(year).unwrap().to_string_value(),
            "-001-05-04"
        );
    }
}
