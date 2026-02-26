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

import (
	"testing"

	thriftapi "github.com/infiniflow/infinity-go-sdk/internal/thrift"
)

// TestGetInfinity tests getting a database object
func TestGetInfinity(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	// Get default database
	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}
	if db == nil {
		t.Fatal("Database is nil")
	}

	// Verify database name? Database struct does not expose dbName field.
	// We can at least ensure it's not nil.
	// Disconnect (handled by defer)
}

// TestListInfinity tests listing databases
func TestListInfinity(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	// List databases
	resp, err := conn.ListDatabases()
	if err != nil {
		t.Fatalf("Failed to list databases: %v", err)
	}
	if resp == nil {
		t.Fatal("ListDatabases response is nil")
	}

	// Type assert to *thriftapi.ListDatabaseResponse
	listResp, ok := resp.(*thriftapi.ListDatabaseResponse)
	if !ok {
		t.Fatal("Failed to type assert ListDatabases response")
	}

	// Check that default_db is in the list
	found := false
	for _, dbName := range listResp.DbNames {
		if dbName == "default_db" {
			found = true
			break
		}
	}
	if !found {
		t.Errorf("default_db not found in database list: %v", listResp.DbNames)
	}
}
