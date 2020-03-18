# Makefile

OBJS = convtojump.h convtojump.o scan.o parse.o register.o generate.o

convtojump : ${OBJS}
	gcc -o $@ ${OBJS} -Wall

clean :
	${RM} *.o convtojump
