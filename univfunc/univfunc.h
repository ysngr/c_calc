/* univfunc.h */
#ifndef UNIVFUNC_H
#define UNIVFUNC_H

#include <stdio.h>
#include <stdlib.h>

#define True 1
#define False 0

#define MAXSEQ 32
#define EMPTY 0ull

typedef unsigned long long Integer;

Integer pair(Integer, Integer);
Integer left(Integer);
Integer right(Integer);

Integer encode(Integer*, Integer);
void decode(Integer, Integer*);

Integer element(Integer, Integer);
Integer length(Integer);
Integer replace(Integer, Integer, Integer);
Integer sequence(Integer, Integer);

int is_code(Integer);
Integer executable(Integer, Integer);

Integer comp(Integer, Integer);

#endif
