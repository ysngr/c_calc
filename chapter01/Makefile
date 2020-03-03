# Makefile

OBJS = funcprog.o scan.o parse.o

funcprog : ${OBJS}
	gcc -o $@ ${OBJS} -Wall

clean :
	${RM} *.o funcprog
