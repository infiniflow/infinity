// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

package infinity

// InfinityConnection defines the interface for connecting to Infinity
type InfinityConnection interface {
	// CreateDatabase creates a new database
	CreateDatabase(dbName string, conflictType ConflictType, comment string) (Database, error)

	// ListDatabases lists all databases
	ListDatabases() (interface{}, error)

	// ShowDatabase shows details of a specific database
	ShowDatabase(dbName string) (interface{}, error)

	// DropDatabase drops a database
	DropDatabase(dbName string, conflictType ConflictType) (interface{}, error)

	// GetDatabase gets a database object
	GetDatabase(dbName string) (Database, error)

	// ShowCurrentNode shows current node information
	ShowCurrentNode() (interface{}, error)

	// Disconnect closes the connection
	Disconnect() (interface{}, error)

	// CreateDatabaseSnapshot creates a snapshot of a database
	CreateDatabaseSnapshot(snapshotName string, dbName string) (interface{}, error)

	// RestoreDatabaseSnapshot restores a database from a snapshot
	RestoreDatabaseSnapshot(snapshotName string) (interface{}, error)

	// CreateSystemSnapshot creates a system-wide snapshot
	CreateSystemSnapshot(snapshotName string) (interface{}, error)

	// RestoreSystemSnapshot restores the system from a snapshot
	RestoreSystemSnapshot(snapshotName string) (interface{}, error)

	// ListSnapshots lists all snapshots
	ListSnapshots() (interface{}, error)

	// ShowSnapshot shows details of a specific snapshot
	ShowSnapshot(snapshotName string) (interface{}, error)

	// DropSnapshot drops a snapshot
	DropSnapshot(snapshotName string) (interface{}, error)

	// Cleanup performs cleanup operations
	Cleanup() (interface{}, error)

	// Optimize optimizes a table
	Optimize(dbName string, tableName string) (interface{}, error)

	// TestCommand executes a test command
	TestCommand(commandContent string) error

	// FlushData flushes data to disk
	FlushData() error

	// FlushCatalog flushes catalog to disk
	FlushCatalog() error

	// SetConfig sets a configuration value
	SetConfig(configName string, configValue interface{}) (interface{}, error)

	// ShowConfig shows a configuration value
	ShowConfig(configName string) (*ConfigResponse, error)
}

// ConfigResponse represents the response from show_config
type ConfigResponse struct {
	ErrorCode   ErrorCode
	ErrorMsg    string
	ConfigName  string
	ConfigValue interface{}
}
