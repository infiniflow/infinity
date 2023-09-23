# big slt test

## prerequisite

1. Install [rust](https://www.rust-lang.org/tools/install).
2. Install [sqllogictest cli tool](git@github.com:risinglightdb/sqllogictest-rs.git). Run `cargo install sqllogicaltest-bin` under source code so that `sqllogictest` can be used.
3. run `python3 test_script.py`. It will generate big test file under tmp directory, then run `sqllogictest` to check. The output is in log file in tmp.
