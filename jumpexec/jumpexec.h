/* jumpexec.h */
#ifndef JUMPEXEC_H
#define JUMPEXEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define True 1
#define False 0
#define Empty -1

#define MAXSTRLEN 128

#define UNKNOWN -1
#define END_OF_FILE 0

#define NAME_N 1     // Name
#define IF_N 2       // if
#define RETURN_N 3   // return
#define GOTO_N 4     // goto
#define INT_N 5      // int
#define NUM_N 6      // Number
#define INC_N 7      // ++
#define CDEC_N 8     // --'
#define ASSIGN_N 9   // =
#define RE_N 10      // >
#define LPAREN_N 11  // (
#define RPAREN_N 12  // )
#define LBRACE_N 13  // {
#define RBRACE_N 14  // }
#define COMMA_N 15   // ,
#define COLON_N 16   // :
#define SEMI_N 17    // ;

#define VARIABLE_N 18

#define GOTO_STAT 1
#define NAT_ASSIGN_STAT 2
#define VAR_ASSIGN_STAT 3
#define INCR_STAT 4
#define CDECR_STAT 5
#define IF_GOTO_STAT 6
#define RETURN_STAT 7


/* scan.c */
extern char str[MAXSTRLEN];
void initialize_scan(char*);
int scan(void);
void finalize_scan(void);

/* parse.c */
int parse(char*);

/* register.c */
void initialize_register(void);
void register_variable(int, int);
void register_statement(int, int, int);
void register_paramvalue(int*, int);
void update_variable(int, int);
void finalize_register(void);

// debug function
void print_statlist(void);

#endif
