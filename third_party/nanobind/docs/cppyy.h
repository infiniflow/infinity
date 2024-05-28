float test_0000(uint16_t a, int32_t b, uint32_t c, int64_t d, uint64_t e,
                float f) {
    return a + b + c + d + e + f;
}

struct Struct0 {
    uint16_t a;
    int32_t b;
    uint32_t c;
    int64_t d;
    uint64_t e;
    float f;
    Struct0(uint16_t a, int32_t b, uint32_t c, int64_t d, uint64_t e, float f)
        : a(a), b(b), c(c), d(d), e(e), f(f) {}
    float sum() const { return a + b + c + d + e + f; }
};
