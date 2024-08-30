from infinity import index


class SimpleEmbeddingGenerator:
    def columns():
        return {"c1": {"type": "int"}, "c2": {"type": "vector,4,float"}}

    def gen_factory():
        def gen(insert: int):
            for i in range(insert):
                yield i, [0.1, 0.2, 0.3, 0.4]

        return gen

    def index():
        return index.IndexInfo(
            "c2",
            index.IndexType.Hnsw,
            {"M": "16", "ef_construction": "20", "metric": "l2"},
        )


class SimpleVarcharGenerator:
    def columns():
        return {"c1": {"type": "int"}, "c2": {"type": "varchar"}}

    def gen_factory():
        def gen(insert: int):
            for i in range(insert):
                if i % 2 == 0:
                    yield i, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                else:
                    yield i, "test"

        return gen

    def index():
        return index.IndexInfo("c2", index.IndexType.FullText)


class SimpleTensorGenerator:
    def columns():
        return {"c1": {"type": "int"}, "c2": {"type": "tensor,4,float"}}

    def gen_factory():
        def gen(insert: int):
            for i in range(insert):
                if i % 2 == 0:
                    yield i, [0.1, 0.2, 0.3, 0.4]
                else:
                    yield i, [[0.1, 0.2, 0.3, 0.4], [0.5, 0.6, 0.7, 0.8]]

        return gen
