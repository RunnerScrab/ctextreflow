CC = gcc
CFLAGS = -Wall -Og -g
LDFLAGS = -Og -lbsd -g
OBJ = charvector.o talloc.o textutil.o bitutils.o utf8.o cmdlexer.o leditor.o

leditor: ${OBJ} leditortest.o

wraptest2: wraptest2.o ${OBJ}




%.o: %.c
	${CC} -c $< ${CFLAGS}

clean:
	rm -f *.o wraptest2
