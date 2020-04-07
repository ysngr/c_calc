# Makefile

OBJS = functojump.h functojump.o scan.o parse.o register.o generate.o expand.o flatten.o

functojump : ${OBJS}
	gcc -o $@ ${OBJS} -Wall

clean :
	${RM} *.o functojump
