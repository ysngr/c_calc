/* funcprog.h */
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
#define ELSE_N 3     // else
#define WHILE_N 4    // while
#define LOOP_N 5     // loop
#define RETURN_N 6   // return
#define INT_N 7      // int
#define NUM_N 8      // Number
#define PLUS_N 9     // +
#define INC_N 10     // ++
#define MINUS_N 11   // -
#define DEC_N 12     // --
#define MUL_N 13     // *
#define DIV_N 14     // /
#define MOD_N 15     // %
#define ASSIGN_N 16  // =
#define EQUAL_N 17   // ==
#define NOTEQ_N 18   // !=
#define LE_N 19      // <
#define LEEQ_N 20    // <=
#define RE_N 21      // >
#define REEQ_N 22    // >=
#define AND_N 23     // &&
#define OR_N 24      // ||
#define NOT_N 25     // !
#define LPAREN_N 26  // (
#define RPAREN_N 27  // )
#define LBRACE_N 28  // {
#define RBRACE_N 29  // }
#define COMMA_N 30   // ,
#define COLON_N 31   // :
#define SEMI_N 32    // ;


/* scan.c */
extern char str[MAXSTRLEN];
extern int num;
void init_scan(const char*);
int scan(void);
void end_scan(void);

/* parse.c */
void parse(void);

/* register.c */
void init_register(void);
void init_fprog_list(void);
void define_variable(void);
void reference_variable(void);

void debugprint(void);
