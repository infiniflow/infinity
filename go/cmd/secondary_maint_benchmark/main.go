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

const (
	batchSize   = 2000
	rowsPerStep = 5000
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

func timed(label string, fn func() error) {
	start := time.Now()
	if err := fn(); err != nil {
		log.Fatalf("%s: %v", label, err)
	}
	elapsed := time.Since(start).Seconds()
	fmt.Printf("MAINT %s %.6f  # %.1f us/row\n", label, elapsed, elapsed/float64(rowsPerStep)*1e6)
}

func main() {
	hostFlag := flag.String("host", "127.0.0.1", "server host")
	portFlag := flag.Int("port", 23817, "server port")
	rowsFlag := flag.Int("rows", envInt("SEC_ROWS", 100000), "number of rows")
	flag.Parse()

	rows := *rowsFlag
	conn, db, err := connectDeadline(*hostFlag, *portFlag, 180*time.Second)
	if err != nil {
		log.Fatal(err)
	}
	defer func() { _, _ = conn.Disconnect() }()

	if _, err := db.DropTable("sec_maint", infinity.ConflictTypeIgnore); err != nil {
		log.Fatalf("drop table: %v", err)
	}
	tbl, err := db.CreateTable("sec_maint", infinity.TableSchema{
		{Name: "id", DataType: "int"},
		{Name: "name", DataType: "varchar"},
		{Name: "score", DataType: "double"},
	}, infinity.ConflictTypeError)
	if err != nil {
		log.Fatalf("create table: %v", err)
	}

	for base := 0; base < rows; base += batchSize {
		end := base + batchSize
		if end > rows {
			end = rows
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
			log.Fatalf("insert: %v", err)
		}
	}
	if err := conn.FlushData(); err != nil {
		log.Fatalf("flush: %v", err)
	}
	time.Sleep(1 * time.Second)

	steps := []struct {
		label  string
		idx    string
		column string
	}{
		{"none", "", ""},
		{"score", "idx_score", "score"},
		{"score+name", "idx_name", "name"},
		{"score+name+id", "idx_id", "id"},
	}

	const marker = 5000.0
	for step, s := range steps {
		if s.idx != "" {
			if _, err := tbl.CreateIndex(s.idx, infinity.NewIndexInfo(s.column, infinity.IndexTypeSecondary, nil), infinity.ConflictTypeError, ""); err != nil {
				log.Fatalf("create index %s: %v", s.idx, err)
			}
		}
		lo := step * rowsPerStep
		hi := lo + rowsPerStep
		value := marker + float64(step)
		timed("update_"+s.label, func() error {
			_, err := tbl.Update(fmt.Sprintf("id >= %d and id < %d", lo, hi), map[string]interface{}{"score": value})
			return err
		})
		timed("delete_"+s.label, func() error {
			_, err := tbl.Delete(fmt.Sprintf("score = %.1f", value))
			return err
		})
	}
}
