/* PoC H (C side): the exact formatter the engine uses for result REALs.
 * `column_vector_impl.cpp:971-975` -> `std::to_string(double)` == printf("%f"),
 * i.e. fixed 6 decimals. This program prints `%f` for each value on stdin so we
 * can diff it against Rust's `format!("{:.6}")`.
 * Build: cc c_format.c -o c_format
 */
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char line[256];
    while (fgets(line, sizeof(line), stdin)) {
        if (line[0] == '\n' || line[0] == '\0') continue;
        double v = strtod(line, NULL);
        printf("%f\n", v); /* glibc printf "%f" == std::to_string(double) */
    }
    return 0;
}
