/* univfunc.h */
#ifndef UNIVFUNC_H
#define UNIVFUNC_H

#include <stdio.h>
#include <stdlib.h>

#define True 1
#define False 0

#define MAXSEQ 32
#define EMPTY -1

int pair(int, int);
int left(int);
int right(int);

int encode(int*, int);
void decode(int, int*);

int element(int, int);
int length(int);
int replace(int, int, int);
int sequence(int, int);

int is_code(int);

#endif
