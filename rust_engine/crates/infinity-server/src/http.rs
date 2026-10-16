//! Minimal HTTP/1.1 JSON front-end seed (plan §2.5 / M4). Hand-rolled (no
//! framework dependency) so it builds offline; a production front-end will use a
//! real HTTP stack (axum/hyper) per the plan's dependency table.
//!
//! Routes:
//!   `GET  /health`  -> `ok`
//!   `POST /query`   body `{"sql": "..."}` -> `{"columns":[...],"rows":[[...]]}` or `{"error":"..."}`

use std::io::{Read, Write};
use std::net::{TcpListener, TcpStream};
use std::sync::Arc;

use infinity_executor::{Engine, QueryResult};

use crate::{Error, Result};

/// A tiny HTTP server that runs SQL against an [`Engine`].
pub struct HttpServer {
    listener: TcpListener,
    engine: Arc<Engine>,
}

impl HttpServer {
    pub fn bind(addr: &str, engine: Arc<Engine>) -> Result<Self> {
        let listener = TcpListener::bind(addr).map_err(|e| Error::io(addr, e))?;
        Ok(Self { listener, engine })
    }

    pub fn local_addr(&self) -> Result<String> {
        Ok(self
            .listener
            .local_addr()
            .map_err(|e| Error::io("<addr>", e))?
            .to_string())
    }

    pub fn run(self) -> ! {
        for stream in self.listener.incoming() {
            match stream {
                Ok(s) => {
                    let engine = self.engine.clone();
                    std::thread::spawn(move || {
                        let _ = handle(s, &engine);
                    });
                }
                Err(_) => continue,
            }
        }
        unreachable!()
    }
}

fn json_escape(s: &str) -> String {
    let mut out = String::with_capacity(s.len());
    for c in s.chars() {
        match c {
            '"' => out.push_str("\\\""),
            '\\' => out.push_str("\\\\"),
            '\n' => out.push_str("\\n"),
            '\r' => out.push_str("\\r"),
            '\t' => out.push_str("\\t"),
            _ => out.push(c),
        }
    }
    out
}

fn rows_json(columns: &[String], rows: &[Vec<String>]) -> String {
    let cols = columns
        .iter()
        .map(|c| format!("\"{}\"", json_escape(c)))
        .collect::<Vec<_>>()
        .join(",");
    let rs = rows
        .iter()
        .map(|r| {
            let cells = r
                .iter()
                .map(|v| format!("\"{}\"", json_escape(v)))
                .collect::<Vec<_>>()
                .join(",");
            format!("[{cells}]")
        })
        .collect::<Vec<_>>()
        .join(",");
    format!("{{\"columns\":[{cols}],\"rows\":[{rs}]}}")
}

/// Extracts the `sql` field from a tiny JSON object like `{"sql":"..."}`.
fn extract_sql(body: &str) -> Option<String> {
    let key = body.find("\"sql\"")? + 5;
    let rest = &body[key..];
    let colon = rest.find(':')? + 1;
    let mut chars = rest[colon..].chars().peekable();
    while matches!(chars.peek(), Some(c) if c.is_whitespace()) {
        chars.next();
    }
    if chars.next() != Some('"') {
        return None;
    }
    let mut out = String::new();
    let mut escaped = false;
    for c in chars {
        if escaped {
            out.push(c);
            escaped = false;
        } else if c == '\\' {
            escaped = true;
        } else if c == '"' {
            return Some(out);
        } else {
            out.push(c);
        }
    }
    None
}

/// Routes a request to `(status, body)`.
pub fn route(engine: &Engine, method: &str, path: &str, body: &str) -> (u16, String) {
    match (method, path) {
        ("GET", "/health") => (200, "ok".to_string()),
        ("POST", "/query") => {
            let Some(sql) = extract_sql(body) else {
                return (400, "{\"error\":\"missing sql\"}".to_string());
            };
            match engine.execute(&sql) {
                Ok(results) => {
                    let mut cols = Vec::new();
                    let mut rows = Vec::new();
                    for r in results {
                        if let QueryResult::Rows { columns, rows: rs } = r {
                            cols = columns;
                            rows = rs;
                        }
                    }
                    (200, rows_json(&cols, &rows))
                }
                Err(e) => (
                    500,
                    format!("{{\"error\":\"{}\"}}", json_escape(&e.to_string())),
                ),
            }
        }
        _ => (404, "{\"error\":\"not found\"}".to_string()),
    }
}

fn handle(mut stream: TcpStream, engine: &Engine) -> Result<()> {
    let mut buf = Vec::new();
    let mut tmp = [0u8; 1024];
    // read until end of headers
    let header_end = loop {
        let n = stream.read(&mut tmp).map_err(|e| Error::io("<http>", e))?;
        if n == 0 {
            return Ok(());
        }
        buf.extend_from_slice(&tmp[..n]);
        if let Some(pos) = find_subslice(&buf, b"\r\n\r\n") {
            break pos + 4;
        }
    };
    let head = String::from_utf8_lossy(&buf[..header_end]).to_string();
    let mut lines = head.lines();
    let request_line = lines.next().unwrap_or("");
    let mut parts = request_line.split_whitespace();
    let method = parts.next().unwrap_or("");
    let path = parts.next().unwrap_or("");

    let content_length = head
        .lines()
        .find_map(|l| {
            let l = l.to_ascii_lowercase();
            l.strip_prefix("content-length:")
                .map(|v| v.trim().parse::<usize>().unwrap_or(0))
        })
        .unwrap_or(0);
    while buf.len() < header_end + content_length {
        let n = stream.read(&mut tmp).map_err(|e| Error::io("<http>", e))?;
        if n == 0 {
            break;
        }
        buf.extend_from_slice(&tmp[..n]);
    }
    let body = String::from_utf8_lossy(&buf[header_end..]).to_string();

    let (status, resp_body) = route(engine, method, path, &body);
    let reason = if status == 200 { "OK" } else { "Error" };
    let response = format!(
        "HTTP/1.1 {status} {reason}\r\nContent-Type: application/json\r\nContent-Length: {}\r\nConnection: close\r\n\r\n{}",
        resp_body.len(),
        resp_body
    );
    stream
        .write_all(response.as_bytes())
        .map_err(|e| Error::io("<http>", e))?;
    stream.flush().map_err(|e| Error::io("<http>", e))?;
    Ok(())
}

fn find_subslice(hay: &[u8], needle: &[u8]) -> Option<usize> {
    hay.windows(needle.len()).position(|w| w == needle)
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    #[test]
    fn route_query_and_health() {
        let dir = tempdir().unwrap();
        let engine = Engine::open(dir.path()).unwrap();
        engine
            .execute("CREATE TABLE t (id INT, name VARCHAR);")
            .unwrap();
        engine.execute("INSERT INTO t VALUES (1, 'a');").unwrap();

        let (s, b) = route(&engine, "POST", "/query", "{\"sql\":\"SELECT * FROM t;\"}");
        assert_eq!(s, 200);
        assert!(b.contains("\"columns\":[\"id\",\"name\"]"), "{b}");
        assert!(b.contains("[\"1\",\"a\"]"), "{b}");

        assert_eq!(
            route(&engine, "GET", "/health", ""),
            (200, "ok".to_string())
        );
        assert_eq!(route(&engine, "POST", "/query", "{}").0, 400);
        assert_eq!(route(&engine, "GET", "/nope", "").0, 404);
    }

    #[test]
    fn http_over_socket() {
        let dir = tempdir().unwrap();
        let engine = Arc::new(Engine::open(dir.path()).unwrap());
        engine.execute("CREATE TABLE t (id INT);").unwrap();
        engine.execute("INSERT INTO t VALUES (42);").unwrap();
        let server = HttpServer::bind("127.0.0.1:0", engine).unwrap();
        let addr = server.local_addr().unwrap();
        std::thread::spawn(move || server.run());

        let mut stream = TcpStream::connect(&addr).unwrap();
        let body = "{\"sql\":\"SELECT * FROM t;\"}";
        let req = format!(
            "POST /query HTTP/1.1\r\nHost: x\r\nContent-Length: {}\r\n\r\n{}",
            body.len(),
            body
        );
        stream.write_all(req.as_bytes()).unwrap();
        let mut resp = String::new();
        stream.read_to_string(&mut resp).unwrap();
        assert!(resp.starts_with("HTTP/1.1 200"), "{resp}");
        assert!(resp.contains("[\"42\"]"), "{resp}");
    }
}
