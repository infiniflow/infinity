#!/usr/bin/env python3

src_fp = "./test/data/csv/enwiki_9999.csv"
dst_fp = "./test/data/csv/enwiki_embedding_plus_9999.csv"

def generate():
    """Generate enwiki_embedding_plus_9999.csv with all index types

    Table structure:
    - doctitle: varchar (from original)
    - docdate: varchar (from original)
    - body: varchar (from original)
    - num: int (row number, high secondary index)
    - vector_col: vector, 2048, float (hnsw vector index)
    - multi_vector_col: multivector, 2, float (2 * 1024 = 2048 values, hnsw multi vector index)
    - category: varchar (A/B/C/D, low secondary index)
    - sparse_col: sparse, 1024, float, int8 (bmp index)
    """
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
                # Strip the original line (tab-separated: title\tdate\tbody)
                original = line.rstrip()

                # Generate 2048-dim vector (all values = i)
                vector_2048 = ', '.join([str(i)] * 2048)

                # Generate multi-vector (2 vectors, each 1024-dim, total 2048 values)
                multi_vector = ', '.join([str(i)] * 2048)

                # Category (cycle through A, B, C, D)
                category = categories[i % 4]

                # Generate sparse vector (100 elements out of 1024, with int values 0-100)
                sparse_pairs = generate_sparse(i)
                sparse_parts = [f"{idx}:{val}" for idx, val in sparse_pairs]
                sparse_str = '[' + ', '.join(sparse_parts) + ']'

                # Combine: original + num + vector + multi_vector + category + sparse
                new_line = f"{original}\t{i}\t[{vector_2048}]\t[{multi_vector}]\t{category}\t{sparse_str}\n"
                dst.write(new_line)


if __name__ == "__main__":
    generate()
