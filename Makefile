# cpf2csv

include config.mk

SRC = cpf2csv.c util.c
OBJ = ${SRC:.c=.o}

all: options cpf2csv

options:
	@echo cpf2csv build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk

cpf2csv: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

debug: CFLAGS += ${DEBUG}
debug: all

clean:
	@echo cleaning
	@rm -f cpf2csv ${OBJ}

.PHONY: all options debug clean 
