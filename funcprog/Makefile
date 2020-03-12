# Makefile

OBJS = funcprog.h funcprog.o scan.o parse.o register.o

funcprog : ${OBJS}
	gcc -o $@ ${OBJS} -Wall

clean :
	${RM} *.o funcprog
