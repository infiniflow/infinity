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
	"sync"
	"time"
)

// ConnectionPoolConfig contains configuration for the connection pool
type ConnectionPoolConfig struct {
	URI                 URI
	InitialSize         int           // Initial number of connections to create
	MaxIdleTime         time.Duration // Maximum time a connection can be idle
	HealthCheckInterval time.Duration // Interval for health checks (ignored)
	WaitTimeout         time.Duration // Maximum time to wait for a connection from the pool (ignored)
	BlockWhenExhausted  bool          // Whether to block when pool is exhausted (ignored)
}

// DefaultConnectionPoolConfig returns a default configuration
func DefaultConnectionPoolConfig(uri URI) ConnectionPoolConfig {
	return ConnectionPoolConfig{
		URI:                 uri,
		InitialSize:         10,
		MaxIdleTime:         30 * time.Minute,
		HealthCheckInterval: 5 * time.Minute,
		WaitTimeout:         30 * time.Second,
		BlockWhenExhausted:  true,
	}
}

// PooledConnection wraps a connection with pool metadata
type PooledConnection struct {
	conn       *InfinityConnection
	createdAt  time.Time
	lastUsedAt time.Time
}

// ConnectionPool manages a pool of Infinity connections
type ConnectionPool struct {
	uri          URI
	config       ConnectionPoolConfig
	factory      func(URI) (*InfinityConnection, error)
	mu           sync.Mutex
	closed       bool
	available    []*PooledConnection
	createdCount int
	connToPooled map[*InfinityConnection]*PooledConnection
}

// NewConnectionPool creates a new connection pool
func NewConnectionPool(config ConnectionPoolConfig, factory func(URI) (*InfinityConnection, error)) (*ConnectionPool, error) {
	if config.InitialSize < 0 {
		config.InitialSize = 0
	}
	if config.MaxIdleTime <= 0 {
		config.MaxIdleTime = 30 * time.Minute
	}
	if factory == nil {
		factory = Connect
	}

	pool := &ConnectionPool{
		uri:          config.URI,
		config:       config,
		factory:      factory,
		available:    make([]*PooledConnection, 0, config.InitialSize),
		connToPooled: make(map[*InfinityConnection]*PooledConnection),
	}

	// Create initial connections
	pool.mu.Lock()
	defer pool.mu.Unlock()
	for i := 0; i < config.InitialSize; i++ {
		pooledConn, err := pool.createConnection()
		if err != nil {
			pool.Close()
			return nil, fmt.Errorf("failed to create initial connection %d: %w", i, err)
		}
		pool.available = append(pool.available, pooledConn)
	}

	return pool, nil
}

// createConnection creates a new pooled connection
// Caller must hold p.mu lock
func (p *ConnectionPool) createConnection() (*PooledConnection, error) {
	conn, err := p.factory(p.uri)
	if err != nil {
		return nil, err
	}

	now := time.Now()
	pooledConn := &PooledConnection{
		conn:       conn,
		createdAt:  now,
		lastUsedAt: now,
	}

	p.connToPooled[conn] = pooledConn
	p.createdCount++

	return pooledConn, nil
}

// Get gets a connection from the pool
func (p *ConnectionPool) Get() (*InfinityConnection, error) {
	return p.GetContext(context.Background())
}

// GetContext gets a connection from the pool with context
func (p *ConnectionPool) GetContext(ctx context.Context) (*InfinityConnection, error) {
	// Check if context is already done
	if err := ctx.Err(); err != nil {
		return nil, err
	}

	p.mu.Lock()
	defer p.mu.Unlock()

	if p.closed {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection pool is closed")
	}

	// Clean up idle or invalid connections from available slice
	valid := p.available[:0]
	for _, pooledConn := range p.available {
		// Check if connection is still alive
		if !pooledConn.conn.IsConnected() {
			p.removeConnection(pooledConn)
			continue
		}
		// Check if connection has been idle too long
		if time.Since(pooledConn.lastUsedAt) > p.config.MaxIdleTime {
			p.removeConnection(pooledConn)
			continue
		}
		valid = append(valid, pooledConn)
	}
	p.available = valid

	// If there are available connections, take the last one
	if len(p.available) > 0 {
		pooledConn := p.available[len(p.available)-1]
		p.available = p.available[:len(p.available)-1]
		pooledConn.lastUsedAt = time.Now()
		conn := pooledConn.conn
		return conn, nil
	}

	// No available connections, create a new one
	pooledConn, err := p.createConnection()
	if err != nil {
		return nil, err
	}
	return pooledConn.conn, nil
}

// Put returns a connection to the pool
func (p *ConnectionPool) Put(conn *InfinityConnection) error {
	if conn == nil {
		return NewInfinityException(int(ErrorCodeInvalidParameterValue), "Cannot return nil connection to pool")
	}

	p.mu.Lock()
	defer p.mu.Unlock()

	if p.closed {
		conn.Disconnect()
		return NewInfinityException(int(ErrorCodeClientClose), "Connection pool is closed")
	}

	pooledConn, ok := p.connToPooled[conn]
	if !ok {
		// Connection not from this pool, close it
		conn.Disconnect()
		return NewInfinityException(int(ErrorCodeInvalidParameterValue), "Connection not from this pool")
	}

	// Check if connection is still valid
	if !conn.IsConnected() {
		p.removeConnection(pooledConn)
		return NewInfinityException(int(ErrorCodeClientClose), "Connection is dead, removed from pool")
	}

	// Check if connection is already in available list (double release)
	for _, available := range p.available {
		if available.conn == conn {
			return NewInfinityException(int(ErrorCodeInvalidParameterValue), "Connection is already in pool (double release)")
		}
	}

	// Always return connection to pool (no max size limit)
	pooledConn.lastUsedAt = time.Now()
	p.available = append(p.available, pooledConn)
	return nil
}

// removeConnection closes a pooled connection and removes it from tracking
func (p *ConnectionPool) removeConnection(pooledConn *PooledConnection) {
	if pooledConn.conn != nil {
		delete(p.connToPooled, pooledConn.conn)
		pooledConn.conn.Disconnect()
		p.createdCount--
	}
}

// isClosed checks if the pool is closed
func (p *ConnectionPool) isClosed() bool {
	p.mu.Lock()
	defer p.mu.Unlock()
	return p.closed
}

// Close closes all connections in the pool
func (p *ConnectionPool) Close() error {
	p.mu.Lock()
	if p.closed {
		p.mu.Unlock()
		return NewInfinityException(int(ErrorCodeClientClose), "Connection pool already closed")
	}
	p.closed = true

	// Close all connections
	for conn := range p.connToPooled {
		conn.Disconnect()
	}
	// Clear maps and slices
	p.connToPooled = make(map[*InfinityConnection]*PooledConnection)
	p.available = nil
	p.createdCount = 0

	p.mu.Unlock()
	return nil
}

// Stats returns statistics about the pool
type PoolStats struct {
	TotalConnections     int
	AvailableConnections int
	InUseConnections     int
	Closed               bool
}

// Stats returns current pool statistics
func (p *ConnectionPool) Stats() PoolStats {
	p.mu.Lock()
	defer p.mu.Unlock()

	total := p.createdCount
	available := len(p.available)
	inUse := total - available

	return PoolStats{
		TotalConnections:     total,
		AvailableConnections: available,
		InUseConnections:     inUse,
		Closed:               p.closed,
	}
}

// Size returns the current number of connections in the pool
func (p *ConnectionPool) Size() int {
	p.mu.Lock()
	defer p.mu.Unlock()
	return p.createdCount
}

// Available returns the number of available connections in the pool
func (p *ConnectionPool) Available() int {
	p.mu.Lock()
	defer p.mu.Unlock()
	return len(p.available)
}
