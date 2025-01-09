import re


def parse_log1(log_file: str) -> tuple[list[int], list[int]]:
    # line example: [12:56:58.986] [815716] [info] Compact commit: test_compact2, new segment: 1492, old segment: 1473 1474 1475 1476
    lines = []
    with open(log_file, "r") as f:
        for line in f:
            if "Compact commit: " in line:
                lines.append(line)
    remove_segs = set()
    add_segs = set()
    re_pattern = re.compile(
        r".*Compact commit: (\w+), new segment: (\d+), old segment: (.*)"
    )
    for line in lines:
        m = re_pattern.match(line)
        if not m:
            print("No match")

        grp = m.groups()
        table_name = grp[0]
        new_segment = int(grp[1])
        old_segments = list(map(int, grp[2].split(" ")[:-1]))
        # print(f"Compact {old_segments} -> {new_segment}")

        add_segs.add(new_segment)
        for old_segment in old_segments:
            if old_segment in remove_segs:
                raise ValueError(
                    f"Old segment {old_segment} has already been compacted"
                )
            if old_segment in add_segs:
                add_segs.remove(old_segment)
            remove_segs.add(old_segment)

    remove_segs = list(remove_segs)
    add_segs = list(add_segs)
    remove_segs.sort()
    add_segs.sort()
    return remove_segs, add_segs


def parse_log2(log_file) -> tuple[list[int], list[int]]:
    # line example: [11:08:19.727] [55245] [info] Read delta op: AddSegmentEntryOp begin_ts: 8142, txn_id: 4281, commit_ts: 8143, merge_flag: 2, encode: #default_db#test_compact2#3978 min_row_ts: 8143 max_row_ts: 8143 first_delete_ts: 18446744073709551615 row_capacity: 8388608 row_count: 3 actual_row_count: 3 column_count: 2
    # line2 example:
    #     [COMPACT]
    # Compact:
    # db name: default_db
    # table name: test_compact2
    # deprecated segment: 49328 | 49329 | 49330 | 49331 |
    # new segment: segment_id: 49333, column_count: 2, row_count: 12, actual_row_count: 12, row_capacity: 8388608, block_info count: 1

    lines = []
    lines2 = []
    with open(log_file, "r") as f:
        for line in f:
            if "AddSegmentEntryOp" in line:
                lines.append(line)
            elif "[COMPACT]" in line:
                concat_line = line
                for i in range(5):
                    try:
                        concat_line += next(f)
                    except StopIteration:
                        break
                concat_line = concat_line.replace("\n", " ")
                lines2.append(concat_line)
    remove_segs2 = set()
    add_segs2 = set()
    re_pattern = re.compile(r".*AddSegmentEntryOp.*merge_flag: (\d+), encode: (\S+)")
    for line in lines:
        m = re_pattern.match(line)
        if not m:
            print("No match")
            continue

        grp = m.groups()
        merge_flag = int(grp[0])
        encode = grp[1]
        segment_id = int(encode.split("#")[-1])
        if merge_flag == 2:
            add_segs2.add(segment_id)
        elif merge_flag == 1:
            if segment_id in add_segs2:
                add_segs2.remove(segment_id)
            remove_segs2.add(segment_id)

    re_pattern2 = re.compile(r".*deprecated segment: ([\d\| ]*).*segment_id: (\d+)")
    for line2 in lines2:
        # print(line2)
        m = re_pattern2.match(line2)
        if not m:
            print("No match")
            continue

        grp = m.groups()
        old_segments = list(map(int, grp[0].split("|")[:-1]))
        new_segment = int(grp[1])
        add_segs2.add(new_segment)
        for old_segment in old_segments:
            if old_segment in remove_segs2:
                raise ValueError(
                    f"Old segment {old_segment} has already been compacted"
                )
            else:
                if old_segment in add_segs2:
                    add_segs2.remove(old_segment)
                remove_segs2.add(old_segment)

    remove_segs2 = list(remove_segs2)
    add_segs2 = list(add_segs2)
    remove_segs2.sort()
    add_segs2.sort()
    return remove_segs2, add_segs2


if __name__ == "__main__":
    log_file1 = "restart_test.log.37"
    remove_segs, add_segs = parse_log1(log_file1)
    print("remove_segs", remove_segs)
    print("add_segs", add_segs)

    log_file2 = "restart_test.log.38"
    remove_segs2, add_segs2 = parse_log2(log_file2)
    print("remove_segs2", remove_segs2)
    print("add_segs2", add_segs2)
