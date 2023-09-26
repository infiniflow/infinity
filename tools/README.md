# how to run slt test

## directory structure

* **tools/sqllogicaltest.py** is the main script to run slt test. It will call other python script to generate test data and call sqllogictest cli tool to run the test cases.  
* **test/data/sql** contains slt files. slt file records the test process used by sqllogictest cli tool.
* **test/data/csv** contains csv files. some slt files may use csv files to create table.

## prerequisite

1. Install [rust](https://www.rust-lang.org/tools/install).
2. Install [sqllogictest cli tool](git@github.com:risinglightdb/sqllogictest-rs.git). Run `cargo install sqllogicaltest-bin` under source code so that `sqllogictest` can be used.
3. run `where sqllogictest` to get your path of sqllogictest cli tool. Remember it.

## start the test

1. run `sudo rm -rf /tmp/infinity/*` to remove all previous data.
2. run `infinity_main`, so the database is ready for test.
3. run `python3 ./tools/sqllogictest.py --path=YOUR_CLI_TOOL_PATH` under **the root path of infinity project**.  
   This python script will first call `generate_big.py`, `generate_fvecs.py` and so on to generate big test file under coresponding directory, then call sqllogictest cli tool to run the test cases. The cli tools will compare the actual result of infinity_main and result in slt file. Difference will be marked **RED**. Only when all slt test is passed, the test is passed.
