# Makefile

OBJS = convtojump.h convtojump.o scan.o parse.o register.o generator.o

convtojump : ${OBJS}
	gcc -o $@ ${OBJS} -Wall

clean :
	${RM} *.o convtojump
