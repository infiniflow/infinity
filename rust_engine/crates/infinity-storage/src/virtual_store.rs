//! File access layer — the counterpart of the reference's `VirtualStore`
//! (`src/storage/io/virtual_store.cppm` / `virtual_store_impl.cpp`).
//!
//! The C++ build has one `VirtualStore` with a local and an object-store
//! backend. In Rust local files are plain `std::fs` operations, and the
//! object-store path lives in [`crate::s3`]; this module covers the local
//! semantics the unit tests pin (open/append/read, directory management,
//! rename, truncate, merge, cleanup and the restore snapshot compression
//! format).
//!
//! C++: `src/storage/io/virtual_store_impl.cpp:38` (`String2StorageType`) and
//! `:81` (`ToString(StorageType)`).

use std::fs::{File, OpenOptions};
use std::io::{Read, Seek, SeekFrom, Write};
use std::path::{Path, PathBuf};

use infinity_common::{Error, Result};

/// C++: `StorageType` (`src/storage/io/virtual_store.cppm`).
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub enum StorageType {
    #[default]
    Local,
    Minio,
    AwsS3,
    AzureBlob,
    Gcs,
    Oss,
    Cos,
    Obs,
    Hdfs,
    Nfs,
    Invalid,
}

/// C++: `String2StorageType` (`virtual_store_impl.cpp:38`). Only `local` and
/// `minio` have a dedicated type in the reference; every other cloud name maps
/// to `kLocal` until its backend is enabled.
pub fn string_to_storage_type(storage_type: &str) -> StorageType {
    match storage_type {
        "local" => StorageType::Local,
        "minio" => StorageType::Minio,
        "aws_s3" | "azure_blob" | "gcs" | "oss" | "cos" | "obs" | "hdfs" | "nfs" => {
            StorageType::Local
        }
        _ => StorageType::Invalid,
    }
}

impl StorageType {
    /// C++: `ToString(StorageType)` (`virtual_store_impl.cpp:81`).
    pub fn as_str(self) -> &'static str {
        match self {
            StorageType::Local => "local",
            StorageType::Minio => "minio",
            StorageType::AwsS3 => "aws s3",
            StorageType::AzureBlob => "azure blob",
            StorageType::Gcs => "google cloud storage",
            StorageType::Oss => "aliyun object storage service",
            StorageType::Cos => "tencent cloud object storage",
            StorageType::Obs => "huawei object storage service",
            StorageType::Hdfs => "hadoop file system",
            StorageType::Nfs => "network file system",
            StorageType::Invalid => "invalid",
        }
    }
}

/// C++: `FileAccessMode` (`src/storage/io/local_file_handle.cppm`).
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum FileAccessMode {
    Read,
    Write,
    Append,
}

/// An open file handle, the counterpart of the reference's `LocalFileHandle`.
#[derive(Debug)]
pub struct FileHandle {
    path: PathBuf,
    file: File,
    mode: FileAccessMode,
}

impl FileHandle {
    pub fn path(&self) -> &Path {
        &self.path
    }

    /// C++ `LocalFileHandle::Append`: writes at the current position.
    pub fn append(&mut self, data: &[u8]) -> Result<()> {
        self.file
            .write_all(data)
            .map_err(|e| Error::io(&self.path, e))
    }

    /// C++ `LocalFileHandle::Sync`.
    pub fn sync(&mut self) -> Result<()> {
        self.file.sync_all().map_err(|e| Error::io(&self.path, e))
    }

    /// C++ `LocalFileHandle::Read(buffer, nbytes)`: reads up to `len` bytes
    /// from the current offset and returns how many were read.
    pub fn read(&mut self, len: usize) -> Result<Vec<u8>> {
        let mut buf = vec![0u8; len];
        let n = self
            .file
            .read(&mut buf)
            .map_err(|e| Error::io(&self.path, e))?;
        buf.truncate(n);
        Ok(buf)
    }

    /// C++ `LocalFileHandle::Seek`.
    pub fn seek(&mut self, offset: u64) -> Result<u64> {
        self.file
            .seek(SeekFrom::Start(offset))
            .map_err(|e| Error::io(&self.path, e))
    }

    /// C++ `LocalFileHandle::FileSize`.
    pub fn file_size(&self) -> Result<u64> {
        Ok(self
            .file
            .metadata()
            .map_err(|e| Error::io(&self.path, e))?
            .len())
    }

    pub fn mode(&self) -> FileAccessMode {
        self.mode
    }

    /// C++ `LocalFileHandle::MmapRead`: a no-op in the reference build.
    pub fn mmap_read(&self, _name: impl AsRef<Path>) -> Result<()> {
        Ok(())
    }

    /// C++ `LocalFileHandle::Unmmap`: a no-op in the reference build.
    pub fn unmmap(&self, _name: impl AsRef<Path>) -> Result<()> {
        Ok(())
    }
}

/// C++: `VirtualStore` (`src/storage/io/virtual_store.cppm`).
pub struct VirtualStore;

impl VirtualStore {
    /// C++ `VirtualStore::Open` (`virtual_store_impl.cpp`).
    pub fn open(path: impl AsRef<Path>, mode: FileAccessMode) -> Result<FileHandle> {
        let path = path.as_ref().to_path_buf();
        let file = match mode {
            FileAccessMode::Read => OpenOptions::new().read(true).open(&path),
            // The reference opens `kWrite` as `O_RDWR | O_CREAT` with no
            // `O_TRUNC`, so an existing file keeps its contents and the caller
            // overwrites from offset 0.
            FileAccessMode::Write => OpenOptions::new()
                .create(true)
                .truncate(false)
                .read(true)
                .write(true)
                .open(&path),
            FileAccessMode::Append => OpenOptions::new().append(true).open(&path),
        }
        .map_err(|e| Error::io(&path, e))?;
        Ok(FileHandle { path, file, mode })
    }

    /// C++ `VirtualStore::Exists`.
    pub fn exists(path: impl AsRef<Path>) -> bool {
        path.as_ref().exists()
    }

    /// C++ `VirtualStore::DeleteFile`.
    pub fn delete_file(path: impl AsRef<Path>) -> Result<()> {
        let path = path.as_ref();
        match std::fs::remove_file(path) {
            Ok(()) => Ok(()),
            Err(e) if e.kind() == std::io::ErrorKind::NotFound => Ok(()),
            Err(e) => Err(Error::io(path, e)),
        }
    }

    /// C++ `VirtualStore::MakeDirectory`: creates every missing parent.
    pub fn make_directory(path: impl AsRef<Path>) -> Result<()> {
        let path = path.as_ref();
        std::fs::create_dir_all(path).map_err(|e| Error::io(path, e))
    }

    /// C++ `VirtualStore::RemoveDirectory`.
    pub fn remove_directory(path: impl AsRef<Path>) -> Result<()> {
        let path = path.as_ref();
        match std::fs::remove_dir_all(path) {
            Ok(()) => Ok(()),
            Err(e) if e.kind() == std::io::ErrorKind::NotFound => Ok(()),
            Err(e) => Err(Error::io(path, e)),
        }
    }

    /// C++ `VirtualStore::CleanupDirectory`: removes the directory's contents
    /// but keeps the directory itself.
    pub fn cleanup_directory(path: impl AsRef<Path>) -> Result<()> {
        let path = path.as_ref();
        let entries = match std::fs::read_dir(path) {
            Ok(entries) => entries,
            Err(e) if e.kind() == std::io::ErrorKind::NotFound => return Ok(()),
            Err(e) => return Err(Error::io(path, e)),
        };
        for entry in entries {
            let entry = entry.map_err(|e| Error::io(path, e))?;
            let entry_path = entry.path();
            if entry_path.is_dir() {
                std::fs::remove_dir_all(&entry_path).map_err(|e| Error::io(&entry_path, e))?;
            } else {
                std::fs::remove_file(&entry_path).map_err(|e| Error::io(&entry_path, e))?;
            }
        }
        Ok(())
    }

    /// C++ `VirtualStore::Rename`.
    pub fn rename(old_path: impl AsRef<Path>, new_path: impl AsRef<Path>) -> Result<()> {
        let old_path = old_path.as_ref();
        let new_path = new_path.as_ref();
        std::fs::rename(old_path, new_path).map_err(|e| Error::io(old_path, e))
    }

    /// C++ `VirtualStore::Truncate`: sets the file length to `size` bytes.
    pub fn truncate(path: impl AsRef<Path>, size: u64) -> Result<()> {
        let path = path.as_ref();
        let file = OpenOptions::new()
            .write(true)
            .open(path)
            .map_err(|e| Error::io(path, e))?;
        file.set_len(size).map_err(|e| Error::io(path, e))
    }

    /// C++ `VirtualStore::Merge` (`virtual_store_impl.cpp:314`): appends the
    /// whole of `src_path` to `dst_path`.
    pub fn merge(dst_path: impl AsRef<Path>, src_path: impl AsRef<Path>) -> Result<()> {
        let dst_path = dst_path.as_ref();
        let src_path = src_path.as_ref();
        let mut src = File::open(src_path).map_err(|e| Error::io(src_path, e))?;
        let mut dst = OpenOptions::new()
            .append(true)
            .open(dst_path)
            .map_err(|e| Error::io(dst_path, e))?;
        std::io::copy(&mut src, &mut dst).map_err(|e| Error::io(dst_path, e))?;
        dst.sync_all().map_err(|e| Error::io(dst_path, e))
    }

    /// C++ `VirtualStore::BeginCompress` (`virtual_store_impl.cpp:449`).
    pub fn begin_compress(compressed_file: impl AsRef<Path>) -> Result<Compressor> {
        let path = compressed_file.as_ref().to_path_buf();
        let file = File::create(&path).map_err(|e| Error::io(&path, e))?;
        Ok(Compressor { path, file })
    }

    /// C++ `VirtualStore::AddFileCompress` (`virtual_store_impl.cpp:454`): one
    /// record = `filename_len:u64 | filename | raw_len:u64 | md5[16] |
    /// compressed_len:i32 | lz4_block`.
    pub fn add_file_compress(
        compressor: &mut Compressor,
        input_file: impl AsRef<Path>,
    ) -> Result<()> {
        let input_file = input_file.as_ref();
        let raw = std::fs::read(input_file).map_err(|e| Error::io(input_file, e))?;
        let name = input_file.to_string_lossy();
        let md5 = md5_bytes(&raw);

        compressor
            .file
            .write_all(&(name.len() as u64).to_le_bytes())
            .and_then(|_| compressor.file.write_all(name.as_bytes()))
            .and_then(|_| compressor.file.write_all(&(raw.len() as u64).to_le_bytes()))
            .and_then(|_| compressor.file.write_all(&md5))
            .map_err(|e| Error::io(&compressor.path, e))?;

        let compressed = lz4_flex::block::compress(&raw);
        compressor
            .file
            .write_all(&(compressed.len() as i32).to_le_bytes())
            .and_then(|_| compressor.file.write_all(&compressed))
            .map_err(|e| Error::io(&compressor.path, e))
    }

    /// C++ `VirtualStore::EndCompress` (`virtual_store_impl.cpp:498`).
    pub fn end_compress(mut compressor: Compressor) -> Result<()> {
        compressor
            .file
            .flush()
            .and_then(|_| compressor.file.sync_all())
            .map_err(|e| Error::io(&compressor.path, e))
    }
}

/// An in-progress `BeginCompress` output stream.
#[derive(Debug)]
pub struct Compressor {
    path: PathBuf,
    file: File,
}

impl Compressor {
    pub fn path(&self) -> &Path {
        &self.path
    }
}

/// RFC 1321 MD5 over raw bytes, used by the snapshot compression format.
fn md5_bytes(input: &[u8]) -> [u8; 16] {
    const S: [u32; 64] = [
        7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 5, 9, 14, 20, 5, 9, 14, 20, 5,
        9, 14, 20, 5, 9, 14, 20, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 6, 10,
        15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21,
    ];
    let mut msg = input.to_vec();
    let bit_len = (msg.len() as u64).wrapping_mul(8);
    msg.push(0x80);
    while msg.len() % 64 != 56 {
        msg.push(0);
    }
    msg.extend_from_slice(&bit_len.to_le_bytes());

    let (mut a0, mut b0, mut c0, mut d0) = (0x6745_2301u32, 0xefcd_ab89, 0x98ba_dcfe, 0x1032_5476);
    for chunk in msg.chunks(64) {
        let mut m = [0u32; 16];
        for (i, w) in chunk.chunks(4).enumerate() {
            m[i] = u32::from_le_bytes([w[0], w[1], w[2], w[3]]);
        }
        let (mut a, mut b, mut c, mut d) = (a0, b0, c0, d0);
        for (i, &s) in S.iter().enumerate() {
            let (f, g) = match i {
                0..=15 => ((b & c) | (!b & d), i),
                16..=31 => ((d & b) | (!d & c), (5 * i + 1) % 16),
                32..=47 => (b ^ c ^ d, (3 * i + 5) % 16),
                _ => (c ^ (b | !d), (7 * i) % 16),
            };
            let k = (((i + 1) as f64).sin().abs() * 4_294_967_296.0) as u32;
            let tmp = d;
            d = c;
            c = b;
            let x = a.wrapping_add(f).wrapping_add(k).wrapping_add(m[g]);
            b = b.wrapping_add(x.rotate_left(s));
            a = tmp;
        }
        a0 = a0.wrapping_add(a);
        b0 = b0.wrapping_add(b);
        c0 = c0.wrapping_add(c);
        d0 = d0.wrapping_add(d);
    }

    let mut out = [0u8; 16];
    for (i, v) in [a0, b0, c0, d0].into_iter().enumerate() {
        out[i * 4..i * 4 + 4].copy_from_slice(&v.to_le_bytes());
    }
    out
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    fn payload(len: usize) -> Vec<u8> {
        (0..len).map(|i| (i + 1) as u8).collect()
    }

    /// C++: `src/unit_test/storage/io/virtual_store_ut.cpp:37` (`TestAppend`).
    #[test]
    fn append_then_delete() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("test_file2.abc");
        let mut handle = VirtualStore::open(&path, FileAccessMode::Write).unwrap();
        handle.append(&payload(10)).unwrap();
        handle.sync().unwrap();
        VirtualStore::delete_file(&path).unwrap();
        assert!(!VirtualStore::exists(&path));
    }

    /// C++: `src/unit_test/storage/io/virtual_store_ut.cpp:57` (`TestDir`).
    #[test]
    fn dir_create_and_remove() {
        let dir = tempdir().unwrap();
        let dir = dir.path().join("unit_test");
        let path = dir.join("test_file.test");
        VirtualStore::make_directory(&dir).unwrap();

        let mut handle = VirtualStore::open(&path, FileAccessMode::Write).unwrap();
        handle.append(&payload(10)).unwrap();
        handle.sync().unwrap();

        assert!(VirtualStore::exists(&path));
        assert!(VirtualStore::exists(&dir));

        VirtualStore::remove_directory(&dir).unwrap();
        assert!(!VirtualStore::exists(&path));
        assert!(!VirtualStore::exists(&dir));
    }

    /// C++: `src/unit_test/storage/io/virtual_store_ut.cpp:85` (`TestRead`).
    #[test]
    fn write_then_read() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("test_file_read.abc");
        let mut write = VirtualStore::open(&path, FileAccessMode::Write).unwrap();
        write.append(&payload(10)).unwrap();
        write.sync().unwrap();
        drop(write);

        let mut read = VirtualStore::open(&path, FileAccessMode::Read).unwrap();
        let data = read.read(10).unwrap();
        assert_eq!(data, payload(10));
        VirtualStore::delete_file(&path).unwrap();
        assert!(!VirtualStore::exists(&path));
    }

    /// C++: `src/unit_test/storage/io/virtual_store_ut.cpp:119` (`TestRename`).
    #[test]
    fn rename_moves_the_file() {
        let dir = tempdir().unwrap();
        let old_path = dir.path().join("test_file_old.abc");
        let new_path = dir.path().join("test_file_new.abc");
        let mut handle = VirtualStore::open(&old_path, FileAccessMode::Write).unwrap();
        handle.append(&payload(10)).unwrap();
        handle.sync().unwrap();
        drop(handle);

        VirtualStore::rename(&old_path, &new_path).unwrap();
        assert!(!VirtualStore::exists(&old_path));
        assert!(VirtualStore::exists(&new_path));

        VirtualStore::delete_file(&new_path).unwrap();
        assert!(!VirtualStore::exists(&new_path));
    }

    /// C++: `src/unit_test/storage/io/virtual_store_ut.cpp:147` (`TestTruncate`).
    #[test]
    fn truncate_cuts_the_tail() {
        let dir = tempdir().unwrap();
        let path = dir.path().join("test_file_truncate.abc");
        let mut handle = VirtualStore::open(&path, FileAccessMode::Write).unwrap();
        handle.append(&payload(20)).unwrap();
        handle.sync().unwrap();
        drop(handle);

        VirtualStore::truncate(&path, 10).unwrap();
        let mut read = VirtualStore::open(&path, FileAccessMode::Read).unwrap();
        let data = read.read(10).unwrap();
        assert_eq!(data, payload(10));
        VirtualStore::delete_file(&path).unwrap();
        assert!(!VirtualStore::exists(&path));
    }

    /// C++: `src/unit_test/storage/io/virtual_store_ut.cpp:184` (`TestMerge`).
    #[test]
    fn merge_appends_src_to_dst() {
        let dir = tempdir().unwrap();
        let dst_path = dir.path().join("test_file_append_dst.abc");
        let src_path = dir.path().join("test_file_append_src.abc");

        let mut src = VirtualStore::open(&src_path, FileAccessMode::Write).unwrap();
        src.append(&payload(10)).unwrap();
        src.sync().unwrap();
        drop(src);

        let dst_data: Vec<u8> = (0..5).map(|i| (i + 10) as u8).collect();
        let mut dst = VirtualStore::open(&dst_path, FileAccessMode::Write).unwrap();
        dst.append(&dst_data).unwrap();
        dst.sync().unwrap();
        drop(dst);

        VirtualStore::merge(&dst_path, &src_path).unwrap();

        let mut read = VirtualStore::open(&dst_path, FileAccessMode::Read).unwrap();
        let combined = read.read(15).unwrap();
        assert_eq!(combined.len(), 15);
        assert_eq!(&combined[..5], &dst_data[..]);
        assert_eq!(&combined[5..], &payload(10)[..]);

        VirtualStore::delete_file(&src_path).unwrap();
        VirtualStore::delete_file(&dst_path).unwrap();
        assert!(!VirtualStore::exists(&src_path));
        assert!(!VirtualStore::exists(&dst_path));
    }

    /// C++: `src/unit_test/storage/io/virtual_store_ut.cpp:240` (`TestCleanDir`).
    #[test]
    fn cleanup_directory_keeps_the_directory() {
        let dir = tempdir().unwrap();
        let dir = dir.path().join("cleanup_test_dir");
        let file1 = dir.join("file1.txt");
        let file2 = dir.join("file2.txt");
        VirtualStore::make_directory(&dir).unwrap();

        let mut h1 = VirtualStore::open(&file1, FileAccessMode::Write).unwrap();
        h1.append(&payload(10)).unwrap();
        h1.sync().unwrap();
        drop(h1);

        let mut h2 = VirtualStore::open(&file2, FileAccessMode::Write).unwrap();
        h2.append(&payload(20)).unwrap();
        h2.sync().unwrap();
        drop(h2);

        VirtualStore::cleanup_directory(&dir).unwrap();
        assert!(!VirtualStore::exists(&file1));
        assert!(!VirtualStore::exists(&file2));
        assert!(VirtualStore::exists(&dir));

        VirtualStore::remove_directory(&dir).unwrap();
        assert!(!VirtualStore::exists(&dir));
    }

    /// C++: `src/unit_test/storage/io/virtual_store_ut.cpp:362` (`TestStorageType`).
    #[test]
    fn storage_type_conversions() {
        assert_eq!(string_to_storage_type("local"), StorageType::Local);
        assert_eq!(string_to_storage_type("minio"), StorageType::Minio);
        for name in ["aws_s3", "azure_blob", "gcs", "oss", "cos", "hdfs", "nfs"] {
            assert_eq!(string_to_storage_type(name), StorageType::Local);
        }
        assert_eq!(StorageType::AwsS3.as_str(), "aws s3");
        assert_eq!(StorageType::AzureBlob.as_str(), "azure blob");
        assert_eq!(StorageType::Gcs.as_str(), "google cloud storage");
        assert_eq!(StorageType::Oss.as_str(), "aliyun object storage service");
        assert_eq!(StorageType::Cos.as_str(), "tencent cloud object storage");
        assert_eq!(StorageType::Obs.as_str(), "huawei object storage service");
        assert_eq!(StorageType::Hdfs.as_str(), "hadoop file system");
        assert_eq!(StorageType::Nfs.as_str(), "network file system");
    }

    /// C++: `src/unit_test/storage/io/virtual_store_ut.cpp:400` (`TestFileCompress`).
    #[test]
    fn file_compress_writes_a_record() {
        let dir = tempdir().unwrap();
        let file_path = dir.path().join("test_compress.txt");
        let content: String = ('a'..='z').map(|c| format!("{c} ")).collect();
        std::fs::write(&file_path, &content).unwrap();

        let mut compressor = VirtualStore::begin_compress(dir.path().join("compressed")).unwrap();
        VirtualStore::add_file_compress(&mut compressor, &file_path).unwrap();
        let compressed_path = compressor.path().to_path_buf();
        VirtualStore::end_compress(compressor).unwrap();

        let bytes = std::fs::read(&compressed_path).unwrap();
        let name_len = u64::from_le_bytes(bytes[0..8].try_into().unwrap()) as usize;
        let mut off = 8 + name_len;
        let raw_len = u64::from_le_bytes(bytes[off..off + 8].try_into().unwrap()) as usize;
        off += 8;
        assert_eq!(raw_len, content.len());
        assert_eq!(&bytes[off..off + 16], &md5_bytes(content.as_bytes()));
        off += 16;
        let comp_len = i32::from_le_bytes(bytes[off..off + 4].try_into().unwrap()) as usize;
        off += 4;
        let raw = lz4_flex::block::decompress(&bytes[off..off + comp_len], raw_len).unwrap();
        assert_eq!(raw, content.as_bytes());
    }
}
