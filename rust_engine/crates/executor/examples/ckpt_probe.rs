//! How much of a checkpoint is CPU and how much is fsync?
//!
//! Drives the work only; the syscall breakdown comes from `strace -c -f` on the
//! outside. Used to decide whether a checkpoint sweep belongs on a CPU pool.

// The production allocator (mimalloc) comes from `common`, so a probe that
// measures allocator behaviour already uses the same one.

use executor::Engine;

fn main() {
    let dir = tempfile::tempdir().unwrap();
    let e = Engine::open(dir.path()).unwrap();
    e.set_memindex_quota(1 << 30);
    let rows: usize = std::env::args()
        .nth(1)
        .and_then(|v| v.parse().ok())
        .unwrap_or(200_000);
    e.execute_blocking("CREATE TABLE p (id INT, a INT, b INT, c VARCHAR);")
        .unwrap();
    let mut values = Vec::new();
    for i in 0..rows {
        values.push(format!("({i},{},{},'row-{i}')", i % 97, i % 13));
        if values.len() == 1000 {
            e.execute_blocking(&format!("INSERT INTO p VALUES {};", values.join(",")))
                .unwrap();
            values.clear();
        }
    }
    if !values.is_empty() {
        e.execute_blocking(&format!("INSERT INTO p VALUES {};", values.join(",")))
            .unwrap();
    }
    // The first checkpoint writes every block; the second rewrites one block,
    // which is the steady state the worker count is judged on.
    let started = std::time::Instant::now();
    executor::block_on(e.flush()).unwrap();
    eprintln!("cold checkpoint of {rows} rows: {:?}", started.elapsed());
    e.execute_blocking("INSERT INTO p VALUES (999999,1,1,'x');")
        .unwrap();
    let started = std::time::Instant::now();
    executor::block_on(e.flush()).unwrap();
    eprintln!("steady checkpoint: {:?}", started.elapsed());
}
