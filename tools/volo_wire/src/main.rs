mod svc;
include!(concat!(env!("OUT_DIR"), "/volo_gen.rs"));

use volo::net::incoming::DefaultIncoming;
use volo_thrift::codec::default::thrift::MakeThriftCodec;
use volo_thrift::codec::default::DefaultMakeCodec;

/// Unframed binary thrift, so the same python/go SDKs that talk to the Rust
/// server can talk to this one.
#[tokio::main(flavor = "multi_thread")]
async fn main() {
    let addr = std::env::var("VOLO_WIRE_ADDR").unwrap_or_else(|_| "127.0.0.1:28317".into());
    let listener = tokio::net::TcpListener::bind(&addr).await.unwrap();
    println!("volo_wire listening on {addr}");
    let _ = volo_gen::infinity::InfinityServiceServer::new(svc::Svc)
        .make_codec(DefaultMakeCodec::new(MakeThriftCodec::new()))
        .run(DefaultIncoming::from(listener))
        .await;
}
