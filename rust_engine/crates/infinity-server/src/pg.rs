//! Minimal PostgreSQL wire-protocol front-end (plan §2.5 / M4). Supports the
//! startup handshake and the **simple query** protocol so a standard client
//! (`psql`) can run SQL against the engine. The extended query protocol, SSL and
//! auth are out of scope for this seed (matching Infinity's own PG stub).
//!
//! [`PGProtocolHandler`] is the port of `src/network/pg_protocol_handler.cppm`:
//! it owns the framing (startup header/body, SSL refusal, frontend command
//! parsing, response frames) and the command loop, while [`PgServer`] only
//! accepts connections and hands each one a handler. Frontend commands beyond
//! the simple query (`Parse`/`Bind`/`Execute`, copy sub-protocol, …) are
//! recognised and framed but answered with `ReadyForQuery`, exactly like the
//! previous loop (the C++ stub does the same).

use std::io::{Read, Write};
use std::net::TcpListener;
use std::sync::Arc;

use infinity_executor::{Engine, QueryResult};

use crate::{Error, Result};

/// Size of the length field every PG message starts with.
const LENGTH_FIELD_SIZE: usize = 4;
/// `SSLRequest` magic version number.
const SSL_REQUEST_CODE: i32 = 80877103;

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

fn read_exact_n(reader: &mut impl Read, n: usize) -> Result<Vec<u8>> {
    let mut buf = vec![0u8; n];
    reader
        .read_exact(&mut buf)
        .map_err(|e| Error::io("<pg>", e))?;
    Ok(buf)
}

fn read_i32(reader: &mut impl Read) -> Result<i32> {
    let b = read_exact_n(reader, 4)?;
    Ok(i32::from_be_bytes([b[0], b[1], b[2], b[3]]))
}

/// Takes a NUL-terminated string out of a command body. A truncated body yields
/// an empty string instead of panicking.
fn take_cstr(body: &[u8], offset: &mut usize) -> String {
    if *offset >= body.len() {
        *offset = body.len();
        return String::new();
    }
    let rest = &body[*offset..];
    let end = rest
        .iter()
        .position(|byte| *byte == 0)
        .unwrap_or(rest.len());
    *offset += end + usize::from(end < rest.len());
    String::from_utf8_lossy(&rest[..end]).into_owned()
}

/// One frontend command (`PGMessageType::k*Command` in
/// `src/network/pg_message.cppm`).
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum PgCommand {
    /// `Q`: a statement to run immediately.
    SimpleQuery(String),
    /// `P`: extended-protocol statement definition.
    Parse { statement: String, query: String },
    /// `B`: bind a prepared statement to a portal.
    Bind { portal: String, statement: String },
    /// `D`: describe a statement (`S`) or portal (`P`).
    Describe { target: u8, name: String },
    /// `E`: execute a portal.
    Execute { portal: String },
    /// `C`: close a statement or portal.
    Close { target: u8, name: String },
    /// `S`: end of an extended-protocol cycle.
    Sync,
    /// `H`: flush the output buffer.
    Flush,
    /// `X`: client is done.
    Terminate,
    /// `d`: copy sub-protocol payload.
    CopyData(Vec<u8>),
    /// `c`: copy sub-protocol end.
    CopyDone,
    /// `f`: copy sub-protocol failure.
    CopyFail(String),
    /// A message type this stub does not classify.
    Unknown(u8),
}

impl PgCommand {
    /// The wire byte that starts this command.
    pub fn kind(&self) -> u8 {
        match self {
            Self::SimpleQuery(_) => b'Q',
            Self::Parse { .. } => b'P',
            Self::Bind { .. } => b'B',
            Self::Describe { .. } => b'D',
            Self::Execute { .. } => b'E',
            Self::Close { .. } => b'C',
            Self::Sync => b'S',
            Self::Flush => b'H',
            Self::Terminate => b'X',
            Self::CopyData(_) => b'd',
            Self::CopyDone => b'c',
            Self::CopyFail(_) => b'f',
            Self::Unknown(kind) => *kind,
        }
    }

    /// Parses a frontend message body.
    pub fn from_wire(kind: u8, body: &[u8]) -> Self {
        let mut offset = 0;
        match kind {
            b'Q' => Self::SimpleQuery(take_cstr(body, &mut offset)),
            b'P' => {
                let statement = take_cstr(body, &mut offset);
                let query = take_cstr(body, &mut offset);
                Self::Parse { statement, query }
            }
            b'B' => {
                let portal = take_cstr(body, &mut offset);
                let statement = take_cstr(body, &mut offset);
                Self::Bind { portal, statement }
            }
            b'D' => {
                let target = body.first().copied().unwrap_or(0);
                offset = 1;
                let name = take_cstr(body, &mut offset);
                Self::Describe { target, name }
            }
            b'E' => Self::Execute {
                portal: take_cstr(body, &mut offset),
            },
            b'C' => {
                let target = body.first().copied().unwrap_or(0);
                offset = 1;
                let name = take_cstr(body, &mut offset);
                Self::Close { target, name }
            }
            b'S' => Self::Sync,
            b'H' => Self::Flush,
            b'X' => Self::Terminate,
            b'd' => Self::CopyData(body.to_vec()),
            b'c' => Self::CopyDone,
            b'f' => Self::CopyFail(take_cstr(body, &mut offset)),
            other => Self::Unknown(other),
        }
    }
}

/// `AuthenticationOk`.
fn authentication_frame() -> Vec<u8> {
    typed(b'R', &be_i32(0))
}

/// `ParameterStatus`.
fn parameter_frame(key: &str, value: &str) -> Vec<u8> {
    typed(b'S', &cstr(&[key, value]))
}

/// `BackendKeyData`.
fn backend_key_frame(process_id: i32, secret_key: i32) -> Vec<u8> {
    let mut key = Vec::new();
    key.extend_from_slice(&be_i32(process_id));
    key.extend_from_slice(&be_i32(secret_key));
    typed(b'K', &key)
}

/// `ReadyForQuery` in the idle transaction state.
fn ready_frame() -> Vec<u8> {
    typed(b'Z', b"I")
}

/// `RowDescription` for text-format columns.
fn row_description_frame(columns: &[String]) -> Vec<u8> {
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

/// `DataRow` with every value in text format.
fn data_row_frame(row: &[String]) -> Vec<u8> {
    let mut p = Vec::new();
    p.extend_from_slice(&be_i16(row.len() as i16));
    for cell in row {
        let bytes = cell.as_bytes();
        p.extend_from_slice(&be_i32(bytes.len() as i32));
        p.extend_from_slice(bytes);
    }
    typed(b'D', &p)
}

/// `CommandComplete`.
fn command_complete_frame(tag: &str) -> Vec<u8> {
    typed(b'C', &cstr(&[tag]))
}

/// `ErrorResponse` from `(field type, text)` pairs; the trailing NUL of the
/// field list is added here (`PGProtocolHandler::send_error_response`).
fn error_response_frame(fields: &[(u8, &str)]) -> Vec<u8> {
    let mut p = Vec::new();
    for (field_type, text) in fields {
        p.push(*field_type);
        p.extend_from_slice(text.as_bytes());
        p.push(0);
    }
    p.push(0);
    typed(b'E', &p)
}

/// Reads frontend messages and writes the backend ones for one connection.
///
/// The type is generic over the byte streams so the framing can be tested
/// without a socket; [`PgServer`] instantiates it with a buffered
/// `TcpStream` pair.
pub struct PGProtocolHandler<R, W> {
    reader: R,
    writer: W,
}

impl<R: Read, W: Write> PGProtocolHandler<R, W> {
    pub fn new(reader: R, writer: W) -> Self {
        Self { reader, writer }
    }

    /// Reads the startup header and answers `SSLRequest` with `N` (SSL is not
    /// supported). Returns the body size that follows the version field.
    pub fn read_startup_header(&mut self) -> Result<u32> {
        loop {
            let length = read_i32(&mut self.reader)?;
            if !(8..=10_000).contains(&length) {
                return Err(Error::Invalid(format!("bad startup length {length}")));
            }
            let version = read_i32(&mut self.reader)?;
            if version == SSL_REQUEST_CODE {
                self.writer
                    .write_all(b"N")
                    .map_err(|e| Error::io("<pg>", e))?;
                self.writer.flush().map_err(|e| Error::io("<pg>", e))?;
                continue;
            }
            return Ok((length - 2 * LENGTH_FIELD_SIZE as i32) as u32);
        }
    }

    /// Consumes the startup parameters (`key\0value\0…\0`).
    pub fn read_startup_body(&mut self, body_size: u32) -> Result<()> {
        read_exact_n(&mut self.reader, body_size as usize)?;
        Ok(())
    }

    pub fn send_authentication(&mut self) -> Result<()> {
        self.write_frame(&authentication_frame())
    }

    pub fn send_parameter(&mut self, key: &str, value: &str) -> Result<()> {
        self.write_frame(&parameter_frame(key, value))
    }

    pub fn send_backend_key(&mut self, process_id: i32, secret_key: i32) -> Result<()> {
        self.write_frame(&backend_key_frame(process_id, secret_key))
    }

    pub fn send_ready_for_query(&mut self) -> Result<()> {
        self.write_frame(&ready_frame())
    }

    /// The full `Connection::SendStartupResponse` sequence.
    pub fn send_startup_ok(&mut self) -> Result<()> {
        let mut out = Vec::new();
        out.extend_from_slice(&authentication_frame());
        out.extend_from_slice(&parameter_frame("server_version", "14.0"));
        out.extend_from_slice(&parameter_frame("client_encoding", "UTF8"));
        out.extend_from_slice(&backend_key_frame(1234, 5678));
        out.extend_from_slice(&ready_frame());
        self.write_frame(&out)
    }

    /// Reads one frontend command; `None` when the client closed the socket.
    pub fn read_command(&mut self) -> Result<Option<PgCommand>> {
        let mut typ = [0u8; 1];
        if self.reader.read_exact(&mut typ).is_err() {
            return Ok(None);
        }
        let length = read_i32(&mut self.reader)?;
        if length < LENGTH_FIELD_SIZE as i32 {
            return Err(Error::Invalid(format!("bad command length {length}")));
        }
        let body = read_exact_n(
            &mut self.reader,
            (length - LENGTH_FIELD_SIZE as i32) as usize,
        )?;
        Ok(Some(PgCommand::from_wire(typ[0], &body)))
    }

    pub fn send_row_description(&mut self, columns: &[String]) -> Result<()> {
        self.write_frame(&row_description_frame(columns))
    }

    pub fn send_data_row(&mut self, row: &[String]) -> Result<()> {
        self.write_frame(&data_row_frame(row))
    }

    pub fn send_command_complete(&mut self, tag: &str) -> Result<()> {
        self.write_frame(&command_complete_frame(tag))
    }

    pub fn send_error_response(&mut self, fields: &[(u8, &str)]) -> Result<()> {
        self.write_frame(&error_response_frame(fields))
    }

    /// Runs one simple query and writes its whole response, ending with
    /// `ReadyForQuery`.
    pub fn execute_simple_query(&mut self, engine: &Engine, sql: &str) -> Result<()> {
        let mut out = Vec::new();
        match engine.execute(sql) {
            Ok(results) => {
                let mut rows_sent = 0;
                for result in results {
                    if let QueryResult::Rows { columns, rows } = result {
                        out.extend_from_slice(&row_description_frame(&columns));
                        for row in &rows {
                            out.extend_from_slice(&data_row_frame(row));
                        }
                        rows_sent += rows.len();
                    }
                }
                out.extend_from_slice(&command_complete_frame(&format!("SELECT {rows_sent}")));
            }
            Err(e) => {
                let message = e.to_string();
                out.extend_from_slice(&error_response_frame(&[
                    (b'S', "ERROR"),
                    (b'C', "C00000"),
                    (b'M', message.as_str()),
                ]));
            }
        }
        out.extend_from_slice(&ready_frame());
        self.write_frame(&out)
    }

    /// Startup handshake followed by the command loop.
    pub fn handle_connection(&mut self, engine: &Engine) -> Result<()> {
        let body_size = self.read_startup_header()?;
        self.read_startup_body(body_size)?;
        self.send_startup_ok()?;
        while let Some(command) = self.read_command()? {
            match command {
                PgCommand::SimpleQuery(sql) => self.execute_simple_query(engine, &sql)?,
                PgCommand::Terminate => return Ok(()),
                // The extended protocol, copy sub-protocol and unknown
                // messages are framed but not executed by this stub.
                _ => self.send_ready_for_query()?,
            }
        }
        Ok(())
    }

    fn write_frame(&mut self, frame: &[u8]) -> Result<()> {
        self.writer
            .write_all(frame)
            .map_err(|e| Error::io("<pg>", e))?;
        self.writer.flush().map_err(|e| Error::io("<pg>", e))?;
        Ok(())
    }
}

/// A tiny PostgreSQL-wire server. Every accepted connection is served by a
/// [`PGProtocolHandler`].
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
                Ok(stream) => {
                    let _ = stream.set_nodelay(true);
                    let Ok(reader) = stream.try_clone() else {
                        continue;
                    };
                    let engine = Arc::clone(&self.engine);
                    std::thread::spawn(move || {
                        let mut handler = PGProtocolHandler::new(reader, stream);
                        let _ = handler.handle_connection(&engine);
                    });
                }
                Err(_) => continue,
            }
        }
        unreachable!()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::io::Cursor;
    use std::net::TcpStream;
    use tempfile::tempdir;

    /// `len | protocol | key\0value\0…\0` startup message. An empty parameter
    /// list produces the 8-byte `SSLRequest` shape.
    fn startup_message(protocol: i32, params: &[&str]) -> Vec<u8> {
        let mut body = if params.is_empty() {
            Vec::new()
        } else {
            let mut body = cstr(params);
            body.push(0);
            body
        };
        let mut msg = Vec::new();
        msg.extend_from_slice(&be_i32((LENGTH_FIELD_SIZE + 4 + body.len()) as i32));
        msg.extend_from_slice(&be_i32(protocol));
        msg.append(&mut body);
        msg
    }

    fn command_message(kind: u8, body: &[u8]) -> Vec<u8> {
        let mut msg = vec![kind];
        msg.extend_from_slice(&be_i32((body.len() + LENGTH_FIELD_SIZE) as i32));
        msg.extend_from_slice(body);
        msg
    }

    #[test]
    fn handler_parses_startup_ssl_and_the_command_loop() {
        // SSLRequest first, then a real startup, one simple query, Terminate.
        let mut input = startup_message(SSL_REQUEST_CODE, &[]);
        input.extend_from_slice(&startup_message(196608, &["user", "postgres"]));
        input.extend_from_slice(&command_message(b'Q', b"SELECT 1;\0"));
        input.extend_from_slice(&command_message(b'X', b""));

        let mut handler = PGProtocolHandler::new(Cursor::new(input), Vec::new());
        let body_size = handler.read_startup_header().unwrap();
        assert_eq!(body_size as usize, cstr(&["user", "postgres"]).len() + 1);
        handler.read_startup_body(body_size).unwrap();
        // The SSL refusal is the only byte written so far.
        assert_eq!(handler.writer, b"N".to_vec());

        assert_eq!(
            handler.read_command().unwrap(),
            Some(PgCommand::SimpleQuery("SELECT 1;".to_string()))
        );
        assert_eq!(handler.read_command().unwrap(), Some(PgCommand::Terminate));
        assert_eq!(handler.read_command().unwrap(), None, "clean EOF");
    }

    #[test]
    fn handler_writes_the_startup_and_response_frames() {
        let mut handler = PGProtocolHandler::new(Cursor::new(Vec::new()), Vec::new());
        handler.send_startup_ok().unwrap();
        let startup = String::from_utf8_lossy(&handler.writer).into_owned();
        assert!(startup.starts_with('R'));
        assert!(startup.contains("server_version\0"));
        assert!(startup.contains("14.0"));
        assert!(startup.contains("client_encoding\0UTF8"));
        assert!(startup.ends_with('I'), "ends with ReadyForQuery(idle)");

        handler.writer.clear();
        handler
            .send_row_description(&["id".to_string(), "name".to_string()])
            .unwrap();
        handler
            .send_data_row(&["1".to_string(), "alpha".to_string()])
            .unwrap();
        handler.send_command_complete("SELECT 1").unwrap();
        let frames = String::from_utf8_lossy(&handler.writer).into_owned();
        assert!(frames.contains("SELECT 1"));

        handler.writer.clear();
        handler
            .send_error_response(&[(b'S', "ERROR"), (b'C', "C00000"), (b'M', "boom")])
            .unwrap();
        // `E` + length + 'S'ERROR\0'C'C00000\0'M'boom\0\0
        assert_eq!(handler.writer[0], b'E');
        let body = &handler.writer[5..];
        assert_eq!(body, b"SERROR\0CC00000\0Mboom\0\0");
    }

    #[test]
    fn handler_classifies_every_frontend_command() {
        assert_eq!(
            PgCommand::from_wire(b'P', b"stmt\0SELECT 1\0\0\0"),
            PgCommand::Parse {
                statement: "stmt".to_string(),
                query: "SELECT 1".to_string()
            }
        );
        assert_eq!(
            PgCommand::from_wire(b'B', b"portal\0stmt\0\0\0\0\0"),
            PgCommand::Bind {
                portal: "portal".to_string(),
                statement: "stmt".to_string()
            }
        );
        assert_eq!(
            PgCommand::from_wire(b'D', b"Sstmt\0"),
            PgCommand::Describe {
                target: b'S',
                name: "stmt".to_string()
            }
        );
        assert_eq!(
            PgCommand::from_wire(b'E', b"portal\0\0\0\0\0"),
            PgCommand::Execute {
                portal: "portal".to_string()
            }
        );
        assert_eq!(
            PgCommand::from_wire(b'C', b"Pportal\0"),
            PgCommand::Close {
                target: b'P',
                name: "portal".to_string()
            }
        );
        assert_eq!(PgCommand::from_wire(b'S', b""), PgCommand::Sync);
        assert_eq!(PgCommand::from_wire(b'H', b""), PgCommand::Flush);
        assert_eq!(PgCommand::from_wire(b'X', b""), PgCommand::Terminate);
        assert_eq!(
            PgCommand::from_wire(b'd', b"payload"),
            PgCommand::CopyData(b"payload".to_vec())
        );
        assert_eq!(PgCommand::from_wire(b'c', b""), PgCommand::CopyDone);
        assert_eq!(
            PgCommand::from_wire(b'f', b"oops\0"),
            PgCommand::CopyFail("oops".to_string())
        );
        assert_eq!(PgCommand::from_wire(b'?', b""), PgCommand::Unknown(b'?'));

        for command in [
            PgCommand::SimpleQuery(String::new()),
            PgCommand::Parse {
                statement: String::new(),
                query: String::new(),
            },
            PgCommand::Bind {
                portal: String::new(),
                statement: String::new(),
            },
            PgCommand::Describe {
                target: b'S',
                name: String::new(),
            },
            PgCommand::Execute {
                portal: String::new(),
            },
            PgCommand::Close {
                target: b'S',
                name: String::new(),
            },
            PgCommand::Sync,
            PgCommand::Flush,
            PgCommand::Terminate,
            PgCommand::CopyData(Vec::new()),
            PgCommand::CopyDone,
            PgCommand::CopyFail(String::new()),
            PgCommand::Unknown(b'?'),
        ] {
            let kind = command.kind();
            assert_eq!(
                PgCommand::from_wire(kind, b"").kind(),
                kind,
                "round-trips through its wire byte"
            );
        }
    }

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
        stream
            .write_all(&startup_message(
                196608,
                &["user", "postgres", "database", "db"],
            ))
            .unwrap();

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
