//! Config validation invariants (plan §6 Gate G / INVARIANT_MATRIX G1).
//!
//! Mirrors the C++ `Config` range checks (`config_ut.cpp`): a known config key
//! with a malformed or out-of-range value must be **rejected**, never silently
//! clamped.

use infinity_executor::Engine;

fn engine() -> (tempfile::TempDir, Engine) {
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    (dir, e)
}

#[test]
fn valid_values_are_accepted() {
    for sql in [
        "SET CONFIG result_cache \"on\";",
        "SET CONFIG result_cache \"suspend\";",
        "SET CONFIG result_cache \"off\";",
        "SET CONFIG cache_result_capacity 5000;",
        "SET CONFIG CHECKPOINT_INTERVAL 0;",
        "SET CONFIG CHECKPOINT_INTERVAL 30;",
    ] {
        let (_d, e) = engine();
        assert!(e.execute(sql).is_ok(), "should accept: {sql}");
    }
}

#[test]
fn malformed_enum_value_is_rejected() {
    let (_d, e) = engine();
    assert!(
        e.execute("SET CONFIG result_cache \"sometimes\";").is_err(),
        "an unknown result_cache mode must be rejected"
    );
}

#[test]
fn out_of_range_capacity_is_rejected() {
    let (_d, e) = engine();
    // 0 is not a valid capacity (a positive integer is required)
    assert!(e.execute("SET CONFIG cache_result_capacity 0;").is_err());
    // a non-numeric value is rejected too
    assert!(e
        .execute("SET CONFIG cache_result_capacity \"big\";")
        .is_err());
    // a rejected value is not stored (it must not be silently clamped)
    assert!(e.execute("SET CONFIG cache_result_capacity -1;").is_err());
}

#[test]
fn unknown_keys_are_tolerated() {
    let (_d, e) = engine();
    // keys the engine does not model keep the historical lenient behaviour
    assert!(e.execute("SET CONFIG some_future_knob 42;").is_ok());
}
