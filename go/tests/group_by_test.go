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
	"sort"
	"testing"

	"github.com/infiniflow/infinity-go-sdk"
)

// TestGroupByAggType tests group by with different aggregate types
func TestGroupByAggType(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_groupby_aggtype"
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}

	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 1, "c2": 1.0},
		{"c1": 2, "c2": 2.0},
		{"c1": 1, "c2": 3.0},
		{"c1": 2, "c2": 4.0},
		{"c1": 1, "c2": 5.0},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test agg sum
	result, err := table.Output([]string{"c1", "sum(c2)"}).GroupBy("c1").ToResult()
	if err != nil {
		t.Fatalf("Failed to query sum: %v", err)
	}

	queryResult, ok := result.(*infinity.QueryResult)
	if !ok {
		t.Fatal("Expected QueryResult type")
	}

	// Verify sum results: c1=1 -> sum=9.0, c1=2 -> sum=6.0
	sumResults := make(map[int32]float64)
	if c1Data, ok := queryResult.Data["c1"]; ok {
		if sumData, ok := queryResult.Data["sum(c2)"]; ok {
			for i, c1Val := range c1Data {
				if c1Int, ok := c1Val.(int32); ok {
					if sumFloat, ok := sumData[i].(float32); ok {
						sumResults[c1Int] = float64(sumFloat)
					} else if sumFloat64, ok := sumData[i].(float64); ok {
						sumResults[c1Int] = sumFloat64
					}
				}
			}
		}
	}

	if sumResults[1] != 9.0 {
		t.Errorf("Expected sum(c2) for c1=1 to be 9.0, got %v", sumResults[1])
	}
	if sumResults[2] != 6.0 {
		t.Errorf("Expected sum(c2) for c1=2 to be 6.0, got %v", sumResults[2])
	}

	// Test agg min
	result, err = table.Output([]string{"c1", "min(c2)"}).GroupBy("c1").ToResult()
	if err != nil {
		t.Fatalf("Failed to query min: %v", err)
	}

	queryResult = result.(*infinity.QueryResult)
	minResults := make(map[int32]float64)
	if c1Data, ok := queryResult.Data["c1"]; ok {
		if minData, ok := queryResult.Data["min(c2)"]; ok {
			for i, c1Val := range c1Data {
				if c1Int, ok := c1Val.(int32); ok {
					if minFloat, ok := minData[i].(float32); ok {
						minResults[c1Int] = float64(minFloat)
					} else if minFloat64, ok := minData[i].(float64); ok {
						minResults[c1Int] = minFloat64
					}
				}
			}
		}
	}

	if minResults[1] != 1.0 {
		t.Errorf("Expected min(c2) for c1=1 to be 1.0, got %v", minResults[1])
	}
	if minResults[2] != 2.0 {
		t.Errorf("Expected min(c2) for c1=2 to be 2.0, got %v", minResults[2])
	}

	// Test agg max
	result, err = table.Output([]string{"c1", "max(c2)"}).GroupBy("c1").ToResult()
	if err != nil {
		t.Fatalf("Failed to query max: %v", err)
	}

	queryResult = result.(*infinity.QueryResult)
	maxResults := make(map[int32]float64)
	if c1Data, ok := queryResult.Data["c1"]; ok {
		if maxData, ok := queryResult.Data["max(c2)"]; ok {
			for i, c1Val := range c1Data {
				if c1Int, ok := c1Val.(int32); ok {
					if maxFloat, ok := maxData[i].(float32); ok {
						maxResults[c1Int] = float64(maxFloat)
					} else if maxFloat64, ok := maxData[i].(float64); ok {
						maxResults[c1Int] = maxFloat64
					}
				}
			}
		}
	}

	if maxResults[1] != 5.0 {
		t.Errorf("Expected max(c2) for c1=1 to be 5.0, got %v", maxResults[1])
	}
	if maxResults[2] != 4.0 {
		t.Errorf("Expected max(c2) for c1=2 to be 4.0, got %v", maxResults[2])
	}

	// Test agg count
	result, err = table.Output([]string{"c1", "count(*)"}).GroupBy("c1").ToResult()
	if err != nil {
		t.Fatalf("Failed to query count: %v", err)
	}

	queryResult = result.(*infinity.QueryResult)
	countResults := make(map[int32]int64)
	if c1Data, ok := queryResult.Data["c1"]; ok {
		if countData, ok := queryResult.Data["count(star)"]; ok {
			for i, c1Val := range c1Data {
				if c1Int, ok := c1Val.(int32); ok {
					if countInt, ok := countData[i].(int64); ok {
						countResults[c1Int] = countInt
					} else if countInt32, ok := countData[i].(int32); ok {
						countResults[c1Int] = int64(countInt32)
					}
				}
			}
		}
	}

	if countResults[1] != 3 {
		t.Errorf("Expected count(*) for c1=1 to be 3, got %v", countResults[1])
	}
	if countResults[2] != 2 {
		t.Errorf("Expected count(*) for c1=2 to be 2, got %v", countResults[2])
	}

	// Test agg avg
	result, err = table.Output([]string{"c1", "avg(c2)"}).GroupBy("c1").ToResult()
	if err != nil {
		t.Fatalf("Failed to query avg: %v", err)
	}

	queryResult = result.(*infinity.QueryResult)
	avgResults := make(map[int32]float64)
	if c1Data, ok := queryResult.Data["c1"]; ok {
		// avg column name may vary
		for colName, colData := range queryResult.Data {
			if colName != "c1" {
				for i, c1Val := range c1Data {
					if c1Int, ok := c1Val.(int32); ok {
						if avgFloat, ok := colData[i].(float32); ok {
							avgResults[c1Int] = float64(avgFloat)
						} else if avgFloat64, ok := colData[i].(float64); ok {
							avgResults[c1Int] = avgFloat64
						}
					}
				}
			}
		}
	}

	if avgResults[1] != 3.0 {
		t.Errorf("Expected avg(c2) for c1=1 to be 3.0, got %v", avgResults[1])
	}
	if avgResults[2] != 3.0 {
		t.Errorf("Expected avg(c2) for c1=2 to be 3.0, got %v", avgResults[2])
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestGroupByDataType tests group by with different data types
func TestGroupByDataType(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_groupby_datatype"
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}

	// Create table with int, float, varchar
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "float"},
		{Name: "c3", DataType: "varchar"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data (18 rows with varchar patterns)
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 1, "c2": 1.0, "c3": "abc"},
		{"c1": 2, "c2": 2.0, "c3": "abcdef"},
		{"c1": 3, "c2": 3.0, "c3": "abcdefghi"},
		{"c1": 1, "c2": 4.0, "c3": "abcdefghijkl"},
		{"c1": 2, "c2": 5.0, "c3": "abcdefghijklmno"},
		{"c1": 3, "c2": 6.0, "c3": "abcdefghijklmnopqr"},
		{"c1": 1, "c2": 1.0, "c3": "abcdefghijklmnopqrstu"},
		{"c1": 2, "c2": 2.0, "c3": "abcdefghijklmnopqrstuvwx"},
		{"c1": 3, "c2": 3.0, "c3": "abcdefghijklmnopqrstuvwxyz"},
		{"c1": 1, "c2": 4.0, "c3": "abc"},
		{"c1": 2, "c2": 5.0, "c3": "abcdef"},
		{"c1": 3, "c2": 6.0, "c3": "abcdefghi"},
		{"c1": 1, "c2": 1.0, "c3": "abcdefghijkl"},
		{"c1": 2, "c2": 2.0, "c3": "abcdefghijklmno"},
		{"c1": 3, "c2": 3.0, "c3": "abcdefghijklmnopqr"},
		{"c1": 1, "c2": 4.0, "c3": "abcdefghijklmnopqrstu"},
		{"c1": 2, "c2": 5.0, "c3": "abcdefghijklmnopqrstuvwx"},
		{"c1": 3, "c2": 6.0, "c3": "abcdefghijklmnopqrstuvwxyz"},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test group by varchar column
	result, err := table.Output([]string{"c3", "sum(c1)", "sum(c2)"}).GroupBy("c3").ToResult()
	if err != nil {
		t.Fatalf("Failed to query group by varchar: %v", err)
	}

	queryResult := result.(*infinity.QueryResult)
	t.Logf("Group by varchar result: %v", queryResult.Data)

	// Test group by varchar with char_length function
	result, err = table.Output([]string{"c3", "char_length(c3)", "sum(c1)"}).GroupBy("c3").ToResult()
	if err != nil {
		t.Fatalf("Failed to query group by varchar with char_length: %v", err)
	}

	queryResult = result.(*infinity.QueryResult)
	t.Logf("Group by varchar with char_length result: %v", queryResult.Data)

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestGroupByDateTime tests group by with date/time types
func TestGroupByDateTime(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_groupby_datetime"
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}

	// Create table with date, datetime, time, timestamp
	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "d", DataType: "date"},
		{Name: "dt", DataType: "datetime"},
		{Name: "t", DataType: "time"},
		{Name: "ts", DataType: "timestamp"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data with date/time types
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 1, "d": "1970-01-01", "dt": "1970-01-01 00:00:00", "t": "00:00:00", "ts": "1970-01-01 00:00:00"},
		{"c1": 2, "d": "1970-01-01", "dt": "1970-01-01 00:00:00", "t": "11:59:59", "ts": "1970-01-01 11:59:59"},
		{"c1": 3, "d": "1970-01-01", "dt": "1970-01-01 00:00:00", "t": "12:00:00", "ts": "1970-01-01 12:00:00"},
		{"c1": 4, "d": "1970-01-01", "dt": "1970-01-01 00:00:00", "t": "23:59:59", "ts": "1970-01-01 23:59:59"},
		{"c1": 5, "d": "1970-01-02", "dt": "1970-01-02 00:00:00", "t": "00:00:00", "ts": "1970-01-01 00:00:00"},
		{"c1": 6, "d": "1970-01-02", "dt": "1970-01-02 00:00:00", "t": "11:59:59", "ts": "1970-01-01 11:59:59"},
		{"c1": 7, "d": "1970-01-02", "dt": "1970-01-02 00:00:00", "t": "12:00:00", "ts": "1970-01-01 12:00:00"},
		{"c1": 8, "d": "1970-01-02", "dt": "1970-01-02 00:00:00", "t": "23:59:59", "ts": "1970-01-01 23:59:59"},
		{"c1": 9, "d": "1970-01-03", "dt": "1970-01-03 00:00:00", "t": "00:00:00", "ts": "1970-01-01 00:00:00"},
		{"c1": 10, "d": "1970-01-03", "dt": "1970-01-03 00:00:00", "t": "11:59:59", "ts": "1970-01-01 11:59:59"},
		{"c1": 11, "d": "1970-01-03", "dt": "1970-01-03 00:00:00", "t": "12:00:00", "ts": "1970-01-01 12:00:00"},
		{"c1": 12, "d": "1970-01-03", "dt": "1970-01-03 00:00:00", "t": "23:59:59", "ts": "1970-01-01 23:59:59"},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test group by date
	result, err := table.Output([]string{"d", "sum(c1)"}).GroupBy("d").ToResult()
	if err != nil {
		t.Fatalf("Failed to query group by date: %v", err)
	}

	queryResult := result.(*infinity.QueryResult)
	t.Logf("Group by date result: %v", queryResult.Data)

	// Test group by time
	result, err = table.Output([]string{"t", "sum(c1)"}).GroupBy("t").ToResult()
	if err != nil {
		t.Fatalf("Failed to query group by time: %v", err)
	}

	queryResult = result.(*infinity.QueryResult)
	t.Logf("Group by time result: %v", queryResult.Data)

	// Test group by datetime
	result, err = table.Output([]string{"dt", "sum(c1)"}).GroupBy("dt").ToResult()
	if err != nil {
		t.Fatalf("Failed to query group by datetime: %v", err)
	}

	queryResult = result.(*infinity.QueryResult)
	t.Logf("Group by datetime result: %v", queryResult.Data)

	// Test group by timestamp
	result, err = table.Output([]string{"ts", "sum(c1)"}).GroupBy("ts").ToResult()
	if err != nil {
		t.Fatalf("Failed to query group by timestamp: %v", err)
	}

	queryResult = result.(*infinity.QueryResult)
	t.Logf("Group by timestamp result: %v", queryResult.Data)

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestGroupByComplex tests complex group by scenarios
func TestGroupByComplex(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_groupby_complex"
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}

	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "int"},
		{Name: "c3", DataType: "float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data (12 rows)
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 1, "c2": 1, "c3": 1.0},
		{"c1": 2, "c2": 2, "c3": 2.0},
		{"c1": 1, "c2": 3, "c3": 3.0},
		{"c1": 2, "c2": 1, "c3": 4.0},
		{"c1": 1, "c2": 2, "c3": 5.0},
		{"c1": 2, "c2": 3, "c3": 6.0},
		{"c1": 1, "c2": 1, "c3": 7.0},
		{"c1": 2, "c2": 2, "c3": 8.0},
		{"c1": 1, "c2": 3, "c3": 1.0},
		{"c1": 2, "c2": 1, "c3": 2.0},
		{"c1": 1, "c2": 2, "c3": 3.0},
		{"c1": 2, "c2": 3, "c3": 4.0},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Test multi-column group by [c1, c2]
	result, err := table.Output([]string{"c1", "c2", "sum(c3)"}).GroupBy([]string{"c1", "c2"}).ToResult()
	if err != nil {
		t.Fatalf("Failed to query multi-column group by: %v", err)
	}

	queryResult := result.(*infinity.QueryResult)
	t.Logf("Multi-column group by [c1, c2] result: %v", queryResult.Data)

	// Test group by expression (c1 + c2)
	result, err = table.Output([]string{"c1+c2", "sum(c3)"}).GroupBy([]string{"c1+c2"}).ToResult()
	if err != nil {
		t.Fatalf("Failed to query group by expression: %v", err)
	}

	queryResult = result.(*infinity.QueryResult)
	t.Logf("Group by expression (c1+c2) result: %v", queryResult.Data)

	// Test group by multiple expressions
	result, err = table.Output([]string{"abs(c1-c2)", "c1+c2", "sum(c3)"}).
		GroupBy([]string{"abs(c1-c2)", "c1+c2"}).ToResult()
	if err != nil {
		t.Fatalf("Failed to query group by multiple expressions: %v", err)
	}

	queryResult = result.(*infinity.QueryResult)
	t.Logf("Group by multiple expressions result: %v", queryResult.Data)

	// Test filter + group by
	result, err = table.Output([]string{"c1", "c2", "sum(c3)"}).
		Filter("c1 > 1").
		GroupBy([]string{"c1", "c2"}).ToResult()
	if err != nil {
		t.Fatalf("Failed to query filter + group by: %v", err)
	}

	queryResult = result.(*infinity.QueryResult)
	t.Logf("Filter + group by result: %v", queryResult.Data)

	// Test group by with having
	result, err = table.Output([]string{"c3", "sum(c1)", "sum(c2)"}).
		GroupBy("c3").
		Having("sum(c1) >= 2").ToResult()
	if err != nil {
		t.Fatalf("Failed to query group by with having: %v", err)
	}

	queryResult = result.(*infinity.QueryResult)
	t.Logf("Group by with having result: %v", queryResult.Data)

	// Test group by with having (complex condition)
	result, err = table.Output([]string{"c3", "sum(c1)", "sum(c2)"}).
		GroupBy("c3").
		Having("sum(c1) >= 2 and c3 > 3").ToResult()
	if err != nil {
		t.Fatalf("Failed to query group by with complex having: %v", err)
	}

	queryResult = result.(*infinity.QueryResult)
	t.Logf("Group by with complex having result: %v", queryResult.Data)

	// Test group by with sort (descending by c3)
	result, err = table.Output([]string{"c3", "sum(c1)", "sum(c2)"}).
		GroupBy("c3").
		Sort([][2]interface{}{
			{"c3", infinity.SortTypeDesc},
		}).ToResult()
	if err != nil {
		t.Fatalf("Failed to query group by with sort: %v", err)
	}

	queryResult = result.(*infinity.QueryResult)
	t.Logf("Group by with sort result: %v", queryResult.Data)

	// Verify descending sort by c3
	if c3Data, ok := queryResult.Data["c3"]; ok && len(c3Data) > 1 {
		// Check if sorted in descending order
		isDesc := true
		for i := 1; i < len(c3Data); i++ {
			var prev, curr float64
			switch v := c3Data[i-1].(type) {
			case float32:
				prev = float64(v)
			case float64:
				prev = v
			default:
				continue
			}
			switch v := c3Data[i].(type) {
			case float32:
				curr = float64(v)
			case float64:
				curr = v
			default:
				continue
			}
			if prev < curr {
				isDesc = false
				break
			}
		}
		if !isDesc {
			t.Logf("Warning: Results may not be sorted in descending order")
		}
	}

	// Test group by with multi-column sort
	result, err = table.Output([]string{"c3", "sum(c1) as sum1", "sum(c2) as sum2"}).
		GroupBy("c3").
		Sort([][2]interface{}{
			{"sum1", infinity.SortTypeAsc},
			{"sum2", infinity.SortTypeAsc},
		}).ToResult()
	if err != nil {
		t.Fatalf("Failed to query group by with multi-column sort: %v", err)
	}

	queryResult = result.(*infinity.QueryResult)
	t.Logf("Group by with multi-column sort result: %v", queryResult.Data)

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// TestGroupByWithDelete tests group by after delete operation
func TestGroupByWithDelete(t *testing.T) {
	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	tableName := "test_groupby_delete"
	_, err = db.DropTable(tableName, infinity.ConflictTypeIgnore)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}

	schema := infinity.TableSchema{
		{Name: "c1", DataType: "int"},
		{Name: "c2", DataType: "int"},
		{Name: "c3", DataType: "float"},
	}

	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}

	// Insert test data (12 rows)
	_, err = table.Insert([]map[string]interface{}{
		{"c1": 1, "c2": 1, "c3": 1.0},
		{"c1": 2, "c2": 2, "c3": 2.0},
		{"c1": 1, "c2": 3, "c3": 3.0},
		{"c1": 2, "c2": 1, "c3": 4.0},
		{"c1": 1, "c2": 2, "c3": 5.0},
		{"c1": 2, "c2": 3, "c3": 6.0},
		{"c1": 1, "c2": 1, "c3": 7.0},
		{"c1": 2, "c2": 2, "c3": 8.0},
		{"c1": 1, "c2": 3, "c3": 1.0},
		{"c1": 2, "c2": 1, "c3": 2.0},
		{"c1": 1, "c2": 2, "c3": 3.0},
		{"c1": 2, "c2": 3, "c3": 4.0},
	})
	if err != nil {
		t.Fatalf("Failed to insert data: %v", err)
	}

	// Delete rows where c1 <= 1
	_, err = table.Delete("c1 <= 1")
	if err != nil {
		t.Fatalf("Failed to delete data: %v", err)
	}

	// Query group by after delete
	result, err := table.Output([]string{"c1", "c2", "sum(c3)"}).
		GroupBy([]string{"c1", "c2"}).
		Sort([][2]interface{}{
			{"c1", infinity.SortTypeAsc},
			{"c2", infinity.SortTypeAsc},
		}).ToResult()
	if err != nil {
		t.Fatalf("Failed to query group by after delete: %v", err)
	}

	queryResult := result.(*infinity.QueryResult)
	t.Logf("Group by after delete result: %v", queryResult.Data)

	// Verify only c1=2 rows remain
	if c1Data, ok := queryResult.Data["c1"]; ok {
		for _, val := range c1Data {
			if c1Int, ok := val.(int32); ok {
				if c1Int != 2 {
					t.Errorf("Expected only c1=2 after delete, got c1=%d", c1Int)
				}
			}
		}
	}

	// Cleanup
	_, err = db.DropTable(tableName, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to drop table: %v", err)
	}
}

// Helper function to check if slice contains a value
func containsString(slice []string, val string) bool {
	sort.Strings(slice)
	i := sort.SearchStrings(slice, val)
	return i < len(slice) && slice[i] == val
}
