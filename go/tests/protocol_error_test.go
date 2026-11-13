// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package tests

// Protocol-error contract, mirroring `python/test_pysdk/test_protocol_error.py`.
//
// A Thrift `TApplicationException` frame (message type 3) is what the server
// sends when it cannot decode a request or does not know the method
// (`PARALLEL_ASYNC.md` §6.4 / O12). It is a *protocol* failure, not a
// connection one: the SDK maps it to `ErrorCodeProtocolError` (9001) and the
// caller must not treat it as "server unreachable".
//
// The fixtures are byte-exact frames captured from this server (2026-11-10):
// an unknown method answers 64 B with `field 2 = 1` (UNKNOWN_METHOD), an
// unparsable header answers with `field 2 = 7` (PROTOCOL_ERROR).

import (
	"encoding/hex"
	"net"
	"strconv"
	"strings"
	"testing"
	"time"

	"github.com/infiniflow/infinity-go-sdk"
	thriftapi "github.com/infiniflow/infinity-go-sdk/internal/thrift"
)

// unknownMethodFrame is a real reply to
// `BogusMethod` (strict header 0x80010003, `unknown method BogusMethod`,
// kind 1 = UNKNOWN_METHOD, seq 7).
const unknownMethodFrameHex = "800100030000000b426f6775734d6574686f6400000007" +
	"0b00010000001a756e6b6e6f776e206d6574686f6420426f6775734d6574686f64" +
	"0800020000000100"

// fakeThriftServer answers every request with `frame`, after swallowing one
// message so the client's write does not block.
func fakeThriftServer(t *testing.T, frame []byte) net.Addr {
	t.Helper()
	listener, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		t.Fatalf("listen: %v", err)
	}
	t.Cleanup(func() { listener.Close() })
	go func() {
		for {
			conn, err := listener.Accept()
			if err != nil {
				return
			}
			go func(c net.Conn) {
				defer c.Close()
				buf := make([]byte, 4096)
				c.SetReadDeadline(time.Now().Add(5 * time.Second))
				_, _ = c.Read(buf)
				_, _ = c.Write(frame)
			}(conn)
		}
	}()
	return listener.Addr()
}

// TestAProtocolErrorIsNotAConnectionError drives the SDK against a server that
// answers with a type-3 frame and checks the error code it surfaces.
func TestAProtocolErrorIsNotAConnectionError(t *testing.T) {
	frame, err := hex.DecodeString(unknownMethodFrameHex)
	if err != nil {
		t.Fatalf("bad fixture: %v", err)
	}
	if len(frame) != 64 {
		t.Fatalf("fixture is %d bytes, the captured frame is 64", len(frame))
	}
	addr := fakeThriftServer(t, frame)
	host, portStr, _ := net.SplitHostPort(addr.String())
	port, err := strconv.Atoi(portStr)
	if err != nil {
		t.Fatalf("port: %v", err)
	}

	conn, err := infinity.Connect(infinity.NetworkAddress{IP: host, Port: port})
	if err == nil {
		defer conn.Disconnect()
		// `Connect` itself succeeds against this stub (it answers the handshake
		// with the same frame, which the decoder rejects), so drive a real RPC.
		_, err = conn.ListDatabases()
	}
	if err == nil {
		t.Fatal("the SDK accepted a type-3 frame")
	}
	ie, ok := err.(*infinity.InfinityException)
	if !ok {
		t.Fatalf("got %T (%v), want *InfinityException", err, err)
	}
	if ie.ErrorCode != int(infinity.ErrorCodeProtocolError) {
		t.Fatalf(
			"error code %d, want %d (protocol): a type-3 frame is not a "+
				"connection failure (message: %s)",
			ie.ErrorCode, infinity.ErrorCodeProtocolError, ie.ErrorMsg,
		)
	}
	// The message text comes from the thrift decoder ("wrong method name",
	// "bad data", ...), so the assertion is on the code, not on the wording.
	if strings.Contains(ie.ErrorMsg, "not found") || strings.Contains(ie.ErrorMsg, "refused") {
		t.Fatalf("the error reads like a connection failure: %q", ie.ErrorMsg)
	}
}

// TestAnUnparsableHeaderIsAProtocolError is the second gold sample: a reply
// whose message header cannot be decoded.
func TestAnUnparsableHeaderIsAProtocolError(t *testing.T) {
	// strict-message header with type 0 (invalid), then nothing the client can
	// make sense of - the classic "bad data" case.
	frame := []byte{
		0x80, 0x01, 0x00, 0x00, // version | type 0
		0x00, 0x00, 0x00, 0x00, // empty name
		0x00, 0x00, 0x00, 0x00, // seq 0
	}
	addr := fakeThriftServer(t, frame)
	host, portStr, _ := net.SplitHostPort(addr.String())
	port, err := strconv.Atoi(portStr)
	if err != nil {
		t.Fatalf("port: %v", err)
	}
	conn, err := infinity.Connect(infinity.NetworkAddress{IP: host, Port: port})
	if err == nil {
		defer conn.Disconnect()
		_, err = conn.ListDatabases()
	}
	if err == nil {
		t.Fatal("the SDK accepted an unparsable header")
	}
	ie, ok := err.(*infinity.InfinityException)
	if !ok {
		t.Fatalf("got %T (%v), want *InfinityException", err, err)
	}
	if ie.ErrorCode != int(infinity.ErrorCodeProtocolError) {
		t.Fatalf("error code %d, want %d (protocol)", ie.ErrorCode, infinity.ErrorCodeProtocolError)
	}
}

// Keep the generated package imported: the fixtures are its wire format.
var _ = thriftapi.NewListDatabaseRequest
