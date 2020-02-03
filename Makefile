CC = gcc
CFLAGS = -Wall -Og -g
LDFLAGS = -g

wraptest2: wraptest2.o charvector.o talloc.o textutil.o

%.o: %.c
	${CC} -c $< ${CFLAGS}
