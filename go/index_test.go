// Copyright 2024 The InfiniFlow Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package infinity

import (
	"testing"

	thriftapi "github.com/infiniflow/infinity-go-sdk/internal/thrift"
)

// The SDK enum is 1-based and the thrift one is 0-based, so the two ordinals of a
// kind are deliberately different; only the *names* line up.
func TestIndexTypeCSPANNNamesMatchTheThriftEnum(t *testing.T) {
	if got := IndexTypeCSPANN.String(); got != "CSPANN" {
		t.Fatalf("IndexTypeCSPANN.String() = %q, want CSPANN", got)
	}
	if got := thriftapi.IndexType_CSPANN.String(); got != "CSPANN" {
		t.Fatalf("thrift IndexType_CSPANN.String() = %q, want CSPANN", got)
	}
	if int64(thriftapi.IndexType_CSPANN) != 10 {
		t.Fatalf("thrift CSPANN ordinal = %d, want 10 (appended to the IDL)", thriftapi.IndexType_CSPANN)
	}
	parsed, err := thriftapi.IndexTypeFromString("CSPANN")
	if err != nil {
		t.Fatalf("IndexTypeFromString(CSPANN): %v", err)
	}
	if parsed != thriftapi.IndexType_CSPANN {
		t.Fatalf("round trip = %v, want CSPANN", parsed)
	}
}

// Every known thrift value has a name: a new enum entry that is missing from the
// String() switch shows up here rather than as "<UNSET>" in a SHOW INDEX row.
func TestEveryThriftIndexTypeHasAName(t *testing.T) {
	for value := range thriftapi.IndexTypeValues() {
		if name := value.String(); name == "<UNSET>" {
			t.Fatalf("thrift IndexType %d has no name", int64(value))
		}
	}
}
