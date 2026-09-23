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

import "fmt"

// IndexType represents the type of index
type IndexType int

const (
	IndexTypeIVF                 IndexType = 1
	IndexTypeHnsw                IndexType = 2
	IndexTypeFullText            IndexType = 3
	IndexTypeSecondary           IndexType = 4
	IndexTypeSecondaryFunctional IndexType = 5
	IndexTypeEMVB                IndexType = 6
	IndexTypeBMP                 IndexType = 7
	IndexTypeDiskAnn             IndexType = 8
	IndexTypePLAID               IndexType = 9
	IndexTypeSMVE                IndexType = 10
)

func (it IndexType) String() string {
	switch it {
	case IndexTypeIVF:
		return "IVF"
	case IndexTypeHnsw:
		return "Hnsw"
	case IndexTypeFullText:
		return "FullText"
	case IndexTypeSecondary:
		return "Secondary"
	case IndexTypeSecondaryFunctional:
		return "SecondaryFunctional"
	case IndexTypeEMVB:
		return "EMVB"
	case IndexTypeBMP:
		return "BMP"
	case IndexTypeDiskAnn:
		return "DiskAnn"
	case IndexTypePLAID:
		return "PLAID"
	case IndexTypeSMVE:
		return "SMVE"
	default:
		return "Unknown"
	}
}

// InitParameter represents an initialization parameter for index
type InitParameter struct {
	ParamName  string
	ParamValue string
}

func (ip InitParameter) String() string {
	return fmt.Sprintf("InitParameter(%s, %s)", ip.ParamName, ip.ParamValue)
}

// IndexInfo represents information about an index
type IndexInfo struct {
	TargetName string
	IndexType  IndexType
	Params     map[string]string
}

func NewIndexInfo(targetName string, indexType IndexType, params map[string]string) *IndexInfo {
	if params == nil {
		params = make(map[string]string)
	}
	return &IndexInfo{
		TargetName: targetName,
		IndexType:  indexType,
		Params:     params,
	}
}

// SparsegramAnalyzer builds the analyzer name of a sparse gram full-text index.
//
// The analyzer emits content-defined n-grams of the whole value instead of
// tokens, which is what lets RegexFilter use the index: the literals a pattern
// proves mandatory are turned into gram lookups before the regular expression
// runs, so the regular expression only verifies candidates. Chinese, Japanese
// and Korean values additionally get one and two character grams, so a single
// character or a two character word can narrow as well.
//
// minGram is the shortest window considered, in characters (3 by default);
// maxGram the longest (12 by default, and raising it adds rare, high
// information grams for little index space); foldCase emits the lowercased form
// of every gram as well, which a case insensitive pattern needs to use the
// index at all.
//
// The name is stored in the index definition, so changing it means building a
// new index.
func SparsegramAnalyzer(minGram, maxGram int, foldCase bool) (string, error) {
	if minGram < 1 || maxGram < minGram {
		return "", NewInfinityException(
			int(ErrorCodeInvalidIndexParam),
			fmt.Sprintf("Expected 1 <= minGram <= maxGram, but got minGram=%d and maxGram=%d", minGram, maxGram),
		)
	}
	name := fmt.Sprintf("sparsegram-%d-%d", minGram, maxGram)
	if foldCase {
		name += "-fold"
	}
	return name, nil
}

// NewSparsegramIndexInfo builds a full-text index that makes RegexFilter use
// grams, equivalent to NewIndexInfo(targetName, IndexTypeFullText,
// map[string]string{"analyzer": name}).
//
// Example:
//
//	info, err := infinity.NewSparsegramIndexInfo("doc", 3, 12, true)
//	table.CreateIndex("idx", info)
func NewSparsegramIndexInfo(targetName string, minGram, maxGram int, foldCase bool) (*IndexInfo, error) {
	analyzer, err := SparsegramAnalyzer(minGram, maxGram, foldCase)
	if err != nil {
		return nil, err
	}
	return NewIndexInfo(targetName, IndexTypeFullText, map[string]string{"analyzer": analyzer}), nil
}

func (ii IndexInfo) String() string {
	return fmt.Sprintf("IndexInfo(%s, %s, %v)", ii.TargetName, ii.IndexType.String(), ii.Params)
}

// CopyFileType represents the type of file for import/export
type CopyFileType int

const (
	CopyFileTypeCSV   CopyFileType = 0
	CopyFileTypeJSON  CopyFileType = 1
	CopyFileTypeJSONL CopyFileType = 2
	CopyFileTypeFVECS CopyFileType = 3
	CopyFileTypeCSR   CopyFileType = 4
	CopyFileTypeBVECS CopyFileType = 5
)

// ImportOption represents options for importing data
type ImportOption struct {
	HasHeader    bool
	Delimiter    rune
	CopyFileType CopyFileType
}

func NewImportOption() *ImportOption {
	return &ImportOption{
		HasHeader:    false,
		Delimiter:    ',',
		CopyFileType: CopyFileTypeCSV,
	}
}

// ExportOption represents options for exporting data
type ExportOption struct {
	HasHeader    bool
	Delimiter    rune
	CopyFileType CopyFileType
	Offset       int
	Limit        int
	RowLimit     int
}

func NewExportOption() *ExportOption {
	return &ExportOption{
		HasHeader:    false,
		Delimiter:    ',',
		CopyFileType: CopyFileTypeCSV,
		Offset:       0,
		Limit:        0,
		RowLimit:     0,
	}
}

// AlterIndexOptions represents options for altering an index
type AlterIndexOptions struct {
	IndexName string
	OptParams []*InitParameter
}
