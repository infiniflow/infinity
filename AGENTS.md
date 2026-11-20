# Infinity Project Instructions for GitHub Copilot

This file provides context, build instructions, and coding standards for the Infinity project.
It is structured to follow GitHub Copilot's [customization guidelines](https://docs.github.com/en/copilot/concepts/prompting/response-customization).

## 1. Project Overview
Infinity is an AI-native database built for LLM applications, providing incredibly fast hybrid search of dense embedding, sparse embedding, tensor, and full-text.

- **Core**: C++23 (requires Clang 20+)
- **SDK**: Python 3.13+

- **Architecture**: Single-binary.
  - `src/parser`, `src/planner`, `src/executor`: Query pipeline.
  - `src/storage`: Storage engine.
  - `src/network`: Communication.

## 2. Directory Structure
- `src/`: Core database engine source code.
  - `parser/`, `planner/`, `executor/`: Query processing pipeline.
  - `storage/`: Storage engine (buffer manager, wal, etc.).
  - `network/`: Server-client communication.
- `python/`: Python SDK (`infinity_sdk`) and Python-based tests.
- `test/`:
  - `sql/`: SQL Logic Tests (`.slt` files) for functional verification.
  - `data/`: Test datasets.
- `benchmark/`: Performance benchmarking tools.
- `cmake/`: CMake modules and dependency finders.
- `scripts/`: CI/CD and build helper scripts.

## 3. Build Instructions
The project uses **CMake** with **Ninja** generator and requires **Clang 20+**.

### Core Engine (C++)
- **Standard Build**:
  ```bash
  cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -S . -B cmake-build-debug
  cmake --build cmake-build-debug --target infinity
  ```
- **Developer Shortcut** (if `ymake` is available):
  - `ymake 1 d infinity`: Build infinity target in debug mode.

### Python SDK
Use `uv` for package management.
```bash
(cd python/infinity_sdk && uv build)
```

## 4. Testing Instructions

### Unit Tests (C++)
- **Build**: `ymake 1 d test_main` or `cmake --build cmake-build-debug --target test_main`
- **Run**: `./cmake-build-debug/src/test_main`

### Python SDK Tests
Located in `python/`. Use `uv` to manage the environment.

1. **Setup Environment**:
   ```bash
   uv sync --python 3.13 --frozen --all-extras
   source .venv/bin/activate
   # uv pip install python/infinity_sdk # This is handled by uv sync if infinity_sdk is in workspace
   ```

2. **Run Tests**:
   - **Pre-requisite**: Start Infinity server first!
     ```bash
     # Default configuration
     # Note: The log of infinity is at /var/infinity/log/infinity.log
     nohup ./cmake-build-debug/src/infinity > /dev/null 2>/dev/null &

     # To enable debug/trace logging:
     # 1. Create a config file (e.g., cp conf/infinity_conf.toml my_conf.toml)
     # 2. Set `log_level = "debug"` or `"trace"` in the config file.
     # 3. Start with -f:
     # Note: The log of infinity is at /var/infinity/log/infinity.log
     nohup ./cmake-build-debug/src/infinity -f my_conf.toml > /dev/null 2>/dev/null &
     ```
   - **Run Test**:
     ```bash
     uv run pytest python/test/cases/test_basic.py::TestInfinity::test_basic
     ```
   - **Note**: `local infinity` mode is deprecated.

### Functional Tests (SQL)
- SQL Logic Tests (`.slt`) are located in `test/sql`.
- **Requirement**: Ensure `sqllogictest` binary is in your system `PATH`.

## 5. Coding Standards & Guidelines
- **C++ Standard**: C++23. Use modern features but ensure compatibility with Clang 20.
- **C++ Formatting**: **MUST** use `clang-format-20` and follow `.clang-format` configuration.
- **Dependencies**: Managed via `vcpkg` (integrated in CMake).
- **Python**: **ALWAYS** use `uv` instead of `pip` for installing Python packages. **ALWAYS** use the python interpreter and pytest from the virtual environment created by `uv` at the project root (e.g. `uv run python`, `uv run pytest`), NOT the system ones.

## 6. Shell Command Execution Guidelines
When executing shell commands:
- NEVER prefix commands with environment variable assignments such as "PYTHONPATH=...", "ENV=...", or "FOO=bar cmd".
- If an environment variable is required, ask the user to configure it permanently (e.g., via pytest.ini, .env, or shell profile) instead of injecting it inline.
- ONLY execute direct commands like "pytest", "python -m pytest", or "uv run pytest".
- NEVER use chained commands with "&&", ";", or "|".

## 7. Rust Engine — Async Conventions

The engine runs every statement on **one** runtime. That runtime is the only
place a request can make progress, so a worker held while waiting is a
potential deadlock, not merely a slow path. Every rule below follows from one
direction:

> **Write the async form first. A synchronous form may exist only where
> something outside the engine forces it, and it must say so.**

Two consequences shape the rest of this section. First, "it compiles and the
tests pass" is not evidence that a conversion is right — a parked worker only
misbehaves under concurrency. Second, the absence of the word `block_on` is not
evidence that a call is safe; most of the stalls in this engine's history came
from ordinary synchronous calls made from the wrong place.

### 7.1 Default direction: convert, do not bridge

When a synchronous call site needs an asynchronous callee, the answer is
**make the chain async**, not "wrap it in `block_on`". Work down this list and
stop at the first step that applies:

1. **Make the caller `async`.** If its caller is also ours, convert that too.
   Keep going up until you reach a boundary you genuinely do not own. This is
   the normal path — most conversions end here, and each one removes a place
   where a thread can be parked.
2. **Move the operation off the path.** If the caller cannot become async (a
   `Drop`, a callback from a foreign synchronous API), restructure so the wait
   happens somewhere that can await, instead of parking a thread mid-chain.
3. **Scope the bridge.** If a bridge is truly unavoidable, make it one
   well-named function that states why, or gate it behind `#[cfg(test)]` so
   production cannot reach it. One bridge, one comment, one place.
4. **Only then** keep a `block_on` — under the conditions in §7.4.

Two habits to avoid, both of which cost real debugging time:

- **Do not add a sync twin "for flexibility".** A synchronous variant beside
  the async one, written because it was convenient at one call site, ends up
  bridging a lock that the async path needs and deadlocks the commit. If nobody
  needs the sync form, do not write it.
- **Do not treat an existing `block_on` as furniture.** Each one was a decision
  made under constraints that may no longer hold. Re-derive it: "who calls
  this, and can they await?" If the answer is "nobody", delete it (§7.13); if
  it is "our code, and it can", convert it.

The asymmetry is deliberate: an `async fn` costs a caller nothing it cannot
already do, while a parked thread costs a worker and a deadlock risk that only
appears under load. When the choice is unclear, prefer the async form and keep
the boundary explicit at the outermost edge rather than scattered through the
middle.

### 7.2 Nothing blocking may run inside a task

> **A task may not be paused for an unbounded time while holding a worker.**

The loudest violation is a `block_on` inside an `async fn`: it parks the
worker outright. If the awaited work needs a worker to finish (a bottom half, a
row allocator, a fan-out on the blocking pool, a `tokio::sync::Mutex` held by
another task), the process deadlocks instead of failing.

The quiet violations are just as real and contain none of those words:

- **A synchronous system call** — `fsync`, `rename`, `mmap` faults, a
  synchronous database batch, a DNS lookup, `std::thread::sleep`. A `fsync`
  during a checkpoint runs on whichever worker called it, and a slow disk
  stalls every task behind it.
- **A `std::sync::Mutex` held across an `.await`** — the same stall one layer
  up (§7.8).
- **A CPU-bound leaf** — encoding a batch of rows, decoding a block, walking a
  graph — left on a worker (§7.10).
- **A blocking channel receive or a thread join** on the request path.

Apply the test literally: *can this task be paused for an unbounded time while
holding a worker?* If yes, it is a bug however it is spelled. §7.1 says how to
get out of it; §7.4 lists the cases where the answer may legitimately be "yes".

### 7.3 I/O and the system-call boundary

Prefer an asynchronous I/O API whenever the library offers one. `tokio::fs`
covers open, read, write, `sync_all`, `rename`, `create_dir_all` and the rest;
using it means the syscall happens on the blocking pool while the worker stays
free. The same applies to sockets, timers and channels — if there is an async
form, the synchronous form has no reason to exist on a request path.

Some operations have **no** portable async form. The common case is opening a
*directory* to `fsync` it after a rename, which `tokio::fs` cannot do. Those
go through `spawn_blocking` as one bounded job (§7.10) — not inline, and not via
`block_in_place`.

Two rules for this boundary:

- **Do not hand-roll a poll loop, a thread, or a second runtime** to drive
  asynchronous I/O from synchronous code. If the caller can await, make it
  async; if it cannot, `spawn_blocking` the whole synchronous operation once
  rather than reimplementing the reactor.
- **A blocking call inside `spawn_blocking` is fine; an `.await` inside it is
  not.** Hoist the wait out of the closure first (§7.10).

### 7.4 Where a blocking call is legitimate

A blocking call may remain only when **nothing async can absorb it**. Each
instance needs a comment saying why:

1. **The library has no async form and the work is short and bounded.** The
   directory `fsync` above is the example: one syscall, no waiting on another
   task. It goes to the blocking pool.
2. **A synchronous public adapter**, with an explicit assertion that it is off
   the runtime and a panic message that names the failure mode. Its callers
   must be genuinely synchronous: a plain `fn main`, a foreign trait.
3. **The process's single runtime entry**, e.g. a CLI `main` calling
   `block_on_runtime(async { … })` exactly once.
4. **A thread that owns its own runtime**, driving that runtime directly
   (`runtime.block_on(future)` inside the thread that built it). That thread is
   not a pool worker, so nothing else waits on it.
5. **A foreign API forces it** — a synchronous trait method we must implement,
   or a `Drop`. Dispatch the whole operation to `spawn_blocking` so the
   `block_on` runs off the runtime's task context, and say so in a comment.

Before reaching for any of these, check whether the async form exists — in
`tokio::fs`, `tokio::sync`, or an `async fn` we can write ourselves. The order
is:

> **async I/O if the API offers it → `spawn_blocking` if the call is blocking
> and bounded → `block_on` only at a boundary we do not own.**

Reaching for `block_on` first is how a module ends up with three different
bridges over calls that an async API makes trivial.

Anything else is a defect. Prefer `#[cfg(test)]` on a sync wrapper over leaving
it reachable from production.

### 7.5 `Drop`, `spawn_blocking`, `block_on`, and thread pools

Each of these is a tool with a narrow use and a wide misuse. The rule for all
of them is the same: **they move work, they do not make it async.** Choosing one
because it silences a compiler error is how the engine acquired the bridges this
section exists to prevent.

**`Drop`** — cannot await, so it must never be where the async work gets
*finished*.

- A destructor may only do **synchronous cleanup**: close a channel, release a
  handle, drop a queue. Closing a channel is not the same as draining it; state
  that explicitly in a comment, or the next reader will assume the work ran.
- Anything that must complete (flush, drain, stop a worker) belongs in an
  **explicit** `async fn close(&self)` that the caller awaits. Provide it
  whenever the type owns background work; a caller that needs the work done has
  no other correct way to ask.
- Do **not** put `block_on` in `Drop` "for the cases where it works". The
  branch that runs on a runtime worker cannot wait, so the destructor would do
  one thing on one thread and another elsewhere — a difference no caller can
  reason about.
- A `Drop` that only releases resources is fine and common; the rule is about
  `Drop` used as a shutdown hook.

**`spawn_blocking`** — for a call that blocks and has no async form.

- Use it for a bounded, one-shot blocking operation: a syscall the async API
  does not cover, a synchronous database batch, a pure-CPU leaf that would
  otherwise hold a worker.
- Never `.await` inside the closure; hoist the wait out first.
- Do not nest a fan-out inside it, and do not use it to wrap work that is
  already async — that just moves the parking to another thread.
- Do not use it as a substitute for `tokio::fs`. If an async form exists, the
  blocking one has no reason to be on a request path.

**Split a blocking function before you hand it to the pool.** The pool is
finite and shared, and a job holds one of its threads for its whole duration —
so what a job *contains* matters as much as how long it takes. A function that
mixes concerns is a poor job even when each part looks cheap:

- **Separate CPU from I/O.** A function that decodes a block (CPU) and then
  `fsync`s it (I/O) holds a pool thread through both. Under load the I/O half
  sits waiting for the disk while the CPU half sits waiting for a thread,
  and the two queues feed each other. Do the I/O in the caller (awaited) and
  pool only the CPU, or the reverse.
- **Separate I/O from locks.** A job that takes a lock and then reads a file
  makes every other waiter queue behind the disk. Take the lock, copy what is
  needed, release it, then do the I/O.
- **Keep a job's logic flat.** A job that allocates unboundedly, calls back
  into the engine, or fans out again can block on something only a *free*
  thread can produce. That is a deadlock, not a slowdown: the pool has a
  thread cap, and a job that waits for another job on the same saturated pool
  never finishes.
- **Bound the work, not just the wait.** A job that scans a table of unknown
  size holds its thread for an unknown time. Chunk it, or make it a fan-out of
  bounded jobs, so one statement cannot own a pool thread indefinitely.

When a function is hard to split, that is a signal it belongs in a different
layer: a CPU-only core that the pool can call, wrapped by an async function
that owns the I/O and the locks. Prefer a small pure function over a large
mixed one — the same reason the fan-out primitives take a pure-CPU leaf.

**`block_on`** — only at a boundary we do not own (§7.4). It is a statement
that "this thread has nothing better to do", which is true for a `fn main` and
false for a worker.

**Thread pools and work-stealing runtimes we did not build** (rayon, a
hand-rolled pool, a second tokio runtime) — do not introduce them.

- The engine has **one** runtime, and every task must be visible to it. A second
  executor has its own threads, its own blocking semantics, and no way to
  participate in the engine's ordering or its shutdown; work that lands there
  is invisible to `drain`, to the commit order, and to a cancelled statement.
- A parallel loop over independent items is an **async fan-out** (a bounded
  `map_async` / `map_cpu_bound` shape), not a new pool. If the items are pure
  CPU, the fan-out dispatches them to the existing blocking pool; if they
  await, they stay tasks on the engine's runtime.
- A `rayon`-style dependency is not "just for this one hot loop". It brings a
  second scheduler into a process whose whole design is one, and the loop that
  needed it is almost always expressible as the existing fan-out.
- If a genuinely synchronous library requires its own thread, isolate it behind
  one adapter with a comment, and keep it out of the request path — the same
  discipline §7.4 asks of `block_on`.

### 7.6 Nested `block_on` panics

`Runtime::block_on` panics with *"Cannot start a runtime from within a
runtime"*. So an `async fn` that **ends** by parking the thread cannot itself
be driven by `block_on`. That combination is a startup crash, and no unit test
that only awaits will catch it.

- Do not mix "async entry" with "parks its caller" in the same function. Keep
  it sync, or replace the inner park with `.await`.
- A runtime-entry helper should assert it is off the runtime, so the mistake
  fails loudly at the first call rather than inside tokio.

### 7.7 Async trait methods

A handler that takes a borrow (`&Cmd`) and is generic cannot return
`impl Future`: the returned type would have to name a lifetime the compiler
cannot infer. Use an `AsyncFn`-family bound instead:

```rust
pub async fn replay<F>(&self, cmd: &Cmd, mut f: F) -> Result<()>
where
    F: AsyncFnMut(&Cmd) -> Result<()>,
```

A boxed `FnMut -> BoxFuture` works but allocates per call; the `AsyncFn`
traits do not. Break **recursive** `async fn` `Send`-ness cycles (a nested
sub-select) by giving the recursive call a named boxed `Send` future; a
self-referential `async fn` cannot prove its own `Send`.

### 7.8 Locks across `.await`

A `std::sync::MutexGuard` is not `Send`, so holding one across `.await` makes
the whole future non-`Send` and the statement cannot run on a worker.

- Short critical sections (one hash lookup, one map update) stay
  `std::sync::Mutex`; do not convert them to `tokio::sync::Mutex`, which would
  force every synchronous accessor to become async.
- Only a lock genuinely **held across** an `.await` uses `tokio::sync::Mutex`.
- If a guard must span an await, restructure into a block scope or hoist the
  await before the guard is taken. `clippy::await_holding_lock` catches the
  accidental cases; a deliberate process-wide lock needs an `#[allow]` with a
  comment explaining why the serialisation is the point.

A guard held across an await **and** a blocking call inside the same critical
section is worse than either alone: the worker is parked while other tasks
queue on the lock. Fix the blocking call first (§7.1).

### 7.9 Bound the future's size

The statement chain is the largest future in the engine. Inlined into a
generated RPC dispatch or an 8 MiB worker stack it overflows. Box at the choke
points — the outer `execute`, the parsed-statement entry, the service dispatch,
the top of a select — rather than raising stack sizes. Prefer one pointer per
boundary over a bigger stack everywhere.

A conversion that adds a layer of `async` adds a layer of inlining; when a test
that used to pass starts aborting with a stack overflow, the fix is a box at
the boundary, not a smaller stack.

### 7.10 CPU-bound leaves and the blocking pool

`spawn_blocking` and a `map_cpu_bound`-style fan-out are for **pure CPU** that
would otherwise occupy a worker: a batch of row encodings, block decoding, term
encoding, a per-segment graph descent. They are **not** a general "make it
async" wrapper:

- Never `.await` inside a `spawn_blocking` closure — hoist the wait out first.
- Do not nest a fan-out inside a blocking job; the fan-out helper already
  detects an existing runtime and switches to plain tasks.
- A leaf reached from **async** code is an `async fn` and uses the async
  fan-out. A leaf reached from genuinely synchronous code (a build under a
  `std::sync::MutexGuard`) stays synchronous on the calling thread; do not
  reintroduce a synchronous fan-out helper, because it restores exactly the
  parked-worker path §7.2 forbids.
- Move owned data into the job and return it, instead of cloning; the clone can
  cost more than the work you moved.
- **The leaf must be pure CPU — split it if it is not.** A job that mixes
  decoding with `fsync`, or takes a lock and then reads a file, holds a pool
  thread through both and can deadlock against another job on the same
  saturated pool. §7.5 gives the split rules; the fan-out signature enforces
  the shape by taking a synchronous body that cannot await.
- `block_in_place` is **not** an alternative to this list. It keeps the work on
  a worker and merely lets tokio replace that worker, hiding the cost rather
  than removing it. The fix is to make the leaf async (if it awaits) or
  `spawn_blocking` it (if it is pure CPU). Remaining uses are debt, not a
  pattern.

### 7.11 Ordering invariants

WAL frames, row-id allocation and same-table bottom halves all depend on **one
consumer over one FIFO**. Keep that shape when converting a thread to a task;
never "parallelise" it into multiple consumers. A lock that serialises writers
may be held across awaits (a `tokio::sync::Mutex`), but the ordering it
enforces must stay visible in the code — one writer, one order.

When an async conversion touches an ordering path, state the invariant in a
comment at the point where it is enforced. The next conversion will read that
comment, not the history.

### 7.12 Before claiming a conversion is done

- `cargo check --workspace --all-targets`
- `cargo test --workspace` (default stack; `RUST_MIN_STACK` should not be
  needed)
- `cargo clippy --workspace --all-targets`
- `cargo fmt --all`
- **Start the server and run a query.** Unit tests cannot catch a nested
  `block_on` or a startup ordering mistake; the crash only shows when the front
  ends boot.
- Update this section if a rule turned out to be wrong, and say so rather than
  silently overwriting it.

### 7.13 Delete dead code instead of making it compile

When an async conversion breaks a call site, check whether that call site has a
caller **before** adapting it. Code kept alive only by mechanical fixes — a
thread pool with no production user, a synchronous handler chain whose top has
zero callers, a fan-out helper nobody calls — is not part of the design any
more.

"Make it compile" is the wrong default; "does this still belong here" is the
maintenance question. Deleting is part of the conversion, not a follow-up.
