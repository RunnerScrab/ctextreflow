CC = gcc
CFLAGS = -Wall -Og -g
LDFLAGS = -g

wraptest2: wraptest2.o charvector.a textutil.o

charvector.a: charvector.o talloc.o
	ar rcs charvector.a $<

%.o: %.c
	${CC} -c $< ${CFLAGS}
