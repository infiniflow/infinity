#!/usr/bin/env python3

src_fp = "./test/data/csv/enwiki_9999.csv"


def generate(num_rows: int = 9999):
    """Generate enwiki_embedding_plus CSV with all index types

    Args:
        num_rows: Number of rows to generate (1-9999), default 9999

    Raises:
        ValueError: if num_rows is not in range 1-9999
    """
    if not isinstance(num_rows, int) or num_rows < 1 or num_rows > 9999:
        raise ValueError("num_rows must be an integer between 1 and 9999")

    # Table structure:
    # doctitle: varchar (from original)
    # docdate: varchar (from original)
    # body: varchar (from original)
    # num: int (row number, high secondary index)
    # vector_col: vector, 2048, float (hnsw vector index)
    # multi_vector_col: multivector, 2, float (2 * 1024 = 2048 values, hnsw multi vector index)
    # category: varchar (A/B/C/D, low secondary index)
    # sparse_col: sparse, 1024, float, int16
    dst_fp = f"./test/data/csv/enwiki_embedding_plus_{num_rows}.csv"

    # Categories: A, B, C, D
    categories = ['A', 'B', 'C', 'D']

    # Generate sparse vector: 100 random indices out of 1024, with int values 1-100
    import random
    random.seed(42)

    def generate_sparse(row_id):
        random.seed(row_id + 42)
        indices = random.sample(range(1024), 100)
        values = [float(random.randint(1, 100)) for _ in range(100)]
        sorted_pairs = sorted(zip(indices, values))
        return sorted_pairs

    with open(src_fp) as src:
        with open(dst_fp, 'w') as dst:
            for i, line in enumerate(src):
                if i >= num_rows:
                    break
                # Strip the original line (tab-separated: title\tdate\tbody)
                original = line.rstrip()

                # Generate 2048-dim vector (all values = i)
                vector_2048 = ', '.join([str(i)] * 2048)

                # Generate multi-vector (2 vectors, each 1024-dim, total 2048 values)
                multi_vector = ', '.join([str(i)] * 2048)

                # Category (cycle through A, B, C, D)
                category = categories[i % 4]

                # Generate sparse vector (100 elements out of 1024, with int values 1-100)
                sparse_pairs = generate_sparse(i)
                sparse_parts = [f"{idx}:{val}" for idx, val in sparse_pairs]
                sparse_str = '[' + ', '.join(sparse_parts) + ']'

                # Combine: original + num + vector + multi_vector + category + sparse
                new_line = f"{original}\t{i}\t[{vector_2048}]\t[{multi_vector}]\t{category}\t{sparse_str}\n"
                dst.write(new_line)


if __name__ == "__main__":
    generate()
