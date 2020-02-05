CC = gcc
CFLAGS = -Wall -Ofast
LDFLAGS = -Ofast

wraptest2: wraptest2.o charvector.o talloc.o textutil.o

%.o: %.c
	${CC} -c $< ${CFLAGS}

clean:
	rm -f *.o wraptest2
