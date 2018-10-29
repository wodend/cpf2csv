/* system uri information */
static char uriprefix[] = "file://";
static int uriprefixlen = 9;

/* max width crop area specs */
static int cropvalwidth = 4;
static char *pdftotextargs[] = {
	"pdftotext",
	/* crop area specs */
	"-x", NULL,
	"-y", NULL, 
	"-W", NULL,
	"-H", NULL,
	NULL, /* file */
	"/dev/stdout",
	NULL
};

/* quarter number by index */
static const char quarter[] = { 
	1,  2,  3,  
        5,  6,  7,
        9, 10, 11, 
       13, 14, 15,
       17, 18, 19
};

