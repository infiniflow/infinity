//! Minimal PostgreSQL wire-protocol front-end (plan §2.5 / M4). Supports the
//! startup handshake and the **simple query** protocol so a standard client
//! (`psql`) can run SQL against the engine. The extended query protocol, SSL and
//! auth are out of scope for this seed (matching Infinity's own PG stub).

use std::io::{Read, Write};
use std::net::{TcpListener, TcpStream};
use std::sync::Arc;

use infinity_executor::{Engine, QueryResult};

use crate::{Error, Result};

fn be_i16(v: i16) -> [u8; 2] {
    v.to_be_bytes()
}
fn be_i32(v: i32) -> [u8; 4] {
    v.to_be_bytes()
}

fn typed(typ: u8, payload: &[u8]) -> Vec<u8> {
    let mut v = Vec::with_capacity(payload.len() + 5);
    v.push(typ);
    v.extend_from_slice(&be_i32((payload.len() + 4) as i32));
    v.extend_from_slice(payload);
    v
}

fn cstr(parts: &[&str]) -> Vec<u8> {
    let mut v = Vec::new();
    for p in parts {
        v.extend_from_slice(p.as_bytes());
        v.push(0);
    }
    v
}

/// A tiny PostgreSQL-wire server.
pub struct PgServer {
    listener: TcpListener,
    engine: Arc<Engine>,
}

impl PgServer {
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
                    let _ = s.set_nodelay(true);
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

fn read_exact_n(stream: &mut TcpStream, n: usize) -> Result<Vec<u8>> {
    let mut buf = vec![0u8; n];
    stream
        .read_exact(&mut buf)
        .map_err(|e| Error::io("<pg>", e))?;
    Ok(buf)
}

fn read_i32(stream: &mut TcpStream) -> Result<i32> {
    let b = read_exact_n(stream, 4)?;
    Ok(i32::from_be_bytes([b[0], b[1], b[2], b[3]]))
}

/// Reads a `StartupMessage`; answers `SSLRequest` with `N` and retries.
fn read_startup(stream: &mut TcpStream) -> Result<()> {
    loop {
        let len = read_i32(stream)?;
        if !(8..=10_000).contains(&len) {
            return Err(Error::Invalid(format!("bad startup length {len}")));
        }
        let body = read_exact_n(stream, (len - 4) as usize)?;
        let proto = i32::from_be_bytes([body[0], body[1], body[2], body[3]]);
        if proto == 80877103 {
            // SSLRequest -> reply 'N' and read again
            stream.write_all(b"N").map_err(|e| Error::io("<pg>", e))?;
            continue;
        }
        return Ok(());
    }
}

fn send_startup_ok(stream: &mut TcpStream) -> Result<()> {
    let mut out = Vec::new();
    out.extend_from_slice(&typed(b'R', &be_i32(0))); // AuthenticationOk
    out.extend_from_slice(&typed(b'S', &cstr(&["server_version", "14.0"]))); // ParameterStatus
    out.extend_from_slice(&typed(b'S', &cstr(&["client_encoding", "UTF8"])));
    let mut key = Vec::new();
    key.extend_from_slice(&be_i32(1234));
    key.extend_from_slice(&be_i32(5678));
    out.extend_from_slice(&typed(b'K', &key)); // BackendKeyData
    out.extend_from_slice(&typed(b'Z', b"I")); // ReadyForQuery
    stream.write_all(&out).map_err(|e| Error::io("<pg>", e))?;
    Ok(())
}

fn row_description(columns: &[String]) -> Vec<u8> {
    let mut p = Vec::new();
    p.extend_from_slice(&be_i16(columns.len() as i16));
    for name in columns {
        p.extend_from_slice(name.as_bytes());
        p.push(0);
        p.extend_from_slice(&be_i32(0)); // table oid
        p.extend_from_slice(&be_i16(0)); // column attnum
        p.extend_from_slice(&be_i32(25)); // text
        p.extend_from_slice(&be_i16(-1)); // type size
        p.extend_from_slice(&be_i32(-1)); // type modifier
        p.extend_from_slice(&be_i16(0)); // format
    }
    typed(b'T', &p)
}

fn data_row(row: &[String]) -> Vec<u8> {
    let mut p = Vec::new();
    p.extend_from_slice(&be_i16(row.len() as i16));
    for cell in row {
        let bytes = cell.as_bytes();
        p.extend_from_slice(&be_i32(bytes.len() as i32));
        p.extend_from_slice(bytes);
    }
    typed(b'D', &p)
}

fn command_complete(tag: &str) -> Vec<u8> {
    let mut p = tag.as_bytes().to_vec();
    p.push(0);
    typed(b'C', &p)
}

fn ready() -> Vec<u8> {
    typed(b'Z', b"I")
}

fn handle(mut stream: TcpStream, engine: &Engine) -> Result<()> {
    read_startup(&mut stream)?;
    send_startup_ok(&mut stream)?;

    loop {
        let mut typ = [0u8; 1];
        if stream.read_exact(&mut typ).is_err() {
            return Ok(()); // client closed
        }
        let len = read_i32(&mut stream)?;
        let body = read_exact_n(&mut stream, (len - 4).max(0) as usize)?;
        match typ[0] {
            b'Q' => {
                let sql = String::from_utf8_lossy(body.split(|b| *b == 0).next().unwrap_or(&[]))
                    .to_string();
                let mut out = Vec::new();
                match engine.execute(&sql) {
                    Ok(results) => {
                        let mut rows_sent = 0;
                        for r in results {
                            if let QueryResult::Rows { columns, rows } = r {
                                out.extend_from_slice(&row_description(&columns));
                                for row in &rows {
                                    out.extend_from_slice(&data_row(row));
                                }
                                rows_sent += rows.len();
                            }
                        }
                        out.extend_from_slice(&command_complete(&format!("SELECT {rows_sent}")));
                    }
                    Err(e) => {
                        let mut p = Vec::new();
                        p.extend_from_slice(b"ERROR");
                        p.push(0);
                        p.extend_from_slice(b"C00000");
                        p.push(0);
                        p.extend_from_slice(e.to_string().as_bytes());
                        p.push(0);
                        p.push(0);
                        out.extend_from_slice(&typed(b'E', &p));
                    }
                }
                out.extend_from_slice(&ready());
                stream.write_all(&out).map_err(|e| Error::io("<pg>", e))?;
            }
            b'X' => return Ok(()), // Terminate
            _ => {
                stream
                    .write_all(&ready())
                    .map_err(|e| Error::io("<pg>", e))?;
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    #[test]
    fn pg_simple_query_over_socket() {
        use std::time::Duration;
        let dir = tempdir().unwrap();
        let engine = Arc::new(Engine::open(dir.path()).unwrap());
        engine.execute("CREATE TABLE t (id INT);").unwrap();
        engine.execute("INSERT INTO t VALUES (42);").unwrap();
        let server = PgServer::bind("127.0.0.1:0", engine).unwrap();
        let addr = server.local_addr().unwrap();
        std::thread::spawn(move || server.run());

        let mut stream = TcpStream::connect(&addr).unwrap();
        // startup: len | protocol 3.0 | key\0value\0...\0  (terminated by \0)
        let mut params = cstr(&["user", "postgres", "database", "db"]);
        params.push(0);
        let mut startup = Vec::new();
        startup.extend_from_slice(&be_i32((4 + 4 + params.len()) as i32));
        startup.extend_from_slice(&be_i32(196608));
        startup.extend_from_slice(&params);
        stream.write_all(&startup).unwrap();

        // simple query
        let q = b"SELECT * FROM t;";
        let mut msg = vec![b'Q'];
        msg.extend_from_slice(&be_i32((q.len() + 1 + 4) as i32));
        msg.extend_from_slice(q);
        msg.push(0);
        stream.write_all(&msg).unwrap();

        // read the response until the socket would block
        stream
            .set_read_timeout(Some(Duration::from_millis(500)))
            .unwrap();
        let mut resp = Vec::new();
        let mut buf = [0u8; 4096];
        loop {
            match stream.read(&mut buf) {
                Ok(0) | Err(_) => break,
                Ok(n) => resp.extend_from_slice(&buf[..n]),
            }
        }
        let text = String::from_utf8_lossy(&resp);
        assert!(text.contains("42"), "response: {text:?}");
        assert!(text.contains("SELECT 1"), "response: {text:?}");
    }
}
