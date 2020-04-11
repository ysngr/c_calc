# Makefile

CC = gcc
CFLAGS = -Wall
OBJS = functojump.h functojump.o scan.o parse.o register.o generate.o expand.o flatten.o replace.o

functojump : ${OBJS}
	gcc -o $@ ${OBJS}

clean :
	${RM} *.o functojump
