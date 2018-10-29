#include <ctype.h>
#include <math.h>
#include <poppler.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "config.h"
#include "util.h"

/* macros */
#define MID(a, b)    (((a) + (b)) / 2)

enum pipe {
	READ,
	WRITE
};

enum quarter {
	FALL,
	WINTER,
	SPRING
};

typedef struct {
	int x, y; /* crop area top left corner position */
	int w, h; /* crop area width and height */
} CropArea;

typedef struct {
	int year[3][5];
	int y; /* y position for all quarters */
	int end; /* right x position for last quarter */
} QPosition;

static PopplerPage *popplerpage(const char *filename, int pageind);
static char *pdftotext(char *filename, CropArea ca);
static void xmid(GList *rects, int *points, int n);
static void getqpos(QPosition *qpos, PopplerPage *page, int nyr);
static void yearcas(CropArea years[5][3], QPosition *qpos, int nyr);

/* return value must be freed */
PopplerPage *
popplerpage(const char *filename, int pageind)
{
	PopplerDocument *doc;
	PopplerPage *page;
	char *cwd, *uri;
	int urilen;

	cwd = getcwd(NULL, 0);
	urilen = uriprefixlen + strlen(cwd) + strlen(filename);
	uri = ecalloc(urilen, 1);
	snprintf(uri, urilen, "%s%s/%s", uriprefix, cwd, filename);
	doc = poppler_document_new_from_file(uri, NULL, NULL);
	if (!doc)
		die("poppler document %s:", uri);
	page = poppler_document_get_page(doc, pageind);
	if (!page)
		die("poppler get page %d:", pageind);
	free(cwd);
	free(uri);
	g_object_unref(doc);
	return page;
}

/* return value must be freed */
char *
pdftotext(char *filename, CropArea ca)
{
	pid_t cpid;
	char *text = NULL, *castr, *argv[12];
	int i, castrlen, fd[2];
	
	castrlen = 4 * (cropvalwidth + 1);
	castr = ecalloc(1, castrlen);
	snprintf(castr, castrlen, "%d,%d,%d,%d", ca.x, ca.y, ca.w, ca.h);
	memmove(argv, pdftotextargs, sizeof(pdftotextargs));
	argv[2] = strtok(castr, ",");
	for (i = 4; i < 10; i += 2)
		argv[i] = strtok(NULL, ",");
	argv[--i] = filename;
	if (pipe(fd) < 0)
		die("pipe:");
	switch (cpid = fork()) {
	case -1:
		die("fork:");
		break;
	case 0:
		close(fd[READ]);
		dup2(fd[WRITE], STDOUT_FILENO);
		close(fd[WRITE]);
		execvp(*argv, argv);
		die("exec:");
		break;
	default:
		close(fd[WRITE]);
		text = dread(fd[READ], 1024);
		close(fd[READ]);
		if (waitpid(cpid, NULL, WNOHANG) < 0)
			die("wait:");
		break;
	}
	free(castr);
	return(text);
}

/* add first n x midpoints to points array */
void
xmid(GList *rects, int *points, int n)
{
	int i;
	PopplerRectangle *cur;

	for (i = 0; i < n; i++) {
		if (!rects)
			die("wrong number of years specified\n");
		cur = rects->data;
		points[i] = round(MID(cur->x1, cur->x2));
		rects = rects->next;
	}
}

void
getqpos(QPosition *qpos, PopplerPage *page, int nyr)
{
	double width, height;
	GList *list;

	list = poppler_page_find_text(page, "fall");
	xmid(list, qpos->year[FALL], nyr);
	g_list_free_full(list, (GDestroyNotify)poppler_rectangle_free);
	list = poppler_page_find_text(page, "winter");
	xmid(list, qpos->year[WINTER], nyr);
	g_list_free_full(list, (GDestroyNotify)poppler_rectangle_free);
	list = poppler_page_find_text(page, "spring");
	xmid(list, qpos->year[SPRING], nyr);
	poppler_page_get_size(page, &width, &height);
	qpos->y = round(height) - round(((PopplerRectangle *)list->data)->y2);
	g_list_free_full(list, (GDestroyNotify)poppler_rectangle_free);
	qpos->end = round(width);
}

void
yearcas(CropArea years[5][3], QPosition *qpos, int nyr)
{
	int left = 0, right = 0, i, j;

	for (i = 0; i < nyr; i++) {
		for (j = 0; j < 3; j++) {
			if (j == SPRING && (i == (nyr - 1))) {
				right = qpos->end;
			} else if (j == SPRING) {
				right = MID(qpos->year[j][i],\
					qpos->year[0][i + 1]);
			} else {
				right = MID(qpos->year[j][i],\
					qpos->year[j + 1][i]);
			}
			years[i][j].x = left;
			years[i][j].y = round(qpos->y * 1.05);
			years[i][j].w = right - left;
			years[i][j].h = round(qpos->y * 3.4);
			left = right;
		}
	}
}

void
usage()
{
	die("usage: cpf2csv [-n<quarters>] PDF-file [CSV-file]\n");
}

void
ctrltosc(char *text)
{
	char *c;

	c = text;
	while (*c) {
		if (iscntrl((int)*c) || *c == ',') {
			*c = ';';
		}
		c++;
	}
}

int 
main(int argc, char *argv[])
{
	int i, j, q = 0, nyr = 4;
	char *text, *fname, *foutname, *fout = NULL;
	PopplerPage *page;
	QPosition qpos;
	CropArea years[5][3];
	FILE *f;

	switch (argc) {
	case 2:
		fname = argv[1];
		break;
	case 3:
		if (argv[1][0] == '-') {
			if (sscanf(argv[1], "-n%d", &nyr) != 1)
				usage();
			fname = argv[2];
		} else {
			fname = argv[1];
			fout = argv[2];
		}
		break;
	case 4:
		if (sscanf(argv[1], "-n%d", &nyr) != 1)
			usage();
		fname = argv[2];
		fout = argv[3];
		break;
	default:
		usage();
		
	}
	page = popplerpage(fname, 0);
	getqpos(&qpos, page, nyr);
	yearcas(years, &qpos, nyr);
	if (fout) {
		f = fopen(fout, "a");
		if (!f)
			die("fopen:");
	} else {
		f = stdout;
	}
	i = strlen(fname);
	foutname = ecalloc(i, 1);
	strncpy(foutname, fname, i - 4);
	foutname[i] = '\0';
	for (i = 0; i < nyr; i++) {
		for (j = 0; j < 3; j++) {
			text = pdftotext(fname, years[i][j]);
			ctrltosc(text);
			fprintf(f, "%s,%d,%s\n", foutname, quarter[q++], text);
			free(text);
		}
	}	
	if (fout)
		fclose(f);
	free(foutname);
	g_object_unref(page);
	return EXIT_SUCCESS;
}
