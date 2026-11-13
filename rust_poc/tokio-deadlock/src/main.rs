//! PoC A (RUST_REWRITE_PLAN.md 附录 B): prove that a bounded exchange advances
//! only when the consumer drains *concurrently* with the producer — independent
//! of the consumer fragment "type".
//!
//! This encodes the corrected rule from the plan (§3 D4 ⚠️):
//!   "凡经有界队列交接的边，消费者都必须与生产者并发排空" (any bounded-queue edge
//!   must be drained concurrently with its producer).
//!
//! C++ source evidence: `src/scheduler/fragment_context_impl.cpp:592` triggers
//! early parent scheduling based on the *child* (producer) being kParallelStream.
//!
//! Cases:
//!   1. wrong_sequential  : consumer waits for the producer to finish -> DEADLOCK (expected)
//!   2. right_concurrent  : consumer drains concurrently            -> completes
//!   3. early_limit       : consumer stops after K (early LIMIT)    -> producer stops, no hang
//!   4. error_propagation : producer errors                         -> consumer observes close
//!   5. cancellation      : CancellationToken cascades              -> both stop
//!
//! Run:  cargo run -p tokio-deadlock
//! Exit code is non-zero if any case does not behave as required.

use std::time::Duration;

use tokio::sync::mpsc;
use tokio_util::sync::CancellationToken;

const CAP: usize = 1; // capacity 1 is the worst case for handoff deadlocks

#[tokio::main(flavor = "multi_thread", worker_threads = 4)]
async fn main() {
    let cases: Vec<(&str, std::pin::Pin<Box<dyn std::future::Future<Output = Result<(), String>> + Send>>)> = vec![
        ("wrong_sequential (expects deadlock)", Box::pin(case_wrong_sequential())),
        ("right_concurrent (expects progress)", Box::pin(case_right_concurrent())),
        ("early_limit (expects clean stop)", Box::pin(case_early_limit())),
        ("error_propagation", Box::pin(case_error_propagation())),
        ("cancellation", Box::pin(case_cancellation())),
    ];

    let mut failed = 0;
    println!("== PoC A: bounded-exchange progress (capacity = {CAP}) ==");
    for (name, fut) in cases {
        match fut.await {
            Ok(()) => println!("  PASS  {name}"),
            Err(e) => {
                failed += 1;
                println!("  FAIL  {name}: {e}");
            }
        }
    }
    println!("== {} / {} cases passed ==", 5 - failed, 5);
    std::process::exit(if failed == 0 { 0 } else { 1 });
}

/// WRONG: consume only after the producer has fully finished.
/// With capacity 1 and >1 items, the producer blocks on `send().await`
/// and never completes -> the `timeout` must fire.
async fn case_wrong_sequential() -> Result<(), String> {
    let (tx, mut _rx) = mpsc::channel::<u32>(CAP);
    let mut producer = tokio::spawn(async move {
        for i in 0..100u32 {
            // tolerate the receiver being dropped by an aborted parent
            if tx.send(i).await.is_err() {
                return;
            }
        }
    });

    let res = tokio::time::timeout(Duration::from_millis(400), &mut producer).await;
    producer.abort();
    match res {
        Err(_) => Ok(()), // deadlock observed, as required
        Ok(_) => Err("producer unexpectedly completed; the wrong pattern did NOT deadlock".into()),
    }
}

/// RIGHT: drain concurrently with the producer.
async fn case_right_concurrent() -> Result<(), String> {
    const N: u32 = 1000;
    let (tx, mut rx) = mpsc::channel::<u32>(CAP);
    let producer = tokio::spawn(async move {
        for i in 0..N {
            if tx.send(i).await.is_err() {
                return Err("channel closed early".to_string());
            }
        }
        Ok(())
    });

    let mut got: u32 = 0;
    while rx.recv().await.is_some() {
        got += 1;
    }
    producer
        .await
        .map_err(|e| format!("join error: {e}"))?
        .map_err(|e| format!("producer error: {e}"))?;
    if got != N {
        return Err(format!("expected {N} items, got {got}"));
    }
    Ok(())
}

/// RIGHT: early LIMIT — the consumer stops after K and drops the receiver.
/// The producer must observe the closed channel and stop (no hang, no leak).
async fn case_early_limit() -> Result<(), String> {
    let (tx, mut rx) = mpsc::channel::<u32>(CAP);
    let producer = tokio::spawn(async move {
        let mut sent: u32 = 0;
        for i in 0..1_000_000u32 {
            if tx.send(i).await.is_err() {
                return sent; // receiver dropped -> stop producing
            }
            sent += 1;
        }
        sent
    });

    let mut taken = 0;
    for _ in 0..5 {
        if rx.recv().await.is_some() {
            taken += 1;
        }
    }
    drop(rx); // early LIMIT: stop consuming

    let sent = tokio::time::timeout(Duration::from_secs(1), producer)
        .await
        .map_err(|_| "producer hung after receiver was dropped".to_string())?
        .map_err(|e| format!("join error: {e}"))?;
    if taken != 5 {
        return Err(format!("expected to take 5, took {taken}"));
    }
    if sent > 5 + CAP as u32 + 2 {
        return Err(format!("producer kept producing after early stop: sent={sent}"));
    }
    Ok(())
}

/// Error propagation: the producer fails; the consumer's `recv()` must observe
/// the closed channel rather than hanging.
async fn case_error_propagation() -> Result<(), String> {
    let (tx, mut rx) = mpsc::channel::<u32>(CAP);
    let producer = tokio::spawn(async move {
        for i in 0..10u32 {
            tx.send(i).await.map_err(|_| "closed".to_string())?;
        }
        Err::<(), String>("simulated producer failure".into())
    });

    let mut got = 0;
    while rx.recv().await.is_some() {
        got += 1;
    }
    let pr = producer.await.map_err(|e| format!("join error: {e}"))?;
    if pr.is_ok() {
        return Err("expected producer error".into());
    }
    if got != 10 {
        return Err(format!("expected 10 items before error, got {got}"));
    }
    Ok(())
}

/// Cancellation: a CancellationToken cascades to both ends; a mid-stream cancel
/// must stop both the producer and the consumer promptly.
async fn case_cancellation() -> Result<(), String> {
    let token = CancellationToken::new();
    let (tx, mut rx) = mpsc::channel::<u32>(CAP);

    let producer_token = token.clone();
    let producer = tokio::spawn(async move {
        let mut sent = 0u32;
        loop {
            if producer_token.is_cancelled() {
                break;
            }
            if tx.send(sent).await.is_err() {
                break;
            }
            sent += 1;
            tokio::task::yield_now().await;
        }
        sent
    });

    let consumer_token = token.clone();
    let consumer = tokio::spawn(async move {
        let mut got = 0u32;
        loop {
            tokio::select! {
                _ = consumer_token.cancelled() => break,
                msg = rx.recv() => {
                    if msg.is_none() { break; }
                    got += 1;
                }
            }
        }
        got
    });

    // let it stream briefly, then cancel
    tokio::time::sleep(Duration::from_millis(20)).await;
    token.cancel();

    let got = tokio::time::timeout(Duration::from_secs(1), consumer)
        .await
        .map_err(|_| "consumer did not stop after cancellation".to_string())?
        .map_err(|e| format!("consumer join error: {e}"))?;
    let _sent = tokio::time::timeout(Duration::from_secs(1), producer)
        .await
        .map_err(|_| "producer did not stop after cancellation".to_string())?
        .map_err(|e| format!("producer join error: {e}"))?;

    if got == 0 {
        return Err("expected some items before cancellation".into());
    }
    Ok(())
}
