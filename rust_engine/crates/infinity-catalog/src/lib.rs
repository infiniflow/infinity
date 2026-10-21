//! `infinity-catalog`: metadata built on top of the [`MetaStore`] KV substrate
//! (plan §2.2 / §6). Tables get monotonic ids from a persisted counter; a name
//! index maps name -> id; column names are stored with the table.

#![forbid(unsafe_code)]

pub mod cpp_layout;
/// Versioned metadata cache (C++ `MetaCache`).
pub mod meta_cache;
/// C++-compatible value encoding for catalog records (spec §8.6).
pub mod value_format;

use std::sync::{Arc, Mutex};

use infinity_common::{Error, Result};
use infinity_storage::{FjallMetaStore, MetaStore};

const COUNTER_KEY: &[u8] = b"__counter:table";

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

const VIEW_PREFIX: &[u8] = b"view:";

fn view_key(name: &str) -> Vec<u8> {
    format!("view:{name}").into_bytes()
}

/// Metadata of a view. The query is stored as fully qualified SQL text (the
/// executor resolves unqualified table names against the database that owned
/// the session at `CREATE VIEW` time), so it can be re-parsed and expanded
/// from any later session.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct ViewMeta {
    pub name: String,
    /// Output column names, in select-list order.
    pub columns: Vec<String>,
    pub query: String,
}

impl ViewMeta {
    fn encode(&self) -> Vec<u8> {
        format!("{}\u{1e}{}", self.columns.join("\u{1f}"), self.query).into_bytes()
    }

    fn decode(name: &str, bytes: &[u8]) -> Result<Self> {
        let s = String::from_utf8_lossy(bytes);
        let (cols, query) = s
            .split_once(RSEP)
            .ok_or_else(|| Error::Catalog(format!("bad view meta for '{name}'")))?;
        let columns = if cols.is_empty() {
            Vec::new()
        } else {
            cols.split(FSEP).map(str::to_string).collect()
        };
        Ok(Self {
            name: name.to_string(),
            columns,
            query: query.to_string(),
        })
    }
}

/// Catalog façade over a `MetaStore`.
pub struct Catalog {
    store: Arc<dyn MetaStore>,
    counter_lock: Mutex<()>,
    /// Persist **only** the C++ key space (plan §2.6.1 item ④). The native
    /// `tbl:`/`tblname:`/`db:`/`idx:` keys are then never written, so the store
    /// holds nothing but what a C++ engine can read.
    cpp_only: bool,
}

/// The C++ key-space prefixes this catalog owns. Sweeping them is how the
/// [`Catalog`]'s C++-only mode removes everything a drop would otherwise leave
/// behind (`catalog_format.rs`).
const CPP_KEY_PREFIXES: [&str; 5] = ["catalog|", "db|", "tbl|", "col|", "idx|"];

/// One entry of a catalog commit: `Some(value)` writes the key, `None` deletes
/// it (`MetaStore::atomic_batch`).
pub type BatchItem = (Vec<u8>, Option<Vec<u8>>);

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
            cpp_only: false,
        }
    }

    /// A catalog whose **sole** persisted representation is the C++ key space
    /// (plan §2.6.1 item ④).
    pub fn with_store_cpp_only(store: Arc<dyn MetaStore>) -> Self {
        Self {
            store,
            counter_lock: Mutex::new(()),
            cpp_only: true,
        }
    }

    /// Whether this catalog persists only the C++ key space.
    pub fn is_cpp_only(&self) -> bool {
        self.cpp_only
    }

    pub fn open(path: impl AsRef<std::path::Path>) -> Result<Self> {
        let store = FjallMetaStore::open(path, "catalog")?;
        Ok(Self::with_store(Arc::new(store)))
    }

    /// Like [`Self::open`] but with the C++ key space as the sole catalog
    /// (plan §2.6.1 item ④).
    pub fn open_cpp_only(path: impl AsRef<std::path::Path>) -> Result<Self> {
        let store = FjallMetaStore::open(path, "catalog")?;
        Ok(Self::with_store_cpp_only(Arc::new(store)))
    }

    pub fn store(&self) -> &Arc<dyn MetaStore> {
        &self.store
    }

    /// Databases recorded in the C++ key space. An empty database still has a
    /// `catalog|db|{name}|{ts}` record, so the scan is authoritative.
    fn cpp_databases(&self) -> Result<Vec<String>> {
        let mut out = vec!["default_db".to_string()];
        for (k, _) in self.store.scan_prefix(b"catalog|db|")? {
            let text = String::from_utf8_lossy(&k).to_string();
            if let Some(name) = text.split('|').nth(2) {
                if !out.iter().any(|d| d == name) {
                    out.push(name.to_string());
                }
            }
        }
        Ok(out)
    }

    fn cpp_tables(&self) -> Result<Vec<TableMeta>> {
        crate::cpp_layout::read_cpp_tables_as_meta(self.store.as_ref())
    }

    fn cpp_indexes(&self) -> Result<Vec<IndexMeta>> {
        let tables = crate::cpp_layout::read_cpp_layout(self.store.as_ref())?;
        let mut out = Vec::new();
        for def in crate::cpp_layout::read_cpp_indexes(self.store.as_ref())? {
            if let Some(meta) = crate::cpp_layout::index_meta_of(&def, &tables) {
                out.push(meta);
            }
        }
        // A record read back from the C++ key space carries the id the
        // reference handed out; a record without one (a hand-written one) is
        // numbered per table (1-based, as `CREATE INDEX` would) - skipping
        // the ids the other records already hold, so the fallback cannot
        // hand two indexes the same id.
        let mut next: std::collections::HashMap<String, u64> = std::collections::HashMap::new();
        let mut taken: std::collections::HashMap<String, std::collections::HashSet<u64>> =
            std::collections::HashMap::new();
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
        // target such as `c1 + 5`, so it has no C++ `IndexBase`; its name lives
        // under a supplementary table tag (`write_cpp_layout_from`).
        for t in &tables {
            let tag = infinity_storage::catalog_format::catalog_table_tag_key(
                &t.db_id.to_string(),
                &t.table_id.to_string(),
                "rust_name_only_indexes",
            );
            let Some(bytes) = self.store.get(tag.as_bytes())? else {
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
    }

    /// Republished the whole C++ layout from `dbs`/`tables`/`indexes` after
    /// first deleting every record the layout owns, so a dropped entity cannot
    /// survive as a stale key.
    ///
    /// The deletes and the rewritten records reach the store as **one**
    /// `atomic_batch`: a crash between the sweep and the rewrite would leave a
    /// partially emptied `catalog|`/`tbl|`/`col|` layout, which is exactly the
    /// bytes the C++ engine reads on the next open.
    fn rewrite_cpp(
        &self,
        dbs: &[String],
        tables: &[TableMeta],
        indexes: &[IndexMeta],
    ) -> Result<()> {
        // The sweep covers the `catalog|` prefix, which holds the segment and
        // block records of the block mirror: read them first so a DDL republishes
        // them unchanged instead of leaving the C++ reader with a segmentless
        // table.
        let segments = crate::cpp_layout::read_cpp_segments(self.store.as_ref())?;
        let mut batch = self.cpp_layout_sweep(self.store.as_ref())?;
        batch.extend(self.write_cpp_layout_batch(dbs, tables, indexes, &segments)?);
        self.store.atomic_batch(&last_op_per_key(batch))
    }

    /// Republishes the whole C++ layout from the **current** native catalog,
    /// deleting every record the layout owns first. A checkpoint must call
    /// this (not [`Self::write_cpp_layout`]) so a dropped database, table or
    /// index cannot survive as a stale key that `import_cpp_layout` would
    /// resurrect on the next open. Like [`Self::rewrite_cpp`], the sweep and
    /// the rewrite are one commit.
    pub fn rewrite_cpp_layout(&self, store: &dyn MetaStore) -> Result<usize> {
        let dbs = self.list_databases()?;
        let tables = self.list_tables()?;
        let indexes = self.list_indexes()?;
        let segments = crate::cpp_layout::read_cpp_segments(store)?;
        let layout = self.write_cpp_layout_batch(&dbs, &tables, &indexes, &segments)?;
        let written = layout.len();
        let mut batch = self.cpp_layout_sweep(store)?;
        batch.extend(layout);
        store.atomic_batch(&last_op_per_key(batch))?;
        Ok(written)
    }

    /// Publishes the segment layer of a block mirror the engine just wrote
    /// (`catalog|seg|…`, `catalog|blk|…`, `first_delete_ts`, and the table's
    /// `next_segment_id` / `unsealed_segment_id`).
    ///
    /// A targeted write: nothing is swept, so a checkpoint can publish blocks
    /// after [`Self::rewrite_cpp_layout`] (or, in C++-only mode, after the DDL
    /// that created the mirror).
    pub fn publish_cpp_segments(&self, set: &crate::cpp_layout::CppSegmentSet) -> Result<usize> {
        crate::cpp_layout::write_cpp_segments(self.store.as_ref(), set)
    }

    /// Publishes the sealed-chunk layer of the table's index mirrors
    /// (`idx_seg|…`, `idx_seg|…|next_chunk_id`, `idx_chunk|…|chunk_info`), so the
    /// reference knows the segment's rows are already indexed and does not try
    /// to rebuild them into its memory index at startup.
    pub fn publish_cpp_index_segments(
        &self,
        set: &crate::cpp_layout::CppSegmentSet,
        indexes: &[crate::cpp_layout::CppIndexSegment],
    ) -> Result<usize> {
        crate::cpp_layout::write_cpp_index_segments(self.store.as_ref(), set, indexes)
    }

    /// The deletes a C++-layout rewrite performs before writing the layout.
    fn cpp_layout_sweep(&self, store: &dyn MetaStore) -> Result<Vec<BatchItem>> {
        let mut deletes: Vec<BatchItem> = Vec::new();
        for prefix in CPP_KEY_PREFIXES {
            for (k, _) in store.scan_prefix(prefix.as_bytes())? {
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
    fn reserve_table_id(&self) -> Result<(u64, u64)> {
        if self.cpp_only {
            // No native counter: the id is one past the highest table id, which
            // is exactly what `write_cpp_layout_from` derives from the tables.
            let next = self
                .cpp_tables()?
                .iter()
                .map(|t| t.id)
                .max()
                .map_or(0, |v| v + 1);
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
        if self.get_table_by_name(name)?.is_some() {
            return Err(Error::Catalog(format!("table '{name}' already exists")));
        }
        if self.get_view_by_name(name)?.is_some() {
            return Err(Error::Catalog(format!("view '{name}' already exists")));
        }
        // The counter is committed in the same batch as the table, so its lock
        // must be held until the batch lands: two concurrent `CREATE TABLE`s
        // must not claim one id.
        let _counter = self.counter_lock.lock().unwrap();
        let (id, next_counter) = self.reserve_table_id()?;
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
        if self.cpp_only {
            let mut tables = self.cpp_tables()?;
            tables.push(meta.clone());
            self.rewrite_cpp(&self.cpp_databases()?, &tables, &self.cpp_indexes()?)?;
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
        let mut meta = self
            .get_table_by_name(name)?
            .ok_or_else(|| Error::Catalog(format!("table '{name}' does not exist")))?;
        let (types, sparse_stores) = normalize_types(types, sparse_stores, columns.len());
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
        if self.cpp_only {
            let mut tables = self.cpp_tables()?;
            if let Some(slot) = tables.iter_mut().find(|t| t.name == meta.name) {
                *slot = meta.clone();
            }
            self.rewrite_cpp(&self.cpp_databases()?, &tables, &self.cpp_indexes()?)?;
            return Ok(meta);
        }
        self.store.put(&table_key(meta.id), &meta.encode())?;
        Ok(meta)
    }

    /// Removes a column from an existing table.
    pub fn drop_column(&self, name: &str, column: &str) -> Result<usize> {
        let mut meta = self
            .get_table_by_name(name)?
            .ok_or_else(|| Error::Catalog(format!("table '{name}' does not exist")))?;
        let idx = meta
            .columns
            .iter()
            .position(|c| c == column)
            .ok_or_else(|| Error::Catalog(format!("column '{column}' does not exist")))?;
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
        if self.cpp_only {
            let mut tables = self.cpp_tables()?;
            if let Some(slot) = tables.iter_mut().find(|t| t.name == meta.name) {
                *slot = meta;
            }
            self.rewrite_cpp(&self.cpp_databases()?, &tables, &self.cpp_indexes()?)?;
            return Ok(idx);
        }
        self.store.put(&table_key(meta.id), &meta.encode())?;
        Ok(idx)
    }

    pub fn get_table(&self, id: u64) -> Result<Option<TableMeta>> {
        if self.cpp_only {
            return Ok(self.cpp_tables()?.into_iter().find(|t| t.id == id));
        }
        match self.store.get(&table_key(id))? {
            Some(b) => Ok(Some(TableMeta::decode(id, &b)?)),
            None => Ok(None),
        }
    }

    pub fn get_table_by_name(&self, name: &str) -> Result<Option<TableMeta>> {
        if self.cpp_only {
            return Ok(self.cpp_tables()?.into_iter().find(|t| t.name == name));
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
        if self.cpp_only {
            return self.cpp_tables();
        }
        let mut out = Vec::new();
        for (k, v) in self.store.scan_prefix(b"tbl:")? {
            let id: u64 = String::from_utf8_lossy(&k[4..]).parse().unwrap_or(0);
            out.push(TableMeta::decode(id, &v)?);
        }
        Ok(out)
    }

    /// Drops a table by name. Returns `true` if it existed.
    pub fn drop_table(&self, name: &str) -> Result<bool> {
        if self.cpp_only {
            let mut tables = self.cpp_tables()?;
            let before = tables.len();
            tables.retain(|t| t.name != name);
            if tables.len() == before {
                return Ok(false);
            }
            let indexes: Vec<IndexMeta> = self
                .cpp_indexes()?
                .into_iter()
                .filter(|i| i.table != name)
                .collect();
            self.rewrite_cpp(&self.cpp_databases()?, &tables, &indexes)?;
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
        if self.get_table_by_name(new)?.is_some() {
            return Err(Error::Catalog(format!("table '{new}' already exists")));
        }
        let mut meta = self
            .get_table_by_name(old)?
            .ok_or_else(|| Error::Catalog(format!("table '{old}' does not exist")))?;
        meta.name = new.to_string();
        if self.cpp_only {
            let mut tables = self.cpp_tables()?;
            if let Some(slot) = tables.iter_mut().find(|t| t.id == meta.id) {
                *slot = meta;
            }
            let mut indexes = self.cpp_indexes()?;
            for i in indexes.iter_mut().filter(|i| i.table == old) {
                i.table = new.to_string();
            }
            self.rewrite_cpp(&self.cpp_databases()?, &tables, &indexes)?;
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
        if self.cpp_only {
            let mut dbs = self.cpp_databases()?;
            if dbs.iter().any(|d| d == name) {
                return if if_not_exists {
                    Ok(())
                } else {
                    Err(Error::Catalog(format!("database '{name}' already exists")))
                };
            }
            dbs.push(name.to_string());
            self.rewrite_cpp(&dbs, &self.cpp_tables()?, &self.cpp_indexes()?)?;
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
        self.store.put(&key, b"1")?;
        Ok(())
    }

    /// Removes a database registration. `if_exists` tolerates a missing name.
    pub fn drop_database(&self, name: &str, if_exists: bool) -> Result<bool> {
        if self.cpp_only {
            let mut dbs = self.cpp_databases()?;
            let before = dbs.len();
            dbs.retain(|d| d != name);
            let existed = dbs.len() != before;
            if !existed && !if_exists {
                return Err(Error::Catalog(format!("database '{name}' does not exist")));
            }
            if existed {
                let prefix = format!("{name}.");
                let tables: Vec<TableMeta> = self
                    .cpp_tables()?
                    .into_iter()
                    .filter(|t| !t.name.starts_with(&prefix))
                    .collect();
                let tables_names: Vec<&str> = tables.iter().map(|t| t.name.as_str()).collect();
                let indexes: Vec<IndexMeta> = self
                    .cpp_indexes()?
                    .into_iter()
                    .filter(|i| tables_names.contains(&i.table.as_str()))
                    .collect();
                self.rewrite_cpp(&dbs, &tables, &indexes)?;
            }
            return Ok(existed);
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
        if self.cpp_only {
            return self.cpp_databases();
        }
        let mut out = vec!["default_db".to_string()];
        for (k, _) in self.store.scan_prefix(b"db:")? {
            out.push(String::from_utf8_lossy(&k[3..]).to_string());
        }
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
        let short = table.split('.').next_back().unwrap_or(table);
        let id = self
            .list_indexes()?
            .iter()
            .filter(|i| i.table == table || i.table == short)
            .map(|i| i.id)
            .max()
            .map_or(1, |v| v + 1);
        self.create_index_typed_with_id(name, table, column, index_type, options, id)
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
        if self.cpp_only {
            let mut indexes = self.cpp_indexes()?;
            indexes.retain(|i| !(i.name == name && i.table == table));
            indexes.push(meta.clone());
            self.rewrite_cpp(&self.cpp_databases()?, &self.cpp_tables()?, &indexes)?;
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
        if self.cpp_only {
            return self.cpp_indexes();
        }
        self.store
            .scan_prefix(INDEX_PREFIX)?
            .into_iter()
            .map(|(k, v)| IndexMeta::decode(&k[INDEX_PREFIX.len()..], &v))
            .collect()
    }

    /// Drops every index named `name` (the plain `DROP INDEX name` statement
    /// has no table qualifier).
    pub fn drop_index(&self, name: &str) -> Result<bool> {
        if self.cpp_only {
            let mut indexes = self.cpp_indexes()?;
            let before = indexes.len();
            indexes.retain(|i| i.name != name);
            if indexes.len() == before {
                return Ok(false);
            }
            self.rewrite_cpp(&self.cpp_databases()?, &self.cpp_tables()?, &indexes)?;
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
        if self.cpp_only {
            let mut indexes = self.cpp_indexes()?;
            let before = indexes.len();
            indexes.retain(|i| !(i.name == name && (i.table == table || i.table == short)));
            if indexes.len() == before {
                return Ok(false);
            }
            self.rewrite_cpp(&self.cpp_databases()?, &self.cpp_tables()?, &indexes)?;
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

    /// Registers a view. A view may not shadow an existing table (or another
    /// view); the caller stores an already-qualified query.
    pub fn create_view(&self, name: &str, columns: &[String], query: &str) -> Result<ViewMeta> {
        if self.get_table_by_name(name)?.is_some() {
            return Err(Error::Catalog(format!("table '{name}' already exists")));
        }
        if self.get_view_by_name(name)?.is_some() {
            return Err(Error::Catalog(format!("view '{name}' already exists")));
        }
        let meta = ViewMeta {
            name: name.to_string(),
            columns: columns.to_vec(),
            query: query.to_string(),
        };
        self.store.put(&view_key(name), &meta.encode())?;
        Ok(meta)
    }

    pub fn get_view_by_name(&self, name: &str) -> Result<Option<ViewMeta>> {
        match self.store.get(&view_key(name))? {
            Some(b) => Ok(Some(ViewMeta::decode(name, &b)?)),
            None => Ok(None),
        }
    }

    pub fn list_views(&self) -> Result<Vec<ViewMeta>> {
        self.store
            .scan_prefix(VIEW_PREFIX)?
            .into_iter()
            .map(|(k, v)| {
                let name = String::from_utf8_lossy(&k[VIEW_PREFIX.len()..]).to_string();
                ViewMeta::decode(&name, &v)
            })
            .collect()
    }

    /// Drops a view by name. Returns `true` if it existed.
    pub fn drop_view(&self, name: &str) -> Result<bool> {
        if self.store.get(&view_key(name))?.is_none() {
            return Ok(false);
        }
        self.store.delete(&view_key(name))?;
        Ok(true)
    }

    pub fn flush(&self) -> Result<()> {
        self.store.flush()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::cpp_layout::{CppBlock, CppSegment, CppSegmentSet, CPP_MIRROR_TS};
    use tempfile::tempdir;

    fn cols(names: &[&str]) -> Vec<String> {
        names.iter().map(|s| s.to_string()).collect()
    }

    #[test]
    fn cpp_only_catalog_writes_only_the_cpp_key_space() {
        let dir = tempdir().unwrap();
        let store: Arc<dyn MetaStore> =
            Arc::new(FjallMetaStore::open(dir.path(), "catalog").unwrap());
        let c = Catalog::with_store_cpp_only(store.clone());
        assert!(c.is_cpp_only());
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
        let reopened = Catalog::with_store_cpp_only(store.clone());
        assert_eq!(reopened.list_tables().unwrap().len(), 1);
        assert_eq!(reopened.list_indexes().unwrap().len(), 1);

        // Drops clean up the records, not just the in-memory view.
        assert!(reopened.drop_index("i1").unwrap());
        assert!(!reopened.drop_index("i1").unwrap());
        assert!(reopened.drop_table("db2.t").unwrap());
        assert!(!reopened.drop_table("db2.t").unwrap());
        assert!(reopened.drop_database("db2", false).unwrap());
        assert_eq!(reopened.list_databases().unwrap(), vec!["default_db"]);
        assert!(reopened.list_tables().unwrap().is_empty());
        assert!(reopened.list_indexes().unwrap().is_empty());
        assert!(store.scan_prefix(b"catalog|idx|").unwrap().is_empty());
        assert!(store.scan_prefix(b"idx|").unwrap().is_empty());
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
        let store: Arc<dyn MetaStore> =
            Arc::new(FjallMetaStore::open(dir.path(), "catalog").unwrap());
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
        let store: Arc<dyn MetaStore> =
            Arc::new(FjallMetaStore::open(dir.path(), "catalog").unwrap());
        {
            let c = Catalog::with_store_cpp_only(store.clone());
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
        let c = Catalog::with_store_cpp_only(store);
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

    fn counting_catalog(dir: &std::path::Path, cpp_only: bool) -> (Catalog, Arc<Counts>) {
        let inner: Arc<dyn MetaStore> = Arc::new(FjallMetaStore::open(dir, "catalog").unwrap());
        let counts = Arc::new(Counts::default());
        let store: Arc<dyn MetaStore> = Arc::new(CountingStore {
            inner,
            counts: counts.clone(),
        });
        let catalog = if cpp_only {
            Catalog::with_store_cpp_only(store)
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
        let segments = vec![CppSegmentSet {
            db_id: db_id.to_string(),
            table_id: table_id.to_string(),
            segments: vec![CppSegment {
                segment_id: 0,
                create_ts: CPP_MIRROR_TS,
                blocks: vec![
                    CppBlock {
                        block_id: 0,
                        commit_ts: CPP_MIRROR_TS,
                    },
                    CppBlock {
                        block_id: 1,
                        commit_ts: CPP_MIRROR_TS,
                    },
                ],
            }],
        }];

        let batch: std::collections::BTreeMap<Vec<u8>, Vec<u8>> = c
            .write_cpp_layout_batch(&dbs, &tables, &indexes, &segments)
            .unwrap()
            .into_iter()
            .map(|(k, v)| (k, v.expect("the layout writer only writes")))
            .collect();

        let direct: Arc<dyn MetaStore> =
            Arc::new(FjallMetaStore::open(dst.path(), "catalog").unwrap());
        // The direct writer preserves the segment layer already in the store, so
        // publish it first and the two paths see the same input.
        for set in &segments {
            crate::cpp_layout::write_cpp_segments(direct.as_ref(), set).unwrap();
        }
        c.write_cpp_layout(direct.as_ref()).unwrap();
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
        let set = CppSegmentSet {
            db_id: db_id.clone(),
            table_id: tbl_id.clone(),
            segments: vec![CppSegment {
                segment_id: 0,
                create_ts: CPP_MIRROR_TS,
                blocks: vec![
                    CppBlock {
                        block_id: 0,
                        commit_ts: CPP_MIRROR_TS,
                    },
                    CppBlock {
                        block_id: 1,
                        commit_ts: CPP_MIRROR_TS,
                    },
                ],
            }],
        };
        let store = c.store();
        c.publish_cpp_segments(&set).unwrap();

        let back = crate::cpp_layout::read_cpp_segments(store.as_ref()).unwrap();
        assert_eq!(back, vec![set]);
        // `next_segment_id` is the next id to hand out, `unsealed_segment_id`
        // the segment the engine would still append to.
        let tag = |name: &str| {
            store
                .get(
                    infinity_storage::catalog_format::catalog_table_tag_key(&db_id, &tbl_id, name)
                        .as_bytes(),
                )
                .unwrap()
                .map(|v| String::from_utf8_lossy(&v).to_string())
        };
        assert_eq!(tag("next_segment_id").as_deref(), Some("1"));
        assert_eq!(tag("unsealed_segment_id").as_deref(), Some("0"));

        // Publishing again with fewer blocks must not leave the stale record.
        let shrunk = CppSegmentSet {
            db_id,
            table_id: tbl_id,
            segments: vec![CppSegment {
                segment_id: 0,
                create_ts: CPP_MIRROR_TS,
                blocks: vec![CppBlock {
                    block_id: 0,
                    commit_ts: CPP_MIRROR_TS,
                }],
            }],
        };
        c.publish_cpp_segments(&shrunk).unwrap();
        let back = crate::cpp_layout::read_cpp_segments(store.as_ref()).unwrap();
        assert_eq!(back, vec![shrunk]);
    }
}
