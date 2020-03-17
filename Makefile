CC = gcc
CFLAGS = -Wall -Og
LDFLAGS = -Og

wraptest2: wraptest2.o charvector.o talloc.o textutil.o bitutils.o utf8.o

%.o: %.c
	${CC} -c $< ${CFLAGS}

clean:
	rm -f *.o wraptest2
