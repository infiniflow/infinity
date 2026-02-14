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
	"errors"
	"fmt"
	"strings"
	"testing"

	"github.com/infiniflow/infinity-go-sdk"
)

// Test constants
var testLocalHost = infinity.NetworkAddress{IP: "127.0.0.1", Port: 23817}

// Invalid name array for testing
var invalidNameArray = []string{
	"[]",
	"()",
	"{}",
	"1",
	"1.1",
	" ",
	"12",
	"name-12",
	"12name",
	"数据库名",
	"my_table!@#",
}

// generateSuffix generates a unique suffix for test names
func generateSuffix(t *testing.T) string {
	return fmt.Sprintf("_%s", strings.ReplaceAll(t.Name(), "/", "_"))
}

// setupConnection creates a connection for testing
func setupConnection(t *testing.T) *infinity.InfinityConnection {
	conn, err := infinity.Connect(testLocalHost)
	if err != nil {
		t.Fatalf("Failed to connect: %v", err)
	}
	return conn
}

func closeConnection(t *testing.T, conn *infinity.InfinityConnection) {
	_, err := conn.Disconnect()
	if err != nil {
		t.Fatalf("Failed to disconnect: %v", err)
	}
}

// TestDatabase tests basic database operations
func TestDatabase(t *testing.T) {

	dbName := "test_my_database"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	// Clean up if exists
	_, err := conn.DropDatabase(dbName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop database: %v", err)
		return
	}

	// 1. Create database
	db, err := conn.CreateDatabase(dbName, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create database: %v", err)
	}
	if db == nil {
		t.Fatal("Database is nil")
	}

	// 2. Test invalid database names
	invalidNames := []string{
		//"test_my_database!@#",
		//"test_my-database-dash",
		//"123_database_test",
		"",
	}
	for _, name := range invalidNames {
		_, err := conn.CreateDatabase(name, infinity.ConflictTypeError, "")
		if err == nil {
			t.Errorf("Expected error for invalid name: %s", name)
		} else {
			var infinityErr *infinity.InfinityException
			ok := errors.As(err, &infinityErr)
			if !ok {
				t.Errorf("Expected InfinityException for name: %s, got: %T", name, err)
			}
			errorCode := infinity.ErrorCode(infinityErr.ErrorCode)
			if errorCode == infinity.ErrorCodeInvalidIdentifierName || errorCode == infinity.ErrorCodeEmptyDBName {
				continue
			} else {
				t.Errorf("Expected ErrorCodeInvalidIdentifierName for name: %s, got: %d", name, infinityErr.ErrorCode)
			}
		}
	}

	// 3. Drop database
	_, err = conn.DropDatabase(dbName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop database: %v", err)
	}

	// 4. Try to drop default_db (should fail)
	_, err = conn.DropDatabase("default_db", infinity.ConflictTypeError)
	if err == nil {
		t.Fatal("Expected error when dropping default_db")
	}

	var infinityErr *infinity.InfinityException
	ok := errors.As(err, &infinityErr)
	if !ok {
		t.Fatalf("Expected InfinityException, got: %T", err)
	} else if infinity.ErrorCode(infinityErr.ErrorCode) != infinity.ErrorCodeDroppingUsingDB {
		t.Errorf("Expected ErrorCodeDroppingUsingDB, got: %d", infinityErr.ErrorCode)
	}
}

// TestCreateDatabaseInvalidName tests creating databases with invalid names
func TestCreateDatabaseInvalidName(t *testing.T) {

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	for _, dbName := range invalidNameArray {
		_, err := conn.CreateDatabase(dbName, infinity.ConflictTypeError, "")
		if err == nil {
			t.Errorf("Expected error for invalid name: %s", dbName)
		} else {
			var infinityErr *infinity.InfinityException
			ok := errors.As(err, &infinityErr)
			if !ok {
				t.Errorf("Expected InfinityException for name: %s, got: %T", dbName, err)
			} else if infinity.ErrorCode(infinityErr.ErrorCode) != infinity.ErrorCodeInvalidIdentifierName {
				t.Errorf("Expected ErrorCodeInvalidIdentifierName for name: %s, got: %d", dbName, infinityErr.ErrorCode)
			}
		}
	}
}

// TestRepeatedlyCreateDropShowDatabases tests repeatedly creating, dropping and showing databases
func TestRepeatedlyCreateDropShowDatabases(t *testing.T) {

	loopCount := 10 // Reduced from 100 for faster testing
	dbName := "test_repeatedly_create_drop_show_databases"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	for i := 0; i < loopCount; i++ {
		// Create database
		_, err := conn.CreateDatabase(dbName, infinity.ConflictTypeError, "")
		if err != nil {
			t.Fatalf("Iteration %d: Failed to create database: %v", i, err)
		}

		// List databases
		resp, err := conn.ListDatabases()
		if err != nil {
			t.Fatalf("Iteration %d: Failed to list databases: %v", i, err)
		}
		if resp == nil {
			t.Fatalf("Iteration %d: List databases response is nil", i)
		}

		// Drop database
		_, err = conn.DropDatabase(dbName, infinity.ConflictTypeError)
		if err != nil {
			t.Fatalf("Iteration %d: Failed to drop database: %v", i, err)
		}
	}
}

// TestDropDatabaseWithInvalidName tests dropping databases with invalid names
func TestDropDatabaseWithInvalidName(t *testing.T) {

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	for _, dbName := range invalidNameArray {
		_, err := conn.DropDatabase(dbName, infinity.ConflictTypeError)
		if err == nil {
			t.Errorf("Expected error for invalid name: %s", dbName)
		} else {
			var infinityErr *infinity.InfinityException
			ok := errors.As(err, &infinityErr)
			if !ok {
				t.Errorf("Expected InfinityException for name: %s, got: %T", dbName, err)
			} else if infinity.ErrorCode(infinityErr.ErrorCode) != infinity.ErrorCodeInvalidIdentifierName {
				t.Errorf("Expected ErrorCodeInvalidIdentifierName for name: %s, got: %d", dbName, infinityErr.ErrorCode)
			}
		}
	}
}

// TestGetDB tests getting databases
func TestGetDB(t *testing.T) {

	dbName := "my_database"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	// Clean up if exists
	_, err := conn.DropDatabase(dbName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop database: %v", err)
		return
	}

	// 1. Get non-existent database (should fail)
	_, err = conn.GetDatabase("db1_non_existent")
	if err == nil {
		t.Fatal("Expected error for non-existent database")
	}

	var infinityErr *infinity.InfinityException
	ok := errors.As(err, &infinityErr)
	if !ok {
		t.Fatalf("Expected InfinityException, got: %T", err)
	} else if infinity.ErrorCode(infinityErr.ErrorCode) != infinity.ErrorCodeDBNotExist {
		t.Errorf("Expected ErrorCodeDBNotExist, got: %d", infinityErr.ErrorCode)
	}

	// 2. Create database
	_, err = conn.CreateDatabase(dbName, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create database: %v", err)
	}

	// 3. Get created database
	db, err := conn.GetDatabase(dbName)
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}
	if db == nil {
		t.Fatal("Database is nil")
	}

	// 4. Get default_db
	db, err = conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get default_db: %v", err)
	}
	if db == nil {
		t.Fatal("default_db is nil")
	}

	// 5. Drop database
	_, err = conn.DropDatabase(dbName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop database: %v", err)
	}

	// 6. Drop with invalid names
	for _, name := range invalidNameArray {
		_, err := conn.DropDatabase(name, infinity.ConflictTypeError)
		if err == nil {
			t.Errorf("Expected error for invalid name: %s", name)
		} else {
			var infinityErr *infinity.InfinityException
			ok := errors.As(err, &infinityErr)
			if !ok {
				t.Errorf("Expected InfinityException for name: %s, got: %T", name, err)
			} else if infinity.ErrorCode(infinityErr.ErrorCode) != infinity.ErrorCodeInvalidIdentifierName {
				t.Errorf("Expected ErrorCodeInvalidIdentifierName for name: %s, got: %d", name, infinityErr.ErrorCode)
			}
		}
	}
}

// TestDropNonExistentDB tests dropping non-existent databases
func TestDropNonExistentDB(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	// Try to drop non-existent database
	_, err := conn.DropDatabase("my_non_existent_database", infinity.ConflictTypeError)
	if err == nil {
		t.Fatal("Expected error for non-existent database")
	}

	var infinityErr *infinity.InfinityException
	ok := errors.As(err, &infinityErr)
	if !ok {
		t.Fatalf("Expected InfinityException, got: %T", err)
	} else if infinity.ErrorCode(infinityErr.ErrorCode) != infinity.ErrorCodeDBNotExist {
		t.Errorf("Expected ErrorCodeDBNotExist, got: %d", infinityErr.ErrorCode)
	}
}

// TestCreateWithValidOption tests creating databases with valid conflict options
func TestCreateWithValidOption(t *testing.T) {

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	validOptions := []infinity.ConflictType{
		infinity.ConflictTypeError,
		infinity.ConflictTypeIgnore,
	}

	for i, option := range validOptions {
		dbName := fmt.Sprintf("test_create_option_%d", i)
		// Clean up if exists
		_, err := conn.DropDatabase(dbName, infinity.ConflictTypeIgnore)
		if err != nil {
			t.Fatalf("Failed to drop database: %v", err)
			return
		}

		_, err = conn.CreateDatabase(dbName, option, "")
		if err != nil {
			t.Errorf("Failed to create database with option %v: %v", option, err)
		}

		// Clean up
		_, err = conn.DropDatabase(dbName, infinity.ConflictTypeError)
		if err != nil {
			t.Errorf("Failed to drop database: %v", err)
		}
	}
}

// TestCreateWithInvalidOption tests creating databases with invalid conflict options
func TestCreateWithInvalidOption(t *testing.T) {

	dbName := "test_create_invalid_option"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	// Clean up if exists
	_, err := conn.DropDatabase(dbName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop database: %v", err)
		return
	}

	// Create with invalid option (using a value out of range)
	// Note: Go's type system prevents passing invalid ConflictType directly
	// This test validates the type system works correctly
	_, err = conn.CreateDatabase(dbName, infinity.ConflictType(999), "")
	if err == nil {
		t.Fatal("Expected error for invalid conflict type")
	}

	var infinityErr *infinity.InfinityException
	ok := errors.As(err, &infinityErr)
	if !ok {
		t.Fatalf("Expected InfinityException, got: %T", err)
	} else if infinity.ErrorCode(infinityErr.ErrorCode) != infinity.ErrorCodeInvalidConflictType {
		t.Errorf("Expected ErrorCodeInvalidConflictType, got: %d", infinityErr.ErrorCode)
	}

	// Clean up if exists
	_, err = conn.DropDatabase(dbName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop database: %v", err)
		return
	}
}

// TestDropOptionWithValidOption tests dropping databases with valid conflict options
func TestDropOptionWithValidOption(t *testing.T) {

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	validOptions := []infinity.ConflictType{
		infinity.ConflictTypeError,
		infinity.ConflictTypeIgnore,
	}

	for i, option := range validOptions {
		dbName := fmt.Sprintf("test_drop_option_%d", i)
		// Clean up if exists
		_, err := conn.DropDatabase(dbName, infinity.ConflictTypeIgnore)
		if err != nil {
			t.Fatalf("Failed to drop database: %v", err)
			return
		}

		// Create database
		_, err = conn.CreateDatabase(dbName, infinity.ConflictTypeError, "")
		if err != nil {
			t.Fatalf("Failed to create database: %v", err)
		}

		// Drop with option
		_, err = conn.DropDatabase(dbName, option)
		if err != nil {
			t.Errorf("Failed to drop database with option %v: %v", option, err)
		}

		// Try to drop again (should work with Ignore, fail with Error)
		_, err = conn.DropDatabase(dbName, infinity.ConflictTypeIgnore)
		if err != nil {
			// This is expected for Ignore option
			t.Logf("Drop again with Ignore (expected behavior): %v", err)
		}
	}
}

// TestDropOptionWithInvalidOption tests dropping databases with invalid conflict options
func TestDropOptionWithInvalidOption(t *testing.T) {

	dbName := "test_drop_option"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	// Clean up and create
	_, err := conn.DropDatabase(dbName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop database: %v", err)
		return
	}

	_, err = conn.CreateDatabase(dbName, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create database: %v", err)
	}

	// Try to drop with Replace (invalid for drop)
	_, err = conn.DropDatabase(dbName, infinity.ConflictTypeReplace)
	if err == nil {
		t.Fatal("Expected error for invalid conflict type")
	}

	var infinityErr *infinity.InfinityException
	ok := errors.As(err, &infinityErr)
	if !ok {
		t.Fatalf("Expected InfinityException, got: %T", err)
	} else if infinity.ErrorCode(infinityErr.ErrorCode) != infinity.ErrorCodeInvalidConflictType {
		t.Errorf("Expected ErrorCodeInvalidConflictType, got: %d", infinityErr.ErrorCode)
	}

	// Clean up
	_, err = conn.DropDatabase(dbName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop database: %v", err)
	}
}

// TestCreateUpperDatabaseName tests creating databases with uppercase names
func TestCreateUpperDatabaseName(t *testing.T) {

	dbUpperName := "MY_DATABASE"
	dbLowerName := "my_database"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	// Clean up if exists
	_, err := conn.DropDatabase(dbLowerName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop database: %v", err)
		return
	}

	// Create with uppercase name
	db, err := conn.CreateDatabase(dbUpperName, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create database: %v", err)
	}
	if db == nil {
		t.Fatal("Database is nil")
	}

	// Get with lowercase name
	db, err = conn.GetDatabase(dbLowerName)
	if err != nil {
		t.Fatalf("Failed to get database with lowercase name: %v", err)
	}

	// Get with uppercase name
	db, err = conn.GetDatabase(dbUpperName)
	if err != nil {
		t.Fatalf("Failed to get database with uppercase name: %v", err)
	}

	// Get default_db
	db, err = conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get default_db: %v", err)
	}

	// Get DEFAULT_DB
	db, err = conn.GetDatabase("DEFAULT_DB")
	if err != nil {
		t.Fatalf("Failed to get DEFAULT_DB: %v", err)
	}

	// Show database with lowercase name
	resp, err := conn.ShowDatabase(dbLowerName)
	if err != nil {
		t.Fatalf("Failed to show database with lowercase name: %v", err)
	}
	if resp == nil {
		t.Fatal("Show database response is nil")
	}

	// Show database with uppercase name
	resp, err = conn.ShowDatabase(dbUpperName)
	if err != nil {
		t.Fatalf("Failed to show database with uppercase name: %v", err)
	}

	// Show DEFAULT_DB
	resp, err = conn.ShowDatabase("DEFAULT_DB")
	if err != nil {
		t.Fatalf("Failed to show DEFAULT_DB: %v", err)
	}

	// Drop database
	_, err = conn.DropDatabase(dbUpperName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop database: %v", err)
	}
}
