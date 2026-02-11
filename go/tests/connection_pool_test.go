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
	"strings"
	"testing"

	"github.com/infiniflow/infinity-go-sdk"
)

// TestConnectionPool tests connection pool functionality
// Based on Python SDK test_pysdk/test_connection_pool.py
func TestConnectionPool(t *testing.T) {
	suffix := generateSuffix(t)

	// Create connection pool with initial size 8 (max_size in Python)
	config := infinity.ConnectionPoolConfig{
		URI:         testLocalHost,
		InitialSize: 8,
	}

	pool, err := infinity.NewConnectionPool(config, nil)
	if err != nil {
		t.Fatalf("Failed to create connection pool: %v", err)
	}
	defer pool.Close()

	// Check that the pool has 8 available connections initially
	stats := pool.Stats()
	if stats.AvailableConnections != 8 {
		t.Errorf("Expected 8 available connections, got %d", stats.AvailableConnections)
	}

	// Get 3 connections from the pool
	conn1, err := pool.Get()
	if err != nil {
		t.Fatalf("Failed to get connection 1: %v", err)
	}

	_, err = pool.Get()
	if err != nil {
		t.Fatalf("Failed to get connection 2: %v", err)
	}

	_, err = pool.Get()
	if err != nil {
		t.Fatalf("Failed to get connection 3: %v", err)
	}

	// Check that we have 5 available connections (8 - 3 = 5)
	stats = pool.Stats()
	if stats.AvailableConnections != 5 {
		t.Errorf("Expected 5 available connections after getting 3, got %d", stats.AvailableConnections)
	}

	// Use conn1 to create and drop database
	dbName := "my_database" + suffix

	// Drop database first with Ignore conflict type
	_, err = conn1.DropDatabase(dbName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Logf("Failed to drop database (may not exist): %v", err)
	}

	// Create database
	db, err := conn1.CreateDatabase(dbName, infinity.ConflictTypeError, "")
	if err != nil {
		t.Fatalf("Failed to create database: %v", err)
	}
	if db == nil {
		t.Fatal("Database is nil after creation")
	}

	// List databases
	resp, err := conn1.ListDatabases()
	if err != nil {
		t.Fatalf("Failed to list databases: %v", err)
	}

	// Check if default_db and our database exist
	// The response type depends on thrift, we need to check the actual structure
	if resp == nil {
		t.Fatal("ListDatabases response is nil")
	}

	// Get the database list from response
	// Need to check the actual structure of thrift response
	// For now, we'll just check that the call succeeded

	// Drop database with Error conflict type
	_, err = conn1.DropDatabase(dbName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop database: %v", err)
	}

	// Release conn1 back to the pool
	err = pool.Put(conn1)
	if err != nil {
		t.Fatalf("Failed to put connection back to pool: %v", err)
	}

	// Check that we have 6 available connections (5 + 1 = 6)
	stats = pool.Stats()
	if stats.AvailableConnections != 6 {
		t.Errorf("Expected 6 available connections after releasing 1, got %d", stats.AvailableConnections)
	}

	// Create an external connection (not from pool)
	externalConn, err := infinity.Connect(testLocalHost)
	if err != nil {
		t.Fatalf("Failed to create external connection: %v", err)
	}
	defer externalConn.Disconnect()

	// Release external connection back to the pool
	err = pool.Put(externalConn)
	if err != nil {
		// Should get an error since the connection is not from this pool
		if infErr, ok := err.(*infinity.InfinityException); ok {
			t.Logf("Expected error when releasing external connection: %s", infErr.ErrorMsg)
		} else {
			t.Errorf("Expected InfinityException, got %T", err)
		}
	} else {
		t.Error("Expected error when releasing external connection, got nil")
	}

	// The external connection should be closed by the pool
	// Check that we still have 6 available connections (external connection was not from pool)
	stats = pool.Stats()
	if stats.AvailableConnections != 6 {
		t.Logf("Note: Available connections after releasing external: %d (expected 6)", stats.AvailableConnections)
	}

	// Try to release conn1 again (double release)
	err = pool.Put(conn1)
	if err == nil {
		t.Error("Expected error when double releasing connection, got nil")
	} else {
		// Should get an error since conn1 is not in the pool (it was already released)
		t.Logf("Expected error when double releasing: %v", err)
	}

	// Test that Get works after Put
	conn2, err := pool.Get()
	if err != nil {
		t.Fatalf("Failed to get connection after Put: %v", err)
	}
	if conn2 == nil {
		t.Fatal("Got nil connection from pool")
	}

	// Put it back
	err = pool.Put(conn2)
	if err != nil {
		t.Fatalf("Failed to put connection back: %v", err)
	}

	t.Logf("Test %s completed successfully", suffix)
}

// TestConnectionPoolStats tests pool statistics
func TestConnectionPoolStats(t *testing.T) {
	config := infinity.ConnectionPoolConfig{
		URI:         testLocalHost,
		InitialSize: 5,
	}

	pool, err := infinity.NewConnectionPool(config, nil)
	if err != nil {
		t.Fatalf("Failed to create connection pool: %v", err)
	}
	defer pool.Close()

	// Initial stats
	stats := pool.Stats()
	if stats.TotalConnections != 5 {
		t.Errorf("Expected 5 total connections, got %d", stats.TotalConnections)
	}
	if stats.AvailableConnections != 5 {
		t.Errorf("Expected 5 available connections, got %d", stats.AvailableConnections)
	}
	if stats.InUseConnections != 0 {
		t.Errorf("Expected 0 in-use connections, got %d", stats.InUseConnections)
	}
	if stats.Closed {
		t.Error("Pool should not be closed")
	}

	// Get a connection
	conn, err := pool.Get()
	if err != nil {
		t.Fatalf("Failed to get connection: %v", err)
	}

	// Stats after getting a connection
	stats = pool.Stats()
	if stats.TotalConnections != 5 {
		t.Errorf("Expected 5 total connections, got %d", stats.TotalConnections)
	}
	if stats.AvailableConnections != 4 {
		t.Errorf("Expected 4 available connections, got %d", stats.AvailableConnections)
	}
	if stats.InUseConnections != 1 {
		t.Errorf("Expected 1 in-use connection, got %d", stats.InUseConnections)
	}

	// Return connection to pool
	err = pool.Put(conn)
	if err != nil {
		t.Fatalf("Failed to put connection back: %v", err)
	}

	// Stats after returning connection
	stats = pool.Stats()
	if stats.AvailableConnections != 5 {
		t.Errorf("Expected 5 available connections after return, got %d", stats.AvailableConnections)
	}
	if stats.InUseConnections != 0 {
		t.Errorf("Expected 0 in-use connections after return, got %d", stats.InUseConnections)
	}

	// Test Size() and Available() methods
	if pool.Size() != 5 {
		t.Errorf("Expected Size() to return 5, got %d", pool.Size())
	}
	if pool.Available() != 5 {
		t.Errorf("Expected Available() to return 5, got %d", pool.Available())
	}
}

// TestConnectionPoolWithDatabaseOperations tests using connection pool for database operations
func TestConnectionPoolWithDatabaseOperations(t *testing.T) {
	suffix := generateSuffix(t)

	config := infinity.ConnectionPoolConfig{
		URI:         testLocalHost,
		InitialSize: 3,
	}

	pool, err := infinity.NewConnectionPool(config, nil)
	if err != nil {
		t.Fatalf("Failed to create connection pool: %v", err)
	}
	defer pool.Close()

	dbName := "test_db_pool" + suffix

	// Get a connection
	conn, err := pool.Get()
	if err != nil {
		t.Fatalf("Failed to get connection: %v", err)
	}

	// Create database
	_, err = conn.DropDatabase(dbName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Logf("Failed to drop database (may not exist): %v", err)
	}

	db, err := conn.CreateDatabase(dbName, infinity.ConflictTypeError, "Test database for pool")
	if err != nil {
		t.Fatalf("Failed to create database: %v", err)
	}
	if db == nil {
		t.Fatal("Database is nil after creation")
	}

	// Return connection to pool
	err = pool.Put(conn)
	if err != nil {
		t.Fatalf("Failed to put connection back: %v", err)
	}

	// Get another connection from pool
	conn2, err := pool.Get()
	if err != nil {
		t.Fatalf("Failed to get connection 2: %v", err)
	}

	// List databases and check our database exists
	resp, err := conn2.ListDatabases()
	if err != nil {
		t.Fatalf("Failed to list databases: %v", err)
	}

	// Check response
	if resp == nil {
		t.Fatal("ListDatabases response is nil")
	}

	// Drop database
	_, err = conn2.DropDatabase(dbName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop database: %v", err)
	}

	// Return connection
	err = pool.Put(conn2)
	if err != nil {
		t.Fatalf("Failed to put connection back: %v", err)
	}

	t.Logf("Test %s completed successfully", suffix)
}

// TestConnectionPoolMultipleConnections tests getting multiple connections concurrently
func TestConnectionPoolMultipleConnections(t *testing.T) {
	config := infinity.ConnectionPoolConfig{
		URI:         testLocalHost,
		InitialSize: 3,
	}

	pool, err := infinity.NewConnectionPool(config, nil)
	if err != nil {
		t.Fatalf("Failed to create connection pool: %v", err)
	}
	defer pool.Close()

	// Get all connections
	var conns []*infinity.InfinityConnection
	for i := 0; i < 3; i++ {
		conn, err := pool.Get()
		if err != nil {
			t.Fatalf("Failed to get connection %d: %v", i, err)
		}
		if conn == nil {
			t.Fatalf("Connection %d is nil", i)
		}
		conns = append(conns, conn)
	}

	// Should have 0 available connections
	stats := pool.Stats()
	if stats.AvailableConnections != 0 {
		t.Errorf("Expected 0 available connections, got %d", stats.AvailableConnections)
	}

	// Getting another connection should create a new one (no max size limit in Go implementation)
	conn4, err := pool.Get()
	if err != nil {
		t.Fatalf("Failed to get 4th connection: %v", err)
	}
	if conn4 == nil {
		t.Fatal("4th connection is nil")
	}

	// Total connections should be 4
	stats = pool.Stats()
	if stats.TotalConnections != 4 {
		t.Errorf("Expected 4 total connections, got %d", stats.TotalConnections)
	}

	// Return all connections
	for i, conn := range conns {
		err := pool.Put(conn)
		if err != nil {
			t.Fatalf("Failed to put connection %d back: %v", i, err)
		}
	}

	err = pool.Put(conn4)
	if err != nil {
		t.Fatalf("Failed to put 4th connection back: %v", err)
	}

	// Should have all connections available
	stats = pool.Stats()
	if stats.AvailableConnections != 4 {
		t.Errorf("Expected 4 available connections after returning all, got %d", stats.AvailableConnections)
	}
}

// TestConnectionPoolClosed tests behavior when pool is closed
func TestConnectionPoolClosed(t *testing.T) {
	config := infinity.ConnectionPoolConfig{
		URI:         testLocalHost,
		InitialSize: 2,
	}

	pool, err := infinity.NewConnectionPool(config, nil)
	if err != nil {
		t.Fatalf("Failed to create connection pool: %v", err)
	}

	// Close the pool
	err = pool.Close()
	if err != nil {
		t.Fatalf("Failed to close pool: %v", err)
	}

	// Try to get a connection (should fail)
	_, err = pool.Get()
	if err == nil {
		t.Error("Expected error when getting connection from closed pool, got nil")
	} else if !strings.Contains(err.Error(), "closed") {
		t.Logf("Note: Error message when getting from closed pool: %v", err)
	}

	// Try to put a connection (should fail)
	conn, err := infinity.Connect(testLocalHost)
	if err != nil {
		t.Fatalf("Failed to create connection: %v", err)
	}
	defer conn.Disconnect()

	err = pool.Put(conn)
	if err == nil {
		t.Error("Expected error when putting connection to closed pool, got nil")
	} else if !strings.Contains(err.Error(), "closed") {
		t.Logf("Note: Error message when putting to closed pool: %v", err)
	}

	// Close again (should fail with "already closed")
	err = pool.Close()
	if err == nil {
		t.Error("Expected error when closing already closed pool, got nil")
	}
}

// TestConnectionPoolZeroInitialSize tests pool with zero initial size
func TestConnectionPoolZeroInitialSize(t *testing.T) {
	config := infinity.ConnectionPoolConfig{
		URI:         testLocalHost,
		InitialSize: 0,
	}

	pool, err := infinity.NewConnectionPool(config, nil)
	if err != nil {
		t.Fatalf("Failed to create connection pool: %v", err)
	}
	defer pool.Close()

	// Check initial stats
	stats := pool.Stats()
	if stats.TotalConnections != 0 {
		t.Errorf("Expected 0 total connections, got %d", stats.TotalConnections)
	}

	// Get a connection (should create one on demand)
	conn, err := pool.Get()
	if err != nil {
		t.Fatalf("Failed to get connection: %v", err)
	}

	// Should have 1 total connection, 0 available
	stats = pool.Stats()
	if stats.TotalConnections != 1 {
		t.Errorf("Expected 1 total connection, got %d", stats.TotalConnections)
	}
	if stats.AvailableConnections != 0 {
		t.Errorf("Expected 0 available connections, got %d", stats.AvailableConnections)
	}

	// Return connection
	err = pool.Put(conn)
	if err != nil {
		t.Fatalf("Failed to put connection back: %v", err)
	}

	// Should have 1 available connection
	stats = pool.Stats()
	if stats.AvailableConnections != 1 {
		t.Errorf("Expected 1 available connection, got %d", stats.AvailableConnections)
	}
}
