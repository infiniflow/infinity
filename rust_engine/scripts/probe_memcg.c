/* What does a cgroup's `memory.current` do when a process drops mapped pages?
 *
 * The governor in `executor/src/memory.rs` decides "did that pass free enough?"
 * by re-reading the process's own figure, so two things matter:
 *
 *   1. is the page cache of a mapped file charged to the cgroup at all, and
 *   2. how long after `madvise(DONTNEED)` does the charge go away?
 *
 * (2) is a kernel question, not an engine one: memcg charges are batched per
 * CPU (`MEMCG_CHARGE_BATCH`), so a reader can see a lag of a few pages; if the
 * lag were seconds, the tick would escalate to unmapping blocks every pass.
 *
 * Build and run inside a limited container, e.g.
 *
 *   docker run --rm --memory=512m -v "$PWD":/w -w /w gcc:13 \
 *     sh -c 'cc -O2 -o /tmp/p probe_memcg.c && /tmp/p 64'
 *
 * The argument is the size of the mapping in MiB (default 64).
 *
 * Prints, for both a shared (page-cache) and a private (anonymous) mapping:
 * memory.current before, after touching every page, and the time it takes to
 * fall back once the pages are advised away. Exit status 0 even without a
 * cgroup - it then just reports "not a cgroup host" and measures nothing.
 */
#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#if defined(__linux__)
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#endif

static const char *CG = NULL; /* resolved cgroup v2 directory, may be "" */

#if defined(__linux__)

static uint64_t read_num(const char *dir, const char *name) {
    char path[512];
    snprintf(path, sizeof path, "%s/%s", dir, name);
    FILE *f = fopen(path, "r");
    if (!f)
        return 0;
    char buf[64] = {0};
    if (!fgets(buf, sizeof buf, f)) {
        fclose(f);
        return 0;
    }
    fclose(f);
    if (strncmp(buf, "max", 3) == 0)
        return 0;
    return strtoull(buf, NULL, 10);
}

static uint64_t now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

static const char *resolve_cgroup(void) {
    static char dir[512];
    FILE *f = fopen("/proc/self/cgroup", "r");
    if (!f)
        return NULL;
    char line[512];
    const char *found = NULL;
    while (fgets(line, sizeof line, f)) {
        /* v2: "0::/path" */
        if (strncmp(line, "0::", 3) == 0) {
            char *nl = strchr(line, '\n');
            if (nl)
                *nl = 0;
            snprintf(dir, sizeof dir, "/sys/fs/cgroup%s", line + 3);
            found = dir;
            break;
        }
    }
    fclose(f);
    return found;
}

/* Touch every page and report how long memory.current takes to give the pages
 * back once they are advised away. */
static void probe(const char *label, int shared, size_t bytes) {
    if (!CG)
        return;
    char path[] = "/tmp/probe_memcg.XXXXXX";
    int fd = mkstemp(path);
    if (fd < 0) {
        perror("mkstemp");
        return;
    }
    if (ftruncate(fd, (off_t)bytes)) {
        perror("ftruncate");
    }
    int flags = shared ? MAP_SHARED : MAP_PRIVATE;
    char *p = mmap(NULL, bytes, PROT_READ | PROT_WRITE, flags, fd, 0);
    if (p == MAP_FAILED) {
        perror("mmap");
        close(fd);
        unlink(path);
        return;
    }

    uint64_t before = read_num(CG, "memory.current");
    for (size_t i = 0; i < bytes; i += 4096)
        p[i] = (char)i;
    uint64_t touched = read_num(CG, "memory.current");
    uint64_t t0 = now_ms();
    if (madvise(p, bytes, MADV_DONTNEED))
        perror("madvise");
    uint64_t dropped_at = 0, after = touched;
    for (int i = 0; i < 2000; i++) {
        after = read_num(CG, "memory.current");
        if (after + (bytes * 4 / 5) <= touched) {
            dropped_at = now_ms() - t0;
            break;
        }
        usleep(1000);
    }
    printf("%-8s %4zu MiB: current %6llu -> %6llu KiB (+%lld), after DONTNEED %6llu KiB, lag %llu ms\n",
           label,
           bytes >> 20,
           (unsigned long long)(before >> 10),
           (unsigned long long)(touched >> 10),
           (long long)((touched - before) >> 10),
           (unsigned long long)(after >> 10),
           (unsigned long long)dropped_at);
    munmap(p, bytes);
    close(fd);
    unlink(path);
}
#endif

int main(int argc, char **argv) {
    size_t mib = argc > 1 ? (size_t)atoi(argv[1]) : 64;
    size_t bytes = mib << 20;

#if defined(__linux__)
    CG = resolve_cgroup();
    if (!CG) {
        printf("not a cgroup host (no v2 entry in /proc/self/cgroup): nothing to measure\n");
        return 0;
    }
    printf("cgroup v2: %s\n", CG);
    printf("memory.max: %llu KiB  memory.high: %llu KiB  memory.current: %llu KiB\n",
           (unsigned long long)(read_num(CG, "memory.max") >> 10),
           (unsigned long long)(read_num(CG, "memory.high") >> 10),
           (unsigned long long)(read_num(CG, "memory.current") >> 10));
    printf("memory.stat file: %llu KiB\n", (unsigned long long)(read_num(CG, "memory.stat") >> 10));
    /* memory.stat needs a name lookup; the helper above only reads the first
     * line of a flat file, so read the "file" field explicitly. */
    {
        char path[512];
        snprintf(path, sizeof path, "%s/memory.stat", CG);
        FILE *f = fopen(path, "r");
        char line[256];
        while (f && fgets(line, sizeof line, f)) {
            if (strncmp(line, "file ", 5) == 0) {
                printf("memory.stat:file %llu KiB\n", (unsigned long long)(strtoull(line + 5, NULL, 10) >> 10));
                break;
            }
        }
        if (f)
            fclose(f);
    }
    probe("shared", 1, bytes);
    probe("private", 0, bytes);
    return 0;
#else
    (void)CG;
    (void)bytes;
    printf("this probe needs Linux cgroup v2 (run it in a container)\n");
    return 0;
#endif
}
