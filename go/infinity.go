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

package infinity

import (
	"context"
	"fmt"

	"github.com/apache/thrift/lib/go/thrift"
	thriftapi "github.com/infiniflow/infinity-go-sdk/internal/thrift"
)

// ClientVersion is the version of this Go SDK client
// This should match the protocol version expected by the server
const ClientVersion = 36 // version: 0.7.0.dev2

// Connect creates a new connection to Infinity
// This is a factory function that creates the appropriate connection type based on the URI
// For now, only NetworkAddress (remote thrift connection) is supported
//
// Example:
//
//	conn, err := infinity.Connect(infinity.LocalHost)
//	if err != nil {
//	    log.Fatal(err)
//	}
//	defer conn.Disconnect()
func Connect(uri URI) (*InfinityConnection, error) {
	switch u := uri.(type) {
	case NetworkAddress:
		return NewInfinityConnection(u)
	default:
		return nil, NewInfinityException(
			int(ErrorCodeInvalidServerAddress),
			fmt.Sprintf("Unknown uri type: %T", uri),
		)
	}
}

// InfinityConnection represents a connection to Infinity server
type InfinityConnection struct {
	uri         URI
	transport   thrift.TTransport
	client      *thriftapi.InfinityServiceClient
	sessionID   int64
	isConnected bool
	dbName      string
}

// NewInfinityConnection creates a new connection to Infinity server
func NewInfinityConnection(address NetworkAddress) (*InfinityConnection, error) {
	// Create thrift transport
	// Use TBufferedTransport for sync communication (matching Python's TBufferedTransport)
	transport := thrift.NewTSocketConf(fmt.Sprintf("%s:%d", address.IP, address.Port), &thrift.TConfiguration{})

	// Use buffered transport
	bufferedTransport := thrift.NewTBufferedTransport(transport, 8192)

	// Create binary protocol (matching Python's TBinaryProtocol)
	protocolFactory := thrift.NewTBinaryProtocolFactoryDefault()
	protocol := protocolFactory.GetProtocol(bufferedTransport)

	// Create client
	client := thriftapi.NewInfinityServiceClientProtocol(bufferedTransport, protocol, protocol)

	// Open transport
	if err := bufferedTransport.Open(); err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to connect to server at %s:%d: %v", address.IP, address.Port, err),
		)
	}

	// Create connection object
	conn := &InfinityConnection{
		uri:         address,
		transport:   bufferedTransport,
		client:      client,
		isConnected: true,
		dbName:      "default_db",
	}

	// Perform connect handshake with server
	ctx := context.Background()
	connectReq := thriftapi.NewConnectRequest()
	connectReq.ClientVersion = ClientVersion

	resp, err := client.Connect(ctx, connectReq)
	if err != nil {
		bufferedTransport.Close()
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to connect: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		bufferedTransport.Close()
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Server rejected connection: %s", resp.ErrorMsg),
		)
	}

	// Store session ID from server
	conn.sessionID = resp.SessionID

	return conn, nil
}

// Disconnect closes the connection
func (c *InfinityConnection) Disconnect() (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection already closed")
	}

	// Send disconnect request to server
	ctx := context.Background()
	disconnectReq := thriftapi.NewCommonRequest()
	disconnectReq.SessionID = c.sessionID

	resp, err := c.client.Disconnect(ctx, disconnectReq)
	if err != nil {
		// Log error but still close transport
		fmt.Printf("Warning: error during disconnect: %v\n", err)
	} else if resp.ErrorCode != 0 {
		// Log server error but still close transport
		fmt.Printf("Warning: server error during disconnect: %s\n", resp.ErrorMsg)
	}

	// Close transport
	if c.transport != nil {
		c.transport.Close()
	}

	c.isConnected = false
	c.sessionID = 0
	return struct{}{}, nil
}

// IsConnected returns whether the connection is still active
func (c *InfinityConnection) IsConnected() bool {
	if !c.isConnected {
		return false
	}
	// Also check if the transport is still open
	if c.transport != nil && !c.transport.IsOpen() {
		c.isConnected = false
		return false
	}
	return true
}

// GetSessionID returns the session ID
func (c *InfinityConnection) GetSessionID() int64 {
	return c.sessionID
}

// CreateDatabase creates a new database
func (c *InfinityConnection) CreateDatabase(dbName string, conflictType ConflictType, comment string) (*Database, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Convert ConflictType to CreateConflict
	var createConflict thriftapi.CreateConflict
	switch conflictType {
	case ConflictTypeIgnore:
		createConflict = thriftapi.CreateConflict_Ignore
	case ConflictTypeError:
		createConflict = thriftapi.CreateConflict_Error
	case ConflictTypeReplace:
		createConflict = thriftapi.CreateConflict_Replace
	default:
		return nil, NewInfinityException(int(ErrorCodeInvalidConflictType), "Invalid conflict type")
	}

	// Create request
	req := thriftapi.NewCreateDatabaseRequest()
	req.DbName = dbName
	req.SessionID = c.GetSessionID()
	req.CreateOption = &thriftapi.CreateOption{
		ConflictType: createConflict,
		Properties:   []*thriftapi.Property{}, // empty properties
	}
	req.DbComment = comment

	// Call thrift
	ctx := context.Background()
	resp, err := c.client.CreateDatabase(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to create database: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to create database: %s", resp.ErrorMsg),
		)
	}

	return &Database{conn: c, dbName: dbName}, nil
}

// ListDatabases lists all databases
func (c *InfinityConnection) ListDatabases() (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewListDatabaseRequest()
	req.SessionID = c.GetSessionID()

	// Call thrift
	ctx := context.Background()
	resp, err := c.client.ListDatabase(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to list databases: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to list databases: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// ShowDatabase shows details of a specific database
func (c *InfinityConnection) ShowDatabase(dbName string) (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewShowDatabaseRequest()
	req.DbName = dbName
	req.SessionID = c.GetSessionID()

	// Call thrift
	ctx := context.Background()
	resp, err := c.client.ShowDatabase(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to show database: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to show database: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// DropDatabase drops a database
func (c *InfinityConnection) DropDatabase(dbName string, conflictType ConflictType) (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Convert ConflictType to DropConflict
	var dropConflict thriftapi.DropConflict
	switch conflictType {
	case ConflictTypeIgnore:
		dropConflict = thriftapi.DropConflict_Ignore
	case ConflictTypeError:
		dropConflict = thriftapi.DropConflict_Error
	default:
		return nil, NewInfinityException(int(ErrorCodeInvalidConflictType), "Invalid conflict type for drop")
	}

	// Create request
	req := thriftapi.NewDropDatabaseRequest()
	req.DbName = dbName
	req.SessionID = c.GetSessionID()
	req.DropOption = &thriftapi.DropOption{
		ConflictType: dropConflict,
	}

	// Call thrift
	ctx := context.Background()
	resp, err := c.client.DropDatabase(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to drop database: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to drop database: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// GetDatabase gets a database object
func (c *InfinityConnection) GetDatabase(dbName string) (*Database, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create get database request
	req := thriftapi.NewGetDatabaseRequest()
	req.DbName = dbName
	req.SessionID = c.GetSessionID()

	// Call thrift
	ctx := context.Background()
	resp, err := c.client.GetDatabase(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to get database: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to get database: %s", resp.ErrorMsg),
		)
	}

	return &Database{conn: c, dbName: dbName}, nil
}

// ShowCurrentNode shows current node information
func (c *InfinityConnection) ShowCurrentNode() (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewShowCurrentNodeRequest()
	req.SessionID = c.GetSessionID()

	// Call thrift
	ctx := context.Background()
	resp, err := c.client.ShowCurrentNode(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to show current node: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to show current node: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// CreateDatabaseSnapshot creates a snapshot of a database
func (c *InfinityConnection) CreateDatabaseSnapshot(snapshotName string, dbName string) (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewCreateDatabaseSnapshotRequest()
	req.SessionID = c.GetSessionID()
	req.DbName = dbName
	req.SnapshotName = snapshotName

	// Call thrift
	ctx := context.Background()
	resp, err := c.client.CreateDatabaseSnapshot(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to create database snapshot: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to create database snapshot: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// RestoreDatabaseSnapshot restores a database from a snapshot
func (c *InfinityConnection) RestoreDatabaseSnapshot(snapshotName string) (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewRestoreSnapshotRequest()
	req.SessionID = c.GetSessionID()
	req.SnapshotName = snapshotName
	req.Scope = "database"

	// Call thrift
	ctx := context.Background()
	resp, err := c.client.RestoreSnapshot(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to restore database snapshot: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to restore database snapshot: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// CreateSystemSnapshot creates a system-wide snapshot
func (c *InfinityConnection) CreateSystemSnapshot(snapshotName string) (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewCreateSystemSnapshotRequest()
	req.SessionID = c.GetSessionID()
	req.SnapshotName = snapshotName

	// Call thrift
	ctx := context.Background()
	resp, err := c.client.CreateSystemSnapshot(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to create system snapshot: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to create system snapshot: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// RestoreSystemSnapshot restores the system from a snapshot
func (c *InfinityConnection) RestoreSystemSnapshot(snapshotName string) (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewRestoreSnapshotRequest()
	req.SessionID = c.GetSessionID()
	req.SnapshotName = snapshotName
	req.Scope = "system"

	// Call thrift
	ctx := context.Background()
	resp, err := c.client.RestoreSnapshot(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to restore system snapshot: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to restore system snapshot: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// ListSnapshots lists all snapshots
func (c *InfinityConnection) ListSnapshots() (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewListSnapshotsRequest()
	req.SessionID = c.GetSessionID()

	// Call thrift
	ctx := context.Background()
	resp, err := c.client.ListSnapshots(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to list snapshots: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to list snapshots: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// ShowSnapshot shows details of a specific snapshot
func (c *InfinityConnection) ShowSnapshot(snapshotName string) (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewShowSnapshotRequest()
	req.SessionID = c.GetSessionID()
	req.SnapshotName = snapshotName

	// Call thrift
	ctx := context.Background()
	resp, err := c.client.ShowSnapshot(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to show snapshot: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to show snapshot: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// DropSnapshot drops a snapshot
func (c *InfinityConnection) DropSnapshot(snapshotName string) (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewDropSnapshotRequest()
	req.SessionID = c.GetSessionID()
	req.SnapshotName = snapshotName

	// Call thrift
	ctx := context.Background()
	resp, err := c.client.DropSnapshot(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to drop snapshot: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to drop snapshot: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// Cleanup performs cleanup operations
func (c *InfinityConnection) Cleanup() (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}
	// TODO: Implement thrift call
	return nil, nil
}

// Optimize optimizes a table
func (c *InfinityConnection) Optimize(dbName string, tableName string) (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}
	// TODO: Implement thrift call
	return nil, nil
}

// TestCommand executes a test command
func (c *InfinityConnection) TestCommand(commandContent string) error {
	if !c.isConnected {
		return NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}
	// TODO: Implement thrift call
	return nil
}

// FlushData flushes data to disk
func (c *InfinityConnection) FlushData() error {
	if !c.isConnected {
		return NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewFlushRequest()
	req.SessionID = c.GetSessionID()
	req.FlushType = "data"

	// Call thrift
	ctx := context.Background()
	resp, err := c.client.Flush(ctx, req)
	if err != nil {
		return NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to flush data: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to flush data: %s", resp.ErrorMsg),
		)
	}

	return nil
}

// FlushCatalog flushes catalog to disk
func (c *InfinityConnection) FlushCatalog() error {
	if !c.isConnected {
		return NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}
	// TODO: Implement thrift call
	return nil
}

// SetConfig sets a configuration value
func (c *InfinityConnection) SetConfig(configName string, configValue interface{}) (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Convert configValue to thrift ConfigValue
	thriftConfigValue, err := configValueFromInterface(configValue)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeInvalidParameterValue),
			fmt.Sprintf("Invalid config value: %v", err),
		)
	}

	// Create request
	req := thriftapi.NewSetConfigRequest()
	req.SessionID = c.GetSessionID()
	req.ConfigName = configName
	req.ConfigValue = thriftConfigValue

	// Call thrift
	ctx := context.Background()
	resp, err := c.client.SetConfig(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to set config: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to set config: %s", resp.ErrorMsg),
		)
	}

	return resp, nil
}

// ShowConfig shows a configuration value
func (c *InfinityConnection) ShowConfig(configName string) (*ConfigResponse, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// Create request
	req := thriftapi.NewShowConfigRequest()
	req.SessionID = c.GetSessionID()
	req.ConfigName = configName

	// Call thrift
	ctx := context.Background()
	resp, err := c.client.ShowConfig(ctx, req)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to show config: %v", err),
		)
	}

	// Check response error code
	if resp.ErrorCode != 0 {
		return nil, NewInfinityException(
			int(resp.ErrorCode),
			fmt.Sprintf("Failed to show config: %s", resp.ErrorMsg),
		)
	}

	// Extract config value from thrift ConfigValue
	var configValue interface{}
	if resp.ConfigValue != nil {
		if resp.ConfigValue.StringValue != nil {
			configValue = *resp.ConfigValue.StringValue
		} else if resp.ConfigValue.IntValue != nil {
			configValue = *resp.ConfigValue.IntValue
		} else if resp.ConfigValue.BoolValue != nil {
			configValue = *resp.ConfigValue.BoolValue
		} else if resp.ConfigValue.DoubleValue != nil {
			configValue = *resp.ConfigValue.DoubleValue
		} else {
			configValue = nil
		}
	} else {
		configValue = nil
	}

	return &ConfigResponse{
		ErrorCode:   ErrorCode(resp.ErrorCode),
		ErrorMsg:    resp.ErrorMsg,
		ConfigName:  resp.ConfigName,
		ConfigValue: configValue,
	}, nil
}

// ConfigResponse represents the response from show_config
type ConfigResponse struct {
	ErrorCode   ErrorCode
	ErrorMsg    string
	ConfigName  string
	ConfigValue interface{}
}

// configValueFromInterface converts a Go interface{} to a thrift ConfigValue
func configValueFromInterface(v interface{}) (*thriftapi.ConfigValue, error) {
	cv := thriftapi.NewConfigValue()
	switch val := v.(type) {
	case string:
		cv.StringValue = &val
	case int64:
		cv.IntValue = &val
	case int:
		// Convert int to int64
		i64 := int64(val)
		cv.IntValue = &i64
	case bool:
		cv.BoolValue = &val
	case float64:
		cv.DoubleValue = &val
	case float32:
		// Convert float32 to float64
		f64 := float64(val)
		cv.DoubleValue = &f64
	default:
		return nil, fmt.Errorf("unsupported config value type %T", v)
	}
	return cv, nil
}
