CC = gcc
CFLAGS = -Wall -Og -g -pg
LDFLAGS = -g -pg

wraptest2: wraptest2.o charvector.o talloc.o textutil.o

%.o: %.c
	${CC} -c $< ${CFLAGS}

clean:
	rm -f *.o wraptest2
