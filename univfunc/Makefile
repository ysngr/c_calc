# Makefile

OBJS = univfunc.h iscode.h \
	   univfunc.o iscode_generate.o iscode_scan.o iscode_parse.o iscode_register.o comp.o

univfunc : ${OBJS}
	gcc -o $@ ${OBJS} -Wall

clean :
	${RM} *.o univfunc
