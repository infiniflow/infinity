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

// This example demonstrates the usage of all InfinityConnection interface methods
// defined in infinity.go. It shows how to connect to Infinity server and call
// each method with appropriate parameters.

package main

import (
	"fmt"
	"log"

	infinity "github.com/infiniflow/infinity-go-sdk"
)

func main() {
	fmt.Println("=== Infinity Go SDK Interface Examples ===")
	fmt.Println("Demonstrating all InfinityConnection methods")

	// Connect to Infinity server
	fmt.Println("--- 1. Connect to Infinity ---")
	conn, err := infinity.Connect(infinity.LocalHost)
	if err != nil {
		log.Fatalf("Failed to connect: %v", err)
	}
	fmt.Println("✓ Connected to Infinity successfully")
	fmt.Printf("  Session ID: %d\n", conn.GetSessionID())

	// Ensure disconnect when done
	defer func() {
		_, err := conn.Disconnect()
		if err != nil {
			log.Printf("Error during disconnect: %v", err)
		} else {
			fmt.Println("✓ Disconnected from Infinity")
		}
	}()

	// Test IsConnected method
	fmt.Println("\n--- 2. Connection Status ---")
	fmt.Printf("IsConnected: %v\n", conn.IsConnected())

	// Database operations
	fmt.Println("\n--- 3. Database Operations ---")

	// CreateDatabase
	fmt.Println("\n- CreateDatabase -")
	db, err := conn.CreateDatabase("example_db", infinity.ConflictTypeIgnore, "Example database")
	if err != nil {
		log.Printf("CreateDatabase error (may already exist): %v", err)
		// Try to get existing database
		db, err = conn.GetDatabase("example_db")
		if err != nil {
			log.Printf("Failed to get database: %v", err)
		} else {
			fmt.Println("✓ Got existing database 'example_db'")
		}
	} else {
		fmt.Println("✓ Created database 'example_db'")
	}

	// Use the database object to demonstrate database methods
	if db != nil {
		tables, err := db.ListTables()
		if err != nil {
			log.Printf("ListTables error: %v", err)
		} else {
			fmt.Printf("✓ Tables in database: %v\n", tables)
		}
	}

	// ListDatabases
	fmt.Println("\n- ListDatabases -")
	dbList, err := conn.ListDatabases()
	if err != nil {
		log.Printf("ListDatabases error: %v", err)
	} else {
		fmt.Printf("✓ Database list: %v\n", dbList)
	}

	// ShowDatabase
	fmt.Println("\n- ShowDatabase -")
	dbInfo, err := conn.ShowDatabase("example_db")
	if err != nil {
		log.Printf("ShowDatabase error: %v", err)
	} else {
		fmt.Printf("✓ Database info: %v\n", dbInfo)
	}

	// GetDatabase (already used above, but demonstrate again)
	fmt.Println("\n- GetDatabase -")
	db2, err := conn.GetDatabase("example_db")
	if err != nil {
		log.Printf("GetDatabase error: %v", err)
	} else {
		fmt.Printf("✓ Got database object for 'example_db'\n")
		_ = db2 // Use in further examples if needed
	}

	// GetDatabase (already used above, but demonstrate again)
	fmt.Println("\n- GetDatabase -")
	defaultDb, err := conn.GetDatabase("default_db")
	if err != nil {
		log.Printf("GetDatabase error: %v", err)
	} else {
		fmt.Printf("✓ Got database object for 'example_db'\n")
		_ = defaultDb // Use in further examples if needed
	}

	// DropDatabase (commented out to avoid destroying data)
	// Uncomment to test, but be careful!
	fmt.Println("\n- DropDatabase -")
	_, err = conn.DropDatabase("example_db", infinity.ConflictTypeError)
	if err != nil {
		log.Printf("DropDatabase error: %v", err)
	} else {
		fmt.Println("✓ Dropped database 'example_db'")
	}

	// Node information
	fmt.Println("\n--- 4. Node Information ---")
	fmt.Println("\n- ShowCurrentNode -")
	nodeInfo, err := conn.ShowCurrentNode()
	if err != nil {
		log.Printf("ShowCurrentNode error: %v", err)
	} else {
		fmt.Printf("✓ Current node info: %v\n", nodeInfo)
	}

	//// Snapshot operations
	//fmt.Println("\n--- 5. Snapshot Operations ---")
	//
	//// CreateDatabaseSnapshot
	//fmt.Println("\n- CreateDatabaseSnapshot -")
	//_, err = conn.CreateDatabaseSnapshot("example_snapshot", "example_db")
	//if err != nil {
	//	log.Printf("CreateDatabaseSnapshot error (may not be supported): %v", err)
	//} else {
	//	fmt.Println("✓ Created database snapshot 'example_snapshot'")
	//}
	//
	//// RestoreDatabaseSnapshot
	//fmt.Println("\n- RestoreDatabaseSnapshot -")
	//_, err = conn.RestoreDatabaseSnapshot("example_snapshot")
	//if err != nil {
	//	log.Printf("RestoreDatabaseSnapshot error (may not be supported): %v", err)
	//} else {
	//	fmt.Println("✓ Restored database from snapshot 'example_snapshot'")
	//}
	//
	//// CreateSystemSnapshot
	//fmt.Println("\n- CreateSystemSnapshot -")
	//_, err = conn.CreateSystemSnapshot("system_snapshot")
	//if err != nil {
	//	log.Printf("CreateSystemSnapshot error (may not be supported): %v", err)
	//} else {
	//	fmt.Println("✓ Created system snapshot 'system_snapshot'")
	//}
	//
	//// RestoreSystemSnapshot
	//fmt.Println("\n- RestoreSystemSnapshot -")
	//_, err = conn.RestoreSystemSnapshot("system_snapshot")
	//if err != nil {
	//	log.Printf("RestoreSystemSnapshot error (may not be supported): %v", err)
	//} else {
	//	fmt.Println("✓ Restored system from snapshot 'system_snapshot'")
	//}
	//
	//// ListSnapshots
	//fmt.Println("\n- ListSnapshots -")
	//snapshots, err := conn.ListSnapshots()
	//if err != nil {
	//	log.Printf("ListSnapshots error: %v", err)
	//} else {
	//	fmt.Printf("✓ Snapshots list: %v\n", snapshots)
	//}
	//
	//// ShowSnapshot
	//fmt.Println("\n- ShowSnapshot -")
	//snapshotInfo, err := conn.ShowSnapshot("example_snapshot")
	//if err != nil {
	//	log.Printf("ShowSnapshot error (snapshot may not exist): %v", err)
	//} else {
	//	fmt.Printf("✓ Snapshot info: %v\n", snapshotInfo)
	//}
	//
	//// DropSnapshot
	//fmt.Println("\n- DropSnapshot -")
	//_, err = conn.DropSnapshot("example_snapshot")
	//if err != nil {
	//	log.Printf("DropSnapshot error (snapshot may not exist): %v", err)
	//} else {
	//	fmt.Println("✓ Dropped snapshot 'example_snapshot'")
	//}

	// Maintenance operations
	fmt.Println("\n--- 6. Maintenance Operations ---")

	// Cleanup
	fmt.Println("\n- Cleanup -")
	cleanupResult, err := conn.Cleanup()
	if err != nil {
		log.Printf("Cleanup error: %v", err)
	} else {
		fmt.Printf("✓ Cleanup result: %v\n", cleanupResult)
	}

	// Optimize
	fmt.Println("\n- Optimize -")
	optimizeResult, err := conn.Optimize("example_db", "example_table")
	if err != nil {
		log.Printf("Optimize error (table may not exist): %v", err)
	} else {
		fmt.Printf("✓ Optimize result: %v\n", optimizeResult)
	}

	// TestCommand
	fmt.Println("\n- TestCommand -")
	err = conn.TestCommand("test command")
	if err != nil {
		log.Printf("TestCommand error: %v", err)
	} else {
		fmt.Println("✓ TestCommand executed successfully")
	}

	// FlushData
	fmt.Println("\n- FlushData -")
	err = conn.FlushData()
	if err != nil {
		log.Printf("FlushData error: %v", err)
	} else {
		fmt.Println("✓ Data flushed successfully")
	}

	// FlushCatalog
	fmt.Println("\n- FlushCatalog -")
	err = conn.FlushCatalog()
	if err != nil {
		log.Printf("FlushCatalog error: %v", err)
	} else {
		fmt.Println("✓ Catalog flushed successfully")
	}

	// Configuration operations
	fmt.Println("\n--- 7. Configuration Operations ---")

	// SetConfig
	fmt.Println("\n- SetConfig -")
	setConfigResult, err := conn.SetConfig("log_level", "trace")
	if err != nil {
		log.Printf("SetConfig error: %v", err)
	} else {
		fmt.Printf("✓ SetConfig result: %v\n", setConfigResult)
	}

	// ShowConfig
	fmt.Println("\n- ShowConfig -")
	configResp, err := conn.ShowConfig("log_level")
	if err != nil {
		log.Printf("ShowConfig error: %v", err)
	} else {
		fmt.Printf("✓ To ShowConfig response:\n")
		fmt.Printf("  ConfigName: %s\n", configResp.ConfigName)
		fmt.Printf("  ConfigValue: %v\n", configResp.ConfigValue)
		fmt.Printf("  ErrorCode: %v\n", configResp.ErrorCode)
		fmt.Printf("  ErrorMsg: %s\n", configResp.ErrorMsg)
	}

	fmt.Println("\n=== All Interface Examples Completed ===")
	fmt.Println("\nNote: Many methods return 'TODO: Implement thrift call' errors.")
	fmt.Println("These are expected when the server-side implementation is pending.")
}
