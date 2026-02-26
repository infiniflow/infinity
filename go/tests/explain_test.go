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
	"testing"

	"github.com/infiniflow/infinity-go-sdk"
)

func TestExplain(t *testing.T) {
	tableName := "test_explain_default"

	conn := setupConnection(t)
	defer closeConnection(t, conn)

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		t.Fatalf("Failed to get database: %v", err)
	}

	// Clean up
	db.DropTable(tableName, infinity.ConflictTypeIgnore)

	schema := infinity.TableSchema{
		{
			Name:        "c1",
			DataType:    "varchar",
			Constraints: []infinity.ColumnConstraint{infinity.ConstraintPrimaryKey},
		},
		{
			Name:     "c2",
			DataType: "float",
		},
	}
	table, err := db.CreateTable(tableName, schema, infinity.ConflictTypeError)
	if err != nil {
		t.Fatalf("Failed to create table: %v", err)
	}
	if table == nil {
		t.Fatal("Table is nil")
	}
	defer db.DropTable(tableName, infinity.ConflictTypeError)

	// Insert data
	_, err = table.Insert(map[string]interface{}{
		"c1": "hello",
		"c2": 1.0,
	})
	if err != nil {
		t.Fatalf("Failed to insert row 1: %v", err)
	}
	_, err = table.Insert(map[string]interface{}{
		"c1": "world",
		"c2": 2.0,
	})
	if err != nil {
		t.Fatalf("Failed to insert row 2: %v", err)
	}
	_, err = table.Insert(map[string]interface{}{
		"c1": "hello",
		"c2": 3.0,
	})
	if err != nil {
		t.Fatalf("Failed to insert row 3: %v", err)
	}

	// Explain with default type (Physical)
	res, err := table.Output([]string{"*"}).Explain(infinity.ExplainTypePhysical)
	if err != nil {
		t.Errorf("Failed to explain with default type: %v", err)
	}
	if res == nil {
		t.Error("Explain result is nil")
	}

	// Explain with Ast
	res, err = table.Output([]string{"*"}).Explain(infinity.ExplainTypeAst)
	if err != nil {
		t.Errorf("Failed to explain with Ast: %v", err)
	}
	if res == nil {
		t.Error("Explain result is nil")
	}

	// Explain with UnOpt
	res, err = table.Output([]string{"*"}).Explain(infinity.ExplainTypeUnOpt)
	if err != nil {
		t.Errorf("Failed to explain with UnOpt: %v", err)
	}
	if res == nil {
		t.Error("Explain result is nil")
	}

	// Explain with Opt
	res, err = table.Output([]string{"*"}).Explain(infinity.ExplainTypeOpt)
	if err != nil {
		t.Errorf("Failed to explain with Opt: %v", err)
	}
	if res == nil {
		t.Error("Explain result is nil")
	}

	// Explain with Physical
	res, err = table.Output([]string{"*"}).Explain(infinity.ExplainTypePhysical)
	if err != nil {
		t.Errorf("Failed to explain with Physical: %v", err)
	}
	if res == nil {
		t.Error("Explain result is nil")
	}

	// Explain with Pipeline
	res, err = table.Output([]string{"*"}).Explain(infinity.ExplainTypePipeline)
	if err != nil {
		t.Errorf("Failed to explain with Pipeline: %v", err)
	}
	if res == nil {
		t.Error("Explain result is nil")
	}

	// Explain with Fragment
	res, err = table.Output([]string{"*"}).Explain(infinity.ExplainTypeFragment)
	if err != nil {
		t.Errorf("Failed to explain with Fragment: %v", err)
	}
	if res == nil {
		t.Error("Explain result is nil")
	}

	// Explain with Analyze
	res, err = table.Output([]string{"*"}).Explain(infinity.ExplainTypeAnalyze)
	if err != nil {
		t.Errorf("Failed to explain with Analyze: %v", err)
	}
	if res == nil {
		t.Error("Explain result is nil")
	}
}
