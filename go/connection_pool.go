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

import (
	"context"
	"sync"
)

// ConnectionPool manages a pool of Infinity connections
type ConnectionPool struct {
	uri         URI
	maxSize     int
	connections chan *InfinityConnection
	mu          sync.Mutex
	factory     func(URI) (*InfinityConnection, error)
}

// ConnectionPoolConfig contains configuration for the connection pool
type ConnectionPoolConfig struct {
	URI     URI
	MaxSize int
}

// NewConnectionPool creates a new connection pool
func NewConnectionPool(config ConnectionPoolConfig, factory func(URI) (*InfinityConnection, error)) *ConnectionPool {
	if config.MaxSize <= 0 {
		config.MaxSize = 10
	}
	return &ConnectionPool{
		uri:         config.URI,
		maxSize:     config.MaxSize,
		connections: make(chan *InfinityConnection, config.MaxSize),
		factory:     factory,
	}
}

// Get gets a connection from the pool
func (p *ConnectionPool) Get(ctx context.Context) (*InfinityConnection, error) {
	select {
	case conn := <-p.connections:
		return conn, nil
	case <-ctx.Done():
		return nil, ctx.Err()
	default:
		// Create a new connection if pool is empty
		return p.factory(p.uri)
	}
}

// Put returns a connection to the pool
func (p *ConnectionPool) Put(conn *InfinityConnection) {
	select {
	case p.connections <- conn:
		// Connection returned to pool
	default:
		// Pool is full, close the connection
		conn.Disconnect()
	}
}

// Close closes all connections in the pool
func (p *ConnectionPool) Close() {
	p.mu.Lock()
	defer p.mu.Unlock()

	close(p.connections)
	for conn := range p.connections {
		conn.Disconnect()
	}
}

// Size returns the current number of connections in the pool
func (p *ConnectionPool) Size() int {
	return len(p.connections)
}
