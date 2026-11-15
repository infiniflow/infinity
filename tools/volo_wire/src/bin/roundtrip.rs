//! Decodes a thrift struct from stdin with pilota and re-encodes it to stdout.
//!
//! Used by test_thrift_roundtrip.py to show that the pilota fork's handling of
//! the IDL's reference fields ("&") is wire-transparent: bytes produced by the
//! official thrift encoder survive decode + encode unchanged.

include!(concat!(env!("OUT_DIR"), "/volo_gen.rs"));

use std::io::{Read, Write};

use pilota::thrift::binary::TBinaryProtocol;
use pilota::thrift::Message;
use pilota::{Bytes, BytesMut};
use volo_gen::infinity::{CreateTableRequest, PhysicalType};

fn main() {
    let mut raw = Vec::new();
    std::io::stdin().read_to_end(&mut raw).expect("read stdin");
    let mut input = Bytes::from(raw);

    let req = {
        let mut ip = TBinaryProtocol::new(&mut input, false);
        CreateTableRequest::decode(&mut ip).expect("decode CreateTableRequest")
    };

    let cols = req.column_defs.as_ref().map(|d| d.len()).unwrap_or(0);
    eprintln!("columns={cols}");
    if let Some(defs) = req.column_defs.as_ref() {
        for def in defs {
            let data_type = def.data_type.as_ref();
            let phys = data_type
                .and_then(|d| d.physical_type.as_ref())
                .map(|p| &**p);
            let kind = match phys {
                None => "none".to_string(),
                Some(p) => match p {
                    PhysicalType::NumberType(_) => "NumberType",
                    PhysicalType::VarcharType(_) => "VarcharType",
                    PhysicalType::EmbeddingType(_) => "EmbeddingType",
                    PhysicalType::SparseType(_) => "SparseType",
                    PhysicalType::ArrayType(_) => "ArrayType",
                }
                .to_string(),
            };
            let array_elem = match phys {
                Some(PhysicalType::ArrayType(a)) => a
                    .element_data_type
                    .as_ref()
                    .map(|e| format!("{:?}", e.logic_type))
                    .unwrap_or_else(|| "none".into()),
                _ => "-".into(),
            };
            eprintln!(
                "col {:?} name={:?} logic={:?} phys={kind} array_elem={array_elem}",
                def.id,
                def.name.as_ref().map(|n| n.as_str()),
                data_type.and_then(|d| d.logic_type)
            );
        }
    }

    let mut out = BytesMut::new();
    {
        let mut op = TBinaryProtocol::new(&mut out, false);
        req.encode(&mut op).expect("encode CreateTableRequest");
    }
    std::io::stdout().write_all(&out).expect("write stdout");
}
