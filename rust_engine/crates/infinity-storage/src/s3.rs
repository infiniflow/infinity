//! A minimal S3 client (SigV4, path-style) for the `storage_type = "minio"`
//! object-store mode described by `test/data/config/test_minio_s3_storage.toml`.
//!
//! Only the four operations the mirror sync needs are implemented:
//! `PutObject`, `GetObject`, `DeleteObject` and `ListObjectsV2`. The client
//! speaks plain HTTP/1.1 over a TCP socket (`enable_https = false`, which is
//! what the MinIO fixture uses), so no TLS stack is pulled in.
//!
//! Signing follows the AWS Signature Version 4 specification with region
//! `us-east-1` and service `s3`, the region MinIO accepts by default.

use std::collections::BTreeMap;
use std::io::{BufRead, BufReader, Read, Write};
use std::net::TcpStream;

use hmac::{Hmac, Mac};
use infinity_common::{Error, Result};
use sha2::{Digest, Sha256};

type HmacSha256 = Hmac<Sha256>;

/// Connection settings for the object store.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct S3Config {
    /// `host:port` (no scheme), e.g. `127.0.0.1:9005`.
    pub endpoint: String,
    pub bucket: String,
    pub access_key: String,
    pub secret_key: String,
}

/// A signed S3 request, separated from the IO so signing can be unit-tested.
#[derive(Debug, PartialEq, Eq)]
pub struct SignedRequest {
    pub method: String,
    pub target: String,
    pub headers: Vec<(String, String)>,
    pub body: Vec<u8>,
}

fn hex(bytes: &[u8]) -> String {
    const DIGITS: &[u8; 16] = b"0123456789abcdef";
    let mut out = String::with_capacity(bytes.len() * 2);
    for b in bytes {
        out.push(DIGITS[(b >> 4) as usize] as char);
        out.push(DIGITS[(b & 0x0f) as usize] as char);
    }
    out
}

fn sha256_hex(data: &[u8]) -> String {
    hex(&Sha256::digest(data))
}

fn hmac(key: &[u8], data: &[u8]) -> Vec<u8> {
    let mut mac = HmacSha256::new_from_slice(key).expect("any key length");
    mac.update(data);
    mac.finalize().into_bytes().to_vec()
}

/// `YYYYMMDDTHHMMSSZ` and `YYYYMMDD` from a Unix timestamp (UTC).
pub fn amz_dates(unix_secs: u64) -> (String, String) {
    // Civil-from-days (Howard Hinnant's algorithm), avoiding a chrono dependency.
    let days = (unix_secs / 86_400) as i64;
    let secs = unix_secs % 86_400;
    let z = days + 719_468;
    let era = if z >= 0 { z } else { z - 146_096 } / 146_097;
    let doe = z - era * 146_097;
    let yoe = (doe - doe / 1460 + doe / 36_524 - doe / 146_096) / 365;
    let y = yoe + era * 400;
    let doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    let mp = (5 * doy + 2) / 153;
    let d = doy - (153 * mp + 2) / 5 + 1;
    let m = if mp < 10 { mp + 3 } else { mp - 9 };
    let y = if m <= 2 { y + 1 } else { y };
    let date = format!("{y:04}{m:02}{d:02}");
    let time = format!(
        "{:02}{:02}{:02}",
        secs / 3600,
        (secs % 3600) / 60,
        secs % 60
    );
    (format!("{date}T{time}Z"), date)
}

/// URI-encodes a single path segment per SigV4 (unreserved characters kept).
fn uri_encode(s: &str) -> String {
    let mut out = String::with_capacity(s.len());
    for b in s.bytes() {
        match b {
            b'A'..=b'Z' | b'a'..=b'z' | b'0'..=b'9' | b'-' | b'_' | b'.' | b'~' => {
                out.push(b as char)
            }
            _ => out.push_str(&format!("%{b:02X}")),
        }
    }
    out
}

fn canonical_query(params: &[(&str, &str)]) -> String {
    let mut pairs: Vec<(String, String)> = params
        .iter()
        .map(|(k, v)| (uri_encode(k), uri_encode(v)))
        .collect();
    pairs.sort();
    pairs
        .into_iter()
        .map(|(k, v)| format!("{k}={v}"))
        .collect::<Vec<_>>()
        .join("&")
}

/// Builds a signed request. `key` is the object key (empty for bucket-level
/// operations); `params` are query parameters (`list-type=2`, …).
#[allow(clippy::too_many_arguments)]
pub fn sign_request(
    cfg: &S3Config,
    method: &str,
    key: &str,
    params: &[(&str, &str)],
    body: &[u8],
    unix_secs: u64,
) -> SignedRequest {
    let (amz_date, date) = amz_dates(unix_secs);
    let payload_hash = sha256_hex(body);
    let canonical_uri = if key.is_empty() {
        format!("/{}", uri_encode(&cfg.bucket))
    } else {
        let encoded = key.split('/').map(uri_encode).collect::<Vec<_>>().join("/");
        format!("/{}/{}", uri_encode(&cfg.bucket), encoded)
    };
    let query = canonical_query(params);
    let canonical_headers = format!(
        "host:{}\nx-amz-content-sha256:{}\nx-amz-date:{}\n",
        cfg.endpoint, payload_hash, amz_date
    );
    let signed_headers = "host;x-amz-content-sha256;x-amz-date";
    let canonical_request = format!(
        "{method}\n{canonical_uri}\n{query}\n{canonical_headers}\n{signed_headers}\n{payload_hash}"
    );
    let scope = format!("{date}/us-east-1/s3/aws4_request");
    let string_to_sign = format!(
        "AWS4-HMAC-SHA256\n{amz_date}\n{scope}\n{}",
        sha256_hex(canonical_request.as_bytes())
    );
    let k_date = hmac(
        format!("AWS4{}", cfg.secret_key).as_bytes(),
        date.as_bytes(),
    );
    let k_region = hmac(&k_date, b"us-east-1");
    let k_service = hmac(&k_region, b"s3");
    let k_signing = hmac(&k_service, b"aws4_request");
    let signature = hex(&hmac(&k_signing, string_to_sign.as_bytes()));
    let authorization = format!(
        "AWS4-HMAC-SHA256 Credential={}/{scope}, SignedHeaders={signed_headers}, Signature={signature}",
        cfg.access_key
    );
    let target = if query.is_empty() {
        canonical_uri
    } else {
        format!("{canonical_uri}?{query}")
    };
    let mut headers = vec![
        ("Host".to_string(), cfg.endpoint.clone()),
        ("x-amz-content-sha256".to_string(), payload_hash),
        ("x-amz-date".to_string(), amz_date),
        ("Authorization".to_string(), authorization),
        ("Content-Length".to_string(), body.len().to_string()),
    ];
    if !body.is_empty() {
        headers.push((
            "Content-Type".to_string(),
            "application/octet-stream".into(),
        ));
    }
    SignedRequest {
        method: method.to_string(),
        target,
        headers,
        body: body.to_vec(),
    }
}

/// A tiny HTTP/1.1 response.
#[derive(Debug)]
pub struct HttpResponse {
    pub status: u16,
    pub body: Vec<u8>,
}

fn read_http_response(stream: &mut TcpStream) -> Result<HttpResponse> {
    let mut reader = BufReader::new(
        stream
            .try_clone()
            .map_err(|e| Error::Storage(e.to_string()))?,
    );
    let mut status_line = String::new();
    reader
        .read_line(&mut status_line)
        .map_err(|e| Error::Storage(e.to_string()))?;
    let status: u16 = status_line
        .split_whitespace()
        .nth(1)
        .and_then(|s| s.parse().ok())
        .ok_or_else(|| Error::Storage(format!("bad HTTP status line: {status_line:?}")))?;
    let mut headers: BTreeMap<String, String> = BTreeMap::new();
    loop {
        let mut line = String::new();
        reader
            .read_line(&mut line)
            .map_err(|e| Error::Storage(e.to_string()))?;
        let line = line.trim_end_matches(['\r', '\n']);
        if line.is_empty() {
            break;
        }
        if let Some((k, v)) = line.split_once(':') {
            headers.insert(k.trim().to_ascii_lowercase(), v.trim().to_string());
        }
    }
    let mut body = Vec::new();
    if headers
        .get("transfer-encoding")
        .is_some_and(|v| v.to_ascii_lowercase().contains("chunked"))
    {
        loop {
            let mut size_line = String::new();
            reader
                .read_line(&mut size_line)
                .map_err(|e| Error::Storage(e.to_string()))?;
            let size = usize::from_str_radix(size_line.trim(), 16)
                .map_err(|_| Error::Storage(format!("bad chunk size: {size_line:?}")))?;
            if size == 0 {
                let mut trailer = String::new();
                let _ = reader.read_line(&mut trailer);
                break;
            }
            let mut chunk = vec![0u8; size];
            reader
                .read_exact(&mut chunk)
                .map_err(|e| Error::Storage(e.to_string()))?;
            body.extend_from_slice(&chunk);
            let mut crlf = [0u8; 2];
            reader
                .read_exact(&mut crlf)
                .map_err(|e| Error::Storage(e.to_string()))?;
        }
    } else if let Some(len) = headers.get("content-length") {
        let len: usize = len
            .parse()
            .map_err(|_| Error::Storage("bad content-length".to_string()))?;
        body.resize(len, 0);
        reader
            .read_exact(&mut body)
            .map_err(|e| Error::Storage(e.to_string()))?;
    } else {
        reader
            .read_to_end(&mut body)
            .map_err(|e| Error::Storage(e.to_string()))?;
    }
    Ok(HttpResponse { status, body })
}

/// S3 client bound to one bucket.
#[derive(Debug, Clone)]
pub struct S3Client {
    cfg: S3Config,
}

impl S3Client {
    pub fn new(cfg: S3Config) -> Self {
        Self { cfg }
    }

    pub fn config(&self) -> &S3Config {
        &self.cfg
    }

    fn send(
        &self,
        method: &str,
        key: &str,
        params: &[(&str, &str)],
        body: &[u8],
    ) -> Result<HttpResponse> {
        let now = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .map(|d| d.as_secs())
            .unwrap_or(0);
        let req = sign_request(&self.cfg, method, key, params, body, now);
        let mut stream = TcpStream::connect(&self.cfg.endpoint)
            .map_err(|e| Error::Storage(format!("S3 connect {}: {e}", self.cfg.endpoint)))?;
        let mut head = format!("{} {} HTTP/1.1\r\n", req.method, req.target);
        for (k, v) in &req.headers {
            head.push_str(&format!("{k}: {v}\r\n"));
        }
        head.push_str("Connection: close\r\n\r\n");
        stream
            .write_all(head.as_bytes())
            .and_then(|_| stream.write_all(&req.body))
            .map_err(|e| Error::Storage(format!("S3 write: {e}")))?;
        stream.flush().map_err(|e| Error::Storage(e.to_string()))?;
        read_http_response(&mut stream)
    }

    fn check(resp: HttpResponse, what: &str) -> Result<Vec<u8>> {
        if (200..300).contains(&resp.status) {
            return Ok(resp.body);
        }
        let text = String::from_utf8_lossy(&resp.body).to_string();
        Err(Error::Storage(format!(
            "S3 {what} failed with HTTP {}: {}",
            resp.status,
            text.chars().take(400).collect::<String>()
        )))
    }

    /// Creates the bucket unless it already exists.
    pub fn ensure_bucket(&self) -> Result<()> {
        let resp = self.send("PUT", "", &[], b"")?;
        if (200..300).contains(&resp.status) || resp.status == 409 {
            return Ok(());
        }
        let text = String::from_utf8_lossy(&resp.body).to_string();
        Err(Error::Storage(format!(
            "S3 create bucket failed with HTTP {}: {text}",
            resp.status
        )))
    }

    pub fn put_object(&self, key: &str, body: &[u8]) -> Result<()> {
        let resp = self.send("PUT", key, &[], body)?;
        Self::check(resp, "PutObject").map(|_| ())
    }

    pub fn get_object(&self, key: &str) -> Result<Option<Vec<u8>>> {
        let resp = self.send("GET", key, &[], b"")?;
        if resp.status == 404 || resp.status == 403 {
            return Ok(None);
        }
        Self::check(resp, "GetObject").map(Some)
    }

    pub fn delete_object(&self, key: &str) -> Result<()> {
        let resp = self.send("DELETE", key, &[], b"")?;
        if resp.status == 404 {
            return Ok(());
        }
        Self::check(resp, "DeleteObject").map(|_| ())
    }

    /// Lists every object under `prefix` as `(key, size)` pairs.
    pub fn list_objects(&self, prefix: &str) -> Result<Vec<(String, usize)>> {
        let mut out = Vec::new();
        let mut token: Option<String> = None;
        loop {
            let mut params: Vec<(&str, &str)> = vec![("list-type", "2"), ("prefix", prefix)];
            let token_value;
            if let Some(t) = &token {
                token_value = t.clone();
                params.push(("continuation-token", &token_value));
            }
            let resp = self.send("GET", "", &params, b"")?;
            let body = Self::check(resp, "ListObjectsV2")?;
            let text = String::from_utf8_lossy(&body);
            for chunk in text.split("<Contents>").skip(1) {
                let key = between(chunk, "<Key>", "</Key>").unwrap_or_default();
                let size = between(chunk, "<Size>", "</Size>")
                    .and_then(|s| s.parse().ok())
                    .unwrap_or(0);
                if !key.is_empty() {
                    out.push((key, size));
                }
            }
            match between(&text, "<NextContinuationToken>", "</NextContinuationToken>") {
                Some(t) if !t.is_empty() => token = Some(t),
                _ => break,
            }
        }
        Ok(out)
    }
}

fn between(text: &str, open: &str, close: &str) -> Option<String> {
    let start = text.find(open)? + open.len();
    let end = text[start..].find(close)? + start;
    Some(text[start..end].to_string())
}

/// Uploads every file under `local_root` to `<key_prefix>/<relative path>` and
/// removes objects that no longer exist locally. Unchanged sizes are skipped,
/// so a checkpoint over an unchanged tree costs one `ListObjectsV2`.
pub fn sync_tree(
    client: &S3Client,
    local_root: &std::path::Path,
    key_prefix: &str,
) -> Result<usize> {
    use std::collections::BTreeSet;
    let mut local: Vec<(String, std::path::PathBuf, usize)> = Vec::new();
    collect_files(local_root, local_root, &mut local)?;
    let remote = client.list_objects(&format!("{key_prefix}/"))?;
    let remote_map: BTreeMap<String, usize> = remote.into_iter().collect();
    let mut uploaded = 0;
    let mut live = BTreeSet::new();
    for (rel, path, size) in &local {
        let key = format!("{key_prefix}/{rel}");
        live.insert(key.clone());
        if remote_map.get(&key).copied() == Some(*size) {
            continue;
        }
        let body = std::fs::read(path).map_err(|e| Error::io(path, e))?;
        client.put_object(&key, &body)?;
        uploaded += 1;
    }
    for key in remote_map.keys() {
        if !live.contains(key) {
            client.delete_object(key)?;
        }
    }
    Ok(uploaded)
}

/// Downloads every object under `<key_prefix>/` into `local_root`, recreating
/// the directory layout. Existing files of the right size are left alone.
pub fn restore_tree(
    client: &S3Client,
    local_root: &std::path::Path,
    key_prefix: &str,
) -> Result<usize> {
    let objects = client.list_objects(&format!("{key_prefix}/"))?;
    let mut restored = 0;
    for (key, size) in objects {
        let rel = key
            .strip_prefix(&format!("{key_prefix}/"))
            .unwrap_or(&key)
            .to_string();
        let path = local_root.join(&rel);
        if std::fs::metadata(&path).map(|m| m.len() as usize).ok() == Some(size) {
            continue;
        }
        let Some(body) = client.get_object(&key)? else {
            continue;
        };
        if let Some(parent) = path.parent() {
            std::fs::create_dir_all(parent).map_err(|e| Error::io(parent, e))?;
        }
        std::fs::write(&path, body).map_err(|e| Error::io(&path, e))?;
        restored += 1;
    }
    Ok(restored)
}

fn collect_files(
    root: &std::path::Path,
    dir: &std::path::Path,
    out: &mut Vec<(String, std::path::PathBuf, usize)>,
) -> Result<()> {
    let entries = match std::fs::read_dir(dir) {
        Ok(e) => e,
        Err(e) if e.kind() == std::io::ErrorKind::NotFound => return Ok(()),
        Err(e) => return Err(Error::io(dir, e)),
    };
    for entry in entries.flatten() {
        let path = entry.path();
        if path.is_dir() {
            collect_files(root, &path, out)?;
        } else if let Ok(meta) = path.metadata() {
            let rel = path
                .strip_prefix(root)
                .unwrap_or(&path)
                .to_string_lossy()
                .replace('\\', "/");
            out.push((rel, path, meta.len() as usize));
        }
    }
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    fn cfg() -> S3Config {
        S3Config {
            endpoint: "127.0.0.1:9005".into(),
            bucket: "infinity".into(),
            access_key: "minioadmin".into(),
            secret_key: "minioadmin".into(),
        }
    }

    #[test]
    fn amz_dates_are_utc() {
        // 2024-01-02T03:04:05Z
        let (date_time, date) = amz_dates(1_704_164_645);
        assert_eq!(date, "20240102");
        assert_eq!(date_time, "20240102T030405Z");
    }

    #[test]
    fn signature_matches_aws_example_shape() {
        // Deterministic inputs: the signature must be stable for the same time.
        let req = sign_request(&cfg(), "GET", "a/b.txt", &[], b"", 1_704_164_645);
        assert_eq!(req.method, "GET");
        assert_eq!(req.target, "/infinity/a/b.txt");
        let auth = req
            .headers
            .iter()
            .find(|(k, _)| k == "Authorization")
            .map(|(_, v)| v.clone())
            .unwrap();
        assert!(auth.starts_with(
            "AWS4-HMAC-SHA256 Credential=minioadmin/20240102/us-east-1/s3/aws4_request, \
             SignedHeaders=host;x-amz-content-sha256;x-amz-date, Signature="
        ));
        assert_eq!(auth.len(), auth.trim().len());
        // A different body must change the signature.
        let other = sign_request(&cfg(), "PUT", "a/b.txt", &[], b"hello", 1_704_164_645);
        assert_ne!(
            auth,
            other
                .headers
                .iter()
                .find(|(k, _)| k == "Authorization")
                .unwrap()
                .1
        );
    }
}
