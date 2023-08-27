#ifndef LQ_FS_API_H
#define LQ_FS_API_H

typedef struct fs_data *fshandle;

fshandle fsopen(const char *fn, const char *mode);

int fsclose(fshandle h);

size_t fswrite(const void *ptr, size_t n, size_t m, fshandle h);

void stdflush(int addNewline);

size_t bytesum(const char *fn);

size_t fsread(void *ptr, size_t n, size_t m, fshandle h);

int fsrm(const char *fn);

int fsmkdir(const char *path);

int fsprint(const char *s);

int fsprinterr(const char *s);

char *fsls(const char *dir);

#endif
