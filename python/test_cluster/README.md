# Prerequisite

```bash
sudo apt install bridge-utils
```

## Run all cluster test cases
python3 tools/run_cluster_test.py --infinity_path=cmake-build-debug/src/infinity --infinity_dir=/var/infinity

## Run specific test case
python3 tools/run_cluster_test.py --test_case=test_basic.py::test_admin --infinity_path=cmake-build-debug/src/infinity --infinity_dir=/var/infinity