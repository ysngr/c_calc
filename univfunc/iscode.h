/* iscode.h */
#ifndef ISCODE_H
#define ISCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define True 1
#define False 0
#define Empty -1
#define Error -1

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


/* generate.c */
extern char filename[MAXSTRLEN];

/* scan.c */
// #define PRINTPROG  // for debug
extern char str[MAXSTRLEN];
extern int num;
void init_scan(void);
int scan(void);
void end_scan(void);

/* parse.c */
// #define PRINT_ERRMSG  // for debug
int parse(void);
void error(char*);

/* register.c */
void init_register(void);
void init_fprog_list(void);
void define_variable(void);
void reference_variable(void);
void generate_label(void);
void paste_label(char*);
void check_label_link(void);


// debug function
void print_list(void);

#endif
