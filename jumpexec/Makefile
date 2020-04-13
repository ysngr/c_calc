# Makefile

CC = gcc
CFLAGS = -Wall
OBJS = jumpexec.h jumpexec.o scan.o parse.o register.o

functojump : ${OBJS}
	gcc -o $@ ${OBJS}

clean :
	${RM} *.o functojump
