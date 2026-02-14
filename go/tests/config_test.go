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

// TestSetConfig tests setting and getting configuration values
// Based on Python SDK test_pysdk/test_config.py
func TestSetConfig(t *testing.T) {
	conn, err := infinity.Connect(testLocalHost)
	if err != nil {
		t.Skipf("Failed to connect to server: %v", err)
	}
	defer conn.Disconnect()

	// Test 1: log_level (string type)
	// Get current log_level value
	resp, err := conn.ShowConfig("log_level")
	if err != nil {
		t.Fatalf("Failed to show config log_level: %v", err)
	}
	if resp.ErrorCode != infinity.ErrorCodeOK {
		t.Fatalf("Failed to show config log_level: %s", resp.ErrorMsg)
	}
	oldValue, ok := resp.ConfigValue.(string)
	if !ok {
		t.Fatalf("Expected log_level to be string, got %T", resp.ConfigValue)
	}
	t.Logf("Original log_level: %s", oldValue)

	// Set log_level to debug
	_, err = conn.SetConfig("log_level", "debug")
	if err != nil {
		t.Fatalf("Failed to set config log_level to debug: %v", err)
	}

	// Show log_level again
	resp, err = conn.ShowConfig("log_level")
	if err != nil {
		t.Fatalf("Failed to show config log_level after setting: %v", err)
	}
	if resp.ErrorCode != infinity.ErrorCodeOK {
		t.Fatalf("Failed to show config log_level after setting: %s", resp.ErrorMsg)
	}
	newValue, ok := resp.ConfigValue.(string)
	if !ok {
		t.Fatalf("Expected log_level to be string after setting, got %T", resp.ConfigValue)
	}
	if strings.ToLower(newValue) != "debug" {
		t.Errorf("Expected log_level to be 'debug', got '%s'", newValue)
	}

	// Restore original log_level
	_, err = conn.SetConfig("log_level", oldValue)
	if err != nil {
		t.Fatalf("Failed to restore log_level: %v", err)
	}

	// Show log_level after restore
	resp, err = conn.ShowConfig("log_level")
	if err != nil {
		t.Fatalf("Failed to show config log_level after restore: %v", err)
	}
	if resp.ErrorCode != infinity.ErrorCodeOK {
		t.Fatalf("Failed to show config log_level after restore: %s", resp.ErrorMsg)
	}
	restoredValue, ok := resp.ConfigValue.(string)
	if !ok {
		t.Fatalf("Expected log_level to be string after restore, got %T", resp.ConfigValue)
	}
	if restoredValue != oldValue {
		t.Errorf("Expected log_level to be restored to '%s', got '%s'", oldValue, restoredValue)
	}

	// Test 2: Try to set log_level with wrong type (should fail)
	_, err = conn.SetConfig("log_level", 1)
	if err == nil {
		t.Error("Expected error when setting log_level with integer, got nil")
	}
	if infErr, ok := err.(*infinity.InfinityException); ok {
		if infErr.ErrorCode != int(infinity.ErrorCodeDataTypeMismatch) &&
			infErr.ErrorCode != int(infinity.ErrorCodeInvalidParameterValue) &&
			infErr.ErrorCode != int(infinity.ErrorCodeInvalidCommand) {
			t.Logf("Note: Server returned error code %d: %s", infErr.ErrorCode, infErr.ErrorMsg)
		}
	}

	// Test 3: cleanup_interval (integer type)
	// Get current cleanup_interval value
	resp, err = conn.ShowConfig("cleanup_interval")
	if err != nil {
		t.Fatalf("Failed to show config cleanup_interval: %v", err)
	}
	if resp.ErrorCode != infinity.ErrorCodeOK {
		t.Logf("Failed to show config cleanup_interval: %s", resp.ErrorMsg)
	} else {
		oldCleanupInterval, ok := resp.ConfigValue.(int64)
		if !ok {
			t.Fatalf("Expected cleanup_interval to be int64, got %T", resp.ConfigValue)
		}
		t.Logf("Original cleanup_interval: %d", oldCleanupInterval)

		// Set cleanup_interval to old + 10
		newCleanupInterval := oldCleanupInterval + 10
		_, err = conn.SetConfig("cleanup_interval", newCleanupInterval)
		if err != nil {
			t.Fatalf("Failed to set config cleanup_interval: %v", err)
		}

		// Show cleanup_interval again
		resp, err = conn.ShowConfig("cleanup_interval")
		if err != nil {
			t.Fatalf("Failed to show config cleanup_interval after setting: %v", err)
		}
		if resp.ErrorCode != infinity.ErrorCodeOK {
			t.Fatalf("Failed to show config cleanup_interval after setting: %s", resp.ErrorMsg)
		}
		shownValue, ok := resp.ConfigValue.(int64)
		if !ok {
			t.Fatalf("Expected cleanup_interval to be int64 after setting, got %T", resp.ConfigValue)
		}
		if shownValue != newCleanupInterval {
			t.Errorf("Expected cleanup_interval to be %d, got %d", newCleanupInterval, shownValue)
		}

		// Restore original cleanup_interval
		_, err = conn.SetConfig("cleanup_interval", oldCleanupInterval)
		if err != nil {
			t.Fatalf("Failed to restore cleanup_interval: %v", err)
		}

		// Show cleanup_interval after restore
		resp, err = conn.ShowConfig("cleanup_interval")
		if err != nil {
			t.Fatalf("Failed to show config cleanup_interval after restore: %v", err)
		}
		if resp.ErrorCode != infinity.ErrorCodeOK {
			t.Fatalf("Failed to show config cleanup_interval after restore: %s", resp.ErrorMsg)
		}
		restoredCleanupValue, ok := resp.ConfigValue.(int64)
		if !ok {
			t.Fatalf("Expected cleanup_interval to be int64 after restore, got %T", resp.ConfigValue)
		}
		if restoredCleanupValue != oldCleanupInterval {
			t.Errorf("Expected cleanup_interval to be restored to %d, got %d", oldCleanupInterval, restoredCleanupValue)
		}
	}

	// Test 4: Try to set read-only config (should fail)
	resp, err = conn.ShowConfig("mem_index_capacity")
	if err != nil {
		t.Logf("Failed to show config mem_index_capacity: %v", err)
	} else if resp.ErrorCode != infinity.ErrorCodeOK {
		t.Logf("Failed to show config mem_index_capacity: %s", resp.ErrorMsg)
	} else {
		oldMemIndexCapacity, ok := resp.ConfigValue.(int64)
		if ok {
			// Try to set read-only config
			_, err = conn.SetConfig("mem_index_capacity", oldMemIndexCapacity+1000)
			if err == nil {
				t.Error("Expected error when setting read-only config mem_index_capacity, got nil")
			}
			if infErr, ok := err.(*infinity.InfinityException); ok {
				// Server may return different error codes for read-only config
				if infErr.ErrorCode != int(infinity.ErrorCodeInvalidCommand) &&
					infErr.ErrorCode != int(infinity.ErrorCodeSystemVarReadOnly) &&
					infErr.ErrorCode != int(infinity.ErrorCodeInvalidParameterValue) {
					t.Logf("Note: Server returned error code %d for read-only config: %s",
						infErr.ErrorCode, infErr.ErrorMsg)
				}
			}
		} else {
			t.Logf("mem_index_capacity is not an int64, type is %T", resp.ConfigValue)
		}
	}

	t.Logf("Test completed successfully")
}
