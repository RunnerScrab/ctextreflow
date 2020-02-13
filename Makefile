CC = gcc
CFLAGS = -Wall -Ofast -g
LDFLAGS = -Ofast -g

wraptest2: wraptest2.o charvector.o talloc.o textutil.o utf8.o

%.o: %.c
	${CC} -c $< ${CFLAGS}

clean:
	rm -f *.o wraptest2
