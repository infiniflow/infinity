package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"strconv"
	"time"

	infinity "github.com/infiniflow/infinity-go-sdk"
)

func envInt(key string, def int) int {
	v := os.Getenv(key)
	if v == "" {
		return def
	}
	n, err := strconv.Atoi(v)
	if err != nil {
		log.Fatalf("invalid %s=%q: %v", key, v, err)
	}
	return n
}

func connectDeadline(host string, port int, timeout time.Duration) (*infinity.InfinityConnection, *infinity.Database, error) {
	deadline := time.Now().Add(timeout)
	for {
		conn, err := infinity.Connect(infinity.NetworkAddress{IP: host, Port: port})
		if err == nil {
			db, err := conn.GetDatabase("default_db")
			if err == nil {
				if _, err := db.ListTables(); err == nil {
					return conn, db, nil
				}
			}
			_, _ = conn.Disconnect()
		}
		if time.Now().After(deadline) {
			return nil, nil, fmt.Errorf("connect %s:%d failed: %w", host, port, err)
		}
		time.Sleep(1 * time.Second)
	}
}

func insertBatches(tbl *infinity.Table, start, count int) error {
	const batch = 2000
	for base := start; base < start+count; base += batch {
		end := base + batch
		if end > start+count {
			end = start + count
		}
		chunk := make([]map[string]interface{}, 0, end-base)
		for i := base; i < end; i++ {
			chunk = append(chunk, map[string]interface{}{
				"id":    i,
				"name":  fmt.Sprintf("n-%d", i),
				"score": float64(i % 1000),
			})
		}
		if _, err := tbl.Insert(chunk); err != nil {
			return err
		}
	}
	return nil
}

func timeit(label string, fn func() error) float64 {
	start := time.Now()
	if err := fn(); err != nil {
		log.Fatalf("%s: %v", label, err)
	}
	elapsed := time.Since(start).Seconds()
	fmt.Printf("SEC %s %.6f\n", label, elapsed)
	return elapsed
}

func main() {
	hostFlag := flag.String("host", "127.0.0.1", "server host")
	portFlag := flag.Int("port", 23817, "server port")
	rowsFlag := flag.Int("rows", envInt("SEC_ROWS", 100000), "number of rows")
	pointsFlag := flag.Int("points", envInt("SEC_POINTS", 200), "number of point queries")
	rangesFlag := flag.Int("ranges", envInt("SEC_RANGES", 100), "number of range queries")
	flag.Parse()

	rows := *rowsFlag
	points := *pointsFlag
	ranges := *rangesFlag

	conn, db, err := connectDeadline(*hostFlag, *portFlag, 180*time.Second)
	if err != nil {
		log.Fatal(err)
	}
	defer func() { _, _ = conn.Disconnect() }()

	if _, err := db.DropTable("sec_bench", infinity.ConflictTypeIgnore); err != nil {
		log.Fatalf("drop table: %v", err)
	}
	tbl, err := db.CreateTable("sec_bench", infinity.TableSchema{
		{Name: "id", DataType: "int"},
		{Name: "name", DataType: "varchar"},
		{Name: "score", DataType: "double"},
	}, infinity.ConflictTypeError)
	if err != nil {
		log.Fatalf("create table: %v", err)
	}

	timeit("insert", func() error { return insertBatches(tbl, 0, rows) })
	if err := conn.FlushData(); err != nil {
		log.Fatalf("flush: %v", err)
	}
	time.Sleep(1 * time.Second)

	point := func() error {
		for i := 0; i < points; i++ {
			if _, err := tbl.Output([]string{"id"}).Filter(fmt.Sprintf("score = %.1f", float64(i%1000))).ToResult(); err != nil {
				return err
			}
		}
		return nil
	}
	rangeQ := func() error {
		for i := 0; i < ranges; i++ {
			lo := float64(i % 900)
			if _, err := tbl.Output([]string{"id"}).Filter(fmt.Sprintf("score >= %.1f and score < %.1f", lo, lo+10)).ToResult(); err != nil {
				return err
			}
		}
		return nil
	}
	varcharPoint := func() error {
		for i := 0; i < points; i++ {
			if _, err := tbl.Output([]string{"id"}).Filter(fmt.Sprintf("name = 'n-%d'", (i*7)%rows)).ToResult(); err != nil {
				return err
			}
		}
		return nil
	}

	if err := point(); err != nil {
		log.Fatalf("warmup noindex_point: %v", err)
	}
	timeit("noindex_point", point)
	if err := rangeQ(); err != nil {
		log.Fatalf("warmup noindex_range: %v", err)
	}
	timeit("noindex_range", rangeQ)

	timeit("build_score", func() error {
		_, err := tbl.CreateIndex("idx_score", infinity.NewIndexInfo("score", infinity.IndexTypeSecondary, nil), infinity.ConflictTypeError, "")
		return err
	})
	timeit("build_name", func() error {
		_, err := tbl.CreateIndex("idx_name", infinity.NewIndexInfo("name", infinity.IndexTypeSecondary, nil), infinity.ConflictTypeError, "")
		return err
	})
	timeit("build_id", func() error {
		_, err := tbl.CreateIndex("idx_id", infinity.NewIndexInfo("id", infinity.IndexTypeSecondary, nil), infinity.ConflictTypeError, "")
		return err
	})

	// Seal the three indexes before timing lookups. Intent: both engines must
	// be measured on the sealed shape - the reference writes the chunk inside
	// `CREATE INDEX`, this engine only in a `DumpMemIndex` transaction
	// (`FLUSH DATA` is a checkpoint). Skipping it would time this engine's
	// memory tails, where a lock gates every lookup.
	for _, name := range []string{"idx_score", "idx_name", "idx_id"} {
		if _, err := tbl.DumpIndex(name); err != nil {
			log.Fatalf("dump index %s: %v", name, err)
		}
	}

	uniquePoint := func() error {
		for i := 0; i < points; i++ {
			if _, err := tbl.Output([]string{"id", "name"}).Filter(fmt.Sprintf("id = %d", i*499)).ToResult(); err != nil {
				return err
			}
		}
		return nil
	}

	if err := uniquePoint(); err != nil {
		log.Fatalf("warmup index_unique_point: %v", err)
	}
	timeit("index_unique_point", uniquePoint)
	if err := point(); err != nil {
		log.Fatalf("warmup index_point: %v", err)
	}
	timeit("index_point", point)
	if err := rangeQ(); err != nil {
		log.Fatalf("warmup index_range: %v", err)
	}
	timeit("index_range", rangeQ)
	if err := varcharPoint(); err != nil {
		log.Fatalf("warmup index_varchar_point: %v", err)
	}
	timeit("index_varchar_point", varcharPoint)

	timeit("insert_with_index", func() error { return insertBatches(tbl, rows, 20000) })
	timeit("update_with_index", func() error {
		_, err := tbl.Update("id >= 0 and id < 5000", map[string]interface{}{"score": 1234.0})
		return err
	})
	timeit("delete_with_index", func() error {
		_, err := tbl.Delete("score = 1234.0")
		return err
	})

	for _, name := range []string{"idx_score", "idx_name", "idx_id"} {
		if _, err := tbl.DropIndex(name, infinity.ConflictTypeError); err != nil {
			log.Fatalf("drop index %s: %v", name, err)
		}
	}
	timeit("update_noindex", func() error {
		_, err := tbl.Update("id >= 20000 and id < 25000", map[string]interface{}{"score": 777.0})
		return err
	})
	timeit("delete_noindex", func() error {
		_, err := tbl.Delete("score = 777.0")
		return err
	})

	if err := conn.FlushData(); err != nil {
		log.Fatalf("flush: %v", err)
	}
}
