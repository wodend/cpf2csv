#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "util.h"

/* return value must be freed, terminates data with null byte */
char *
dread(int fd, int bufsize)
{
	int i = 0, b = 0;
	ssize_t rval;
	char *data = NULL;

	do {
		data = erealloc(data, ++i, bufsize);
		if ((rval = read(fd, data, bufsize)) == -1)
			die("read:");
		b += rval;
	} while (rval == bufsize);
	data[b] = '\0';
	return(data);
}


void
die(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	if (fmt[0] && fmt[strlen(fmt)-1] == ':') {
		fputc(' ', stderr);
		perror(NULL);
	}
	exit(EXIT_FAILURE);
}

void *
ecalloc(size_t nmemb, size_t size)
{
	void *alloc;

	if (!(alloc = calloc(nmemb, size)))
		die("calloc:");
	return alloc;
}

void *
erealloc(void *ptr, size_t nmemb, size_t size)
{
	void *tmpptr = NULL;

	if (nmemb == 0 || size == 0)
		free(ptr);
	else if (!(tmpptr = realloc(ptr, nmemb * size)))
		die("realloc:");
	return tmpptr;
}
