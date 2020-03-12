# Makefile

OBJS = jumpprog.h jumpprog.o scan.o parse.o register.o

jumpprog : ${OBJS}
	gcc -o $@ ${OBJS} -Wall

clean :
	${RM} *.o jumpprog
