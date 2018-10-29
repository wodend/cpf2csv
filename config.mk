# cpf2csv version
VERSION = 0.1

# includes and libs
INCS = `pkg-config --cflags poppler-glib`
LIBS = -lm \
       `pkg-config --libs poppler-glib`

# flags
CPPFLAGS = -DVERSION=\"${VERSION}\"
CFLAGS   = -std=c99 -pedantic -Wall -Os ${INCS} ${CPPFLAGS}
LDFLAGS  = -s ${LIBS}
DEBUG    = -ggdb -DDEBUG

# compiler and linker
CC = cc
