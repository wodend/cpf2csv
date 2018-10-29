#ifndef UTIL_H
#define UTIL_H

char *dread(int fd, int bufsize);
void die(const char *fmt, ...);
void *ecalloc(size_t nmemb, size_t size);
void *erealloc(void *ptr, size_t nmemb, size_t size);

#endif
