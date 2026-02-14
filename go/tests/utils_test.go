package tests

import (
	"testing"
)

// parseStringVector parses string vector from bytes
// It first tries length-prefixed format (4-byte little-endian length + string bytes),
// then falls back to null terminator separated format for backward compatibility
func parseStringVector(data []byte) []string {
	result := make([]string, 0)
	offset := 0

	// First, try length-prefixed parsing
	for offset < len(data) {
		if offset+4 > len(data) {
			break
		}

		// Read 4-byte length prefix (little-endian)
		length := int(data[offset]) | int(data[offset+1])<<8 |
			int(data[offset+2])<<16 | int(data[offset+3])<<24
		offset += 4

		if offset+length > len(data) {
			// Invalid length, fallback to null terminator separated format
			return parseStringVectorFallback(data)
		}

		// Extract string
		strBytes := data[offset : offset+length]
		result = append(result, string(strBytes))
		offset += length
	}

	// If no strings parsed and data is not empty, fallback to null terminator format
	if len(result) == 0 && len(data) > 0 {
		return parseStringVectorFallback(data)
	}

	return result
}

// parseStringVectorFallback parses string vector using null terminator separator
// This is for backward compatibility
func parseStringVectorFallback(data []byte) []string {
	result := make([]string, 0)
	start := 0
	for i, b := range data {
		if b == 0 {
			if i > start {
				result = append(result, string(data[start:i]))
			}
			start = i + 1
		}
	}
	if start < len(data) {
		result = append(result, string(data[start:]))
	}
	return result
}

func TestParseStringVector(t *testing.T) {
	// Test length-prefixed encoding (Thrift format)
	t.Run("LengthPrefixedFormat", func(t *testing.T) {
		testData := make([]byte, 0)

		// Add "num" (length 3)
		testData = append(testData, 3, 0, 0, 0)
		testData = append(testData, []byte("num")...)

		// Add "body" (length 4)
		testData = append(testData, 4, 0, 0, 0)
		testData = append(testData, []byte("body")...)

		// Add "vec" (length 3)
		testData = append(testData, 3, 0, 0, 0)
		testData = append(testData, []byte("vec")...)

		result := parseStringVector(testData)
		expected := []string{"num", "body", "vec"}

		if len(result) != len(expected) {
			t.Errorf("Expected %d strings, got %d", len(expected), len(result))
		}

		for i, str := range expected {
			if result[i] != str {
				t.Errorf("Expected %s at index %d, got %s", str, i, result[i])
			}
		}
	})

	// Test null terminator separated format (backward compatibility)
	t.Run("NullTerminatorFormat", func(t *testing.T) {
		testData := []byte("hello\x00world\x00test\x00")
		result := parseStringVector(testData)
		expected := []string{"hello", "world", "test"}

		if len(result) != len(expected) {
			t.Errorf("Expected %d strings, got %d", len(expected), len(result))
		}

		for i, str := range expected {
			if result[i] != str {
				t.Errorf("Expected %s at index %d, got %s", str, i, result[i])
			}
		}
	})

	// Test empty data
	t.Run("EmptyData", func(t *testing.T) {
		testData := []byte{}
		result := parseStringVector(testData)

		if len(result) != 0 {
			t.Errorf("Expected empty result, got %d strings", len(result))
		}
	})

	// Test single string with length-prefixed format
	t.Run("SingleStringLengthPrefixed", func(t *testing.T) {
		testData := make([]byte, 0)
		testData = append(testData, 5, 0, 0, 0)
		testData = append(testData, []byte("hello")...)

		result := parseStringVector(testData)
		expected := []string{"hello"}

		if len(result) != 1 || result[0] != expected[0] {
			t.Errorf("Expected [hello], got %v", result)
		}
	})

	// Test single string without null terminator (fallback)
	t.Run("SingleStringNoTerminator", func(t *testing.T) {
		testData := []byte("hello")
		result := parseStringVector(testData)
		expected := []string{"hello"}

		if len(result) != 1 || result[0] != expected[0] {
			t.Errorf("Expected [hello], got %v", result)
		}
	})

	// Test strings with spaces (length-prefixed format)
	t.Run("StringsWithSpaces", func(t *testing.T) {
		testData := make([]byte, 0)
		testData = append(testData, 11, 0, 0, 0)
		testData = append(testData, []byte("hello world")...)

		result := parseStringVector(testData)
		expected := []string{"hello world"}

		if len(result) != 1 || result[0] != expected[0] {
			t.Errorf("Expected [hello world], got %v", result)
		}
	})

	// Test Unicode strings (length-prefixed format)
	t.Run("UnicodeStrings", func(t *testing.T) {
		testData := make([]byte, 0)
		unicodeStr := "Hello 世界 🌍"
		testData = append(testData, byte(len(unicodeStr)), 0, 0, 0)
		testData = append(testData, []byte(unicodeStr)...)

		result := parseStringVector(testData)
		expected := []string{unicodeStr}

		if len(result) != 1 || result[0] != expected[0] {
			t.Errorf("Expected [%s], got %v", unicodeStr, result)
		}
	})
}
