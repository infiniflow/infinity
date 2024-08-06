# Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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


class CaseLabel:
    """
    Testcase Levels
    CI Regression:
        L0:
            part of CI Regression
            triggered by GitHub commit
            optional used for dev to verify his fix before submitting a PR(like smoke)
            ~100 testcases and run in 3 mins
        L1:
            part of CI Regression
            triggered by GitHub commit
            must pass before merge
            run in 15 mins
    Benchmark:
        L2:
            E2E tests and bug-fix verification
            Nightly run triggered by cron job
            run in 60 mins
        L3:
            Stability/Performance/reliability, etc. special tests
            Triggered by cron job or manually
            run duration depends on test configuration
        Loadbalance:
            loadbalance testcases which need to be run in multi query nodes
        ClusterOnly:
            For functions only suitable to cluster mode
        GPU:
            For GPU supported cases
    """
    L0 = "L0"
    L1 = "L1"
    L2 = "L2"
    L3 = "L3"
    RBAC = "RBAC"
    Loadbalance = "Loadbalance"  # loadbalance testcases which need to be run in multi query nodes
    ClusterOnly = "ClusterOnly"  # For functions only suitable to cluster mode
    MultiQueryNodes = "MultiQueryNodes"  # for 8 query nodes configs tests, such as resource group
    GPU = "GPU"
