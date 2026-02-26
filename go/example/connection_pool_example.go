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

package main

import (
	"context"
	"fmt"
	"log"
	"sync"
	"time"

	"github.com/infiniflow/infinity-go-sdk"
)

func main() {
	fmt.Println("=== Infinity Go SDK Connection Pool Examples ===\n")

	// Example 1: Basic connection pool usage
	exampleBasicPoolUsage()

	// Example 2: Pool with custom configuration
	exampleCustomConfig()

	// Example 3: Concurrent access to pool
	exampleConcurrentAccess()

	// Example 4: Pool statistics
	examplePoolStats()

	// Example 5: Pool with context timeout
	exampleContextTimeout()

	// Example 6: Connection reuse
	exampleConnectionReuse()
}

// exampleBasicPoolUsage demonstrates basic pool operations
func exampleBasicPoolUsage() {
	fmt.Println("--- Example 1: Basic Pool Usage ---")

	// Create a pool with default configuration
	config := infinity.DefaultConnectionPoolConfig(infinity.LocalHost)
	pool, err := infinity.NewConnectionPool(config, nil)
	if err != nil {
		log.Printf("Failed to create pool: %v", err)
		return
	}
	defer pool.Close()

	fmt.Printf("Pool created with initial size: %d\n", config.InitialSize)

	// Get a connection from the pool
	conn, err := pool.Get()
	if err != nil {
		log.Printf("Failed to get connection: %v", err)
		return
	}

	fmt.Printf("Got connection, Session ID: %d\n", conn.GetSessionID())

	// Use the connection for database operations
	_, err = conn.GetDatabase("default_db")
	if err != nil {
		log.Printf("Failed to get database: %v", err)
		pool.Put(conn)
		return
	}
	fmt.Printf("Got database: default_db\n")

	// Return connection to pool
	err = pool.Put(conn)
	if err != nil {
		log.Printf("Failed to return connection: %v", err)
		return
	}
	fmt.Println("Connection returned to pool")

	fmt.Println()
}

// exampleCustomConfig demonstrates pool with custom configuration
func exampleCustomConfig() {
	fmt.Println("--- Example 2: Custom Pool Configuration ---")

	// Create custom configuration
	config := infinity.ConnectionPoolConfig{
		URI:                 infinity.LocalHost,
		InitialSize:         2,
		MaxIdleTime:         10 * time.Minute,
		HealthCheckInterval: 1 * time.Minute,
		WaitTimeout:         10 * time.Second,
		BlockWhenExhausted:  true,
	}

	pool, err := infinity.NewConnectionPool(config, func(uri infinity.URI) (*infinity.InfinityConnection, error) {
		// Custom factory function - can add logging, metrics, etc.
		fmt.Println("  [Factory] Creating new connection...")
		return infinity.Connect(uri)
	})
	if err != nil {
		log.Printf("Failed to create pool: %v", err)
		return
	}
	defer pool.Close()

	fmt.Printf("Pool created with custom config:\n")
	fmt.Printf("  Initial Size: %d\n", config.InitialSize)
	fmt.Printf("  Max Idle Time: %v\n", config.MaxIdleTime)
	fmt.Printf("  Wait Timeout: %v\n", config.WaitTimeout)

	// Get multiple connections to see initial connections being used
	for i := 0; i < 3; i++ {
		conn, err := pool.Get()
		if err != nil {
			log.Printf("Failed to get connection %d: %v", i, err)
			continue
		}
		fmt.Printf("  Got connection %d, Session ID: %d\n", i+1, conn.GetSessionID())
		pool.Put(conn)
	}

	fmt.Println()
}

// exampleConcurrentAccess demonstrates concurrent access to the pool
func exampleConcurrentAccess() {
	fmt.Println("--- Example 3: Concurrent Pool Access ---")

	config := infinity.ConnectionPoolConfig{
		URI:                 infinity.LocalHost,
		InitialSize:         1,
		BlockWhenExhausted:  true,
		WaitTimeout:         5 * time.Second,
	}

	pool, err := infinity.NewConnectionPool(config, nil)
	if err != nil {
		log.Printf("Failed to create pool: %v", err)
		return
	}
	defer pool.Close()

	fmt.Printf("Pool initial size: %d, simulating %d concurrent workers\n", config.InitialSize, 5)

	var wg sync.WaitGroup
	for i := 0; i < 5; i++ {
		wg.Add(1)
		go func(workerID int) {
			defer wg.Done()

			// Get connection from pool
			conn, err := pool.Get()
			if err != nil {
				fmt.Printf("  Worker %d: Failed to get connection: %v\n", workerID, err)
				return
			}

			fmt.Printf("  Worker %d: Got connection (Session ID: %d)\n", workerID, conn.GetSessionID())

			// Simulate some work
			time.Sleep(500 * time.Millisecond)

			// Return connection to pool
			err = pool.Put(conn)
			if err != nil {
				fmt.Printf("  Worker %d: Failed to return connection: %v\n", workerID, err)
				return
			}
			fmt.Printf("  Worker %d: Returned connection\n", workerID)
		}(i)
	}

	wg.Wait()
	fmt.Println("All workers completed")
	fmt.Println()
}

// examplePoolStats demonstrates pool statistics
func examplePoolStats() {
	fmt.Println("--- Example 4: Pool Statistics ---")

	config := infinity.DefaultConnectionPoolConfig(infinity.LocalHost)
	pool, err := infinity.NewConnectionPool(config, nil)
	if err != nil {
		log.Printf("Failed to create pool: %v", err)
		return
	}
	defer pool.Close()

	// Print initial stats
	stats := pool.Stats()
	fmt.Printf("Initial pool stats:\n")
	printStats(stats)

	// Get some connections
	conns := make([]*infinity.InfinityConnection, 0, 3)
	for i := 0; i < 3; i++ {
		conn, err := pool.Get()
		if err != nil {
			log.Printf("Failed to get connection: %v", err)
			break
		}
		conns = append(conns, conn)
	}

	// Print stats while connections are in use
	stats = pool.Stats()
	fmt.Printf("\nStats with %d connections in use:\n", len(conns))
	printStats(stats)

	// Return connections
	for _, conn := range conns {
		pool.Put(conn)
	}

	// Print final stats
	stats = pool.Stats()
	fmt.Printf("\nStats after returning connections:\n")
	printStats(stats)

	fmt.Println()
}

func printStats(stats infinity.PoolStats) {
	fmt.Printf("  Total Connections: %d\n", stats.TotalConnections)
	fmt.Printf("  Available Connections: %d\n", stats.AvailableConnections)
	fmt.Printf("  In Use Connections: %d\n", stats.InUseConnections)
	fmt.Printf("  Closed: %v\n", stats.Closed)
}

// exampleContextTimeout demonstrates pool with context timeout
func exampleContextTimeout() {
	fmt.Println("--- Example 5: Context Timeout ---")

	// Create a pool with only 1 connection
	config := infinity.ConnectionPoolConfig{
		URI:                 infinity.LocalHost,
		InitialSize:         1,
		BlockWhenExhausted:  true,
		WaitTimeout:         2 * time.Second,
	}

	pool, err := infinity.NewConnectionPool(config, nil)
	if err != nil {
		log.Printf("Failed to create pool: %v", err)
		return
	}
	defer pool.Close()

	// Get the only connection
	conn1, err := pool.Get()
	if err != nil {
		log.Printf("Failed to get first connection: %v", err)
		return
	}
	fmt.Printf("Got first connection (Session ID: %d)\n", conn1.GetSessionID())

	// Try to get another connection with short timeout
	ctx, cancel := context.WithTimeout(context.Background(), 1*time.Second)
	defer cancel()

	fmt.Println("Trying to get second connection (should timeout)...")
	start := time.Now()
	conn2, err := pool.GetContext(ctx)
	elapsed := time.Since(start)

	if err != nil {
		fmt.Printf("Expected timeout occurred after %v: %v\n", elapsed, err)
	} else {
		fmt.Printf("Unexpectedly got second connection\n")
		pool.Put(conn2)
	}

	// Return first connection
	pool.Put(conn1)
	fmt.Println("First connection returned to pool")

	fmt.Println()
}

// exampleConnectionReuse demonstrates connection reuse
func exampleConnectionReuse() {
	fmt.Println("--- Example 6: Connection Reuse ---")

	config := infinity.DefaultConnectionPoolConfig(infinity.LocalHost)
	pool, err := infinity.NewConnectionPool(config, nil)
	if err != nil {
		log.Printf("Failed to create pool: %v", err)
		return
	}
	defer pool.Close()

	// Get and return the same connection multiple times
	sessionIDs := make(map[int64]int)

	for i := 0; i < 5; i++ {
		conn, err := pool.Get()
		if err != nil {
			log.Printf("Failed to get connection: %v", err)
			continue
		}

		sessionID := conn.GetSessionID()
		sessionIDs[sessionID]++
		fmt.Printf("Iteration %d: Got connection with Session ID: %d\n", i+1, sessionID)

		pool.Put(conn)
	}

	fmt.Printf("\nSession ID reuse count:\n")
	for sessionID, count := range sessionIDs {
		fmt.Printf("  Session ID %d: used %d times\n", sessionID, count)
	}

	fmt.Println()
}
