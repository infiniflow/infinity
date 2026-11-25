package main

import (
	"encoding/binary"
	"flag"
	"fmt"
	"log"
	"math"
	"os"
	"sort"
	"sync"
	"time"

	infinity "github.com/infiniflow/infinity-go-sdk"
)

func readFvecs(path string, maxN int) ([][]float64, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}
	if len(data) < 4 {
		return nil, fmt.Errorf("empty fvecs: %s", path)
	}
	dim := int(binary.LittleEndian.Uint32(data[0:4]))
	stride := (dim + 1) * 4
	n := len(data) / stride
	if maxN > 0 && n > maxN {
		n = maxN
	}
	out := make([][]float64, n)
	for i := 0; i < n; i++ {
		off := i*stride + 4
		vec := make([]float64, dim)
		for d := 0; d < dim; d++ {
			bits := binary.LittleEndian.Uint32(data[off+d*4 : off+(d+1)*4])
			vec[d] = float64(math.Float32frombits(bits))
		}
		out[i] = vec
	}
	return out, nil
}

func readIvecs(path string, maxN int) ([][]int64, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}
	if len(data) < 4 {
		return nil, fmt.Errorf("empty ivecs: %s", path)
	}
	dim := int(binary.LittleEndian.Uint32(data[0:4]))
	stride := (dim + 1) * 4
	n := len(data) / stride
	if maxN > 0 && n > maxN {
		n = maxN
	}
	out := make([][]int64, n)
	for i := 0; i < n; i++ {
		off := i*stride + 4
		vec := make([]int64, dim)
		for d := 0; d < dim; d++ {
			v := int32(binary.LittleEndian.Uint32(data[off+d*4 : off+(d+1)*4]))
			vec[d] = int64(v)
		}
		out[i] = vec
	}
	return out, nil
}

func toInt64(v interface{}) int64 {
	switch x := v.(type) {
	case int64:
		return x
	case int32:
		return int64(x)
	case int:
		return int64(x)
	case uint64:
		return int64(x)
	default:
		return -1
	}
}

func synthVec(i int, dim int) []float64 {
	v := make([]float64, dim)
	for d := 0; d < dim; d++ {
		x := uint32((i*1315423911 + d*2654435761) & 0xffff)
		v[d] = float64(x) / 65535.0
	}
	return v
}

func connectWithRetry(addr infinity.NetworkAddress, timeout time.Duration) (*infinity.InfinityConnection, error) {
	deadline := time.Now().Add(timeout)
	for {
		conn, err := infinity.Connect(addr)
		if err == nil {
			return conn, nil
		}
		if time.Now().After(deadline) {
			return nil, err
		}
		time.Sleep(250 * time.Millisecond)
	}
}

func main() {
	host := flag.String("host", "127.0.0.1", "server host")
	port := flag.Int("port", 23817, "server port")
	dataset := flag.String("dataset", "synthetic", "dataset: synthetic | sift1m | gist1m")
	tableFlag := flag.String("table", "", "table name (default: bench_<dataset>_<encode>)")
	encode := flag.String("encode", "plain", "HNSW encode: plain | lvq | rabitq")
	mParam := flag.Int("m", 16, "HNSW M")
	efConstruction := flag.Int("ef-construction", 200, "HNSW ef_construction")
	ef := flag.Int("ef", 200, "HNSW ef")
	topn := flag.Int("topn", 100, "topn")
	rows := flag.Int("rows", 20000, "rows for synthetic dataset")
	dimFlag := flag.Int("dim", 128, "vector dimension for synthetic dataset")
	numQueries := flag.Int("num-queries", 0, "1-thread query count (0 = auto)")
	threads := flag.Int("threads", 8, "concurrent goroutines")
	multiQueries := flag.Int("multi-queries", 0, "multi-thread total queries (0 = auto)")
	skipImport := flag.Bool("skip-import", false, "skip import and index creation")
	basePath := flag.String("base-path", "", "override base fvecs path")
	queryPath := flag.String("query-path", "", "override query fvecs path")
	gtPath := flag.String("gt-path", "", "override groundtruth ivecs path")
	ciOut := flag.Bool("ci", false, "emit 'Total cost : <sec> s' lines for CI regression checks")
	flag.Parse()

	dim := *dimFlag
	if *dataset == "sift1m" {
		dim = 128
		if *basePath == "" {
			*basePath = "test/data/benchmark/sift_1m/sift_base.fvecs"
		}
		if *queryPath == "" {
			*queryPath = "test/data/benchmark/sift_1m/sift_query.fvecs"
		}
		if *gtPath == "" {
			*gtPath = "test/data/benchmark/sift_1m/sift_groundtruth.ivecs"
		}
	} else if *dataset == "gist1m" {
		dim = 960
		if *basePath == "" {
			*basePath = "test/data/benchmark/gist_1m/gist_base.fvecs"
		}
		if *queryPath == "" {
			*queryPath = "test/data/benchmark/gist_1m/gist_query.fvecs"
		}
		if *gtPath == "" {
			*gtPath = "test/data/benchmark/gist_1m/gist_groundtruth.ivecs"
		}
	}

	nqTarget := *numQueries
	if nqTarget <= 0 {
		if *dataset == "synthetic" {
			nqTarget = 200
		} else {
			nqTarget = 1000
		}
	}
	mqTarget := *multiQueries
	if mqTarget <= 0 {
		if *dataset == "synthetic" {
			mqTarget = 1000
		} else {
			mqTarget = 4000
		}
	}

	tableName := *tableFlag
	if tableName == "" {
		tableName = fmt.Sprintf("bench_%s_%s", *dataset, *encode)
	}

	addr := infinity.NetworkAddress{IP: *host, Port: *port}
	conn, err := connectWithRetry(addr, 60*time.Second)
	if err != nil {
		log.Fatalf("connect: %v", err)
	}
	defer conn.Disconnect()

	db, err := conn.GetDatabase("default_db")
	if err != nil {
		log.Fatalf("get db: %v", err)
	}

	var tbl *infinity.Table
	if !*skipImport {
		_, _ = db.DropTable(tableName, infinity.ConflictTypeIgnore)
		schema := infinity.TableSchema{
			{
				Name:     "v",
				DataType: fmt.Sprintf("vector,%d,float", dim),
			},
		}
		tbl, err = db.CreateTable(tableName, schema, infinity.ConflictTypeError)
		if err != nil {
			log.Fatalf("create table: %v", err)
		}

		t0 := time.Now()
		if *dataset == "synthetic" {
			const batch = 1000
			for start := 0; start < *rows; start += batch {
				end := start + batch
				if end > *rows {
					end = *rows
				}
				chunk := make([]map[string]interface{}, 0, end-start)
				for i := start; i < end; i++ {
					chunk = append(chunk, map[string]interface{}{"v": synthVec(i, dim)})
				}
				if _, err := tbl.Insert(chunk); err != nil {
					log.Fatalf("insert batch %d: %v", start, err)
				}
			}
			_ = conn.FlushData()
		} else {
			if _, err := tbl.ImportData(*basePath, &infinity.ImportOption{CopyFileType: infinity.CopyFileTypeFVECS}); err != nil {
				log.Fatalf("import_data: %v", err)
			}
		}
		importSec := time.Since(t0).Seconds()
		fmt.Printf("Import %s: %.2fs\n", *dataset, importSec)

		t0 = time.Now()
		idxInfo := infinity.NewIndexInfo(
			"v",
			infinity.IndexTypeHnsw,
			map[string]string{
				"M":               fmt.Sprintf("%d", *mParam),
				"ef_construction": fmt.Sprintf("%d", *efConstruction),
				"metric":          "l2",
				"encode":          *encode,
			},
		)
		if _, err := tbl.CreateIndex("idx_hnsw", idxInfo, infinity.ConflictTypeError, ""); err != nil {
			log.Fatalf("create_index: %v", err)
		}
		buildSec := time.Since(t0).Seconds()
		fmt.Printf("Build HNSW (%s): %.2fs\n", *encode, buildSec)
		if *ciOut {
			fmt.Printf("Total cost : %.6f s\n", buildSec)
		}
	} else {
		tbl, err = db.GetTable(tableName)
		if err != nil {
			log.Fatalf("get table: %v", err)
		}
	}

	var queries [][]float64
	var gt [][]int64
	if *dataset == "synthetic" {
		queries = make([][]float64, nqTarget)
		for i := 0; i < nqTarget; i++ {
			queries[i] = synthVec(i*7+1, dim)
		}
	} else {
		queries, err = readFvecs(*queryPath, nqTarget)
		if err != nil {
			log.Fatalf("read queries: %v", err)
		}
		gt, err = readIvecs(*gtPath, nqTarget)
		if err != nil {
			log.Fatalf("read gt: %v", err)
		}
	}
	nq := len(queries)
	knnParams := map[string]string{"ef": fmt.Sprintf("%d", *ef)}

	warmup := 50
	if warmup > nq {
		warmup = nq
	}
	for i := 0; i < warmup; i++ {
		if _, err := tbl.Output([]string{"_row_id"}).MatchDense("v", queries[i], "float", "l2", *topn, knnParams).ToResult(); err != nil {
			log.Fatalf("warmup: %v", err)
		}
	}

	latencies := make([]float64, nq)
	recallSum := 0.0
	tStart := time.Now()
	for i := 0; i < nq; i++ {
		t0 := time.Now()
		resAny, err := tbl.Output([]string{"_row_id"}).MatchDense("v", queries[i], "float", "l2", *topn, knnParams).ToResult()
		dt := time.Since(t0).Seconds() * 1000.0
		if err != nil {
			log.Fatalf("query %d: %v", i, err)
		}
		latencies[i] = dt

		if len(gt) > i {
			qr := resAny.(*infinity.QueryResult)
			col, ok := qr.Data["ROW_ID"]
			if !ok {
				col = qr.Data["_row_id"]
			}
			truth := make(map[int64]struct{}, *topn)
			for _, id := range gt[i][:*topn] {
				truth[id] = struct{}{}
			}
			hits := 0
			for _, v := range col {
				if _, ok := truth[toInt64(v)]; ok {
					hits++
				}
			}
			recallSum += float64(hits) / float64(*topn)
		}
	}
	total1 := time.Since(tStart).Seconds()
	sort.Float64s(latencies)
	mean := 0.0
	for _, l := range latencies {
		mean += l
	}
	mean /= float64(nq)
	p50 := latencies[nq*50/100]
	p95 := latencies[nq*95/100]
	p99 := latencies[nq*99/100]
	if len(gt) > 0 {
		fmt.Printf("[1-thread] queries=%d total=%.2fs QPS=%.1f mean=%.3fms p50=%.3fms p95=%.3fms p99=%.3fms recall@%d=%.4f\n",
			nq, total1, float64(nq)/total1, mean, p50, p95, p99, *topn, recallSum/float64(nq))
	} else {
		fmt.Printf("[1-thread] queries=%d total=%.2fs QPS=%.1f mean=%.3fms p50=%.3fms p95=%.3fms p99=%.3fms\n",
			nq, total1, float64(nq)/total1, mean, p50, p95, p99)
	}
	if *ciOut {
		fmt.Printf("Total cost : %.6f s\n", total1)
	}

	if *threads > 1 {
		conns := make([]*infinity.InfinityConnection, *threads)
		tbls := make([]*infinity.Table, *threads)
		for tid := 0; tid < *threads; tid++ {
			c, err := infinity.Connect(addr)
			if err != nil {
				log.Fatalf("connect %d: %v", tid, err)
			}
			conns[tid] = c
			d, err := c.GetDatabase("default_db")
			if err != nil {
				log.Fatalf("get db %d: %v", tid, err)
			}
			t, err := d.GetTable(tableName)
			if err != nil {
				log.Fatalf("get table %d: %v", tid, err)
			}
			tbls[tid] = t
		}

		perThread := mqTarget / *threads
		if perThread < 1 {
			perThread = 1
		}
		var wg sync.WaitGroup
		wg.Add(*threads)
		t0 := time.Now()
		for tid := 0; tid < *threads; tid++ {
			go func(id int) {
				defer wg.Done()
				t := tbls[id]
				for j := 0; j < perThread; j++ {
					idx := (id*perThread + j) % nq
					if _, err := t.Output([]string{"_row_id"}).MatchDense("v", queries[idx], "float", "l2", *topn, knnParams).ToResult(); err != nil {
						log.Fatalf("worker %d query %d: %v", id, j, err)
					}
				}
			}(tid)
		}
		wg.Wait()
		elapsed := time.Since(t0).Seconds()
		totalQ := perThread * *threads
		fmt.Printf("[%d-threads] queries=%d total=%.2fs QPS=%.1f\n", *threads, totalQ, elapsed, float64(totalQ)/elapsed)
		if *ciOut {
			fmt.Printf("Total cost : %.6f s\n", elapsed)
		}

		for _, c := range conns {
			c.Disconnect()
		}
	}
}
