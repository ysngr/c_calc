# Makefile

OBJS = functojump.h functojump.o scan.o parse.o register.o generate.o

functojump : ${OBJS}
	gcc -o $@ ${OBJS} -Wall

clean :
	${RM} *.o functojump
