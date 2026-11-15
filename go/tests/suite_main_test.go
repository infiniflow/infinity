// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package tests

// The suite entry point, in the shape the ragflow API tests use: run the
// pre-flight once, run the cases, report.
//
// The pre-flight is not decoration. Without it a stopped engine shows up as
// "connection refused" in several hundred tests, all at once, and the first
// line of the report is whichever case happened to run first. One statement
// here names the actual problem.
//
// `-short` skips it, which is how a case with no server in it (the wire
// format unit tests) is run on its own.

import (
	"flag"
	"fmt"
	"os"
	"testing"
)

func TestMain(m *testing.M) {
	// `testing.Short` reads the parsed flags, and the testing package only
	// parses them inside `m.Run` - so the pre-flight has to parse first.
	flag.Parse()

	if !testing.Short() {
		fmt.Fprintf(os.Stderr, "Connecting to %s:%d\n", TestConfig.URI.IP, TestConfig.URI.Port)
		if err := TestConfig.Ping(); err != nil {
			fmt.Fprintf(os.Stderr, "Engine is not answering: %v\n", err)
			os.Exit(1)
		}
	}

	code := m.Run()

	fmt.Fprintln(os.Stderr, "Disconnected")
	os.Exit(code)
}
