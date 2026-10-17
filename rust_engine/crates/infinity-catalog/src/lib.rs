//! `infinity-catalog`: metadata built on top of the [`MetaStore`] KV substrate
//! (plan §2.2 / §6). Tables get monotonic ids from a persisted counter; a name
//! index maps name -> id; column names are stored with the table.

#![forbid(unsafe_code)]

pub mod cpp_layout;
/// C++-compatible value encoding for catalog records (spec §8.6).
pub mod value_format;

use std::sync::{Arc, Mutex};

use infinity_common::{Error, Result};
use infinity_storage::{FjallMetaStore, MetaStore};

const COUNTER_KEY: &[u8] = b"__counter:table";

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
                format!("{c}{FSEP}{d}{FSEP}{n}{FSEP}{ty}{FSEP}{cm}{FSEP}{cid}")
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
        for part in cols.split(RSEP).filter(|c| !c.is_empty()) {
            let mut f = part.split(FSEP);
            let n = f.next().unwrap_or(part);
            let d = f.next().unwrap_or("");
            let nn = f.next().unwrap_or("0");
            let ty = f.next().unwrap_or("");
            let cm = f.next().unwrap_or("");
            let cid = f.next().and_then(|v| v.parse::<u64>().ok());
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
        })
    }
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
        let name = String::from_utf8_lossy(name).to_string();
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

fn index_key(name: &str) -> Vec<u8> {
    format!("idx:{name}").into_bytes()
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
        // Records read back from the C++ key space do not carry the id; number
        // them per table (1-based, as `CREATE INDEX` would).
        let mut next: std::collections::HashMap<String, u64> = std::collections::HashMap::new();
        for im in out.iter_mut() {
            if im.id == 0 {
                let n = next.entry(im.table.clone()).or_insert(0);
                *n += 1;
                im.id = *n;
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
    fn rewrite_cpp(
        &self,
        dbs: &[String],
        tables: &[TableMeta],
        indexes: &[IndexMeta],
    ) -> Result<()> {
        for prefix in CPP_KEY_PREFIXES {
            for (k, _) in self.store.scan_prefix(prefix.as_bytes())? {
                self.store.delete(&k)?;
            }
        }
        for k in ["next_database_id", "next_table_id", "meta|version"] {
            self.store.delete(k.as_bytes())?;
        }
        self.write_cpp_layout_from(self.store.as_ref(), dbs, tables, indexes)?;
        Ok(())
    }

    fn read_counter(&self) -> Result<u64> {
        Ok(self
            .store
            .get(COUNTER_KEY)?
            .map(|b| u64::from_le_bytes(b[..8].try_into().unwrap()))
            .unwrap_or(0))
    }

    fn alloc_id(&self) -> Result<u64> {
        if self.cpp_only {
            // No native counter: the id is one past the highest table id, which
            // is exactly what `write_cpp_layout_from` derives from the tables.
            return Ok(self
                .cpp_tables()?
                .iter()
                .map(|t| t.id)
                .max()
                .map_or(0, |v| v + 1));
        }
        let _g = self.counter_lock.lock().unwrap();
        // Table ids start at 0, as the C++ engine's do; the counter holds the
        // next id to hand out.
        let mut next = self.read_counter()?;
        while self.store.get(&table_key(next))?.is_some() {
            next += 1;
        }
        self.store.put(COUNTER_KEY, &(next + 1).to_le_bytes())?;
        Ok(next)
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
        if self.get_table_by_name(name)?.is_some() {
            return Err(Error::Catalog(format!("table '{name}' already exists")));
        }
        let id = self.alloc_id()?;
        let meta = TableMeta {
            id,
            name: name.to_string(),
            columns: columns.to_vec(),
            defaults: defaults.to_vec(),
            not_nulls: not_nulls.to_vec(),
            types: types.to_vec(),
            comments: comments.to_vec(),
            ids: (0..columns.len() as u64).collect(),
        };
        if self.cpp_only {
            let mut tables = self.cpp_tables()?;
            tables.push(meta.clone());
            self.rewrite_cpp(&self.cpp_databases()?, &tables, &self.cpp_indexes()?)?;
            return Ok(meta);
        }
        self.store.atomic_write(&[
            (table_key(id), meta.encode()),
            (name_key(name), id.to_le_bytes().to_vec()),
            (COUNTER_KEY.to_vec(), (id + 1).to_le_bytes().to_vec()),
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
        let mut meta = self
            .get_table_by_name(name)?
            .ok_or_else(|| Error::Catalog(format!("table '{name}' does not exist")))?;
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
                self.store.delete(&table_key(t.id))?;
                self.store.delete(&name_key(name))?;
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
        self.store.atomic_write(&[
            (table_key(meta.id), meta.encode()),
            (name_key(new), meta.id.to_le_bytes().to_vec()),
        ])?;
        self.store.delete(&name_key(old))?;
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
            indexes.retain(|i| i.name != name);
            indexes.push(meta.clone());
            self.rewrite_cpp(&self.cpp_databases()?, &self.cpp_tables()?, &indexes)?;
            return Ok(meta);
        }
        self.store.put(&index_key(name), meta.encode().as_slice())?;
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

    /// Drops an index by name. Returns `true` if it existed.
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
        let existed = self.store.get(&index_key(name))?.is_some();
        self.store.delete(&index_key(name))?;
        Ok(existed)
    }

    pub fn flush(&self) -> Result<()> {
        self.store.flush()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
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
    fn drop_table() {
        let dir = tempdir().unwrap();
        let c = Catalog::open(dir.path()).unwrap();
        c.create_table("d", &cols(&["a"])).unwrap();
        assert!(c.drop_table("d").unwrap());
        assert!(c.get_table_by_name("d").unwrap().is_none());
        assert!(!c.drop_table("d").unwrap());
    }
}
