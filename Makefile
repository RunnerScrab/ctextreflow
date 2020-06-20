CC = gcc
CFLAGS = -Wall -Og -g
LDFLAGS = -Og -lbsd -g
OBJ = charvector.o talloc.o textutil.o bitutils.o utf8.o

wraptest2: wraptest2.o charvector.o talloc.o textutil.o bitutils.o utf8.o

leditor: ${OBJ} leditor.o


%.o: %.c
	${CC} -c $< ${CFLAGS}

clean:
	rm -f *.o wraptest2
