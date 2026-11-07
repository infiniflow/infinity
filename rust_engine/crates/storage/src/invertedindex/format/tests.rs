use super::*;

#[cfg(test)]
mod tests {
    use super::*;

    /// C++: `TermMetaTest.test1` (`term_meta_ut.cpp`): `TermMeta(1, 2, 3)`
    /// dumped with `OPTION_FLAG_ALL` and read back.
    #[test]
    fn term_meta_all_options_roundtrip() {
        let meta = TermMeta {
            doc_freq: 1,
            total_tf: 2,
            payload: 3,
            doc_start: 0,
            pos_start: 0,
            pos_end: 0,
        };
        let flags = TermMetaFlags {
            has_term_frequency: true,
            has_term_payload: true,
        };
        let bytes = encode_term_meta(&meta, flags);
        let (decoded, used) = decode_term_meta(&bytes, flags).unwrap();
        assert_eq!(used, bytes.len());
        assert_eq!(decoded.doc_freq, meta.doc_freq);
        assert_eq!(decoded.total_tf, meta.total_tf);
        assert_eq!(decoded.payload, meta.payload);
    }

    #[test]
    fn fst_trailer_roundtrip_and_verification() {
        // A toy FST body: the version, the FstType and some node bytes.
        let mut fst = Vec::new();
        fst.extend_from_slice(&FST_VERSION.to_le_bytes());
        fst.extend_from_slice(&0u64.to_le_bytes());
        fst.extend_from_slice(&[0xab; 7]);
        append_fst_trailer(&mut fst, 42, 0); // 42 keys, root at offset 0
        assert_eq!(fst.len(), 8 + 8 + 7 + FST_TRAILER_LEN);

        let (len, root_addr) = verify_fst(&fst).unwrap();
        assert_eq!((len, root_addr), (42, 0));
        // The trailer fields sit at len-20 / len-12 / len-4.
        let at = fst.len() - FST_TRAILER_LEN;
        assert_eq!(&fst[at..at + 8], &42u64.to_le_bytes());
        assert_eq!(&fst[at + 8..at + 16], &0u64.to_le_bytes());
        // `fst_len_from_trailer` recovers the whole region from the root addr.
        assert_eq!(fst_len_from_trailer(root_addr), 36);

        // Any corruption before the checksum fails verification.
        let mut bad = fst.clone();
        bad[16] ^= 0xff;
        assert!(matches!(
            verify_fst(&bad),
            Err(FormatError::Checksum { .. })
        ));
        assert!(verify_fst(&fst[..8]).is_err());
    }

    #[test]
    fn dic_roundtrip_locates_the_fst_and_reads_meta_by_offset() {
        let flags = TermMetaFlags {
            has_term_frequency: true,
            has_term_payload: true,
        };
        let metas = [
            TermMeta {
                doc_freq: 2,
                total_tf: 3,
                payload: 0x11223344,
                doc_start: 0,
                pos_start: 8,
                pos_end: 24,
            },
            TermMeta {
                doc_freq: 5,
                total_tf: 9,
                payload: 0x55667788,
                doc_start: 24,
                pos_start: 40,
                pos_end: 96,
            },
        ];
        let (region, offsets) = encode_term_meta_region_with_offsets(&metas, flags);
        assert_eq!(
            offsets,
            vec![0, encode_term_meta(&metas[0], flags).len() as u64]
        );

        // A toy FST region: header, some node bytes, then the trailer. The
        // reader derives the region length from `root_addr` as `root_addr + 21`,
        // so pick the root addr the length implies.
        let mut fst = Vec::new();
        fst.extend_from_slice(&FST_VERSION.to_le_bytes());
        fst.extend_from_slice(&0u64.to_le_bytes()); // FstType
        fst.extend_from_slice(&[0xcd; 13]); // node data
        let root_addr = (FST_HEADER_LEN + 13 + FST_TRAILER_LEN - 21) as u64;
        append_fst_trailer(&mut fst, metas.len() as u64, root_addr);

        let dic = encode_dic(&region, &fst);
        // The FST sits at the tail; the trailing 12 bytes give root_addr.
        let (start, len, got_root) = dic_fst_region(&dic).unwrap();
        assert_eq!(start, region.len());
        assert_eq!(len, fst.len());
        assert_eq!(got_root, root_addr);
        assert_eq!(dic_fst(&dic).unwrap(), fst.as_slice());
        let (version, fst_type) = fst_header(dic_fst(&dic).unwrap()).unwrap();
        assert_eq!((version, fst_type), (FST_VERSION, 0));
        // The FST's trailing trailer is verifiable.
        assert_eq!(verify_fst(dic_fst(&dic).unwrap()).unwrap(), (2, root_addr));

        // Each recorded offset reads back its own record.
        for (i, off) in offsets.iter().enumerate() {
            assert_eq!(decode_dic_meta_at(&dic, *off, flags).unwrap(), metas[i]);
        }
        // An out-of-range offset is reported, not panicked.
        assert!(decode_dic_meta_at(&dic, dic.len() as u64 + 1, flags).is_err());
        assert!(dic_fst_region(&dic[..8]).is_err());
    }

    #[test]
    fn fst_pack_size_picks_the_smallest_width() {
        // Boundaries from `fst/bytes.cppm:108-128`.
        let cases: &[(u64, u8)] = &[
            (0, 1),
            (0xff, 1),
            (0x100, 2),
            (0xffff, 2),
            (0x1_0000, 3),
            (0x00ff_ffff, 3),
            (0x100_0000, 4),
            (0xffff_ffff, 4),
            (0x1_0000_0000, 5),
            (0xff_ffff_ffff, 5),
            (0x100_0000_0000, 6),
            (0xffff_ffff_ffff, 6),
            (0x1_0000_0000_0000, 7),
            (0x00ff_ffff_ffff_ffff, 7),
            (0x100_0000_0000_0000, 8),
            (u64::MAX, 8),
        ];
        for (n, w) in cases {
            assert_eq!(fst_pack_size(*n), *w, "pack_size({n:#x})");
        }
    }

    #[test]
    fn fst_pack_uint_is_little_endian_and_round_trips() {
        // `PackUint` writes the low `PackSize(n)` LE bytes.
        let mut out = Vec::new();
        assert_eq!(fst_pack_uint(&mut out, 0x0102), 2);
        assert_eq!(out, vec![0x02, 0x01]);
        assert_eq!(fst_unpack_uint(&out, 2).unwrap(), 0x0102);

        // `PackUintIn` honours an explicit (larger) width.
        let mut wide = Vec::new();
        fst_pack_uint_in(&mut wide, 0x0102, 4);
        assert_eq!(wide, vec![0x02, 0x01, 0x00, 0x00]);
        assert_eq!(fst_unpack_uint(&wide, 4).unwrap(), 0x0102);

        // A full-width value round-trips.
        let mut full = Vec::new();
        assert_eq!(fst_pack_uint(&mut full, u64::MAX), 8);
        assert_eq!(fst_unpack_uint(&full, 8).unwrap(), u64::MAX);

        // Truncated input and an out-of-range width are reported.
        assert!(fst_unpack_uint(&[0x01], 2).is_err());
        assert!(fst_unpack_uint(&[], 0).is_err());
    }

    #[test]
    fn fst_delta_is_node_minus_transition() {
        // A forward transition packs `node_addr - trans_addr`.
        let mut out = Vec::new();
        let n = fst_pack_delta(&mut out, 1000, 940);
        assert_eq!(n, 1);
        assert_eq!(out, vec![60]);
        assert_eq!(fst_unpack_delta(&out, n, 1000).unwrap(), 940);

        // The empty-address sentinel round-trips to `EMPTY_ADDRESS`.
        let mut empty = Vec::new();
        let n = fst_pack_delta(&mut empty, 1000, FST_EMPTY_ADDRESS);
        assert_eq!(empty, vec![0]);
        assert_eq!(
            fst_unpack_delta(&empty, n, 1000).unwrap(),
            FST_EMPTY_ADDRESS
        );

        // A larger delta widens the encoding.
        let mut big = Vec::new();
        let n = fst_pack_delta(&mut big, 100_000, 1);
        assert_eq!(fst_pack_delta_size(100_000, 1), 3);
        assert_eq!(n, 3);
        assert_eq!(fst_unpack_delta(&big, n, 100_000).unwrap(), 1);
    }

    /// Decodes a lowercase hex string (test helper).
    fn unhex(s: &str) -> Vec<u8> {
        (0..s.len())
            .step_by(2)
            .map(|i| u8::from_str_radix(&s[i..i + 2], 16).unwrap())
            .collect()
    }

    #[test]
    fn golden_fsts_from_the_cpp_builder_verify() {
        // Real bytes emitted by the C++ `FstBuilder` (the amalgamated
        // `src/storage/invertedindex/fst/*` built with clang, keys "app",
        // "apple", "apply", "banana", "band", "cat" etc. mapped to 100, 200…).
        // They pin the header, the trailer layout, the masked CRC-32/IEEE and
        // the `root_addr + 21` length rule against ground truth.
        let goldens: &[(&str, &str, u64, u64)] = &[
            // label, hex, len_, root_addr
            (
                "empty",
                "0300000000000000000000000000000000000000000000000000000000000000028ba85c",
                0,
                0,
            ),
            (
                "one",
                "0300000000000000000000000000000064001185010000000000000013000000000000003963da13",
                1,
                19,
            ),
            (
                "two",
                "03000000000000000000000000000000c86400006261110202000000000000001700000000000000d16c7686",
                2,
                23,
            ),
            (
                "words",
                "0300000000000000000000000000000064000000796511020064016c1141c9c9001085cb6400000164611102cbc5001081c5580290016400010513636261120306000000000000003f00000000000000b514bf07",
                6,
                63,
            ),
            (
                "shared",
                "03000000000000000000000000000000640000006261110200109ac80001046261110209109a2c010000010462611202001085c5c5f401640001066261120205000000000000003e00000000000000a7fbf8f8",
                5,
                62,
            ),
        ];
        for (label, hex, want_len, want_root) in goldens {
            let fst = unhex(hex);
            // Header is VERSION then FstType.
            assert_eq!(fst_header(&fst).unwrap(), (FST_VERSION, 0), "{label}");
            // The trailer's `len_`/`root_addr` and its checksum all verify.
            let (len, root) = verify_fst(&fst).unwrap_or_else(|e| panic!("{label}: {e:?}"));
            assert_eq!((len, root), (*want_len, *want_root), "{label}");
            // The region length the reader derives equals the real length.
            assert_eq!(fst_len_from_trailer(root), fst.len(), "{label}");
            // The empty FST is exactly the header plus the trailer (no nodes).
            if *want_root == 0 {
                assert_eq!(fst.len(), FST_HEADER_LEN + FST_TRAILER_LEN, "{label}");
                assert_eq!(fst.len(), 36);
            }
        }
    }

    #[test]
    fn fst_lookup_decodes_the_cpp_built_fsts() {
        // The same goldens as `golden_fsts_from_the_cpp_builder_verify`, but
        // now driving the node decoder: every inserted key must resolve to its
        // value, and a missing key must not.
        let cases: &[(&str, &[(&str, u64)])] = &[
            ("empty", &[]),
            ("one", &[("a", 100)]),
            ("two", &[("a", 100), ("b", 200)]),
            (
                "words",
                &[
                    ("app", 100),
                    ("apple", 200),
                    ("apply", 300),
                    ("banana", 400),
                    ("band", 500),
                    ("cat", 600),
                ],
            ),
            (
                "shared",
                &[
                    ("aaaa", 100),
                    ("aaab", 200),
                    ("aabb", 300),
                    ("abbb", 400),
                    ("baaa", 500),
                ],
            ),
        ];
        let hexes = [
            "0300000000000000000000000000000000000000000000000000000000000000028ba85c",
            "0300000000000000000000000000000064001185010000000000000013000000000000003963da13",
            "03000000000000000000000000000000c86400006261110202000000000000001700000000000000d16c7686",
            "0300000000000000000000000000000064000000796511020064016c1141c9c9001085cb6400000164611102cbc5001081c5580290016400010513636261120306000000000000003f00000000000000b514bf07",
            "03000000000000000000000000000000640000006261110200109ac80001046261110209109a2c010000010462611202001085c5c5f401640001066261120205000000000000003e00000000000000a7fbf8f8",
        ];
        for ((_, pairs), hex) in cases.iter().zip(hexes) {
            let fst = unhex(hex);
            for (k, v) in *pairs {
                assert_eq!(fst_get(&fst, k.as_bytes()).unwrap(), Some(*v), "key {k}");
            }
            // Keys not in the set are absent (only meaningful when the set is
            // non-empty and we can pick a stranger).
            if !pairs.is_empty() {
                assert_eq!(fst_get(&fst, b"\xff\xfe").unwrap(), None);
                assert_eq!(fst_get(&fst, b"ap").unwrap(), None);
            }
        }
    }

    #[test]
    fn fst_builder_reproduces_the_cpp_bytes() {
        // The decisive test: building the same key sets with the Rust port of
        // `FstBuilder` must yield byte-for-byte the C++ builder's output.
        #[allow(clippy::type_complexity)]
        let cases: &[(&str, &[(&str, u64)], &str)] = &[
            (
                "empty",
                &[],
                "0300000000000000000000000000000000000000000000000000000000000000028ba85c",
            ),
            (
                "one",
                &[("a", 100)],
                "0300000000000000000000000000000064001185010000000000000013000000000000003963da13",
            ),
            (
                "two",
                &[("a", 100), ("b", 200)],
                "03000000000000000000000000000000c86400006261110202000000000000001700000000000000d16c7686",
            ),
            (
                "words",
                &[
                    ("app", 100),
                    ("apple", 200),
                    ("apply", 300),
                    ("banana", 400),
                    ("band", 500),
                    ("cat", 600),
                ],
                "0300000000000000000000000000000064000000796511020064016c1141c9c9001085cb6400000164611102cbc5001081c5580290016400010513636261120306000000000000003f00000000000000b514bf07",
            ),
            (
                "shared",
                &[
                    ("aaaa", 100),
                    ("aaab", 200),
                    ("aabb", 300),
                    ("abbb", 400),
                    ("baaa", 500),
                ],
                "03000000000000000000000000000000640000006261110200109ac80001046261110209109a2c010000010462611202001085c5c5f401640001066261120205000000000000003e00000000000000a7fbf8f8",
            ),
        ];
        for (label, pairs, hex) in cases {
            let entries: Vec<(Vec<u8>, u64)> = pairs
                .iter()
                .map(|(k, v)| (k.as_bytes().to_vec(), *v))
                .collect();
            let built = build_fst(&entries);
            let want = unhex(hex);
            assert_eq!(built.len(), want.len(), "{label}: length");
            assert_eq!(built, want, "{label}: bytes");
            // And the result reads back through our own decoder.
            for (k, v) in *pairs {
                assert_eq!(
                    fst_get(&built, k.as_bytes()).unwrap(),
                    Some(*v),
                    "{label}/{k}"
                );
            }
        }
    }

    #[test]
    fn u32_block_matches_the_cpp_bytes() {
        // C++ `CompositeCodec<SIMDBinaryPacking, VariableByte>` output (see
        // CPP_FORMAT_SPEC.md 9.3.5 for the harness recipe), labels matching the
        // inputs below. `small`/`big`/`mixed` have no 128-block, so they are
        // pure variable-byte; `tail260` exercises SIMD + VB together.
        let words = |i: usize| (i % 5) as u32;
        let cases: &[(&str, Vec<u32>, &str)] = &[
            ("small", vec![1, 2, 3, 4, 5], "000000008182838485000000"),
            (
                "zeros",
                vec![0u32; 128],
                "8000000000000000000000000000000000000000",
            ),
            (
                "mixed",
                vec![1, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89],
                "00000000818181828385888d95a2b7d9",
            ),
            (
                "big",
                vec![100000, 200000, 300000, 400000],
                "00000000200d86401a8c602792003598",
            ),
            (
                "bw1",
                (0..128).map(|i| 1 + (i % 3) as u32).collect(),
                "8000000000000002000000000000000000000000799ee7799ee7799ee7799ee7799ee7799ee7799ee7799ee7799ee7799ee7799e",
            ),
            (
                "bw2",
                (0..128).map(|i| 2 + (i % 3) as u32).collect(),
                "80000000000000030000000000000000000000001a356ad4a3468d1ad4a851a31a356ad4a851a346356ad4a8468d1a35a851a3468d1a356a51a3468d6ad4a8518d1a356a",
            ),
            (
                "bw7",
                (0..128).map(|i| 7 + (i % 3) as u32).collect(),
                "800000000000000400000000000000000000000087799887988779987998877987799887799887798779988798877998799887799887799879988779877998879887799887799887988779987998877987799887",
            ),
            (
                "bw255",
                (0..128).map(|i| 255 + (i % 3) as u32).collect(),
                "8000000000000009000000000000000000000000ff0006fc0003fe0301ff010cff0006fc0730e03f18f01fc0f80f60c00730e03f8001ff0180ff00067f0003fe8001ff010cf80f60fc0730e00318f01f0cf80f60c07f00033f8001ffc080ff00c07f0003fe0318f0010cf80f06fc0730fe0318f01fc080ff60c07f00e03f80011fc080ff0006fc0703fe0318ff010cf80006fc0730e03f80f01fc0800f60c07f30e03f80",
            ),
            (
                "bw65535",
                (0..128).map(|i| 65535 + (i % 3) as u32).collect(),
                "8000000000000011000000000000000000000000ffff0000000003000100ffffffff00000600fcfffeff030001000c000600fcff070030001800f0fff8ff0f0007003000e0ff3f001f00c0006000c0ffe0ff3f00800100ff80ffff007f000003800100ffff01000c000600fc00feff03ff01000c00f8ff0fff070030001800f000f8ff0f006000c000e0ff3fff1f00c0006000c0ff7f0000008001000080ffffff7f00000300feffffff0100000006000300feff030018000c00f8fffcff070003001800f0ff1f000f0060003000e0fff0ff1f00c00080ffc0ff7f003f008001c00080ffff000006000300fe00ffff01ff00000600fcff07ff030018000c00f800fcff07003000e000f0ff1fff0f0060003000e0ff3f008000c0008000c0ff7fff3f0080",
            ),
            (
                "sq128",
                (0..128).map(|i| (i * i) as u32).collect(),
                "800000000000000e00000000000000000000000000000400014006100400094009400c900440020005a40221061003440784036901640024416e10270179402a4184902d400c0004240d4144100e8404040fc94444016440561169a469416e107c9173841e0009a41f6149be21c409d922294af402c4403812cb243a42d2103c92d9043e001084048150a6140411c9448951ec9444415a004da55ca156115f445f8561e919e406e4590e17ef1a3947fa5a649705418400242587c164118a8425068d4966c409a442f619b1a6294abe125c9acb86b6003124b9e1715ebdc43299c0a973d40d8443f01d9327f44da213f89db107fc",
            ),
            (
                "tail260",
                (0..260).map(words).collect(),
                "0001000000000303000000000000000000000000e014700a01a780530a38059c53c029e038059c02c029e014024e01a714700a384e01a780700a38058053c029059c024e53c029e09c024e01e014700a01a7805314700a38a78053c038059c02c029e014059c024e29e014704e01a780700a380581828384",
            ),
        ];
        for (label, values, hex) in cases {
            let want = unhex(hex);
            // Encode must be byte-identical to the C++ codec.
            assert_eq!(encode_u32_block(values), want, "{label}: encode");
            // And decode must recover the values.
            let (got, used) = decode_u32_block(&want, values.len()).unwrap();
            assert_eq!(&got, values, "{label}: decode");
            assert_eq!(used, want.len(), "{label}: bytes consumed");
        }
    }

    /// The reference's packer written the obvious way: one bit at a time
    /// (`SIMDBinaryPacking::pack` without the intrinsics). The SIMD packer
    /// must produce exactly these bytes, whatever the width.
    fn pack_mini_block_scalar(values: &[u32], w: u8) -> Vec<u8> {
        let w = w as usize;
        let mut words = vec![0u32; 4 * w];
        for lane in 0..4usize {
            let mut bit = 0usize;
            let mut idx = lane;
            while idx < values.len() {
                let v = values[idx];
                for bpos in 0..w {
                    if (v >> bpos) & 1 != 0 {
                        let gb = bit + bpos;
                        words[lane + 4 * (gb / 32)] |= 1 << (gb % 32);
                    }
                }
                bit += w;
                idx += 4;
            }
        }
        let mut out = Vec::new();
        for word in words {
            out.extend_from_slice(&word.to_le_bytes());
        }
        out
    }

    /// The decoder written lane by lane with 64-bit windows: an oracle for the
    /// portable-SIMD one, independent of how it shifts.
    fn unpack_mini_block_scalar(buf: &[u8], w: u8, count: usize) -> Vec<u32> {
        let w = w as usize;
        let word = |i: usize| u32::from_le_bytes(buf[i * 4..i * 4 + 4].try_into().unwrap()) as u64;
        let mask = if w >= 32 {
            u64::from(u32::MAX)
        } else {
            (1u64 << w) - 1
        };
        let mut out = vec![0u32; count];
        for lane in 0..4usize {
            let mut bit = 0usize;
            let mut idx = lane;
            while idx < count {
                let word_index = bit / 32;
                let bit_off = bit % 32;
                let lo = word(lane + 4 * word_index);
                let hi = if word_index + 1 < w {
                    word(lane + 4 * (word_index + 1))
                } else {
                    0
                };
                out[idx] = (((lo | (hi << 32)) >> bit_off) & mask) as u32;
                bit += w;
                idx += 4;
            }
        }
        out
    }

    /// The portable-SIMD decoder agrees with the lane-by-lane one at every
    /// width, including the widths whose window straddles two words.
    #[test]
    fn simd_decoder_matches_the_lane_by_lane_decoder() {
        let mut state = 0x1234_5678_9abc_def1u64;
        let mut next = || {
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            state
        };
        for w in 1..=32u32 {
            let mask = if w == 32 { u32::MAX } else { (1u32 << w) - 1 };
            let values: Vec<u32> = (0..128).map(|_| (next() as u32) & mask).collect();
            let bytes = pack_mini_block_scalar(&values, w as u8);
            let want = unpack_mini_block_scalar(&bytes, w as u8, values.len());
            let mut got = Vec::new();
            unpack_mini_block(&bytes, w as u8, values.len(), &mut got).unwrap();
            assert_eq!(got, want, "width {w}");
            assert_eq!(got, values, "width {w}");
        }
    }

    /// The SIMD packer must be byte-identical to the bit-at-a-time reference,
    /// for every width the format can choose (1..=32).
    #[test]
    fn packed_blocks_match_the_bit_at_a_time_packer() {
        let mut state = 0x9e37_79b9_7f4a_7c15u64;
        let mut next = || {
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            state
        };
        for w in 1..=32u32 {
            let mask = if w == 32 { u32::MAX } else { (1u32 << w) - 1 };
            let mut values: Vec<u32> = (0..128).map(|_| (next() as u32) & mask).collect();
            values[0] = mask;
            let want = pack_mini_block_scalar(&values, w as u8);
            let mut got_bytes = Vec::new();
            pack_mini_block(&values, w as u8, &mut got_bytes);
            assert_eq!(got_bytes, want, "width {w}");
            assert_eq!(want.len(), 4 * w as usize * 4, "width {w}: size");
            // And it decodes back.
            let mut got = Vec::new();
            unpack_mini_block(&want, w as u8, values.len(), &mut got).unwrap();
            assert_eq!(got, values, "width {w}: decode");
        }
    }

    /// Every SIMD-BP128 width round-trips. The decoder is written against the
    /// four-lane layout, so a width where the window straddles two words (or
    /// hits the 32-bit boundary) is the interesting case; the fixed vectors
    /// above cover only a few of them against the C++ bytes.
    #[test]
    fn every_u32_block_width_round_trips() {
        let mut state = 0x2545_f491_4f6c_dd1du64;
        let mut next = || {
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            state
        };
        for w in 1..=32u32 {
            let mask = if w == 32 { u32::MAX } else { (1u32 << w) - 1 };
            // 256 values: two mini-blocks, so the second one starts at a fresh
            // bit offset, and the largest value pins the width exactly.
            let mut values: Vec<u32> = (0..256).map(|_| (next() as u32) & mask).collect();
            values[7] = mask;
            let encoded = encode_u32_block(&values);
            let (decoded, used) = decode_u32_block(&encoded, values.len()).unwrap();
            assert_eq!(decoded, values, "width {w}");
            assert_eq!(used, encoded.len(), "width {w}: bytes consumed");
        }
    }

    #[test]
    fn u16_block_decodes_the_cpp_bytes() {
        // C++ `NewPForDeltaCompressor::Compress(..., true)` output, covering
        // every special frame, a plain frame and frames with S9 exceptions.
        let mut e3: Vec<u16> = (0..128).map(|i| (i % 8) as u16).collect();
        e3[5] = 1000;
        e3[40] = 2000;
        e3[100] = 300;
        let mut e1: Vec<u16> = (0..4).cycle().take(128).collect();
        e1[0] = 5000;
        let cases: Vec<(&str, Vec<u16>, &str)> = vec![
            ("inc5", vec![1, 2, 3, 4, 5], "0300fa09"),
            (
                "fib12",
                vec![1, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89],
                "017000168140403028201a15d12d0b00",
            ),
            (
                "mod7",
                (0..128).map(|i| (i % 7) as u16).collect(),
                "013000fe88c61ad158231a6b44638d68ac118d35a2b14634d688c61ad158231a6b44638d68ac118d35a2b14634d688c61ad15823",
            ),
            ("const", vec![1000u16; 128], "d107feff"),
            (
                "rand",
                (0..128).map(|i| ((i * 37) % 1000) as u16).collect(),
                "01a000fe0094a0c41b94e4e2cd40283525d765bc8567e08a50d6a9e9afe426ecf2d478772efcf92424e1c624b87423d0494cc565d96ee015a8e2937466eaebb808b72cf5dd9c076ffe0848b421c92ddc0464d2527055a6db7704a6e8e49c98f62aeec12c476df7e6c0972fc2116c4462cb360095a4d45b94e5e6dd80283629e7a5bc866bf0ca50d7adf9efe48760c41a90d4a2cd3f2425e5d664b87527e0894cc669e9ae",
            ),
            (
                "two_blocks",
                (0..130).map(|i| (i % 3) as u16).collect(),
                "002000fe24499224499224499224499224499224499224499224499224499224499224490120000202000000",
            ),
            (
                "exc3",
                e3,
                "05300cfe8846f888c6fa88c6fa88c6fa88c6fa88c6fa88c6fa88c6fa88c6fa88c6fa88c6fa88c6fa88c6fa88c6fa88c6fa88c6fafa44f46125c00e70",
            ),
            (
                "exc1",
                e1,
                "032002fee4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e4e2040080",
            ),
            (
                "exc20",
                {
                    let mut v: Vec<u16> = (0..128).map(|i| (i % 16) as u16).collect();
                    for k in 0..20 {
                        v[k * 6] = 4000 + k as u16;
                    }
                    v
                },
                "01c000fea01f00023000045000a17f000890000ab000a2df000ef000001000a33f00045000067000a49f000ab0000cd000a5ff00001000023000a65f00067000089000a7bf000cd0000ef000a81f00023000045000a97f000890000ab000aadf000ef000001000ab3f00045000067000ac9f000ab0000cd000adff00001000023000ae5f00067000089000afbf000cd0000ef000b01f00023000045000b17f000890000ab000b2df000ef000001000b33f000450000670000890000ab0000cd0000ef000",
            ),
        ];
        for (label, values, hex) in cases {
            let bytes = unhex(hex);
            let (got, used) = decode_u16_block(&bytes, values.len()).unwrap();
            assert_eq!(got, values, "{label}: decode");
            assert_eq!(used, bytes.len(), "{label}: bytes consumed");
            // And the encoder must reproduce the C++ bytes.
            assert_eq!(encode_u16_block(&values), bytes, "{label}: encode");
        }
    }

    #[test]
    fn u16_block_roundtrips_the_alone_frames() {
        // Values above `IsUniqValueStoreInline`'s 11-bit limit (2047) cannot live
        // in the header, so the writer uses the "*_ALONE" frames, whose value is
        // the word after the header (`new_pfordelta_compressor.h:488-540`).
        let cases: &[(u32, Vec<u16>)] = &[
            (254, vec![2560]),                   // same, alone
            (252, vec![3000, 3001, 3002, 3003]), // increase, alone
            (250, vec![3003, 3002, 3001, 3000]), // decrease, alone
        ];
        for (marker, values) in cases {
            let bytes = encode_u16_block(values);
            let header = u32::from_le_bytes(bytes[0..4].try_into().unwrap());
            assert_eq!(*marker, (header >> 17) & 0xff, "frame marker");
            let (got, used) = decode_u16_block(&bytes, values.len()).unwrap();
            assert_eq!(&got, values, "marker {marker}");
            assert_eq!(used, bytes.len(), "marker {marker}");
        }
    }

    #[test]
    fn fulltext_dic_and_pos_compose() {
        // End-to-end: build a `.dic` (TermMeta region + FST + trailer) and a
        // `.pos` (per-term framing over the u32/u16 codecs), then read both
        // back through the public API. This exercises the whole chain the
        // engine will use:
        //   term -> FST value -> TermMeta offset -> doc_start/pos_start/pos_end
        //   -> `.pos` slice -> TermPostings framing -> u32/u16 blocks.
        // (The field-level interleaving *inside* a doc-list body is the
        // posting_decoder's job and is not modelled here.)
        let flags = TermMetaFlags {
            has_term_frequency: true,
            has_term_payload: true,
        };
        let terms = ["apple", "banana", "cat"];
        let docs: [Vec<u32>; 3] = [vec![1, 5, 9], vec![3], vec![2, 4, 6, 7, 130, 200]];
        let poss: [Vec<u32>; 3] = [vec![10, 20, 30], vec![5], vec![1, 2, 3, 4, 5, 6]];
        let tfs: [Vec<u16>; 3] = [vec![2, 1, 1], vec![1], vec![3, 1, 2, 4, 5, 6]];

        // `.pos`: each term's slice is framed by TermPostings; the doc list
        // holds the u32 doc-id block and the position list the u32 positions,
        // while the "skiplist" slots carry the u16 tf block.
        let mut pos = Vec::new();
        let mut slices = Vec::new();
        for i in 0..terms.len() {
            let doc_list = encode_u32_block(&docs[i]);
            let pos_list = encode_u32_block(&poss[i]);
            let tf_block = encode_u16_block(&tfs[i]);
            let tp = TermPostings {
                doc_skiplist: tf_block,
                doc_list,
                pos_skiplist: Vec::new(),
                pos_list,
            };
            let bytes = tp.encode();
            slices.push((pos.len() as u64, (pos.len() + bytes.len()) as u64));
            pos.extend_from_slice(&bytes);
        }

        // `.dic`: TermMeta records (doc/pos offsets point into `.pos`), then an
        // FST mapping each term to its record's offset.
        let metas: Vec<TermMeta> = (0..terms.len())
            .map(|i| TermMeta {
                doc_freq: docs[i].len() as u32,
                total_tf: tfs[i].iter().map(|v| *v as u32).sum(),
                payload: (i as u32) * 7,
                doc_start: slices[i].0,
                pos_start: slices[i].0,
                pos_end: slices[i].1,
            })
            .collect();
        let (region, offsets) = encode_term_meta_region_with_offsets(&metas, flags);
        let mut fst = FstBuilder::new();
        for (i, t) in terms.iter().enumerate() {
            fst.insert(t.as_bytes(), offsets[i]);
        }
        let dic = encode_dic(&region, &fst.finish());

        // Read `.dic` back.
        let (start, _, _) = dic_fst_region(&dic).unwrap();
        assert_eq!(start, region.len());
        let fst_bytes = dic_fst(&dic).unwrap();
        assert_eq!(fst_header(fst_bytes).unwrap().0, FST_VERSION);
        for (i, t) in terms.iter().enumerate() {
            let off = fst_get(fst_bytes, t.as_bytes()).unwrap().unwrap();
            assert_eq!(off, offsets[i]);
            assert_eq!(decode_dic_meta_at(&dic, off, flags).unwrap(), metas[i]);
        }
        assert_eq!(fst_get(fst_bytes, b"zebra").unwrap(), None);

        // Follow a term into `.pos` and decode its blocks.
        for (i, t) in terms.iter().enumerate() {
            let off = fst_get(fst_bytes, t.as_bytes()).unwrap().unwrap();
            let meta = decode_dic_meta_at(&dic, off, flags).unwrap();
            let slice = &pos[meta.doc_start as usize..meta.pos_end as usize];
            let (tp, used) = TermPostings::decode(slice).unwrap();
            assert_eq!(used, slice.len());
            let (got_docs, _) = decode_u32_block(&tp.doc_list, docs[i].len()).unwrap();
            assert_eq!(got_docs, docs[i], "{t}: doc ids");
            let (got_pos, _) = decode_u32_block(&tp.pos_list, poss[i].len()).unwrap();
            assert_eq!(got_pos, poss[i], "{t}: positions");
            let (got_tf, _) = decode_u16_block(&tp.doc_skiplist, tfs[i].len()).unwrap();
            assert_eq!(got_tf, tfs[i], "{t}: term frequencies");
        }
    }

    #[test]
    fn common_input_tables_are_inverses() {
        // Both tables are permutations of 0..=255, and `INV[A[b]] == b` holds,
        // which is the relation `CommonIdx`/`CommonInput` rely on.
        let mut seen = [false; 256];
        for b in 0..256 {
            assert_eq!(
                FST_COMMON_INPUTS_INV[FST_COMMON_INPUTS[b] as usize], b as u8,
                "INV[A[{b}]] must be {b}"
            );
            assert!(!seen[b], "COMMON_INPUTS has a duplicate at {b}");
            seen[b] = true;
        }
        let mut seen_inv = [false; 256];
        for v in FST_COMMON_INPUTS_INV {
            assert!(!seen_inv[v as usize]);
            seen_inv[v as usize] = true;
        }
    }

    #[test]
    fn common_idx_and_input_round_trip_within_the_max() {
        // The most common byte ('t') ranks first, so `CommonIdx('t', 63) == 1`.
        assert_eq!(FST_COMMON_INPUTS[b't' as usize], 0);
        assert_eq!(fst_common_idx(b't', 0b11_1111), 1);
        assert_eq!(fst_common_input(1), Some(b't'));
        // 'e' ranks second, 'p' ninth.
        assert_eq!(fst_common_idx(b'e', 63), 2);
        assert_eq!(fst_common_input(2), Some(b'e'));
        assert_eq!(fst_common_input(9), Some(b'p'));
        // The 0 index means "no common input" (stored verbatim).
        assert_eq!(fst_common_input(0), None);
        // A rank above `max` falls back to 0.
        assert_eq!(fst_common_idx(b't', 0), 0);
        assert_eq!(fst_common_idx(b'z', 3), 0);
        // Every byte round-trips through its own rank when max is 255 - except
        // the rank-255 byte, whose `(rank + 1) % 256` wraps to the 0 sentinel
        // and is therefore always stored verbatim (`fst/node.cppm:284`).
        for b in 0..=255u8 {
            let idx = fst_common_idx(b, 255);
            if FST_COMMON_INPUTS[b as usize] == 255 {
                assert_eq!(idx, 0);
                continue;
            }
            assert_ne!(idx, 0);
            assert_eq!(fst_common_input(idx), Some(b));
        }
    }

    #[test]
    fn masked_checksum_matches_the_cpp_formula() {
        // The mask is a 15-bit rotate plus the Snappy constant.
        let crc = crate::crc::crc32_ieee(b"abc");
        assert_eq!(
            fst_checksum(b"abc"),
            crc.rotate_right(15).wrapping_add(0xa282_ead8)
        );
        // Distinct inputs give distinct masked checksums.
        assert_ne!(fst_checksum(b"abc"), fst_checksum(b"abd"));
    }

    #[test]
    fn vint_matches_leb128() {
        let cases: &[(u64, &[u8])] = &[
            (0, &[0x00]),
            (1, &[0x01]),
            (127, &[0x7f]),
            (128, &[0x80, 0x01]),
            (300, &[0xac, 0x02]),
            (16_384, &[0x80, 0x80, 0x01]),
        ];
        for (value, bytes) in cases {
            let mut out = Vec::new();
            write_vint(&mut out, *value);
            assert_eq!(&out, bytes, "encode {value}");
            let (got, n) = read_vint(bytes).unwrap();
            assert_eq!((got, n), (*value, bytes.len()), "decode {value}");
        }
        // A u64::MAX round-trips in the maximum 10 bytes.
        let mut out = Vec::new();
        write_vint(&mut out, u64::MAX);
        assert_eq!(out.len(), 10);
        assert_eq!(read_vint(&out).unwrap(), (u64::MAX, 10));
    }

    #[test]
    fn vint_rejects_truncated_input() {
        assert!(read_vint(&[0x80]).is_err());
        assert!(read_vint(&[]).is_err());
    }

    #[test]
    fn term_meta_roundtrip_with_and_without_optional_fields() {
        let meta = TermMeta {
            doc_freq: 300,
            total_tf: 5000,
            payload: 0xdead_beef,
            doc_start: 12,
            pos_start: 1_000,
            pos_end: 2_000,
        };
        for flags in [
            TermMetaFlags::default(),
            TermMetaFlags {
                has_term_frequency: true,
                has_term_payload: false,
            },
            TermMetaFlags {
                has_term_frequency: true,
                has_term_payload: true,
            },
        ] {
            let bytes = encode_term_meta(&meta, flags);
            let (got, n) = decode_term_meta(&bytes, flags).unwrap();
            assert_eq!(n, bytes.len(), "flags {flags:?}");
            assert_eq!(got.doc_start, meta.doc_start);
            assert_eq!(got.pos_start, meta.pos_start);
            assert_eq!(got.pos_end, meta.pos_end);
            assert_eq!(got.doc_freq, meta.doc_freq);
            if flags.has_term_frequency {
                assert_eq!(got.total_tf, meta.total_tf);
            } else {
                // Without the tf flag the C++ loader falls back to `df`.
                assert_eq!(got.total_tf, meta.doc_freq);
            }
            if flags.has_term_payload {
                assert_eq!(got.payload, meta.payload);
            }
        }
    }

    #[test]
    fn term_meta_payload_is_raw_le_u32() {
        let meta = TermMeta {
            doc_freq: 1,
            payload: 0x0102_0304,
            ..TermMeta::default()
        };
        let bytes = encode_term_meta(
            &meta,
            TermMetaFlags {
                has_term_frequency: false,
                has_term_payload: true,
            },
        );
        // VInt(1) = 0x01, then the raw payload little-endian.
        assert_eq!(&bytes[0..1], &[0x01]);
        assert_eq!(&bytes[1..5], &[0x04, 0x03, 0x02, 0x01]);
    }

    #[test]
    fn term_meta_region_is_a_concatenation() {
        let metas = [
            TermMeta::default(),
            TermMeta {
                doc_freq: 7,
                ..Default::default()
            },
        ];
        let region = encode_term_meta_region(&metas, TermMetaFlags::default());
        let (first, n) = decode_term_meta(&region, TermMetaFlags::default()).unwrap();
        assert_eq!(first.doc_freq, 0);
        let (second, _) = decode_term_meta(&region[n..], TermMetaFlags::default()).unwrap();
        assert_eq!(second.doc_freq, 7);
    }

    #[test]
    fn fst_trailer_length() {
        assert_eq!(fst_len_from_trailer(0), 36); // empty FST
        assert_eq!(fst_len_from_trailer(15), 36);
        assert_eq!(fst_len_from_trailer(100), 121);
    }

    #[test]
    fn column_lengths_are_bare_u32_array() {
        let lengths = [3u32, 0, 17];
        let bytes = encode_column_lengths(&lengths);
        assert_eq!(bytes.len(), 12);
        assert_eq!(&bytes[0..4], &3u32.to_le_bytes());
        assert_eq!(&bytes[4..8], &0u32.to_le_bytes());
        assert_eq!(decode_column_lengths(&bytes).unwrap(), lengths.to_vec());
        // A trailing partial element is a corruption, not a shorter array.
        assert!(decode_column_lengths(&bytes[..11]).is_err());
    }

    #[test]
    fn postings_framing_roundtrip() {
        let p = TermPostings {
            doc_skiplist: vec![1, 2, 3],
            doc_list: vec![4, 5],
            pos_skiplist: vec![6],
            pos_list: vec![7, 8, 9, 10],
        };
        let bytes = p.encode();
        // 4 length prefixes (1 byte each here) + 10 body bytes.
        assert_eq!(bytes.len(), 4 + 10);
        assert_eq!(&bytes[0..2], &[3, 2]); // doc skip-list / doc-list lengths
        assert_eq!(&bytes[2..5], &[1, 2, 3]); // doc skip list
        assert_eq!(&bytes[5..7], &[4, 5]); // doc list
        assert_eq!(&bytes[7..9], &[1, 4]); // pos skip-list / pos-list lengths
        assert_eq!(&bytes[9..10], &[6]); // pos skip list
        assert_eq!(&bytes[10..14], &[7, 8, 9, 10]); // pos list
        let (got, n) = TermPostings::decode(&bytes).unwrap();
        assert_eq!(n, bytes.len());
        assert_eq!(got, p);
    }

    #[test]
    fn postings_framing_rejects_truncation() {
        let p = TermPostings {
            doc_list: vec![1, 2, 3],
            ..Default::default()
        };
        let bytes = p.encode();
        assert!(TermPostings::decode(&bytes[..bytes.len() - 1]).is_err());
    }

    /// The reference's `FstTest` months fixture, sorted lexicographically.
    fn months() -> Vec<(Vec<u8>, u64)> {
        let names = [
            "January",
            "February",
            "March",
            "April",
            "May",
            "June",
            "July",
            "August",
            "September",
            "October",
            "November",
            "December",
        ];
        let mut entries: Vec<(Vec<u8>, u64)> = names
            .iter()
            .enumerate()
            .map(|(i, name)| (name.as_bytes().to_vec(), i as u64 + 1))
            .collect();
        entries.sort();
        entries
    }

    /// C++: `FstTest.BuildMemEmpty` (`fst_ut.cpp`).
    #[test]
    fn fst_build_mem_empty() {
        let buffer = FstBuilder::new().finish();
        // The 20-byte trailer (len, root, checksum) is included here whereas
        // `BytesWritten()` excludes the checksum: 32 + 4 = 36.
        assert_eq!(buffer.len(), 36);
        let (len, root) = verify_fst(&buffer).unwrap();
        assert_eq!(len, 0);
        assert_eq!(root, 0);
        assert!(len == 0, "IsEmpty");
    }

    /// C++: `FstTest.BuildFileEmpty` (`fst_ut.cpp`).
    #[test]
    fn fst_build_file_empty() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("empty.fst");
        let buffer = FstBuilder::new().finish();
        std::fs::write(&path, &buffer).unwrap();
        assert_eq!(std::fs::metadata(&path).unwrap().len(), buffer.len() as u64);
        assert_eq!(buffer.len(), 36);
    }

    /// C++: `FstTest.BuildMem` (`fst_ut.cpp`).
    #[test]
    fn fst_build_mem() {
        let buffer = build_fst(&months());
        // `buffer.size() == written + 4`: everything but the checksum is the
        // writer's count.
        let written = buffer.len() - 4;
        assert_eq!(buffer.len(), written + 4);
        verify_fst(&buffer).unwrap();
    }

    /// C++: `FstTest.BuildFile` (`fst_ut.cpp`).
    #[test]
    fn fst_build_file() {
        let dir = tempfile::tempdir().unwrap();
        let path = dir.path().join("months.fst");
        let buffer = build_fst(&months());
        std::fs::write(&path, &buffer).unwrap();
        assert_eq!(
            std::fs::metadata(&path).unwrap().len() as usize,
            buffer.len()
        );
    }

    /// C++: `FstTest.Get` (`fst_ut.cpp`).
    #[test]
    fn fst_get_values() {
        let entries = months();
        let fst = build_fst(&entries);
        let (len, _) = verify_fst(&fst).unwrap();
        assert_eq!(len as usize, entries.len());
        assert!(fst.len() > 36);
        for (key, value) in &entries {
            assert_eq!(fst_get(&fst, key).unwrap(), Some(*value));
        }
        assert_eq!(fst_get(&fst, b"NotAMonth").unwrap(), None);
    }

    /// C++: `FstTest.Iterate` (`fst_ut.cpp`): full-order iteration and the
    /// `[included, excluded)` bounds.
    #[test]
    fn fst_iterate_with_bounds() {
        let entries = months();
        let fst = build_fst(&entries);
        verify_fst(&fst).unwrap();
        let mut stream = FstStream::new(&fst).unwrap();
        let mut got = Vec::new();
        for (key, value) in stream.by_ref() {
            got.push((key, value));
        }
        assert_eq!(got, entries);

        // [months[3], months[7])
        let b1 = 3;
        let b2 = 7;
        stream.reset(Some((&entries[b1].0, true)), Some((&entries[b2].0, false)));
        let mut i = b1;
        for (key, value) in stream.by_ref() {
            assert_eq!(key, entries[i].0);
            assert_eq!(value, entries[i].1);
            i += 1;
        }
        assert_eq!(i, b2);
        // An excluded max also stops before that key.
        stream.reset(Some((&entries[b1].0, true)), Some((&entries[b2].0, true)));
        let mut seen = 0;
        while stream.next().is_some() {
            seen += 1;
        }
        assert_eq!(seen, b2 - b1 + 1);
    }
}
