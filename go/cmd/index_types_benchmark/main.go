package main

import (
	"flag"
	"fmt"
	"log"
	"strconv"
	"time"

	infinity "github.com/infiniflow/infinity-go-sdk"
)

func connectDeadline(host string, port int, timeout time.Duration) (*infinity.InfinityConnection, *infinity.Database, error) {
	deadline := time.Now().Add(timeout)
	for {
		conn, err := infinity.Connect(infinity.NetworkAddress{IP: host, Port: port})
		if err == nil {
			db, err := conn.GetDatabase("default_db")
			if err == nil {
				return conn, db, nil
			}
			_, _ = conn.Disconnect()
		}
		if time.Now().After(deadline) {
			return nil, nil, fmt.Errorf("connect %s:%d failed: %w", host, port, err)
		}
		time.Sleep(1 * time.Second)
	}
}

func makeRow(i int) map[string]interface{} {
	body := fmt.Sprintf("delta epsilon doc%d", i)
	if i%2 == 0 {
		body = fmt.Sprintf("alpha beta gamma doc%d", i)
	}
	return map[string]interface{}{
		"id":    i,
		"title": fmt.Sprintf("doc-%d", i),
		"body":  body,
		"vec":   []float64{float64(i%97) / 97.0, float64(i%13) / 13.0, 0.25, 0.75},
		"sp":    &infinity.SparseVector{Indices: []int{0, i%7 + 1}, Values: []float64{0.5, 1.0}},
	}
}

func batchInsert(tbl *infinity.Table, rows int) error {
	const batch = 1000
	for start := 0; start < rows; start += batch {
		end := start + batch
		if end > rows {
			end = rows
		}
		chunk := make([]map[string]interface{}, 0, end-start)
		for i := start; i < end; i++ {
			chunk = append(chunk, makeRow(i))
		}
		if _, err := tbl.Insert(chunk); err != nil {
			return err
		}
	}
	return nil
}

func runQuery(tbl *infinity.Table, kind string, i int) error {
	switch kind {
	case "fulltext":
		_, err := tbl.Output([]string{"id"}).MatchText("body", "alpha", 10, nil).ToResult()
		return err
	case "hnsw":
		_, err := tbl.Output([]string{"id"}).MatchDense("vec", []float64{0.5, 0.5, 0.25, 0.75}, "float", "l2", 10, nil).ToResult()
		return err
	case "secondary":
		_, err := tbl.Output([]string{"id"}).Filter(fmt.Sprintf("title = 'doc-%d'", i)).ToResult()
		return err
	case "sparse":
		_, err := tbl.Output([]string{"id"}).MatchSparse("sp", &infinity.SparseVector{Indices: []int{0, 1}, Values: []float64{0.5, 1.0}}, "ip", 10, nil).ToResult()
		return err
	default:
		return fmt.Errorf("unknown index kind: %s", kind)
	}
}

func main() {
	hostFlag := flag.String("host", "127.0.0.1", "server host")
	portFlag := flag.Int("port", 23917, "server port")
	rowsFlag := flag.Int("rows", 20000, "number of rows")
	queriesFlag := flag.Int("queries", 50, "number of queries")
	flag.Parse()

	host := *hostFlag
	port := *portFlag
	rows := *rowsFlag
	queries := *queriesFlag
	args := flag.Args()
	if len(args) > 0 {
		host = args[0]
	}
	if len(args) > 1 {
		v, err := strconv.Atoi(args[1])
		if err != nil {
			log.Fatalf("invalid port %q: %v", args[1], err)
		}
		port = v
	}
	if len(args) > 2 {
		v, err := strconv.Atoi(args[2])
		if err != nil {
			log.Fatalf("invalid rows %q: %v", args[2], err)
		}
		rows = v
	}
	if len(args) > 3 {
		v, err := strconv.Atoi(args[3])
		if err != nil {
			log.Fatalf("invalid queries %q: %v", args[3], err)
		}
		queries = v
	}

	conn, db, err := connectDeadline(host, port, 180*time.Second)
	if err != nil {
		log.Fatal(err)
	}
	defer func() { _, _ = conn.Disconnect() }()

	if _, err := db.DropTable("bench_idx", infinity.ConflictTypeIgnore); err != nil {
		log.Fatalf("drop table: %v", err)
	}
	tbl, err := db.CreateTable("bench_idx", infinity.TableSchema{
		{Name: "id", DataType: "int"},
		{Name: "title", DataType: "varchar"},
		{Name: "body", DataType: "varchar"},
		{Name: "vec", DataType: "vector,4,float"},
		{Name: "sp", DataType: "sparse,8,float,int"},
	}, infinity.ConflictTypeError)
	if err != nil {
		log.Fatalf("create table: %v", err)
	}

	if err := batchInsert(tbl, rows); err != nil {
		log.Fatalf("insert: %v", err)
	}
	if err := conn.FlushData(); err != nil {
		log.Fatalf("flush: %v", err)
	}
	time.Sleep(1 * time.Second)
	fmt.Printf("ROWS %d\n", rows)

	indexes := []struct {
		kind string
		info *infinity.IndexInfo
	}{
		{"fulltext", infinity.NewIndexInfo("body", infinity.IndexTypeFullText, nil)},
		{"hnsw", infinity.NewIndexInfo("vec", infinity.IndexTypeHnsw, map[string]string{"M": "16", "ef_construction": "200", "metric": "l2"})},
		{"secondary", infinity.NewIndexInfo("title", infinity.IndexTypeSecondary, nil)},
		{"sparse", infinity.NewIndexInfo("sp", infinity.IndexTypeBMP, nil)},
	}

	for _, idx := range indexes {
		start := time.Now()
		if _, err := tbl.CreateIndex("idx_"+idx.kind, idx.info, infinity.ConflictTypeError, ""); err != nil {
			log.Fatalf("create index %s: %v", idx.kind, err)
		}
		build := time.Since(start).Seconds()
		fmt.Printf("INDEX %s build %.4f\n", idx.kind, build)

		// Seal the index before timing queries. Intent: both engines must be
		// measured on the sealed shape - the reference writes the chunk inside
		// `CREATE INDEX`, this engine only in a `DumpMemIndex` transaction
		// (`FLUSH DATA` is a checkpoint). Skipping it would time this engine's
		// memory tail, where a lock gates every query.
		if _, err := tbl.DumpIndex("idx_" + idx.kind); err != nil {
			log.Fatalf("dump index %s: %v", idx.kind, err)
		}

		if err := runQuery(tbl, idx.kind, 0); err != nil {
			log.Fatalf("warmup query %s: %v", idx.kind, err)
		}
		start = time.Now()
		for i := 0; i < queries; i++ {
			if err := runQuery(tbl, idx.kind, i); err != nil {
				log.Fatalf("query %s: %v", idx.kind, err)
			}
		}
		elapsed := time.Since(start).Seconds()
		fmt.Printf("INDEX %s query %.6f\n", idx.kind, elapsed/float64(queries))
		fmt.Printf("INDEX %s query_total %.4f\n", idx.kind, elapsed)
	}

	if _, err := db.DropTable("bench_idx", infinity.ConflictTypeError); err != nil {
		log.Fatalf("drop table: %v", err)
	}
}
