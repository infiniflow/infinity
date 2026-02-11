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

// This example demonstrates how to connect to and disconnect from Infinity server.
// It requires a running Infinity server on localhost:23817 (default port).

package main

import (
	"flag"
	"fmt"
	"log"
	"os"

	infinity "github.com/infiniflow/infinity-go-sdk"
)

func main() {
	// Parse command line arguments
	var (
		host = flag.String("host", "127.0.0.1", "Infinity server host")
		port = flag.Int("port", 23817, "Infinity server port")
	)
	flag.Parse()

	fmt.Println("=== Infinity Connect/Disconnect Example ===")
	fmt.Printf("Connecting to Infinity at %s:%d\n", *host, *port)

	// Example 1: Basic connect and disconnect
	fmt.Println("\n--- Example 1: Basic Connect and Disconnect ---")

	// Create a connection
	address := infinity.NetworkAddress{IP: *host, Port: *port}
	conn, err := infinity.Connect(address)
	if err != nil {
		log.Fatalf("Failed to connect: %v", err)
	}
	fmt.Println("✓ Connected to Infinity successfully")

	// Access connection properties directly since InfinityConnection is now a struct
	fmt.Printf("  Session ID: %d\n", conn.GetSessionID())
	fmt.Printf("  Is Connected: %v\n", conn.IsConnected())

	// Disconnect
	_, err = conn.Disconnect()
	if err != nil {
		log.Printf("Error during disconnect: %v", err)
	} else {
		fmt.Println("✓ Disconnected from Infinity successfully")
	}

	// Example 2: Using defer for automatic disconnect
	fmt.Println("\n--- Example 2: Using defer for Cleanup ---")

	func() {
		conn2, err := infinity.Connect(address)
		if err != nil {
			log.Fatalf("Failed to connect: %v", err)
		}
		fmt.Println("✓ Connected to Infinity")

		// Use defer to ensure disconnect happens even if panic occurs
		defer func() {
			_, err := conn2.Disconnect()
			if err != nil {
				log.Printf("Error during disconnect: %v", err)
			} else {
				fmt.Println("✓ Automatically disconnected via defer")
			}
		}()

		// Do some work here...
		fmt.Println("  (Doing some work...)")
	}()

	// Example 3: Multiple connections
	fmt.Println("\n--- Example 3: Multiple Connections ---")

	conn3a, err := infinity.Connect(address)
	if err != nil {
		log.Fatalf("Failed to create first connection: %v", err)
	}
	fmt.Println("✓ Created first connection")

	conn3b, err := infinity.Connect(address)
	if err != nil {
		log.Fatalf("Failed to create second connection: %v", err)
	}
	fmt.Println("✓ Created second connection")

	// Both connections have different session IDs
	fmt.Printf("  Connection 1 Session ID: %d\n", conn3a.GetSessionID())
	fmt.Printf("  Connection 2 Session ID: %d\n", conn3b.GetSessionID())

	// Disconnect both
	conn3a.Disconnect()
	conn3b.Disconnect()
	fmt.Println("✓ Disconnected both connections")

	// Example 4: Error handling - invalid host
	fmt.Println("\n--- Example 4: Error Handling - Invalid Host ---")

	invalidAddress := infinity.NetworkAddress{IP: "invalid_host_that_does_not_exist", Port: 12345}
	_, err = infinity.Connect(invalidAddress)
	if err != nil {
		fmt.Printf("✓ Expected error occurred: %v\n", err)
	} else {
		fmt.Println("✗ Unexpectedly succeeded")
	}

	// Example 5: Error handling - double disconnect
	fmt.Println("\n--- Example 5: Error Handling - Double Disconnect ---")

	conn5, err := infinity.Connect(address)
	if err != nil {
		log.Fatalf("Failed to connect: %v", err)
	}
	fmt.Println("✓ Connected to Infinity")

	// First disconnect
	_, err = conn5.Disconnect()
	if err != nil {
		log.Printf("Error during first disconnect: %v", err)
	} else {
		fmt.Println("✓ First disconnect succeeded")
	}

	// Second disconnect should fail
	_, err = conn5.Disconnect()
	if err != nil {
		fmt.Printf("✓ Expected error on double disconnect: %v\n", err)
	} else {
		fmt.Println("✗ Double disconnect unexpectedly succeeded")
	}

	// Example 6: Connection to different servers
	fmt.Println("\n--- Example 6: Custom Server Address ---")

	// Using LocalHost constant
	conn6, err := infinity.Connect(infinity.LocalHost)
	if err != nil {
		log.Printf("Failed to connect to localhost: %v", err)
	} else {
		fmt.Println("✓ Connected to LocalHost")
		conn6.Disconnect()
		fmt.Println("✓ Disconnected from LocalHost")
	}

	// Using custom address
	customAddress := infinity.NetworkAddress{IP: "192.168.1.100", Port: 23817}
	fmt.Printf("  Custom address example: %s\n", customAddress.String())

	fmt.Println("\n=== All Examples Completed ===")

	// Exit successfully
	os.Exit(0)
}
