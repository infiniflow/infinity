# big slt test

## prerequisite

1. Install [rust](https://www.rust-lang.org/tools/install).
2. Install [sqllogictest cli tool](git@github.com:risinglightdb/sqllogictest-rs.git). Run `cargo install sqllogicaltest-bin` under source code so that `sqllogictest` can be used.
3. run `python3 ./tools/generate_big.py` under root directory of the project. It will generate big test file under coresponding directory.
4. run `where sqllogictest` to get the path of the cli tool
5. run `python3 ./tools/sqllogictest.py --path=YOUR_CLI_TOOL_PATH`.
