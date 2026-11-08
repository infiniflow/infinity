//! `infinity-catalog`: metadata built on top of the [`MetaStore`] KV substrate
//! (plan §2.2 / §6). Tables get monotonic ids from a persisted counter; a name
//! index maps name -> id; column names are stored with the table.

#![forbid(unsafe_code)]

pub mod layout;
/// Versioned metadata cache (C++ `MetaCache`).
pub mod meta_cache;
/// C++-compatible value encoding for catalog records (spec §8.6).
pub mod value_format;

use std::collections::HashMap;
use std::sync::{Arc, Mutex};

use common::{Error, Result};
use storage::{MetaStore, RocksMetaStore};

const COUNTER_KEY: &[u8] = b"__counter:table";

/// Placeholder for "this record is created by the DDL in flight".
///
/// The C++ catalog keys carry the creating transaction's commit timestamp
/// (`KeyEncode::CatalogDbKey(db_name, commit_ts)`), and that value only exists
/// once the transaction commits. A DDL therefore stamps its new records with
/// this sentinel and the caller replaces it with the reserved commit timestamp
/// before the batch is written ([`Catalog::restamp`]). It cannot collide with a
/// real timestamp: the clock is a `u64` that increments by two.
pub const DDL_TS_SENTINEL: u64 = u64::MAX;

/// The storage hint of a `SPARSE` column: `WITH (SORT)` / `WITH (SORTED)`.
///
/// C++: `SparseStoreType` (`parser/type/info/sparse_info.h:29-33`). Only
/// `SPARSE` columns carry one; every other column stores `None`.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum SparseStore {
    /// `WITH (SORT)` / the default: values sorted row-wise on write.
    Sort,
    /// `WITH (SORTED)`: the column is declared pre-sorted.
    Sorted,
}

impl SparseStore {
    /// The C++ `SparseStoreType` ordinal (`sparse_info.h:29-33`).
    pub fn as_i64(self) -> i64 {
        match self {
            Self::Sort => 0,
            Self::Sorted => 1,
        }
    }

    fn encode(self) -> &'static str {
        match self {
            Self::Sort => "0",
            Self::Sorted => "1",
        }
    }

    fn decode(s: &str) -> Option<Self> {
        match s {
            "" => None,
            "0" | "SORT" => Some(Self::Sort),
            "1" | "SORTED" => Some(Self::Sorted),
            _ => None,
        }
    }
}

/// Splits a trailing `WITH (SORT|SORTED)` hint off a declared type text, so a
/// type stream can carry the sparse storage hint without a side channel.
///
/// `SPARSE(FLOAT,30000) WITH (SORTED)` -> (`SPARSE(FLOAT,30000)`, `Sorted`).
/// Anything else is returned unchanged with `None`.
pub fn split_sparse_store_suffix(type_text: &str) -> (&str, Option<SparseStore>) {
    let trimmed = type_text.trim_end();
    let up = trimmed.to_ascii_uppercase();
    let Some(pos) = up.rfind("WITH") else {
        return (trimmed, None);
    };
    let tail = up[pos..].replace(char::is_whitespace, "");
    match tail.as_str() {
        "WITH(SORT)" => (trimmed[..pos].trim_end(), Some(SparseStore::Sort)),
        "WITH(SORTED)" => (trimmed[..pos].trim_end(), Some(SparseStore::Sorted)),
        _ => (trimmed, None),
    }
}

/// Metadata of a table.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct TableMeta {
    pub id: u64,
    pub name: String,
    pub columns: Vec<String>,
    /// Per-column default as its `.slt` text form (`None` = no default).
    pub defaults: Vec<Option<String>>,
    /// Per-column `NOT NULL` flag.
    pub not_nulls: Vec<bool>,
    /// Per-column declared type text (e.g. `TENSOR(FLOAT,4)`).
    pub types: Vec<String>,
    /// Per-column `COMMENT '…'`.
    pub comments: Vec<Option<String>>,
    /// Per-column ids. C++ assigns a stable id when a column is created and
    /// never reuses it, so `DROP COLUMN` leaves a hole (the block files are
    /// named `<id>.col`).
    pub ids: Vec<u64>,
    /// Per-column sparse storage hint (`None` for non-sparse columns and for
    /// old catalogs written before the hint was persisted).
    pub sparse_stores: Vec<Option<SparseStore>>,
}

impl TableMeta {
    /// This table pinned to the columns whose ids are `ids`, in the given order.
    ///
    /// The checkpoint writes the schema **as of its bound** (`NewTxn::Checkpoint`
    /// reads `GetColumnDefs` at its begin ts), so a column a later transaction
    /// added must not take part in that write: its cells do not exist yet.
    pub fn at_ids(&self, ids: &[u64]) -> TableMeta {
        let mut pinned = self.clone();
        pinned.columns.clear();
        pinned.defaults.clear();
        pinned.not_nulls.clear();
        pinned.types.clear();
        pinned.comments.clear();
        pinned.sparse_stores.clear();
        pinned.ids.clear();
        for id in ids {
            let Some(at) = self.ids.iter().position(|current| current == id) else {
                continue;
            };
            pinned.columns.push(self.columns[at].clone());
            pinned
                .defaults
                .push(self.defaults.get(at).cloned().flatten());
            pinned
                .not_nulls
                .push(self.not_nulls.get(at).copied().unwrap_or(false));
            pinned.types.push(self.types[at].clone());
            pinned
                .comments
                .push(self.comments.get(at).cloned().flatten());
            pinned
                .sparse_stores
                .push(self.sparse_stores.get(at).cloned().flatten());
            pinned.ids.push(*id);
        }
        pinned
    }
}

/// Field/record separators for the encoded table metadata.
const FSEP: char = '\u{1f}';
const RSEP: char = '\u{1e}';

impl TableMeta {
    pub fn column_count(&self) -> usize {
        self.columns.len()
    }

    fn encode(&self) -> Vec<u8> {
        let cols: Vec<String> = self
            .columns
            .iter()
            .enumerate()
            .map(|(i, c)| {
                // `\u{1}` marks "a default is present" so that `DEFAULT ''`
                // survives the round trip (an empty field means "no default").
                let d = match self.defaults.get(i).cloned().flatten() {
                    Some(v) => format!("\u{1}{v}"),
                    None => String::new(),
                };
                let n = if self.not_nulls.get(i).copied().unwrap_or(false) {
                    "1"
                } else {
                    "0"
                };
                let ty = self.types.get(i).cloned().unwrap_or_default();
                let cm = self.comments.get(i).cloned().flatten().unwrap_or_default();
                let cid = self.ids.get(i).copied().unwrap_or(i as u64);
                // Appended field: old records simply lack it, which decodes as
                // "no sparse hint".
                let ss = self
                    .sparse_stores
                    .get(i)
                    .copied()
                    .flatten()
                    .map(SparseStore::encode)
                    .unwrap_or_default();
                format!("{c}{FSEP}{d}{FSEP}{n}{FSEP}{ty}{FSEP}{cm}{FSEP}{cid}{FSEP}{ss}")
            })
            .collect();
        format!("{}{RSEP}{}", self.name, cols.join(&RSEP.to_string())).into_bytes()
    }

    fn decode(id: u64, bytes: &[u8]) -> Result<Self> {
        let s = String::from_utf8_lossy(bytes);
        let (name, cols) = s
            .split_once(RSEP)
            .ok_or_else(|| Error::Catalog(format!("bad table meta for id {id}")))?;
        let mut columns = Vec::new();
        let mut defaults = Vec::new();
        let mut not_nulls = Vec::new();
        let mut types = Vec::new();
        let mut comments = Vec::new();
        let mut ids = Vec::new();
        let mut sparse_stores = Vec::new();
        for part in cols.split(RSEP).filter(|c| !c.is_empty()) {
            let mut f = part.split(FSEP);
            let n = f.next().unwrap_or(part);
            let d = f.next().unwrap_or("");
            let nn = f.next().unwrap_or("0");
            let ty = f.next().unwrap_or("");
            let cm = f.next().unwrap_or("");
            let cid = f.next().and_then(|v| v.parse::<u64>().ok());
            let ss = f.next().map(SparseStore::decode).unwrap_or(None);
            columns.push(n.to_string());
            defaults.push(match d.strip_prefix('\u{1}') {
                Some(rest) => Some(rest.to_string()),
                None if d.is_empty() => None,
                None => Some(d.to_string()),
            });
            not_nulls.push(nn == "1");
            types.push(ty.to_string());
            comments.push(if cm.is_empty() {
                None
            } else {
                Some(cm.to_string())
            });
            ids.push(cid.unwrap_or(ids.len() as u64));
            sparse_stores.push(ss);
        }
        Ok(Self {
            id,
            name: name.to_string(),
            columns,
            defaults,
            not_nulls,
            types,
            comments,
            ids,
            sparse_stores,
        })
    }
}

/// Strips any `WITH (SORT|SORTED)` suffix from each declared type and returns
/// the bare type texts plus the per-column hint. An explicit hint wins over
/// the one encoded in the text.
fn normalize_types(
    types: &[String],
    sparse_stores: &[Option<SparseStore>],
    len: usize,
) -> (Vec<String>, Vec<Option<SparseStore>>) {
    (0..len)
        .map(|i| {
            let raw = types.get(i).map(String::as_str).unwrap_or("");
            let (bare, from_text) = split_sparse_store_suffix(raw);
            let hint = sparse_stores.get(i).copied().flatten().or(from_text);
            (bare.to_string(), hint)
        })
        .unzip()
}

fn table_key(id: u64) -> Vec<u8> {
    format!("tbl:{id:016}").into_bytes()
}
fn name_key(name: &str) -> Vec<u8> {
    format!("tblname:{name}").into_bytes()
}
fn db_key(name: &str) -> Vec<u8> {
    format!("db:{name}").into_bytes()
}

const INDEX_PREFIX: &[u8] = b"idx:";

/// C++ `NEXT_DATABASE_ID` (`default_values.cppm:341`): the last database id
/// handed out. It lives in the same KV as the reference's, so a directory the
/// reference wrote continues its sequence.
const NEXT_DATABASE_ID_KEY: &[u8] = b"next_database_id";

/// Metadata of an index.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct IndexMeta {
    pub name: String,
    pub table: String,
    pub column: String,
    /// `CREATE INDEX … USING <kind>`, upper-cased (`HNSW`, `FULLTEXT`, …);
    /// empty when the statement did not name one.
    pub index_type: String,
    /// `WITH (…)` options, keys upper-cased as the parser yields them.
    pub options: Vec<(String, String)>,
    /// The C++ index id: a per-table counter starting at 1, never reused, so
    /// `DROP INDEX` leaves a hole (`idx_{id}` directories).
    pub id: u64,
}

/// Separates the stored option pairs; `\x1f` (unit separator) cannot occur in
/// a parsed identifier/value.
const OPT_SEP: char = '\x1f';

impl IndexMeta {
    fn encode(&self) -> Vec<u8> {
        let opts = self
            .options
            .iter()
            .map(|(k, v)| format!("{k}={v}"))
            .collect::<Vec<_>>()
            .join(&OPT_SEP.to_string());
        format!(
            "{}\t{}\t{}\t{}\t{}",
            self.table, self.column, self.index_type, opts, self.id
        )
        .into_bytes()
    }
    fn decode(name: &[u8], bytes: &[u8]) -> Result<Self> {
        let key = String::from_utf8_lossy(name).to_string();
        // Index names are scoped per table, so the key carries
        // `{table}\x1f{name}`; only the part after the separator is the name.
        let name = key
            .rsplit('\x1f')
            .next()
            .unwrap_or(key.as_str())
            .to_string();
        let s = String::from_utf8_lossy(bytes);
        // Older records carried just `table\tcolumn`; newer ones append the
        // index type and the `WITH` options.
        let mut it = s.splitn(5, '\t');
        let table = it
            .next()
            .ok_or_else(|| Error::Catalog(format!("bad index meta for '{name}'")))?;
        let column = it
            .next()
            .ok_or_else(|| Error::Catalog(format!("bad index meta for '{name}'")))?;
        let index_type = it.next().unwrap_or_default().to_string();
        let options: Vec<(String, String)> = it
            .next()
            .unwrap_or_default()
            .split(OPT_SEP)
            .filter(|p| !p.is_empty())
            .filter_map(|p| p.split_once('='))
            .map(|(k, v)| (k.to_string(), v.to_string()))
            .collect();
        let id = it.next().and_then(|v| v.parse().ok()).unwrap_or(0);
        Ok(Self {
            name,
            table: table.to_string(),
            column: column.to_string(),
            index_type,
            options,
            id,
        })
    }
}

/// Index names are scoped to a table (the reference allows the same name on
/// two tables), so the key carries both parts.
fn index_key(table: &str, name: &str) -> Vec<u8> {
    format!("idx:{table}\x1f{name}").into_bytes()
}

/// Catalog façade over a `MetaStore`.
pub struct Catalog {
    store: Arc<dyn MetaStore>,
    counter_lock: Mutex<()>,
    /// Persist **only** the C++ key space (plan §2.6.1 item ④). The native
    /// `tbl:`/`tblname:`/`db:`/`idx:` keys are then never written, so the store
    /// holds nothing but what a C++ engine can read.
    catalog_only: bool,
    /// When a DDL captures its batch ([`Self::capture`]), newly created records
    /// are stamped with [`DDL_TS_SENTINEL`] and the caller substitutes the
    /// transaction's commit timestamp with [`Self::restamp`].
    capture: Mutex<Option<Capture>>,
    /// Decoded table records, keyed by qualified name.
    ///
    /// Resolving a name reads the catalog store and decodes the record, which
    /// costs ~14 us; one query resolves its table's schema about nine times
    /// (scan, index catalog, declared types, row reader, ...), so the engine
    /// spent more time decoding the same record than scanning its rows. Every
    /// path that changes a table clears this first, so a hit answers what a
    /// re-read would.
    table_cache: Mutex<std::collections::HashMap<String, TableMeta>>,
    /// How many DDL calls are in flight. A DDL reads the table it is about to
    /// change and then writes it, so both a cache filled before the call and
    /// one filled *during* it would be stale afterwards: reads bypass the
    /// cache while this is non-zero, and the guard clears it again on exit.
    ddl_depth: std::sync::atomic::AtomicUsize,
    /// Bumped on every invalidation. A read that began before one must not
    /// publish what it read: the record it holds may predate the change that
    /// caused the invalidation.
    ddl_epoch: std::sync::atomic::AtomicU64,
}

/// Ends a DDL call: leaves the depth and drops what was cached while it ran.
struct DdlGuard<'a> {
    catalog: &'a Catalog,
}

impl Drop for DdlGuard<'_> {
    fn drop(&mut self) {
        self.catalog
            .ddl_depth
            .fetch_sub(1, std::sync::atomic::Ordering::SeqCst);
        self.catalog.invalidate_tables();
    }
}

/// The C++ key-space prefixes this catalog owns. Sweeping them is how the
/// [`Catalog`]'s C++-only mode removes everything a drop would otherwise leave
/// behind (`catalog_format.rs`).
const CPP_KEY_PREFIXES: [&str; 5] = ["catalog|", "db|", "tbl|", "col|", "idx|"];

/// One entry of a catalog commit: `Some(value)` writes the key, `None` deletes
/// it (`MetaStore::atomic_batch`).
pub type BatchItem = (Vec<u8>, Option<Vec<u8>>);

/// A DDL's writes while they are still in flight.
///
/// `batch` is what the caller commits; `view` is the same writes as a
/// read-through map, so a statement that performs **several** catalog
/// mutations (`DROP TABLE` removes the table and then its index records,
/// `DROP DATABASE` drops every table it holds) reads the state its own earlier
/// step produced instead of the store's stale one. Without it the second step
/// would rewrite the layout from the pre-step records and resurrect what the
/// first step removed.
#[derive(Default)]
struct Capture {
    batch: Vec<BatchItem>,
    view: HashMap<Vec<u8>, Option<Vec<u8>>>,
}

/// The store as the DDL in flight sees it: the captured writes overlay the
/// committed ones. Only reads go through here; a write inside a capture is
/// already recorded by [`Catalog::rewrite_catalog`].
struct Overlay<'a> {
    inner: &'a dyn MetaStore,
    view: &'a HashMap<Vec<u8>, Option<Vec<u8>>>,
}

impl MetaStore for Overlay<'_> {
    fn get(&self, key: &[u8]) -> Result<Option<Vec<u8>>> {
        match self.view.get(key) {
            Some(value) => Ok(value.clone()),
            None => self.inner.get(key),
        }
    }

    fn put(&self, key: &[u8], value: &[u8]) -> Result<()> {
        self.inner.put(key, value)
    }

    fn delete(&self, key: &[u8]) -> Result<()> {
        self.inner.delete(key)
    }

    fn scan_prefix(&self, prefix: &[u8]) -> Result<Vec<(Vec<u8>, Vec<u8>)>> {
        let mut rows: Vec<(Vec<u8>, Vec<u8>)> = self
            .inner
            .scan_prefix(prefix)?
            .into_iter()
            .filter(|(key, _)| !self.view.contains_key(key))
            .collect();
        for (key, value) in self.view {
            if let (true, Some(value)) = (key.starts_with(prefix), value) {
                rows.push((key.clone(), value.clone()));
            }
        }
        // `MetaStore::scan_prefix` returns key order and the layout readers
        // rely on it.
        rows.sort_by(|a, b| a.0.cmp(&b.0));
        Ok(rows)
    }

    fn flush(&self) -> Result<()> {
        self.inner.flush()
    }
}

/// Keeps only the last operation on each key of a batch.
///
/// A C++-layout rewrite deletes every key the layout owns and then writes the
/// live ones back, so the same key can appear as a delete and a put in one
/// batch. Collapsing duplicates makes the outcome (last write wins)
/// independent of how a backend orders operations inside one commit.
fn last_op_per_key(mut items: Vec<BatchItem>) -> Vec<BatchItem> {
    let mut seen: std::collections::HashSet<Vec<u8>> = Default::default();
    let mut out: Vec<BatchItem> = Vec::with_capacity(items.len());
    for item in items.drain(..).rev() {
        if seen.insert(item.0.clone()) {
            out.push(item);
        }
    }
    out.reverse();
    out
}

impl Catalog {
    pub fn with_store(store: Arc<dyn MetaStore>) -> Self {
        Self {
            store,
            counter_lock: Mutex::new(()),
            catalog_only: false,
            capture: Mutex::new(None),
            table_cache: Mutex::new(std::collections::HashMap::new()),
            ddl_depth: std::sync::atomic::AtomicUsize::new(0),
            ddl_epoch: std::sync::atomic::AtomicU64::new(0),
        }
    }

    /// A catalog whose **sole** persisted representation is the C++ key space
    /// (plan §2.6.1 item ④).
    pub fn with_store_catalog_only(store: Arc<dyn MetaStore>) -> Self {
        Self {
            store,
            counter_lock: Mutex::new(()),
            catalog_only: true,
            capture: Mutex::new(None),
            table_cache: Mutex::new(std::collections::HashMap::new()),
            ddl_depth: std::sync::atomic::AtomicUsize::new(0),
            ddl_epoch: std::sync::atomic::AtomicU64::new(0),
        }
    }

    /// Enters a DDL call: reads stop using the cache and the cache is dropped
    /// both now and when the returned guard goes out of scope.
    fn begin_ddl(&self) -> DdlGuard<'_> {
        self.ddl_depth
            .fetch_add(1, std::sync::atomic::Ordering::SeqCst);
        self.invalidate_tables();
        DdlGuard { catalog: self }
    }

    /// Drops every cached table record and makes in-flight reads decline to
    /// publish theirs.
    ///
    /// A DDL calls this twice: entering it (so the call does not read a record
    /// from before its own change) and again after the commit lands. The second
    /// one is what matters for correctness: a DDL method returns *before* its
    /// transaction commits, and a reader in that window would otherwise
    /// repopulate the cache with the pre-commit schema, which the commit then
    /// makes permanently stale.
    pub fn invalidate_tables(&self) {
        let mut cache = self.table_cache.lock().unwrap();
        self.ddl_epoch
            .fetch_add(1, std::sync::atomic::Ordering::SeqCst);
        cache.clear();
    }

    /// Whether this catalog persists only the C++ key space.
    pub fn is_catalog_only(&self) -> bool {
        self.catalog_only
    }

    pub fn open(path: impl AsRef<std::path::Path>) -> Result<Self> {
        let store = RocksMetaStore::open(path)?;
        Ok(Self::with_store(Arc::new(store)))
    }

    /// Like [`Self::open`] but with the C++ key space as the sole catalog
    /// (plan §2.6.1 item ④).
    pub fn open_catalog_only(path: impl AsRef<std::path::Path>) -> Result<Self> {
        let store = RocksMetaStore::open(path)?;
        Ok(Self::with_store_catalog_only(Arc::new(store)))
    }

    pub fn store(&self) -> &Arc<dyn MetaStore> {
        &self.store
    }

    /// Databases recorded in the C++ key space. An empty database still has a
    /// `catalog|db|{name}|{ts}` record, so the scan is authoritative.
    fn databases(&self) -> Result<Vec<String>> {
        let mut out = vec!["default_db".to_string()];
        for name in self.database_records()? {
            if !out.contains(&name) {
                out.push(name);
            }
        }
        Ok(out)
    }

    /// The databases that actually **have a record** in the C++ key space.
    ///
    /// `default_db` is created by the reference's startup transaction
    /// (`Storage::CreateDefaultDB`, only when `system_start_ts == 0`), so it is
    /// not implied: a directory that never ran that transaction has no record
    /// for it, and `CREATE DATABASE` has to write one.
    fn database_records(&self) -> Result<Vec<String>> {
        let mut out: Vec<String> = Vec::new();
        for (k, _) in self.with_view(|store| store.scan_prefix(b"catalog|db|"))? {
            let text = String::from_utf8_lossy(&k).to_string();
            if let Some(name) = text.split('|').nth(2) {
                if !out.iter().any(|d| d == name) {
                    out.push(name.to_string());
                }
            }
        }
        // A record a `drop|db|…` marker deprecated is not a database
        // (`CatalogMeta::GetDBMeta` reports the name as gone; only `Cleanup`
        // removes the record).
        let mut live = Vec::with_capacity(out.len());
        for name in out {
            if self.has_database_record(&name)? {
                live.push(name);
            }
        }
        Ok(live)
    }

    /// Runs `f` against the store as the DDL in flight sees it: either the
    /// store itself, or the store with the captured writes applied.
    fn with_view<T>(&self, f: impl FnOnce(&dyn MetaStore) -> Result<T>) -> Result<T> {
        let guard = self.capture.lock().unwrap();
        match guard.as_ref() {
            Some(capture) => f(&Overlay {
                inner: self.store.as_ref(),
                view: &capture.view,
            }),
            None => f(self.store.as_ref()),
        }
    }

    fn tables(&self) -> Result<Vec<TableMeta>> {
        // A table hanging under a database `drop|db|…` deprecated is gone with
        // it: the reference's `CleanDB` walks the database's tables, and a
        // reader resolves the name against the live databases first.
        let live = self.databases()?;
        let mut tables = self.with_view(|store| crate::layout::read_tables_as_meta(store))?;
        tables.retain(|t| {
            let db = t.name.split('.').next().unwrap_or(&t.name).to_string();
            live.contains(&db)
        });
        Ok(tables)
    }

    fn indexes(&self) -> Result<Vec<IndexMeta>> {
        self.with_view(|store| {
            let tables = crate::layout::read_layout(store)?;
            let mut out = Vec::new();
            for def in crate::layout::read_indexes(store)? {
                if let Some(meta) = crate::layout::index_meta_of(&def, &tables) {
                    out.push(meta);
                }
            }
            // A record read back from the C++ key space carries the id the
            // reference handed out; a record without one (a hand-written one)
            // is numbered per table (1-based, as `CREATE INDEX` would) -
            // skipping the ids the other records already hold, so the fallback
            // cannot hand two indexes the same id.
            let mut next: HashMap<String, u64> = HashMap::new();
            let mut taken: HashMap<String, std::collections::HashSet<u64>> = HashMap::new();
            for im in out.iter() {
                if im.id > 0 {
                    taken.entry(im.table.clone()).or_default().insert(im.id);
                }
            }
            for im in out.iter_mut() {
                if im.id == 0 {
                    let used = taken.entry(im.table.clone()).or_default();
                    let n = next.entry(im.table.clone()).or_insert(0);
                    loop {
                        *n += 1;
                        if !used.contains(n) {
                            break;
                        }
                    }
                    im.id = *n;
                    used.insert(*n);
                }
            }
            // Name-only indexes — the engine does not materialise an expression
            // target such as `c1 + 5`, so it has no C++ `IndexBase`; its name
            // lives under a supplementary table tag (`write_layout_from`).
            for t in &tables {
                let tag = storage::catalog_format::catalog_table_tag_key(
                    &t.db_id.to_string(),
                    &t.table_id.to_string(),
                    "rust_name_only_indexes",
                );
                let Some(bytes) = store.get(tag.as_bytes())? else {
                    continue;
                };
                for name in String::from_utf8_lossy(&bytes).split(',') {
                    let name = name.trim();
                    if !name.is_empty() {
                        out.push(IndexMeta {
                            name: name.to_string(),
                            table: format!("{}.{}", t.db_name, t.table_name),
                            column: String::new(),
                            index_type: String::new(),
                            options: Vec::new(),
                            id: 0,
                        });
                    }
                }
            }
            Ok(out)
        })
    }

    /// Republished the whole C++ layout from `dbs`/`tables`/`indexes` after
    /// first deleting every record the layout owns, so a dropped entity cannot
    /// survive as a stale key.
    ///
    /// The deletes and the rewritten records reach the store as **one**
    /// `atomic_batch`: a crash between the sweep and the rewrite would leave a
    /// partially emptied `catalog|`/`tbl|`/`col|` layout, which is exactly the
    /// bytes the C++ engine reads on the next open.
    fn rewrite_catalog(
        &self,
        dbs: &[String],
        tables: &[TableMeta],
        indexes: &[IndexMeta],
    ) -> Result<()> {
        // The sweep covers the `catalog|` prefix, which holds the segment and
        // block records of the block mirror: read them first so a DDL republishes
        // them unchanged instead of leaving the C++ reader with a segmentless
        // table.
        let segments = crate::layout::read_segments(self.store.as_ref())?;
        // While a DDL is in flight the layout is computed from what that DDL
        // has already written, not from the store: dropping a database rewrites
        // the layout once per table it holds, and each rewrite must see the
        // previous one.
        let mut slot = self.capture.lock().unwrap();
        let (sweep, layout) = match slot.as_ref() {
            Some(capture) => {
                let view = Overlay {
                    inner: self.store.as_ref(),
                    view: &capture.view,
                };
                (
                    self.layout_sweep(&view)?,
                    self.write_layout_batch_from(&view, dbs, tables, indexes, &segments)?,
                )
            }
            None => (
                self.layout_sweep(self.store.as_ref())?,
                self.write_layout_batch(dbs, tables, indexes, &segments)?,
            ),
        };
        let batch = last_op_per_key([sweep, layout].concat());
        // A DDL under a transaction hands its writes to the caller
        // (`Engine::ddl_commit`), which writes them together with the WAL record
        // that carries the commit timestamp this layout is stamped with.
        if let Some(capture) = slot.as_mut() {
            for (key, value) in &batch {
                capture.view.insert(key.clone(), value.clone());
            }
            capture.batch.extend(batch);
            return Ok(());
        }
        drop(slot);
        self.store.atomic_batch(&batch)
    }

    /// Runs `f` with this catalog's C++-layout writes collected instead of
    /// applied, returning them so the caller can commit them through a
    /// transaction.
    ///
    /// Reads inside `f` see the writes `f` already made ([`Self::with_view`]),
    /// so a statement that mutates the catalog more than once sees its own
    /// effects. Only one DDL runs at a time (`Engine::ddl_commit` holds the
    /// lock), so a single slot is enough; a nested capture is a programming
    /// error and is reported rather than silently nesting.
    pub fn capture<T>(&self, f: impl FnOnce() -> Result<T>) -> Result<(T, Vec<BatchItem>)> {
        {
            let mut slot = self.capture.lock().unwrap();
            if slot.is_some() {
                return Err(Error::Catalog("nested catalog capture".into()));
            }
            *slot = Some(Capture::default());
        }
        let out = f();
        // A native catalog's `f` wrote the catalog's own key space; the C++
        // layout it describes is what the typed WAL command is derived from
        // (`create_table_command_from_batch` and friends read `catalog|tbl|`,
        // `tbl|col|` and `catalog|idx|`), so mirror it inside this capture.
        // C++-only mode already rewrote it inside `f`; a checkpoint mirrors it
        // too, which is what covers a directory no DDL touched.
        if out.is_ok() && !self.catalog_only {
            // Best effort, like the checkpoint's: a column type the C++ encoding
            // does not cover yet leaves the previous layout in place rather than
            // failing the DDL (the next checkpoint reports it).
            let _ = self
                .list_databases()
                .and_then(|dbs| Ok((dbs, self.list_tables()?, self.list_indexes()?)))
                .and_then(|(dbs, tables, indexes)| self.rewrite_catalog(&dbs, &tables, &indexes));
        }
        let capture = self.capture.lock().unwrap().take().unwrap_or_default();
        Ok((out?, last_op_per_key(capture.batch)))
    }

    /// Replaces the DDL sentinel in every **newly written** key with `ts`, the
    /// commit timestamp the caller reserved for the transaction.
    ///
    /// Only the puts are re-stamped: a sentinel names a record this DDL creates.
    /// A **delete** names a record that already exists, so its key carries that
    /// record's *real* commit ts — the sweep that drops a stale layout deletes
    /// such keys, and re-stamping one would aim it at a key that does not exist,
    /// leaving the real record behind. (This is why a `DROP TABLE` after a
    /// restore wrote sentinel-ts keys used to delete nothing.)
    pub fn restamp(batch: &mut [BatchItem], ts: u64) {
        let from = format!("|{DDL_TS_SENTINEL}");
        let to = format!("|{ts}");
        for (key, value) in batch.iter_mut() {
            // Skip deletions: their key is the existing record's, already stamped.
            if value.is_none() {
                continue;
            }
            // A `drop|…` marker carries its timestamp in the **value**
            // (`KeyEncode::DropTableKey(…) -> to_string(commit_ts)`), and the
            // key names the object's create ts - restamping the key would move
            // the marker off the record it deprecates.
            if key.starts_with(b"drop|") {
                *value = Some(ts.to_string().into_bytes());
                continue;
            }
            if let Ok(text) = std::str::from_utf8(key) {
                if text.ends_with(&from) {
                    let mut replaced = text[..text.len() - from.len()].to_string();
                    replaced.push_str(&to);
                    *key = replaced.into_bytes();
                }
            }
        }
    }

    /// Republishes the whole C++ layout from the **current** native catalog,
    /// deleting every record the layout owns first. A checkpoint must call
    /// this (not [`Self::write_layout`]) so a dropped database, table or
    /// index cannot survive as a stale key that `import_layout` would
    /// resurrect on the next open. Like [`Self::rewrite_catalog`], the sweep and
    /// the rewrite are one commit.
    pub fn rewrite_layout(&self, store: &dyn MetaStore) -> Result<usize> {
        let _ddl = self.begin_ddl();
        let dbs = self.list_databases()?;
        let tables = self.list_tables()?;
        let indexes = self.list_indexes()?;
        let segments = crate::layout::read_segments(store)?;
        let layout = self.write_layout_batch(&dbs, &tables, &indexes, &segments)?;
        let written = layout.len();
        let mut batch = self.layout_sweep(store)?;
        batch.extend(layout);
        store.atomic_batch(&last_op_per_key(batch))?;
        Ok(written)
    }

    /// Publishes the segment layer of a block mirror the engine just wrote
    /// (`catalog|seg|…`, `catalog|blk|…`, `first_delete_ts`, and the table's
    /// `next_segment_id` / `unsealed_segment_id`).
    ///
    /// A targeted write: nothing is swept, so a checkpoint can publish blocks
    /// after [`Self::rewrite_layout`] (or, in C++-only mode, after the DDL
    /// that created the mirror).
    pub fn publish_segments(&self, set: &crate::layout::SegmentSet) -> Result<usize> {
        let _ddl = self.begin_ddl();
        crate::layout::write_segments(self.store.as_ref(), set)
    }

    /// Publishes the sealed-chunk layer of the table's index mirrors
    /// (`idx_seg|…`, `idx_seg|…|next_chunk_id`, `idx_chunk|…|chunk_info`), so the
    /// reference knows the segment's rows are already indexed and does not try
    /// to rebuild them into its memory index at startup.
    pub fn publish_index_segments(
        &self,
        set: &crate::layout::SegmentSet,
        indexes: &[crate::layout::IndexSegment],
    ) -> Result<usize> {
        // Same as the segment marks: the dump's command carries the chunk
        // records, so replay re-creates them.
        crate::layout::write_index_segments(self.store.as_ref(), set, indexes)
    }

    /// Marks `segments` as deprecated at `ts` (a compaction's commit ts), for
    /// the table and for each of `index_ids`.
    pub fn drop_segments(
        &self,
        set: &crate::layout::SegmentSet,
        index_ids: &[u64],
        segments: &[u32],
        ts: u64,
    ) -> Result<usize> {
        // No flush: the drop marks ride in the compaction's WAL command and are
        // replayed by `wal::replay`; the store is flushed at the checkpoint.
        crate::layout::write_drops(self.store.as_ref(), set, index_ids, segments, ts)
    }

    /// The segments of `db_id`.`table_id` a compaction deprecated.
    ///
    /// Read through the DDL-in-flight's view, like every other layout read: a
    /// transaction that marks the segments and then asks what is visible (a
    /// `Cleanup` in the same transaction, a read right after `COMPACT`) has to
    /// see its own marks, or it treats the deprecated source segments as live
    /// and reads their rows a second time next to the merged segment's.
    pub fn dropped_segments(&self, db_id: &str, table_id: &str) -> Result<Vec<u32>> {
        self.with_view(|store| crate::layout::dropped_segments(store, db_id, table_id))
    }

    /// The deletes a C++-layout rewrite performs before writing the layout.
    ///
    /// A `drop|…` marker deprecates records without removing them - that is
    /// `Cleanup`'s job - so the sweep has to leave them alone: republishing the
    /// layout is what a `DROP TABLE t; CREATE TABLE t` does, and deleting the
    /// dropped table's records here would give the directory a key space the
    /// reference never has.
    fn layout_sweep(&self, store: &dyn MetaStore) -> Result<Vec<BatchItem>> {
        let mut deletes: Vec<BatchItem> = Vec::new();
        let keep = crate::layout::dropped_record_prefixes(store)?;
        for prefix in CPP_KEY_PREFIXES {
            for (k, _) in store.scan_prefix(prefix.as_bytes())? {
                let key = String::from_utf8_lossy(&k);
                if keep.iter().any(|p| key.starts_with(p.as_str())) {
                    continue;
                }
                deletes.push((k, None));
            }
        }
        for k in ["next_database_id", "next_table_id", "meta|version"] {
            deletes.push((k.as_bytes().to_vec(), None));
        }
        Ok(deletes)
    }

    fn read_counter(&self) -> Result<u64> {
        Ok(self
            .store
            .get(COUNTER_KEY)?
            .map(|b| u64::from_le_bytes(b[..8].try_into().unwrap()))
            .unwrap_or(0))
    }

    /// The next table id, plus the counter value that must be committed with it.
    ///
    /// The caller holds `counter_lock` and commits both in one batch, so the
    /// counter can never run ahead of the table record (`alloc_id` used to
    /// commit it on its own, leaving a gap if the process died right after).
    fn reserve_table_id(&self, table: &str) -> Result<(u64, u64)> {
        if self.catalog_only {
            // Table ids are **per database** (`DBMeta::GetNextTableID` on the
            // database's own `next_table_id` tag), and the counter only ever
            // grows: a dropped table's id is never handed out again, or the new
            // table would inherit its block directory.
            let db_name = table.split('.').next().unwrap_or(table);
            let db_id = self.database_id_of_name(db_name)?.unwrap_or(1).to_string();
            let tag = storage::catalog_format::catalog_db_tag_key(&db_id, "next_table_id");
            let previous = self
                .with_view(|store| store.get(tag.as_bytes()))?
                .and_then(|v| String::from_utf8_lossy(&v).parse::<u64>().ok());
            // The tag is the counter a CREATE TABLE keeps, and it only grows. A
            // directory without it (one this port never stamped) falls back to
            // the live ids, which costs a whole-catalog read.
            let next = match previous {
                Some(previous) => previous,
                None => self
                    .tables()?
                    .iter()
                    .filter(|t| t.name.split('.').next() == Some(db_name))
                    .map(|t| t.id + 1)
                    .max()
                    .unwrap_or(0),
            };
            return Ok((next, next + 1));
        }
        // Table ids start at 0, as the C++ engine's do; the counter holds the
        // next id to hand out.
        let mut next = self.read_counter()?;
        while self.store.get(&table_key(next))?.is_some() {
            next += 1;
        }
        Ok((next, next + 1))
    }

    /// Creates a table with the given column names; fails if the name exists.
    pub fn create_table(&self, name: &str, columns: &[String]) -> Result<TableMeta> {
        self.create_table_with_defaults(name, columns, &vec![None; columns.len()])
    }

    /// Creates a table with explicit per-column defaults and NOT NULL flags.
    pub fn create_table_with_defaults(
        &self,
        name: &str,
        columns: &[String],
        defaults: &[Option<String>],
    ) -> Result<TableMeta> {
        let _ddl = self.begin_ddl();
        self.create_table_full(name, columns, defaults, &vec![false; columns.len()])
    }

    /// Creates a table with full column metadata.
    pub fn create_table_full(
        &self,
        name: &str,
        columns: &[String],
        defaults: &[Option<String>],
        not_nulls: &[bool],
    ) -> Result<TableMeta> {
        let _ddl = self.begin_ddl();
        self.create_table_typed(
            name,
            columns,
            defaults,
            not_nulls,
            &vec![String::new(); columns.len()],
            &vec![None; columns.len()],
        )
    }

    /// Creates a table with full column metadata including type text.
    pub fn create_table_typed(
        &self,
        name: &str,
        columns: &[String],
        defaults: &[Option<String>],
        not_nulls: &[bool],
        types: &[String],
        comments: &[Option<String>],
    ) -> Result<TableMeta> {
        let _ddl = self.begin_ddl();
        self.create_table_typed_with_sparse_stores(
            name,
            columns,
            defaults,
            not_nulls,
            types,
            comments,
            &[],
        )
    }

    /// Creates a table with full column metadata, including the `SPARSE`
    /// storage hint per column.
    ///
    /// A hint may also arrive as a trailing `WITH (SORT|SORTED)` on the type
    /// text; it is split off here so [`TableMeta::types`] always holds the
    /// bare declaration (which is what `DESCRIBE` prints, matching the C++
    /// `DataType::ToString()`).
    // Mirrors `create_table_typed`, plus the per-column sparse hint.
    #[allow(clippy::too_many_arguments)]
    pub fn create_table_typed_with_sparse_stores(
        &self,
        name: &str,
        columns: &[String],
        defaults: &[Option<String>],
        not_nulls: &[bool],
        types: &[String],
        comments: &[Option<String>],
        sparse_stores: &[Option<SparseStore>],
    ) -> Result<TableMeta> {
        let _ddl = self.begin_ddl();
        // The `CREATE TABLE … PROPERTIES (bloom_filter_columns = …)` flag is
        // **not** recorded here: the reference's `ColumnDef::ToJson` omits
        // `build_bloom_filter_`, so the catalog it writes reads back as
        // all-false. The flag survives only in the WAL's `CreateTableV2`.

        if self.get_table_by_name(name)?.is_some() {
            return Err(Error::Catalog(format!("table '{name}' already exists")));
        }
        // The counter is committed in the same batch as the table, so its lock
        // must be held until the batch lands: two concurrent `CREATE TABLE`s
        // must not claim one id.
        let _counter = self.counter_lock.lock().unwrap();
        let (id, next_counter) = self.reserve_table_id(name)?;
        let (types, sparse_stores) = normalize_types(types, sparse_stores, columns.len());
        let meta = TableMeta {
            id,
            name: name.to_string(),
            columns: columns.to_vec(),
            defaults: defaults.to_vec(),
            not_nulls: not_nulls.to_vec(),
            types,
            comments: comments.to_vec(),
            ids: (0..columns.len() as u64).collect(),
            sparse_stores,
        };
        if self.catalog_only {
            let db_name = name.split('.').next().unwrap_or(name).to_string();
            let bare = name.rsplit('.').next().unwrap_or(name).to_string();
            let db_id = self.database_id_of_name(&db_name)?.unwrap_or(1);
            // A fresh name under a live database is one table's worth of
            // records; only a name that already has a record needs the full
            // sweep, which is what reconciles the leftovers (a re-created name,
            // a dropped table's column records).
            let name_records = self.with_view(|store| {
                let prefix =
                    storage::catalog_format::catalog_table_prefix(&db_id.to_string(), &bare);
                store.scan_prefix(prefix.as_bytes())
            })?;
            if name_records.is_empty() && self.has_database_record(&db_name)? {
                let batch = crate::layout::stage_new_table(db_id, &meta, id + 1)?;
                self.stage(&batch)?;
                return Ok(meta);
            }
            let mut tables = self.tables()?;
            tables.push(meta.clone());
            self.rewrite_catalog(&self.databases()?, &tables, &self.indexes()?)?;
            return Ok(meta);
        }
        self.store.atomic_batch(&[
            (table_key(id), Some(meta.encode())),
            (name_key(name), Some(id.to_le_bytes().to_vec())),
            (
                COUNTER_KEY.to_vec(),
                Some(next_counter.to_le_bytes().to_vec()),
            ),
        ])?;
        Ok(meta)
    }

    /// Appends columns (with optional defaults / NOT NULL) to an existing table.
    pub fn add_columns(
        &self,
        name: &str,
        columns: &[String],
        defaults: &[Option<String>],
        not_nulls: &[bool],
    ) -> Result<TableMeta> {
        let _ddl = self.begin_ddl();
        self.add_columns_typed(
            name,
            columns,
            defaults,
            not_nulls,
            &vec![String::new(); columns.len()],
            &vec![None; columns.len()],
        )
    }

    /// Appends columns with type text.
    pub fn add_columns_typed(
        &self,
        name: &str,
        columns: &[String],
        defaults: &[Option<String>],
        not_nulls: &[bool],
        types: &[String],
        comments: &[Option<String>],
    ) -> Result<TableMeta> {
        let _ddl = self.begin_ddl();
        self.add_columns_typed_with_sparse_stores(
            name,
            columns,
            defaults,
            not_nulls,
            types,
            comments,
            &[],
        )
    }

    /// Appends columns with type text and their `SPARSE` storage hints.
    // Mirrors `add_columns_typed`, plus the per-column sparse hint.
    #[allow(clippy::too_many_arguments)]
    pub fn add_columns_typed_with_sparse_stores(
        &self,
        name: &str,
        columns: &[String],
        defaults: &[Option<String>],
        not_nulls: &[bool],
        types: &[String],
        comments: &[Option<String>],
        sparse_stores: &[Option<SparseStore>],
    ) -> Result<TableMeta> {
        let _ddl = self.begin_ddl();
        let mut meta = self
            .get_table_by_name(name)?
            .ok_or_else(|| Error::Catalog(format!("table '{name}' does not exist")))?;
        let (types, sparse_stores) = normalize_types(types, sparse_stores, columns.len());
        let first = meta.columns.len();
        for (i, c) in columns.iter().enumerate() {
            if meta.columns.iter().any(|x| x == c) {
                return Err(Error::Catalog(format!("column '{c}' already exists")));
            }
            meta.columns.push(c.clone());
            meta.defaults.push(defaults.get(i).cloned().flatten());
            meta.not_nulls
                .push(not_nulls.get(i).copied().unwrap_or(false));
            meta.types.push(types.get(i).cloned().unwrap_or_default());
            meta.comments.push(comments.get(i).cloned().flatten());
            meta.sparse_stores
                .push(sparse_stores.get(i).copied().flatten());
            let next_id = meta.ids.iter().copied().max().map_or(0, |v| v + 1);
            meta.ids.push(next_id);
        }
        if self.catalog_only {
            // `NewTxn::AddColumns` writes the added columns' records and the
            // table's `next_column_id` tag, nothing else: each record carries
            // **this DDL's** commit ts (`AddColumnsV2` -> the record is created
            // in that transaction), which a whole-layout rewrite would replace
            // with the table's create ts.
            let db_name = name.split('.').next().unwrap_or(name);
            let db_id = self.database_id_of_name(db_name)?.unwrap_or(1);
            let batch = crate::layout::stage_added_columns(db_id, &meta, first)?;
            self.stage(&batch)?;
            return Ok(meta);
        }
        self.store.put(&table_key(meta.id), &meta.encode())?;
        Ok(meta)
    }

    /// Removes a column from an existing table.
    /// C++ `NewTxn::DropColumns` (`new_txn_impl.cpp:762`).
    ///
    /// The C++ key space is append-only: the column's `tbl|col|` record stays
    /// and the drop is a **marker**. `DropColumnsData` writes
    /// `drop|tbl_col|{db}/{table}/{name}/{create_ts}` - which
    /// `TableMeta::LoadColumnDefs` reads to decide whether the column is live -
    /// and one `drop|blk_col|{db}/{table}/{seg}/{block}/{column_def}` per block.
    /// No record is removed and no block is rewritten: `{column_id}.col` names
    /// the column **id**, ids are never recycled, so the file stays where it is
    /// until `Cleanup` removes it and the columns beside it do not move.
    pub fn drop_column(&self, name: &str, column: &str, blocks: &[(u32, u16)]) -> Result<usize> {
        let _ddl = self.begin_ddl();
        let meta = self
            .get_table_by_name(name)?
            .ok_or_else(|| Error::Catalog(format!("table '{name}' does not exist")))?;
        let idx = meta
            .columns
            .iter()
            .position(|c| c == column)
            .ok_or_else(|| Error::Catalog(format!("column '{column}' does not exist")))?;
        if !self.catalog_only {
            let mut meta = meta;
            meta.columns.remove(idx);
            if idx < meta.defaults.len() {
                meta.defaults.remove(idx);
            }
            if idx < meta.not_nulls.len() {
                meta.not_nulls.remove(idx);
            }
            if idx < meta.types.len() {
                meta.types.remove(idx);
            }
            if idx < meta.comments.len() {
                meta.comments.remove(idx);
            }
            if idx < meta.sparse_stores.len() {
                meta.sparse_stores.remove(idx);
            }
            // The id is retired with the column; later columns keep theirs.
            if idx < meta.ids.len() {
                meta.ids.remove(idx);
            }
            self.store.put(&table_key(meta.id), &meta.encode())?;
            return Ok(idx);
        }
        let (db_str, _) = self.db_and_bare_name(name)?;
        let table_id = meta.id.to_string();
        // The marker is keyed by the ts the column was **created** with, so a
        // column of the same name that is created again later (a new record, a
        // new id) is not hidden by it.
        let prefix = format!("tbl|col|{db_str}|{table_id}|{column}|");
        let newest = self.with_view(|store| {
            let mut newest: Option<(u64, Vec<u8>)> = None;
            for (k, v) in store.scan_prefix(prefix.as_bytes())? {
                let text = String::from_utf8_lossy(&k).to_string();
                let Some(ts) = text.rsplit('|').next().and_then(|t| t.parse::<u64>().ok()) else {
                    continue;
                };
                if newest.as_ref().is_none_or(|(seen, _)| ts >= *seen) {
                    newest = Some((ts, v));
                }
            }
            Ok(newest)
        })?;
        let Some((create_ts, def_json)) = newest else {
            return Err(Error::Catalog(format!("column '{column}' does not exist")));
        };
        let keep = String::from_utf8_lossy(&def_json).to_string();
        let sentinel = Some(DDL_TS_SENTINEL.to_string().into_bytes());
        let mut batch: Vec<BatchItem> = vec![(
            format!("drop|tbl_col|{db_str}/{table_id}/{column}/{create_ts}").into_bytes(),
            sentinel.clone(),
        )];
        // The blocks come from the caller: the catalog keeps no record of them
        // until a checkpoint publishes the segment layer (`read_segments`), and
        // the reference enumerates them from the catalog it maintains eagerly.
        // The executor's block files are the authority here.
        for (seg, blk) in blocks {
            batch.push((
                format!("drop|blk_col|{db_str}/{table_id}/{seg}/{blk}/{keep}").into_bytes(),
                sentinel.clone(),
            ));
        }
        // The value is the commit ts; `restamp` fills it in when the DDL
        // commit takes its timestamp (`KeyEncode::DropTableColumnKey` carries
        // the ts in the value, the key names the object's create ts).
        self.stage(&batch)?;
        Ok(idx)
    }

    /// Every schema change of `table` with the column ids that were live at
    /// it: `(commit ts, ids in schema order)`, ascending.
    ///
    /// Read from the catalog itself (the versioned column records and the
    /// drop markers), so it also covers the changes a checkpoint already
    /// published, whose frames the log no longer has. A replayed row uses it to
    /// learn which ids its cells belong to.
    /// The column ids that were live at `ts`, in schema order.
    ///
    /// The checkpoint writes blocks with the schema of its own commit ts (the
    /// reference reads `GetColumnDefs` there), so a column added later must not
    /// take part in that write.
    pub fn schema_ids_at(&self, table: &str, ts: u64) -> Result<Vec<u64>> {
        let timeline = self.column_timeline(table)?;
        if let Some((_, ids)) = timeline
            .iter()
            .rev()
            .find(|(change_ts, _)| *change_ts <= ts)
        {
            return Ok(ids.clone());
        }
        Ok(self
            .get_table_by_name(table)?
            .map(|meta| {
                let mut ids = meta.ids;
                ids.sort_unstable();
                ids
            })
            .unwrap_or_default())
    }

    pub fn column_timeline(&self, table: &str) -> Result<Vec<(u64, Vec<u64>)>> {
        let (db_id, bare) = self.db_and_bare_name(table)?;
        let Ok(db_id) = db_id.parse::<u64>() else {
            return Ok(Vec::new());
        };
        self.with_view(|store| {
            let Some((table_id, _)) =
                crate::layout::table_record(store, &db_id.to_string(), &bare)?
            else {
                return Ok(Vec::new());
            };
            crate::layout::column_timeline(store, db_id, table_id)
        })
    }

    pub fn get_table(&self, id: u64) -> Result<Option<TableMeta>> {
        if self.catalog_only {
            return Ok(self.tables()?.into_iter().find(|t| t.id == id));
        }
        match self.store.get(&table_key(id))? {
            Some(b) => Ok(Some(TableMeta::decode(id, &b)?)),
            None => Ok(None),
        }
    }

    /// The tables of one database (a `DROP DATABASE` only needs those).
    pub fn tables_of_database(&self, db: &str) -> Result<Vec<TableMeta>> {
        if !self.catalog_only {
            return Ok(self
                .list_tables()?
                .into_iter()
                .filter(|t| t.name.starts_with(&format!("{db}.")))
                .collect());
        }
        let tables = self.with_view(|store| crate::layout::read_tables_of_db(store, db))?;
        Ok(tables.iter().map(crate::layout::table_meta_of).collect())
    }

    pub fn get_table_by_name(&self, name: &str) -> Result<Option<TableMeta>> {
        // Inside a capture the read is answered through an overlay of the
        // transaction's own uncommitted DDL, so a cached record would answer
        // from before that DDL. Captures are DDL-only and short, so they simply
        // bypass the cache.
        if self.capture.lock().unwrap().is_some()
            || self.ddl_depth.load(std::sync::atomic::Ordering::SeqCst) > 0
        {
            return self.get_table_by_name_uncached(name);
        }
        let epoch = self.ddl_epoch.load(std::sync::atomic::Ordering::SeqCst);
        if let Some(meta) = self.table_cache.lock().unwrap().get(name) {
            return Ok(Some(meta.clone()));
        }
        let found = self.get_table_by_name_uncached(name)?;
        if let Some(meta) = &found {
            // Publish only a record read inside one invalidation epoch. The
            // comparison and the insert hold the cache lock, which
            // `invalidate_tables` also takes to bump the epoch, so a clear
            // cannot slip between them.
            let mut cache = self.table_cache.lock().unwrap();
            if self.ddl_epoch.load(std::sync::atomic::Ordering::SeqCst) == epoch {
                cache.insert(name.to_string(), meta.clone());
            }
        }
        Ok(found)
    }

    fn get_table_by_name_uncached(&self, name: &str) -> Result<Option<TableMeta>> {
        if self.catalog_only {
            // A qualified name resolves by its own record; only a bare one has
            // to search every database (the session qualifies names before they
            // get here, so this is the rare path).
            if let Some((db, table)) = name.split_once('.') {
                let found = self.with_view(|store| crate::layout::read_table(store, db, table))?;
                return Ok(found.as_ref().map(crate::layout::table_meta_of));
            }
            return Ok(self.tables()?.into_iter().find(|t| t.name == name));
        }
        match self.store.get(&name_key(name))? {
            Some(b) => {
                let id = u64::from_le_bytes(b[..8].try_into().unwrap());
                self.get_table(id)
            }
            None => Ok(None),
        }
    }

    pub fn list_tables(&self) -> Result<Vec<TableMeta>> {
        if self.catalog_only {
            return self.tables();
        }
        let mut out = Vec::new();
        for (k, v) in self.store.scan_prefix(b"tbl:")? {
            let id: u64 = String::from_utf8_lossy(&k[4..]).parse().unwrap_or(0);
            out.push(TableMeta::decode(id, &v)?);
        }
        Ok(out)
    }

    /// Drops a table by name. Returns `true` if it existed.
    ///
    /// A **marker**, like the reference's `NewTxn::PrepareCommitDropTable`: the
    /// records it deprecates (columns, segment layer, index records) stay until
    /// `Cleanup` walks them, and the name stops resolving because
    /// `get_table_by_name` refuses a record with a marker.
    pub fn drop_table(&self, name: &str) -> Result<bool> {
        let _ddl = self.begin_ddl();
        if self.catalog_only {
            // A name that already has a marker is gone: `DROP TABLE` reports it
            // as absent, exactly as `CatalogMeta::GetTableMeta` does.
            if self.get_table_by_name(name)?.is_none() {
                return Ok(false);
            }
            let (db_id, bare) = self.db_and_bare_name(name)?;
            let Some((table_id, create_ts)) =
                crate::layout::table_record(self.store.as_ref(), &db_id, &bare)?
            else {
                return Ok(false);
            };
            let key =
                crate::layout::dropped_table_key(&db_id, &bare, create_ts, &table_id.to_string());
            self.stage(&[(key.into_bytes(), Some(Self::drop_marker_value()))])?;
            return Ok(true);
        }
        match self.get_table_by_name(name)? {
            Some(t) => {
                // Both keys commit together: a crash can never leave the name
                // pointing at a table record that is already gone.
                self.store
                    .atomic_batch(&[(table_key(t.id), None), (name_key(name), None)])?;
                Ok(true)
            }
            None => Ok(false),
        }
    }

    /// Renames a table, keeping its id and metadata.
    pub fn rename_table(&self, old: &str, new: &str) -> Result<()> {
        let _ddl = self.begin_ddl();
        if self.get_table_by_name(new)?.is_some() {
            return Err(Error::Catalog(format!("table '{new}' already exists")));
        }
        let mut meta = self
            .get_table_by_name(old)?
            .ok_or_else(|| Error::Catalog(format!("table '{old}' does not exist")))?;
        meta.name = new.to_string();
        if self.catalog_only {
            // `PrepareCommitRenameTable` writes two records and nothing else:
            //   drop|tbl_name|{db}/{old}/{create_ts}/{table_id} = <commit ts>
            //   catalog|tbl|{db}|{new}|{commit_ts}            = {table_id}
            // The table keeps its id, so the column, segment, block and index
            // records - all keyed by it - are untouched: no restamp, no
            // renumbering, no copy. The old name stops resolving through the
            // marker (`DBMeta::GetTableID` checks it next to `drop|tbl|`).
            let (db_str, bare_old) = self.db_and_bare_name(old)?;
            let bare_new = new.rsplit('.').next().unwrap_or(new);
            if bare_old == bare_new {
                return Ok(());
            }
            let prefix = format!(
                "{}{bare_old}|",
                storage::catalog_format::catalog_db_table_prefix(&db_str)
            );
            let create_ts = self.with_view(|store| {
                let mut newest = 0u64;
                for (k, _) in store.scan_prefix(prefix.as_bytes())? {
                    let text = String::from_utf8_lossy(&k).to_string();
                    if let Some(ts) = text.rsplit('|').next().and_then(|t| t.parse::<u64>().ok()) {
                        newest = newest.max(ts);
                    }
                }
                Ok(newest)
            })?;
            let sentinel = Some(DDL_TS_SENTINEL.to_string().into_bytes());
            self.stage(&[
                (
                    crate::layout::renamed_table_key(
                        &db_str,
                        &bare_old,
                        create_ts,
                        &meta.id.to_string(),
                    )
                    .into_bytes(),
                    sentinel,
                ),
                (
                    format!(
                        "{}{bare_new}|{DDL_TS_SENTINEL}",
                        storage::catalog_format::catalog_db_table_prefix(&db_str)
                    )
                    .into_bytes(),
                    Some(meta.id.to_string().into_bytes()),
                ),
            ])?;
            return Ok(());
        }
        // One batch: the new name must never exist while the old one still
        // resolves to the same table.
        self.store.atomic_batch(&[
            (table_key(meta.id), Some(meta.encode())),
            (name_key(new), Some(meta.id.to_le_bytes().to_vec())),
            (name_key(old), None),
        ])?;
        Ok(())
    }

    /// Registers a database name. `if_not_exists` tolerates an existing name.
    pub fn create_database(&self, name: &str, if_not_exists: bool) -> Result<()> {
        let _ddl = self.begin_ddl();
        if self.catalog_only {
            // `NewTxn::CreateDatabase` writes one record, one tag and the id
            // counter. A name that already has a record - a live one, a dropped
            // one the `drop|db|…` marker still holds, or the implied
            // `default_db` - keeps the full rewrite below, whose sweep is what
            // reconciles the leftovers. Anything else is the common case, and
            // the rewrite rebuilt every table and index record for nothing:
            // 1000 databases cost 43 ms per call, 10,000 over twenty minutes.
            if name != "default_db" && !self.has_database_record(name)? {
                let id = self.next_id_from_records()?;
                self.stage(&[
                    (
                        storage::catalog_format::NEXT_DATABASE_ID
                            .as_bytes()
                            .to_vec(),
                        Some(id.to_string().into_bytes()),
                    ),
                    (
                        storage::catalog_format::catalog_db_key(name, crate::DDL_TS_SENTINEL)
                            .into_bytes(),
                        Some(id.to_string().into_bytes()),
                    ),
                    (
                        storage::catalog_format::catalog_db_tag_key(
                            &id.to_string(),
                            "next_table_id",
                        )
                        .into_bytes(),
                        Some(b"0".to_vec()),
                    ),
                ])?;
                return Ok(());
            }
            let mut dbs = self.databases()?;
            if self.database_records()?.iter().any(|d| d == name) {
                return if if_not_exists {
                    Ok(())
                } else {
                    Err(Error::Catalog(format!("database '{name}' already exists")))
                };
            }
            // `databases()` already lists `default_db` (the reference
            // implies it), so pushing it again would write the database twice
            // and hand it two ids: the layout would then emit
            // `db|1|next_table_id` *and* `db|2|next_table_id`.
            if !dbs.iter().any(|d| d == name) {
                // `NewTxn::CreateDatabase` applies `IncrLatestID(next_database_id)`
                // and writes the counter in the same transaction.
                let id = self.next_id_from_records()?;
                self.stage(&[(
                    storage::catalog_format::NEXT_DATABASE_ID
                        .as_bytes()
                        .to_vec(),
                    Some(id.to_string().into_bytes()),
                )])?;
                dbs.push(name.to_string());
            }
            self.rewrite_catalog(&dbs, &self.tables()?, &self.indexes()?)?;
            return Ok(());
        }
        let key = db_key(name);
        if self.store.get(&key)?.is_some() {
            return if if_not_exists {
                Ok(())
            } else {
                Err(Error::Catalog(format!("database '{name}' already exists")))
            };
        }
        // The reference keeps a monotonic `next_database_id` tag
        // (`NewCatalog::Init` starts it at 0, `CreateDatabase` merges +1), so
        // an id is never reused and a dropped database does not hand its
        // `db_{id}` directory to the next one. The record carries the id it
        // was created with for the same reason: a position in the listing
        // would renumber `default_db` the moment a name sorts before it.
        let id = self.next_database_id()?;
        self.store
            .put(NEXT_DATABASE_ID_KEY, id.to_string().as_bytes())?;
        self.store.put(&key, id.to_string().as_bytes())?;
        Ok(())
    }

    /// Stages one write into the open capture, or applies it at once when there
    /// is none (a checkpoint or an import path without a DDL transaction).
    ///
    /// The write joins the capture's **view** as well: a statement that writes
    /// a counter and then reads it back (a `CREATE DATABASE` bumping
    /// `next_database_id` before its layout rewrite) must see its own write,
    /// not the value the store had when the statement started.
    pub(crate) fn stage(&self, items: &[BatchItem]) -> Result<()> {
        let mut slot = self.capture.lock().unwrap();
        match slot.as_mut() {
            Some(capture) => {
                for (key, value) in items {
                    capture.view.insert(key.clone(), value.clone());
                }
                capture.batch.extend(items.iter().cloned());
                Ok(())
            }
            None => self.store.atomic_batch(items),
        }
    }

    /// The value a `drop|…` marker carries: the commit ts of the transaction
    /// that deprecated the object.
    fn drop_marker_value() -> Vec<u8> {
        DDL_TS_SENTINEL.to_string().into_bytes()
    }

    /// `(db_id, bare table name)` for a qualified table name.
    fn db_and_bare_name(&self, table: &str) -> Result<(String, String)> {
        let (db_name, bare) = match table.split_once('.') {
            Some((db, bare)) => (db.to_string(), bare.to_string()),
            None => (
                self.databases()?
                    .first()
                    .cloned()
                    .unwrap_or_else(|| "default_db".into()),
                table.to_string(),
            ),
        };
        let db_id = self.database_id_of_name(&db_name)?.unwrap_or(1).to_string();
        Ok((db_id, bare))
    }

    /// Writes the two system tags a C++-written directory always carries
    /// (`NewCatalog::Init`): the catalog version and the database counter. Both
    /// only when absent - a directory that already has them keeps its values.
    pub fn init_key_space(&self) -> Result<()> {
        use storage::catalog_format::{CATALOG_VERSION, META_VERSION, NEXT_DATABASE_ID};
        if self.store.get(META_VERSION.as_bytes())?.is_none() {
            self.store
                .put(META_VERSION.as_bytes(), CATALOG_VERSION.as_bytes())?;
        }
        if self.store.get(NEXT_DATABASE_ID.as_bytes())?.is_none() {
            self.store.put(NEXT_DATABASE_ID.as_bytes(), b"0")?;
        }
        self.store.flush()?;
        Ok(())
    }

    /// The id `CREATE DATABASE` hands out next: the stored counter + 1
    /// (`IncrLatestID(db_id_str, NEXT_DATABASE_ID)`).
    pub fn next_database_id(&self) -> Result<u64> {
        Ok(self
            .store
            .get(NEXT_DATABASE_ID_KEY)?
            .and_then(|v| String::from_utf8_lossy(&v).parse::<u64>().ok())
            .unwrap_or(0)
            + 1)
    }

    /// The same for the C++ key space: `IncrLatestID(NEXT_DATABASE_ID)`, which
    /// is the stored counter + 1 (`NewTxn::CreateDatabase`).
    ///
    /// The counter is the authority (`NewCatalog::Init` writes it on the first
    /// open). A directory that predates it - one this port wrote before the
    /// counter existed - starts from the highest recorded id instead.
    fn next_id_from_records(&self) -> Result<u64> {
        let counter = match self
            .store
            .get(NEXT_DATABASE_ID_KEY)?
            .and_then(|v| String::from_utf8_lossy(&v).parse::<u64>().ok())
        {
            Some(counter) => counter,
            // No counter (a directory this port wrote before it existed, or a
            // catalog driven without `init_key_space`): the ids in use are
            // the recorded ones plus the id `default_db` is implied to have -
            // it is the first database, and `db_id_of` numbers it by position.
            None => self
                .store
                .scan_prefix(b"catalog|db|")?
                .into_iter()
                .filter_map(|(_, v)| String::from_utf8_lossy(&v).parse::<u64>().ok())
                .max()
                .unwrap_or(0)
                .max(self.databases()?.len() as u64),
        };
        Ok(counter + 1)
    }

    /// Every recorded database with its id, in name order.
    pub fn database_ids(&self) -> Result<Vec<(String, u64)>> {
        let mut out = Vec::new();
        for (k, v) in self.store.scan_prefix(b"db:")? {
            let name = String::from_utf8_lossy(&k[3..]).to_string();
            let id = String::from_utf8_lossy(&v).parse::<u64>().unwrap_or(0);
            out.push((name, id));
        }
        Ok(out)
    }

    /// The id of `name`; `None` when the database has no record.
    pub fn database_id_of_name(&self, name: &str) -> Result<Option<u64>> {
        if self.catalog_only {
            // The C++ key space keeps the id in the record's value, and that is
            // what names the `db_{id}` directory - including for a database
            // that was dropped and created again (the dropped record is still
            // there, so the newest one is the live id).
            return Ok(crate::layout::db_record(self.store.as_ref(), name)?.map(|(id, _)| id));
        }
        Ok(self
            .store
            .get(&db_key(name))?
            .map(|v| String::from_utf8_lossy(&v).parse::<u64>().unwrap_or(0)))
    }

    /// Whether `name` is **recorded** as a database, as opposed to being
    /// implied by the C++ reader.
    ///
    /// `default_db` is created by the reference's startup transaction
    /// (`Storage::CreateDefaultDB`), so an empty directory has no record for
    /// it: startup has to write one, and `CREATE DATABASE` must not treat it as
    /// already existing.
    pub fn has_database_record(&self, name: &str) -> Result<bool> {
        if self.catalog_only {
            // A record a `drop|db|…` marker deprecated is not a database:
            // `CatalogMeta::GetDBMeta` reports the name as gone and only
            // `Cleanup` takes the record away.
            let Some((id, ts)) = crate::layout::db_record(self.store.as_ref(), name)? else {
                return Ok(false);
            };
            let marker = crate::layout::dropped_db_key(name, ts, &id.to_string());
            return Ok(self.store.get(marker.as_bytes())?.is_none());
        }
        Ok(self.store.get(&db_key(name))?.is_some())
    }

    /// Removes a database registration. `if_exists` tolerates a missing name.
    ///
    /// Like [`Self::drop_table`] this is a marker in the C++ key space:
    /// `NewTxn::PrepareCommitDropDB` writes
    /// `drop|db|{name}/{create_ts}/{db_id}` and `CatalogMeta::GetDBMeta` reports
    /// the name as gone. The database's tables are **not** touched here — the
    /// reference's cleanup walks them (`NewCatalog::CleanDB`) and removes the
    /// records and files.
    pub fn drop_database(&self, name: &str, if_exists: bool) -> Result<bool> {
        let _ddl = self.begin_ddl();
        if self.catalog_only {
            if !self.has_database_record(name)? {
                return if if_exists {
                    Ok(false)
                } else {
                    Err(Error::Catalog(format!("database '{name}' does not exist")))
                };
            }
            let Some((db_id, create_ts)) = crate::layout::db_record(self.store.as_ref(), name)?
            else {
                return Ok(false);
            };
            let key = crate::layout::dropped_db_key(name, create_ts, &db_id.to_string());
            self.stage(&[(key.into_bytes(), Some(Self::drop_marker_value()))])?;
            return Ok(true);
        }
        let key = db_key(name);
        let existed = self.store.get(&key)?.is_some();
        if !existed && !if_exists {
            return Err(Error::Catalog(format!("database '{name}' does not exist")));
        }
        self.store.delete(&key)?;
        Ok(existed)
    }

    /// Names of all registered databases, plus the built-in `default_db`.
    pub fn list_databases(&self) -> Result<Vec<String>> {
        if self.catalog_only {
            return self.databases();
        }
        // The records are the list; `default_db` is created at startup like any
        // other database (`Storage::Init`), so it is usually one of them. A
        // directory that predates that record still reports it once.
        let mut out = Vec::new();
        for (k, _) in self.store.scan_prefix(b"db:")? {
            out.push(String::from_utf8_lossy(&k[3..]).to_string());
        }
        if !out.iter().any(|d| d == "default_db") {
            out.push("default_db".to_string());
        }
        out.sort();
        Ok(out)
    }

    /// Creates an index on `table(column)`; returns its metadata.
    pub fn create_index(&self, name: &str, table: &str, column: &str) -> Result<IndexMeta> {
        self.create_index_typed(name, table, column, "", &[])
    }

    /// Creates an index, recording the `USING <kind>` type and `WITH (…)`
    /// options so the C++ catalog can be written faithfully.
    pub fn create_index_typed(
        &self,
        name: &str,
        table: &str,
        column: &str,
        index_type: &str,
        options: &[(String, String)],
    ) -> Result<IndexMeta> {
        // Per-table index ids start at 1 and are never reused.
        let id = if self.catalog_only {
            self.next_index_id(table)?
        } else {
            let short = table.split('.').next_back().unwrap_or(table);
            self.list_indexes()?
                .iter()
                .filter(|i| i.table == table || i.table == short)
                .map(|i| i.id)
                .max()
                .map_or(1, |v| v + 1)
        };
        self.create_index_typed_with_id(name, table, column, index_type, options, id)
    }

    /// `TableMeta::GetNextIndexID`: the table's `next_index_id` tag, incremented
    /// and stored, and the new value is the id the index gets.
    ///
    /// The counter is what keeps the ids monotonic: `DROP INDEX` only writes a
    /// marker and leaves the index's records in place, so deriving the next id
    /// from the *live* indexes would hand the dropped index's id to the new one -
    /// which then inherits the old record's key, and with it the drop marker.
    fn next_index_id(&self, table: &str) -> Result<u64> {
        let (db_id, bare) = self.db_and_bare_name(table)?;
        let Some((table_id, _)) = crate::layout::table_record(self.store.as_ref(), &db_id, &bare)?
        else {
            return Ok(1);
        };
        let tag = storage::catalog_format::catalog_table_tag_key(
            &db_id,
            &table_id.to_string(),
            "next_index_id",
        );
        let previous = self
            .with_view(|store| store.get(tag.as_bytes()))?
            .and_then(|v| String::from_utf8_lossy(&v).parse::<u64>().ok())
            .unwrap_or(0);
        let id = previous + 1;
        // The layout rewrite below re-reads this tag and keeps the larger value,
        // so staging it is what makes the counter survive the rewrite.
        self.stage(&[(tag.into_bytes(), Some(id.to_string().into_bytes()))])?;
        Ok(id)
    }

    /// Creates an index under a **given** id.
    ///
    /// Importing a reference-written catalog has to keep the reference's ids:
    /// the id names the index's directory (`idx_{id}/seg_0/`) and its records
    /// (`idx|{db}|{tbl}|{id}|index_base`), so an imported index that got a
    /// different id would have its chunk mirror read as another index's.
    pub fn create_index_typed_with_id(
        &self,
        name: &str,
        table: &str,
        column: &str,
        index_type: &str,
        options: &[(String, String)],
        id: u64,
    ) -> Result<IndexMeta> {
        let meta = IndexMeta {
            name: name.to_string(),
            table: table.to_string(),
            column: column.to_string(),
            index_type: index_type.to_ascii_uppercase(),
            options: options.to_vec(),
            id,
        };
        if self.catalog_only {
            // `NewTxn::CreateIndex` writes the index's own records and nothing
            // else: `catalog|idx|{db}|{table_id}|{name}|{commit_ts}` = the id
            // and `idx|{db}|{table_id}|{id}|index_base` = the reference's
            // `IndexBase` JSON. A whole-catalog rewrite restamped every table,
            // column and segment record with this DDL's ts (measured: the
            // column records moved from their create ts to the index's) and
            // swept records the layout does not emit.
            let db_name = table.split('.').next().unwrap_or(table);
            let bare = table.rsplit('.').next().unwrap_or(table);
            let (db_str, _) = self.db_and_bare_name(table)?;
            let Some(layout) = crate::layout::read_table(self.store.as_ref(), db_name, bare)?
            else {
                return Err(Error::Catalog(format!("table '{table}' does not exist")));
            };
            let table_id = layout.table_id.to_string();
            let mut batch: Vec<BatchItem> = Vec::new();
            if meta.column.is_empty() {
                // An expression index has no `IndexBase`; the reference keeps
                // it in a table tag (see `write_layout_batch`).
                let tag = storage::catalog_format::catalog_table_tag_key(
                    &db_str,
                    &table_id,
                    "rust_name_only_indexes",
                );
                let mut names: Vec<String> = self
                    .with_view(|store| store.get(tag.as_bytes()))?
                    .map(|v| {
                        String::from_utf8_lossy(&v)
                            .split(',')
                            .filter(|s| !s.is_empty())
                            .map(str::to_string)
                            .collect()
                    })
                    .unwrap_or_default();
                names.push(name.to_string());
                batch.push((tag.into_bytes(), Some(names.join(",").into_bytes())));
            } else {
                let def = crate::layout::index_def_for(&meta, &layout)
                    .ok_or_else(|| Error::Catalog(format!("index '{name}' has no C++ record")))?;
                batch.push((
                    storage::catalog_format::catalog_index_key(
                        &db_str,
                        &table_id,
                        name,
                        DDL_TS_SENTINEL,
                    )
                    .into_bytes(),
                    Some(id.to_string().into_bytes()),
                ));
                batch.push((
                    storage::catalog_format::catalog_index_tag_key(
                        &db_str,
                        &table_id,
                        &id.to_string(),
                        "index_base",
                    )
                    .into_bytes(),
                    Some(value_format::index_def_bytes(&def)),
                ));
            }
            self.stage(&batch)?;
            return Ok(meta);
        }
        // The declaration and the legacy-record cleanup are one commit, so the
        // index is never listed twice after a crash.
        let mut batch: Vec<BatchItem> = vec![(index_key(table, name), Some(meta.encode()))];
        // Drop a pre-scoping `idx:{name}` record for the same table so the
        // index is not listed twice.
        let legacy = format!("idx:{name}").into_bytes();
        if let Some(v) = self.store.get(&legacy)? {
            if IndexMeta::decode(name.as_bytes(), &v)?.table == table {
                batch.push((legacy, None));
            }
        }
        self.store.atomic_batch(&batch)?;
        Ok(meta)
    }

    pub fn list_indexes(&self) -> Result<Vec<IndexMeta>> {
        if self.catalog_only {
            return self.indexes();
        }
        self.store
            .scan_prefix(INDEX_PREFIX)?
            .into_iter()
            .map(|(k, v)| IndexMeta::decode(&k[INDEX_PREFIX.len()..], &v))
            .collect()
    }

    /// Drops every index named `name` (the plain `DROP INDEX name` statement
    /// has no table qualifier).
    ///
    /// A marker, like the other drops: `NewTxn::DropIndexByName` leaves
    /// `catalog|idx|…` and the index's chunk records alone and writes
    /// `drop|idx|{db}/{table}/{name}/{create_ts}/{index_id}`; the reference's
    /// cleanup removes both.
    pub fn drop_index(&self, name: &str) -> Result<bool> {
        if self.catalog_only {
            let mut staged: Vec<BatchItem> = Vec::new();
            for def in crate::layout::read_indexes(self.store.as_ref())?
                .iter()
                .filter(|d| d.base.index_name == name)
            {
                let (Some((db_id, table_id)), Some(index_id)) = (def.owner.clone(), def.index_id)
                else {
                    continue;
                };
                let Some((_, create_ts)) =
                    crate::layout::index_record(self.store.as_ref(), &db_id, &table_id, name)?
                else {
                    continue;
                };
                let key = crate::layout::dropped_index_key(
                    &db_id,
                    &table_id,
                    name,
                    create_ts,
                    &index_id.to_string(),
                );
                staged.push((key.into_bytes(), Some(Self::drop_marker_value())));
            }
            // An index whose target is an expression has no `IndexBase` and
            // therefore no `catalog|idx|` record to mark: its name lives in the
            // table's `rust_name_only_indexes` tag, so dropping it means taking
            // it out of that list.
            for table in crate::layout::read_layout(self.store.as_ref())? {
                let tag = storage::catalog_format::catalog_table_tag_key(
                    &table.db_id.to_string(),
                    &table.table_id.to_string(),
                    "rust_name_only_indexes",
                );
                let Some(bytes) = self.store.get(tag.as_bytes())? else {
                    continue;
                };
                let text = String::from_utf8_lossy(&bytes).to_string();
                let names: Vec<&str> = text.split(',').filter(|n| !n.is_empty()).collect();
                if !names.contains(&name) {
                    continue;
                }
                let kept: Vec<&str> = names.into_iter().filter(|n| *n != name).collect();
                staged.push((
                    tag.into_bytes(),
                    if kept.is_empty() {
                        None
                    } else {
                        Some(kept.join(",").into_bytes())
                    },
                ));
            }
            if staged.is_empty() {
                return Ok(false);
            }
            self.stage(&staged)?;
            return Ok(true);
        }
        // The name is shared by every table that owns it, so all of its
        // records go in one commit: a crash cannot leave the index alive on
        // half of them.
        let mut deletes: Vec<BatchItem> = Vec::new();
        for (k, v) in self.store.scan_prefix(INDEX_PREFIX)? {
            let meta = IndexMeta::decode(&k[INDEX_PREFIX.len()..], &v)?;
            if meta.name == name {
                deletes.push((k, None));
            }
        }
        if deletes.is_empty() {
            return Ok(false);
        }
        self.store.atomic_batch(&deletes)?;
        Ok(true)
    }

    /// Drops `table`'s index named `name`. Two tables may own an index with the
    /// same name, so callers that know the table must use this form.
    pub fn drop_index_on(&self, table: &str, name: &str) -> Result<bool> {
        let short = table.split('.').next_back().unwrap_or(table);
        if self.catalog_only {
            let mut indexes = self.indexes()?;
            let before = indexes.len();
            indexes.retain(|i| !(i.name == name && (i.table == table || i.table == short)));
            if indexes.len() == before {
                return Ok(false);
            }
            self.rewrite_catalog(&self.databases()?, &self.tables()?, &indexes)?;
            return Ok(true);
        }
        let key = index_key(table, name);
        if self.store.get(&key)?.is_some() {
            self.store.atomic_batch(&[(key, None)])?;
            return Ok(true);
        }
        // Pre-scoping record: only remove it when it belongs to this table.
        let legacy = format!("idx:{name}").into_bytes();
        if let Some(v) = self.store.get(&legacy)? {
            let meta = IndexMeta::decode(name.as_bytes(), &v)?;
            if meta.table == table || meta.table == short {
                self.store.atomic_batch(&[(legacy, None)])?;
                return Ok(true);
            }
        }
        Ok(false)
    }

    pub fn flush(&self) -> Result<()> {
        self.store.flush()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::layout::{BlockRecord, SegmentRecord, SegmentSet, CPP_MIRROR_TS, UNCOMMIT_TS};
    use tempfile::tempdir;

    fn cols(names: &[&str]) -> Vec<String> {
        names.iter().map(|s| s.to_string()).collect()
    }

    #[test]
    fn cpp_only_catalog_writes_only_the_cpp_key_space() {
        let dir = tempdir().unwrap();
        let store: Arc<dyn MetaStore> = Arc::new(RocksMetaStore::open(dir.path()).unwrap());
        let c = Catalog::with_store_catalog_only(store.clone());
        assert!(c.is_catalog_only());
        c.create_database("db2", false).unwrap();
        let t = c
            .create_table_typed(
                "db2.t",
                &cols(&["id", "name"]),
                &[None, None],
                &[false, false],
                &["INT".to_string(), "VARCHAR".to_string()],
                &[None, None],
            )
            .unwrap();
        c.create_index_typed("i1", "db2.t", "name", "FULLTEXT", &[])
            .unwrap();

        // Reads come back from the C++ records.
        assert_eq!(c.list_databases().unwrap(), vec!["default_db", "db2"]);
        let got = c.get_table_by_name("db2.t").unwrap().unwrap();
        assert_eq!(got.id, t.id);
        assert_eq!(got.columns, cols(&["id", "name"]));
        assert_eq!(c.get_table(t.id).unwrap().unwrap().name, "db2.t");
        let indexes = c.list_indexes().unwrap();
        assert_eq!(indexes.len(), 1);
        assert_eq!(indexes[0].name, "i1");
        assert_eq!(indexes[0].column, "name");
        assert_eq!(indexes[0].index_type, "FULLTEXT");

        // Nothing native was left behind: the C++ key space is the sole catalog.
        for prefix in ["tbl:", "tblname:", "db:", "idx:", "__counter:table"] {
            assert!(
                store.scan_prefix(prefix.as_bytes()).unwrap().is_empty(),
                "native key prefix '{prefix}' is populated"
            );
        }
        // ... and it survives a reopen.
        let reopened = Catalog::with_store_catalog_only(store.clone());
        assert_eq!(reopened.list_tables().unwrap().len(), 1);
        assert_eq!(reopened.list_indexes().unwrap().len(), 1);

        // A drop is a marker, exactly as in the reference: the name stops
        // resolving, and the records it deprecated stay until `Cleanup`.
        assert!(reopened.drop_index("i1").unwrap());
        assert!(!reopened.drop_index("i1").unwrap());
        assert!(reopened.drop_table("db2.t").unwrap());
        assert!(!reopened.drop_table("db2.t").unwrap());
        assert!(reopened.drop_database("db2", false).unwrap());
        assert_eq!(reopened.list_databases().unwrap(), vec!["default_db"]);
        assert!(reopened.list_tables().unwrap().is_empty());
        assert!(reopened.list_indexes().unwrap().is_empty());
        assert!(
            !store.scan_prefix(b"drop|idx|").unwrap().is_empty()
                && !store.scan_prefix(b"drop|tbl|").unwrap().is_empty()
                && !store.scan_prefix(b"drop|db|").unwrap().is_empty(),
            "each drop wrote its marker"
        );
        assert!(
            !store.scan_prefix(b"catalog|idx|").unwrap().is_empty(),
            "the deprecated records stay until Cleanup"
        );
        // ... and a **later** rewrite must not sweep them away: the records a
        // marker owns are still part of the directory the reference reads.
        let before: Vec<Vec<u8>> = ["catalog|tbl|", "tbl|", "tbl|col|", "catalog|idx|"]
            .iter()
            .flat_map(|p| store.scan_prefix(p.as_bytes()).unwrap())
            .map(|(k, _)| k)
            .collect();
        assert!(!before.is_empty(), "the dropped records are in the store");
        reopened.create_database("db3", false).unwrap();
        reopened.rewrite_layout(store.as_ref()).unwrap();
        for key in &before {
            assert!(
                store.get(key).unwrap().is_some(),
                "a layout rewrite deleted a deprecated record: {}",
                String::from_utf8_lossy(key)
            );
        }
    }

    #[test]
    fn create_and_lookup() {
        let dir = tempdir().unwrap();
        let c = Catalog::open(dir.path()).unwrap();
        let t1 = c.create_table("t1", &cols(&["a", "b", "c"])).unwrap();
        let t2 = c.create_table("t2", &cols(&["x"])).unwrap();
        assert_eq!(t1.id, 0);
        assert_eq!(t2.id, 1);
        assert_eq!(c.get_table(0).unwrap(), Some(t1.clone()));
        assert_eq!(c.get_table_by_name("t2").unwrap(), Some(t2.clone()));
        assert_eq!(c.list_tables().unwrap().len(), 2);
    }

    #[test]
    fn duplicate_name_is_rejected() {
        let dir = tempdir().unwrap();
        let c = Catalog::open(dir.path()).unwrap();
        c.create_table("x", &cols(&["a"])).unwrap();
        assert!(c.create_table("x", &cols(&["a"])).is_err());
    }

    #[test]
    fn survives_reopen_with_columns() {
        let dir = tempdir().unwrap();
        {
            let c = Catalog::open(dir.path()).unwrap();
            c.create_table("persisted", &cols(&["id", "name"])).unwrap();
            c.flush().unwrap();
        }
        let c = Catalog::open(dir.path()).unwrap();
        let t = c.get_table_by_name("persisted").unwrap().unwrap();
        assert_eq!(t.columns, cols(&["id", "name"]));
        let t2 = c.create_table("another", &cols(&["z"])).unwrap();
        assert_eq!(t2.id, 1);
    }

    #[test]
    fn sparse_store_hint_survives_reopen() {
        let dir = tempdir().unwrap();
        {
            let c = Catalog::open(dir.path()).unwrap();
            let t = c
                .create_table_typed_with_sparse_stores(
                    "sp",
                    &cols(&["id", "v"]),
                    &[None, None],
                    &[false, false],
                    &["INTEGER".to_string(), "SPARSE(FLOAT,30000)".to_string()],
                    &[None, None],
                    &[None, Some(SparseStore::Sorted)],
                )
                .unwrap();
            // The bare declaration is what `DESCRIBE` prints; the hint lives
            // beside it.
            assert_eq!(t.types[1], "SPARSE(FLOAT,30000)");
            assert_eq!(t.sparse_stores[1], Some(SparseStore::Sorted));
            c.flush().unwrap();
        }
        let c = Catalog::open(dir.path()).unwrap();
        let t = c.get_table_by_name("sp").unwrap().unwrap();
        assert_eq!(t.sparse_stores, vec![None, Some(SparseStore::Sorted)]);
        assert_eq!(t.types[1], "SPARSE(FLOAT,30000)");
    }

    #[test]
    fn sparse_store_suffix_in_type_text_is_split_out() {
        let dir = tempdir().unwrap();
        let c = Catalog::open(dir.path()).unwrap();
        let t = c
            .create_table_typed(
                "sp",
                &cols(&["v", "w"]),
                &[None, None],
                &[false, false],
                &[
                    "SPARSE(FLOAT,100) WITH (SORTED)".to_string(),
                    "SPARSE(FLOAT,100) WITH (sort)".to_string(),
                ],
                &[None, None],
            )
            .unwrap();
        assert_eq!(t.types, vec!["SPARSE(FLOAT,100)", "SPARSE(FLOAT,100)"]);
        assert_eq!(
            t.sparse_stores,
            vec![Some(SparseStore::Sorted), Some(SparseStore::Sort)]
        );
    }

    #[test]
    fn legacy_table_record_decodes_without_a_sparse_hint() {
        let dir = tempdir().unwrap();
        let store: Arc<dyn MetaStore> = Arc::new(RocksMetaStore::open(dir.path()).unwrap());
        // A record written before the hint field existed: six columns, no
        // trailing sparse hint.
        let legacy = "t\u{1e}id\u{1f}\u{1f}0\u{1f}INTEGER\u{1f}\u{1f}0";
        store.put(&table_key(7), legacy.as_bytes()).unwrap();
        let meta = TableMeta::decode(7, legacy.as_bytes()).unwrap();
        assert_eq!(meta.types, vec!["INTEGER"]);
        assert_eq!(meta.sparse_stores, vec![None]);
    }

    #[test]
    fn cpp_only_catalog_keeps_the_sparse_hint() {
        let dir = tempdir().unwrap();
        let store: Arc<dyn MetaStore> = Arc::new(RocksMetaStore::open(dir.path()).unwrap());
        {
            let c = Catalog::with_store_catalog_only(store.clone());
            c.create_table_typed_with_sparse_stores(
                "default_db.sp",
                &cols(&["id", "v"]),
                &[None, None],
                &[false, false],
                &["INTEGER".to_string(), "SPARSE(FLOAT,30000)".to_string()],
                &[None, None],
                &[None, Some(SparseStore::Sorted)],
            )
            .unwrap();
        }
        // The hint is written to the C++ `sort` field and read back from it.
        let c = Catalog::with_store_catalog_only(store);
        let t = c.get_table_by_name("default_db.sp").unwrap().unwrap();
        assert_eq!(t.types[1], "SPARSE(float,int16,30000)");
        assert_eq!(t.sparse_stores[1], Some(SparseStore::Sorted));
    }

    #[test]
    fn drop_table() {
        let dir = tempdir().unwrap();
        let c = Catalog::open(dir.path()).unwrap();
        c.create_table("d", &cols(&["a"])).unwrap();
        assert!(c.drop_table("d").unwrap());
        assert!(c.get_table_by_name("d").unwrap().is_none());
        assert!(!c.drop_table("d").unwrap());
    }

    /// Write counters for a [`CountingStore`], so a test can assert that a DDL
    /// commits once and touches nothing outside its batch.
    #[derive(Default)]
    struct Counts {
        puts: std::sync::atomic::AtomicUsize,
        deletes: std::sync::atomic::AtomicUsize,
        batches: std::sync::atomic::AtomicUsize,
        batch_items: std::sync::atomic::AtomicUsize,
    }

    impl Counts {
        fn reset(&self) {
            use std::sync::atomic::Ordering::SeqCst;
            self.puts.store(0, SeqCst);
            self.deletes.store(0, SeqCst);
            self.batches.store(0, SeqCst);
            self.batch_items.store(0, SeqCst);
        }
        fn get(&self, what: &str) -> usize {
            use std::sync::atomic::Ordering::SeqCst;
            match what {
                "puts" => self.puts.load(SeqCst),
                "deletes" => self.deletes.load(SeqCst),
                "batches" => self.batches.load(SeqCst),
                "items" => self.batch_items.load(SeqCst),
                other => panic!("unknown counter {other}"),
            }
        }
    }

    /// Passes every operation through and counts the write calls.
    struct CountingStore {
        inner: Arc<dyn MetaStore>,
        counts: Arc<Counts>,
    }

    impl MetaStore for CountingStore {
        fn get(&self, key: &[u8]) -> Result<Option<Vec<u8>>> {
            self.inner.get(key)
        }

        fn put(&self, key: &[u8], value: &[u8]) -> Result<()> {
            self.counts
                .puts
                .fetch_add(1, std::sync::atomic::Ordering::SeqCst);
            self.inner.put(key, value)
        }

        fn delete(&self, key: &[u8]) -> Result<()> {
            self.counts
                .deletes
                .fetch_add(1, std::sync::atomic::Ordering::SeqCst);
            self.inner.delete(key)
        }

        fn atomic_batch(&self, items: &[(Vec<u8>, Option<Vec<u8>>)]) -> Result<()> {
            self.counts
                .batches
                .fetch_add(1, std::sync::atomic::Ordering::SeqCst);
            self.counts
                .batch_items
                .fetch_add(items.len(), std::sync::atomic::Ordering::SeqCst);
            self.inner.atomic_batch(items)
        }

        fn scan_prefix(&self, prefix: &[u8]) -> Result<Vec<(Vec<u8>, Vec<u8>)>> {
            self.inner.scan_prefix(prefix)
        }

        fn count_prefix(&self, prefix: &[u8]) -> Result<usize> {
            self.inner.count_prefix(prefix)
        }

        fn flush(&self) -> Result<()> {
            self.inner.flush()
        }
    }

    fn counting_catalog(dir: &std::path::Path, catalog_only: bool) -> (Catalog, Arc<Counts>) {
        let inner: Arc<dyn MetaStore> = Arc::new(RocksMetaStore::open(dir).unwrap());
        let counts = Arc::new(Counts::default());
        let store: Arc<dyn MetaStore> = Arc::new(CountingStore {
            inner,
            counts: counts.clone(),
        });
        let catalog = if catalog_only {
            Catalog::with_store_catalog_only(store)
        } else {
            Catalog::with_store(store)
        };
        (catalog, counts)
    }

    /// `DROP TABLE` is one commit: the name and the table record go together.
    #[test]
    fn drop_table_is_one_batch() {
        let dir = tempdir().unwrap();
        {
            let (c, counts) = counting_catalog(dir.path(), false);
            c.create_table("d", &cols(&["a"])).unwrap();
            counts.reset();
            assert!(c.drop_table("d").unwrap());
            assert_eq!(counts.get("batches"), 1);
            assert_eq!(counts.get("items"), 2);
            assert_eq!(counts.get("puts"), 0);
            assert_eq!(counts.get("deletes"), 0);
        }
        // What a reopened catalog sees: the name is gone, not just the record.
        let c = Catalog::open(dir.path()).unwrap();
        assert!(c.get_table_by_name("d").unwrap().is_none());
        assert!(c.list_tables().unwrap().is_empty());
    }

    /// `RENAME TABLE` is one commit: the table, the new name and the removal of
    /// the old name land together.
    #[test]
    fn rename_table_is_one_batch() {
        let dir = tempdir().unwrap();
        let (c, counts) = counting_catalog(dir.path(), false);
        let created = c.create_table("old", &cols(&["a"])).unwrap();
        counts.reset();
        c.rename_table("old", "new").unwrap();
        assert_eq!(counts.get("batches"), 1);
        assert_eq!(counts.get("items"), 3);
        assert_eq!(counts.get("puts"), 0);
        assert_eq!(counts.get("deletes"), 0);
        assert!(c.get_table_by_name("old").unwrap().is_none());
        let renamed = c.get_table_by_name("new").unwrap().unwrap();
        assert_eq!(renamed.id, created.id);
        assert_eq!(renamed.name, "new");
    }

    /// `CREATE INDEX` writes the declaration and removes a legacy `idx:{name}`
    /// record in one commit, so the index can never be listed twice.
    #[test]
    fn create_index_with_a_legacy_record_is_one_batch() {
        let dir = tempdir().unwrap();
        let (c, counts) = counting_catalog(dir.path(), false);
        c.create_table("t", &cols(&["doc"])).unwrap();
        let legacy = IndexMeta {
            name: "idx".into(),
            table: "t".into(),
            column: "doc".into(),
            index_type: "FULLTEXT".into(),
            options: vec![],
            id: 1,
        };
        c.store()
            .put(b"idx:idx", &legacy.encode())
            .expect("seed the legacy record");
        counts.reset();
        c.create_index_typed("idx", "t", "doc", "FULLTEXT", &[])
            .unwrap();
        assert_eq!(counts.get("batches"), 1);
        assert_eq!(counts.get("items"), 2);
        assert_eq!(counts.get("puts"), 0);
        assert_eq!(counts.get("deletes"), 0);
        assert!(c.store().get(b"idx:idx").unwrap().is_none());
        assert_eq!(c.list_indexes().unwrap().len(), 1);
    }

    /// `DROP INDEX` removes the name from every table that owns it in one
    /// commit.
    #[test]
    fn drop_index_across_two_tables_is_one_batch() {
        let dir = tempdir().unwrap();
        let (c, counts) = counting_catalog(dir.path(), false);
        c.create_table("t1", &cols(&["doc"])).unwrap();
        c.create_table("t2", &cols(&["doc"])).unwrap();
        c.create_index_typed("shared", "t1", "doc", "FULLTEXT", &[])
            .unwrap();
        c.create_index_typed("shared", "t2", "doc", "FULLTEXT", &[])
            .unwrap();
        counts.reset();
        assert!(c.drop_index("shared").unwrap());
        assert_eq!(counts.get("batches"), 1);
        assert_eq!(counts.get("items"), 2);
        assert_eq!(counts.get("puts"), 0);
        assert_eq!(counts.get("deletes"), 0);
        assert!(c.list_indexes().unwrap().is_empty());
    }

    /// The table-qualified drop, used by `DROP TABLE` and `ALTER`, is a commit
    /// too.
    #[test]
    fn drop_index_on_is_one_batch() {
        let dir = tempdir().unwrap();
        let (c, counts) = counting_catalog(dir.path(), false);
        c.create_table("t", &cols(&["doc"])).unwrap();
        c.create_index_typed("i", "t", "doc", "FULLTEXT", &[])
            .unwrap();
        counts.reset();
        assert!(c.drop_index_on("t", "i").unwrap());
        assert_eq!(counts.get("batches"), 1);
        assert_eq!(counts.get("items"), 1);
        assert_eq!(counts.get("puts"), 0);
        assert_eq!(counts.get("deletes"), 0);
        assert!(c.list_indexes().unwrap().is_empty());
    }

    /// In C++-only mode every DDL republishes the whole layout; the prefix
    /// sweep and the rewrite are one commit, so a crash cannot leave a
    /// partially emptied layout for the C++ engine to read.
    #[test]
    fn cpp_only_ddl_is_one_batch() {
        let dir = tempdir().unwrap();
        let (c, counts) = counting_catalog(dir.path(), true);
        c.create_database("db2", false).unwrap();
        c.create_table_typed(
            "db2.t",
            &cols(&["id", "name"]),
            &[None, None],
            &[false, false],
            &["INTEGER".into(), "VARCHAR".into()],
            &[None, None],
        )
        .unwrap();
        counts.reset();
        assert!(c.drop_table("db2.t").unwrap());
        assert_eq!(counts.get("batches"), 1);
        assert_eq!(counts.get("puts"), 0);
        assert_eq!(counts.get("deletes"), 0);
        assert!(c.get_table_by_name("db2.t").unwrap().is_none());
        assert!(c.list_databases().unwrap().contains(&"db2".to_string()));
    }

    /// The batched writer must produce byte-identical records to the direct
    /// one — this is the layout the C++ engine reads.
    #[test]
    fn write_cpp_layout_batch_matches_the_direct_writer() {
        let src = tempdir().unwrap();
        let dst = tempdir().unwrap();
        let c = Catalog::open(src.path()).unwrap();
        c.create_database("db2", false).unwrap();
        c.create_table_typed(
            "db2.t",
            &cols(&["id", "name"]),
            &[None, None],
            &[false, false],
            &["INTEGER".into(), "VARCHAR".into()],
            &[None, None],
        )
        .unwrap();
        c.create_index_typed("i", "db2.t", "name", "FULLTEXT", &[])
            .unwrap();
        let dbs = c.list_databases().unwrap();
        let tables = c.list_tables().unwrap();
        let indexes = c.list_indexes().unwrap();
        // A segment layer as a checkpoint publishes it: one segment, two blocks.
        // `default_db` is the first database, so the reference id is 1.
        let db_id = dbs.iter().position(|d| d == "db2").unwrap() as u64 + 1;
        let table_id = tables.iter().find(|t| t.name == "db2.t").unwrap().id;
        let segments = vec![SegmentSet {
            db_id: db_id.to_string(),
            table_id: table_id.to_string(),
            segments: vec![SegmentRecord {
                segment_id: 0,
                create_ts: CPP_MIRROR_TS,
                first_delete_ts: UNCOMMIT_TS,
                blocks: vec![
                    BlockRecord {
                        block_id: 0,
                        commit_ts: CPP_MIRROR_TS,
                    },
                    BlockRecord {
                        block_id: 1,
                        commit_ts: CPP_MIRROR_TS,
                    },
                ],
            }],
        }];

        let batch: std::collections::BTreeMap<Vec<u8>, Vec<u8>> = c
            .write_layout_batch(&dbs, &tables, &indexes, &segments)
            .unwrap()
            .into_iter()
            .map(|(k, v)| (k, v.expect("the layout writer only writes")))
            .collect();

        let direct: Arc<dyn MetaStore> = Arc::new(RocksMetaStore::open(dst.path()).unwrap());
        // The direct writer preserves the segment layer already in the store, so
        // publish it first and the two paths see the same input.
        for set in &segments {
            crate::layout::write_segments(direct.as_ref(), set).unwrap();
        }
        c.write_layout(direct.as_ref()).unwrap();
        let written: std::collections::BTreeMap<Vec<u8>, Vec<u8>> =
            direct.scan_prefix(b"").unwrap().into_iter().collect();

        assert!(!batch.is_empty());
        assert_eq!(batch, written);
    }

    /// The segment layer is what makes a written block mirror visible to the
    /// reference: `GetTableSegments` / `GetTableSegmentBlocks` enumerate
    /// `catalog|seg|…` and `catalog|blk|…` by prefix, so a table with no
    /// records there opens with zero segments.
    #[test]
    fn segment_layer_round_trips_and_sets_the_table_tags() {
        let src = tempdir().unwrap();
        let c = Catalog::open(src.path()).unwrap();
        c.create_table_typed(
            "t",
            &cols(&["id"]),
            &[None],
            &[false],
            &["INTEGER".into()],
            &[None],
        )
        .unwrap();
        let t = c.list_tables().unwrap()[0].clone();
        let db_id = "0".to_string();
        let tbl_id = t.id.to_string();
        let set = SegmentSet {
            db_id: db_id.clone(),
            table_id: tbl_id.clone(),
            segments: vec![SegmentRecord {
                segment_id: 0,
                create_ts: CPP_MIRROR_TS,
                first_delete_ts: UNCOMMIT_TS,
                blocks: vec![
                    BlockRecord {
                        block_id: 0,
                        commit_ts: CPP_MIRROR_TS,
                    },
                    BlockRecord {
                        block_id: 1,
                        commit_ts: CPP_MIRROR_TS,
                    },
                ],
            }],
        };
        let store = c.store();
        c.publish_segments(&set).unwrap();

        let back = crate::layout::read_segments(store.as_ref()).unwrap();
        assert_eq!(back, vec![set]);
        // `next_segment_id` is written as "0" and never advanced — the reference
        // does the same at `TableMeta::InitSet` (`table_meta_impl.cpp:274`) and
        // derives the id to hand out from the segment map
        // (`MetaTableObject::GetNextSegmentID`, `meta_tree_impl.cpp:657`).
        // `unsealed_segment_id` *is* maintained: it names the segment the engine
        // would still append to.
        let tag = |name: &str| {
            store
                .get(
                    storage::catalog_format::catalog_table_tag_key(&db_id, &tbl_id, name)
                        .as_bytes(),
                )
                .unwrap()
                .map(|v| String::from_utf8_lossy(&v).to_string())
        };
        assert_eq!(tag("next_segment_id").as_deref(), Some("0"));
        assert_eq!(tag("unsealed_segment_id").as_deref(), Some("0"));

        // Publishing again with fewer blocks must not leave the stale record.
        let shrunk = SegmentSet {
            db_id,
            table_id: tbl_id,
            segments: vec![SegmentRecord {
                segment_id: 0,
                create_ts: CPP_MIRROR_TS,
                first_delete_ts: UNCOMMIT_TS,
                blocks: vec![BlockRecord {
                    block_id: 0,
                    commit_ts: CPP_MIRROR_TS,
                }],
            }],
        };
        c.publish_segments(&shrunk).unwrap();
        let back = crate::layout::read_segments(store.as_ref()).unwrap();
        assert_eq!(back, vec![shrunk]);
    }
}
