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
	"time"

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
	transport, err := thrift.NewTSocketTimeout(
		fmt.Sprintf("%s:%d", address.IP, address.Port),
		5*time.Second,
		5*time.Second,
	)
	if err != nil {
		return nil, NewInfinityException(
			int(ErrorCodeCantConnectServer),
			fmt.Sprintf("Failed to create socket: %v", err),
		)
	}

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
	return c.isConnected
}

// GetSessionID returns the session ID
func (c *InfinityConnection) GetSessionID() int64 {
	return c.sessionID
}

// CreateDatabase creates a new database
func (c *InfinityConnection) CreateDatabase(dbName string, conflictType ConflictType, comment string) (Database, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}

	// TODO: Implement thrift call
	// res := c.client.CreateDatabase(dbName, conflictType, comment)
	// if res.ErrorCode != ErrorCodeOK {
	//     return nil, NewInfinityException(int(res.ErrorCode), res.ErrorMsg)
	// }

	return &RemoteDatabase{conn: c, dbName: dbName}, nil
}

// ListDatabases lists all databases
func (c *InfinityConnection) ListDatabases() (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}
	// TODO: Implement thrift call
	return nil, nil
}

// ShowDatabase shows details of a specific database
func (c *InfinityConnection) ShowDatabase(dbName string) (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}
	// TODO: Implement thrift call
	return nil, nil
}

// DropDatabase drops a database
func (c *InfinityConnection) DropDatabase(dbName string, conflictType ConflictType) (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}
	// TODO: Implement thrift call
	return nil, nil
}

// GetDatabase gets a database object
func (c *InfinityConnection) GetDatabase(dbName string) (Database, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}
	// TODO: Implement thrift call to verify database exists
	return &RemoteDatabase{conn: c, dbName: dbName}, nil
}

// ShowCurrentNode shows current node information
func (c *InfinityConnection) ShowCurrentNode() (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}
	// TODO: Implement thrift call
	return nil, nil
}

// CreateDatabaseSnapshot creates a snapshot of a database
func (c *InfinityConnection) CreateDatabaseSnapshot(snapshotName string, dbName string) (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}
	// TODO: Implement thrift call
	return nil, nil
}

// RestoreDatabaseSnapshot restores a database from a snapshot
func (c *InfinityConnection) RestoreDatabaseSnapshot(snapshotName string) (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}
	// TODO: Implement thrift call
	return nil, nil
}

// CreateSystemSnapshot creates a system-wide snapshot
func (c *InfinityConnection) CreateSystemSnapshot(snapshotName string) (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}
	// TODO: Implement thrift call
	return nil, nil
}

// RestoreSystemSnapshot restores the system from a snapshot
func (c *InfinityConnection) RestoreSystemSnapshot(snapshotName string) (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}
	// TODO: Implement thrift call
	return nil, nil
}

// ListSnapshots lists all snapshots
func (c *InfinityConnection) ListSnapshots() (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}
	// TODO: Implement thrift call
	return nil, nil
}

// ShowSnapshot shows details of a specific snapshot
func (c *InfinityConnection) ShowSnapshot(snapshotName string) (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}
	// TODO: Implement thrift call
	return nil, nil
}

// DropSnapshot drops a snapshot
func (c *InfinityConnection) DropSnapshot(snapshotName string) (interface{}, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}
	// TODO: Implement thrift call
	return nil, nil
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
	// TODO: Implement thrift call
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
	// TODO: Implement thrift call
	return nil, nil
}

// ShowConfig shows a configuration value
func (c *InfinityConnection) ShowConfig(configName string) (*ConfigResponse, error) {
	if !c.isConnected {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection is closed")
	}
	// TODO: Implement thrift call
	return nil, nil
}

// ConfigResponse represents the response from show_config
type ConfigResponse struct {
	ErrorCode   ErrorCode
	ErrorMsg    string
	ConfigName  string
	ConfigValue interface{}
}
