/* jumpexec.h */
#ifndef JUMPEXEC_H
#define JUMPEXEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define True 1
#define False 0

#define MAXSTRLEN 128


/* scan.c */
extern char str[MAXSTRLEN];
void initialize_scan(char*);
int scan(void);
void finalize_scan(void);

/* parse.c */
void parse(void);

#endif
