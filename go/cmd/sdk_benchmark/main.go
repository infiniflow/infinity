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

func main() {
	hostFlag := flag.String("host", "127.0.0.1", "server host")
	portFlag := flag.Int("port", 23917, "server port")
	rowsFlag := flag.Int("rows", 20000, "number of rows")
	flag.Parse()

	host := *hostFlag
	port := *portFlag
	rows := *rowsFlag
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

	conn, db, err := connectDeadline(host, port, 120*time.Second)
	if err != nil {
		log.Fatal(err)
	}
	defer func() { _, _ = conn.Disconnect() }()

	if _, err := db.DropTable("bench_t", infinity.ConflictTypeIgnore); err != nil {
		log.Fatalf("drop table: %v", err)
	}
	tbl, err := db.CreateTable("bench_t", infinity.TableSchema{
		{Name: "id", DataType: "int"},
		{Name: "name", DataType: "varchar"},
	}, infinity.ConflictTypeError)
	if err != nil {
		log.Fatalf("create table: %v", err)
	}

	const batch = 1000
	batches := make([][]map[string]interface{}, 0, (rows+batch-1)/batch)
	for b := 0; b < rows; b += batch {
		end := b + batch
		if end > rows {
			end = rows
		}
		chunk := make([]map[string]interface{}, 0, end-b)
		for i := b; i < end; i++ {
			chunk = append(chunk, map[string]interface{}{
				"id":   i,
				"name": fmt.Sprintf("row-%d", i),
			})
		}
		batches = append(batches, chunk)
	}

	start := time.Now()
	for _, chunk := range batches {
		if _, err := tbl.Insert(chunk); err != nil {
			log.Fatalf("insert: %v", err)
		}
	}
	fmt.Printf("Total cost : %.6f s\n", time.Since(start).Seconds())

	start = time.Now()
	res, err := tbl.Output([]string{"id", "name"}).ToResult()
	elapsed := time.Since(start).Seconds()
	if err != nil {
		log.Fatalf("select all: %v", err)
	}
	fmt.Printf("Total cost : %.6f s\n", elapsed)
	qr, ok := res.(*infinity.QueryResult)
	if !ok {
		log.Fatalf("unexpected result type: %T", res)
	}
	if got := len(qr.Data["id"]); got != rows {
		fmt.Fprintf(os.Stderr, "row count mismatch: %d != %d\n", got, rows)
		os.Exit(1)
	}

	start = time.Now()
	if _, err := tbl.Output([]string{"id"}).Filter(fmt.Sprintf("id = %d", rows-1)).ToResult(); err != nil {
		log.Fatalf("point filter: %v", err)
	}
	fmt.Printf("Total cost : %.6f s\n", time.Since(start).Seconds())
}
