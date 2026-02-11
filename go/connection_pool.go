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
	"sync/atomic"
	"time"
)

// ConnectionPoolConfig contains configuration for the connection pool
type ConnectionPoolConfig struct {
	URI             URI
	MaxSize         int           // Maximum number of connections in the pool
	InitialSize     int           // Initial number of connections to create
	MaxIdleTime     time.Duration // Maximum time a connection can be idle
	HealthCheckInterval time.Duration // Interval for health checks
	WaitTimeout     time.Duration // Maximum time to wait for a connection from the pool
	BlockWhenExhausted bool       // Whether to block when pool is exhausted
}

// DefaultConnectionPoolConfig returns a default configuration
func DefaultConnectionPoolConfig(uri URI) ConnectionPoolConfig {
	return ConnectionPoolConfig{
		URI:                 uri,
		MaxSize:             10,
		InitialSize:         2,
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
	useCount   int64
}

// ConnectionPool manages a pool of Infinity connections
type ConnectionPool struct {
	uri         URI
	config      ConnectionPoolConfig
	connections chan *PooledConnection
	allConns    map[*PooledConnection]bool // Track all created connections for cleanup
	mu          sync.RWMutex
	factory     func(URI) (*InfinityConnection, error)
	closed      int32 // Atomic flag for closed state
	wg          sync.WaitGroup
	ctx         context.Context
	cancel      context.CancelFunc
}

// NewConnectionPool creates a new connection pool
func NewConnectionPool(config ConnectionPoolConfig, factory func(URI) (*InfinityConnection, error)) (*ConnectionPool, error) {
	if config.MaxSize <= 0 {
		config.MaxSize = 10
	}
	if config.InitialSize < 0 {
		config.InitialSize = 0
	}
	if config.InitialSize > config.MaxSize {
		config.InitialSize = config.MaxSize
	}
	if config.MaxIdleTime <= 0 {
		config.MaxIdleTime = 30 * time.Minute
	}
	if config.WaitTimeout <= 0 {
		config.WaitTimeout = 30 * time.Second
	}
	if factory == nil {
		factory = Connect
	}

	ctx, cancel := context.WithCancel(context.Background())

	pool := &ConnectionPool{
		uri:         config.URI,
		config:      config,
		connections: make(chan *PooledConnection, config.MaxSize),
		allConns:    make(map[*PooledConnection]bool),
		factory:     factory,
		ctx:         ctx,
		cancel:      cancel,
	}

	// Create initial connections
	for i := 0; i < config.InitialSize; i++ {
		pooledConn, err := pool.createConnection()
		if err != nil {
			pool.Close()
			return nil, fmt.Errorf("failed to create initial connection %d: %w", i, err)
		}
		pool.connections <- pooledConn
	}

	// Start background maintenance goroutine
	if config.HealthCheckInterval > 0 {
		pool.wg.Add(1)
		go pool.maintenance()
	}

	return pool, nil
}

// createConnection creates a new pooled connection
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
		useCount:   0,
	}

	p.mu.Lock()
	p.allConns[pooledConn] = true
	p.mu.Unlock()

	return pooledConn, nil
}

// Get gets a connection from the pool
func (p *ConnectionPool) Get() (*InfinityConnection, error) {
	return p.GetContext(context.Background())
}

// GetContext gets a connection from the pool with context
func (p *ConnectionPool) GetContext(ctx context.Context) (*InfinityConnection, error) {
	if p.isClosed() {
		return nil, NewInfinityException(int(ErrorCodeClientClose), "Connection pool is closed")
	}

	// Create timeout context if BlockWhenExhausted is true
	if p.config.BlockWhenExhausted && p.config.WaitTimeout > 0 {
		var cancel context.CancelFunc
		ctx, cancel = context.WithTimeout(ctx, p.config.WaitTimeout)
		defer cancel()
	}

	for {
		select {
		case pooledConn := <-p.connections:
			// Check if connection is still valid
			if !pooledConn.conn.IsConnected() {
				// Connection is dead, close it and try again
				p.closeConnection(pooledConn)
				continue
			}

			// Check if connection has been idle too long
			if time.Since(pooledConn.lastUsedAt) > p.config.MaxIdleTime {
				// Connection is too old, close it and try again
				p.closeConnection(pooledConn)
				continue
			}

			// Update metadata
			pooledConn.lastUsedAt = time.Now()
			atomic.AddInt64(&pooledConn.useCount, 1)

			return pooledConn.conn, nil

		case <-ctx.Done():
			if ctx.Err() == context.DeadlineExceeded {
				return nil, NewInfinityException(
					int(ErrorCodeTimeout),
					fmt.Sprintf("Timeout waiting for connection from pool after %v", p.config.WaitTimeout),
				)
			}
			return nil, ctx.Err()

		default:
			// No connection available
			if !p.config.BlockWhenExhausted {
				// Try to create a new connection if not at max
				p.mu.RLock()
				currentSize := len(p.allConns)
				p.mu.RUnlock()

				if currentSize < p.config.MaxSize {
					pooledConn, err := p.createConnection()
					if err != nil {
						return nil, err
					}
					pooledConn.lastUsedAt = time.Now()
					atomic.AddInt64(&pooledConn.useCount, 1)
					return pooledConn.conn, nil
				}

				return nil, NewInfinityException(
					int(ErrorCodeTooManyConnections),
					"Connection pool exhausted",
				)
			}
			// Block and wait for a connection
			select {
			case pooledConn := <-p.connections:
				if !pooledConn.conn.IsConnected() {
					p.closeConnection(pooledConn)
					continue
				}
				pooledConn.lastUsedAt = time.Now()
				atomic.AddInt64(&pooledConn.useCount, 1)
				return pooledConn.conn, nil
			case <-ctx.Done():
				if ctx.Err() == context.DeadlineExceeded {
					return nil, NewInfinityException(
						int(ErrorCodeTimeout),
						fmt.Sprintf("Timeout waiting for connection from pool after %v", p.config.WaitTimeout),
					)
				}
				return nil, ctx.Err()
			}
		}
	}
}

// Put returns a connection to the pool
func (p *ConnectionPool) Put(conn *InfinityConnection) error {
	if p.isClosed() {
		// Pool is closed, close the connection
		conn.Disconnect()
		return NewInfinityException(int(ErrorCodeClientClose), "Connection pool is closed")
	}

	if conn == nil {
		return NewInfinityException(int(ErrorCodeInvalidParameterValue), "Cannot return nil connection to pool")
	}

	// Find the pooled connection wrapper
	p.mu.RLock()
	var pooledConn *PooledConnection
	for pc := range p.allConns {
		if pc.conn == conn {
			pooledConn = pc
			break
		}
	}
	p.mu.RUnlock()

	if pooledConn == nil {
		// Connection not from this pool, close it
		conn.Disconnect()
		return NewInfinityException(int(ErrorCodeInvalidParameterValue), "Connection not from this pool")
	}

	// Check if connection is still valid
	if !conn.IsConnected() {
		// Connection is dead, remove it from tracking
		p.closeConnection(pooledConn)
		return NewInfinityException(int(ErrorCodeClientClose), "Connection is dead, removed from pool")
	}

	// Return to pool
	select {
	case p.connections <- pooledConn:
		// Successfully returned to pool
		return nil
	default:
		// Pool is full, close the connection
		p.closeConnection(pooledConn)
		return NewInfinityException(int(ErrorCodeTooManyConnections), "Pool is full, connection closed")
	}
}

// closeConnection closes a pooled connection and removes it from tracking
func (p *ConnectionPool) closeConnection(pooledConn *PooledConnection) {
	p.mu.Lock()
	delete(p.allConns, pooledConn)
	p.mu.Unlock()

	if pooledConn.conn != nil {
		pooledConn.conn.Disconnect()
	}
}

// maintenance runs background maintenance tasks
func (p *ConnectionPool) maintenance() {
	defer p.wg.Done()

	ticker := time.NewTicker(p.config.HealthCheckInterval)
	defer ticker.Stop()

	for {
		select {
		case <-p.ctx.Done():
			return
		case <-ticker.C:
			p.cleanupIdleConnections()
		}
	}
}

// cleanupIdleConnections removes connections that have been idle too long
func (p *ConnectionPool) cleanupIdleConnections() {
	p.mu.Lock()
	defer p.mu.Unlock()

	now := time.Now()
	var toRemove []*PooledConnection

	// Check all connections (this is a simplified check - in production,
	// you might want to check only idle connections)
	for pooledConn := range p.allConns {
		if now.Sub(pooledConn.lastUsedAt) > p.config.MaxIdleTime {
			toRemove = append(toRemove, pooledConn)
		}
	}

	// Remove idle connections
	for _, pooledConn := range toRemove {
		delete(p.allConns, pooledConn)
		go pooledConn.conn.Disconnect()
	}
}

// isClosed checks if the pool is closed
func (p *ConnectionPool) isClosed() bool {
	return atomic.LoadInt32(&p.closed) == 1
}

// Close closes all connections in the pool
func (p *ConnectionPool) Close() error {
	if !atomic.CompareAndSwapInt32(&p.closed, 0, 1) {
		return NewInfinityException(int(ErrorCodeClientClose), "Connection pool already closed")
	}

	// Cancel context to stop maintenance goroutine
	p.cancel()

	// Wait for maintenance goroutine to finish
	p.wg.Wait()

	p.mu.Lock()
	defer p.mu.Unlock()

	// Close all connections
	for pooledConn := range p.allConns {
		if pooledConn.conn != nil {
			pooledConn.conn.Disconnect()
		}
		delete(p.allConns, pooledConn)
	}

	// Drain the channel
	close(p.connections)
	for pooledConn := range p.connections {
		if pooledConn.conn != nil {
			pooledConn.conn.Disconnect()
		}
	}

	return nil
}

// Stats returns statistics about the pool
type PoolStats struct {
	TotalConnections     int
	AvailableConnections int
	InUseConnections     int
	MaxConnections       int
	Closed               bool
}

// Stats returns current pool statistics
func (p *ConnectionPool) Stats() PoolStats {
	p.mu.RLock()
	total := len(p.allConns)
	p.mu.RUnlock()

	available := len(p.connections)

	return PoolStats{
		TotalConnections:     total,
		AvailableConnections: available,
		InUseConnections:     total - available,
		MaxConnections:       p.config.MaxSize,
		Closed:               p.isClosed(),
	}
}

// Size returns the current number of connections in the pool
func (p *ConnectionPool) Size() int {
	p.mu.RLock()
	defer p.mu.RUnlock()
	return len(p.allConns)
}

// Available returns the number of available connections in the pool
func (p *ConnectionPool) Available() int {
	return len(p.connections)
}
