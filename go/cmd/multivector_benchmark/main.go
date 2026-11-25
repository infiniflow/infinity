package main

import (
	"flag"
	"fmt"
	"log"
	"math/rand"
	"strconv"
	"time"

	infinity "github.com/infiniflow/infinity-go-sdk"
)

const (
	dims          = 8
	vectorsPerRow = 4
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

func makeRow(i int, rng *rand.Rand) map[string]interface{} {
	cx := float64(i%97) / 97.0
	cy := float64(i%13) / 13.0
	tail := float64(i%7) * 0.01
	mv := make([][]float64, vectorsPerRow)
	for v := 0; v < vectorsPerRow; v++ {
		vec := make([]float64, dims)
		vec[0] = cx + (rng.Float64()*0.02 - 0.01)
		vec[1] = cy + (rng.Float64()*0.02 - 0.01)
		vec[2] = 0.25
		vec[3] = 0.75
		for d := 4; d < dims; d++ {
			vec[d] = tail
		}
		mv[v] = vec
	}
	return map[string]interface{}{
		"id": i,
		"mv": mv,
	}
}

func makeQuery() [][]float64 {
	q := make([][]float64, vectorsPerRow)
	for j := 0; j < vectorsPerRow; j++ {
		vec := make([]float64, dims)
		vec[0] = 0.5 + float64(j)*0.001
		vec[1] = 0.5
		vec[2] = 0.25
		vec[3] = 0.75
		for d := 4; d < dims; d++ {
			vec[d] = 0.02
		}
		q[j] = vec
	}
	return q
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

	rng := rand.New(rand.NewSource(7))
	batch := make([]map[string]interface{}, rows)
	for i := 0; i < rows; i++ {
		batch[i] = makeRow(i, rng)
	}
	query := makeQuery()

	conn, db, err := connectDeadline(host, port, 180*time.Second)
	if err != nil {
		log.Fatal(err)
	}
	defer func() { _, _ = conn.Disconnect() }()

	indexes := []struct {
		kind     string
		kindType infinity.IndexType
		opts     map[string]string
	}{
		{"emvb", infinity.IndexTypeEMVB, map[string]string{"pq_subspace_num": "4", "pq_subspace_bits": "8"}},
		{"plaid", infinity.IndexTypePLAID, map[string]string{"nbits": "4"}},
		{"smve", infinity.IndexTypeSMVE, map[string]string{}},
	}

	for _, idx := range indexes {
		table := "bench_mv_" + idx.kind
		if _, err := db.DropTable(table, infinity.ConflictTypeIgnore); err != nil {
			log.Fatalf("drop table %s: %v", table, err)
		}
		tbl, err := db.CreateTable(table, infinity.TableSchema{
			{Name: "id", DataType: "int"},
			{Name: "mv", DataType: fmt.Sprintf("tensor,%d,float", dims)},
		}, infinity.ConflictTypeError)
		if err != nil {
			log.Fatalf("create table %s: %v", table, err)
		}

		for start := 0; start < rows; start += 1000 {
			end := start + 1000
			if end > rows {
				end = rows
			}
			if _, err := tbl.Insert(batch[start:end]); err != nil {
				log.Fatalf("insert %s: %v", table, err)
			}
		}
		if err := conn.FlushData(); err != nil {
			log.Fatalf("flush %s: %v", table, err)
		}
		time.Sleep(1 * time.Second)

		start := time.Now()
		if _, err := tbl.CreateIndex("idx_"+idx.kind, infinity.NewIndexInfo("mv", idx.kindType, idx.opts), infinity.ConflictTypeError, ""); err != nil {
			fmt.Printf("INDEX %s build ERROR %v\n", idx.kind, err)
			_, _ = db.DropTable(table, infinity.ConflictTypeIgnore)
			continue
		}
		fmt.Printf("INDEX %s build %.4f\n", idx.kind, time.Since(start).Seconds())

		if _, err := tbl.Output([]string{"id"}).MatchTensor("mv", query, "float", 10, nil).ToResult(); err != nil {
			log.Fatalf("warmup query %s: %v", idx.kind, err)
		}
		start = time.Now()
		for i := 0; i < queries; i++ {
			if _, err := tbl.Output([]string{"id"}).MatchTensor("mv", query, "float", 10, nil).ToResult(); err != nil {
				log.Fatalf("query %s: %v", idx.kind, err)
			}
		}
		elapsed := time.Since(start).Seconds()
		fmt.Printf("INDEX %s query %.6f\n", idx.kind, elapsed/float64(queries))
		fmt.Printf("INDEX %s query_total %.4f\n", idx.kind, elapsed)

		_, _ = db.DropTable(table, infinity.ConflictTypeIgnore)
	}
}
