/* functojump.h */
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
#define GOTO_N 7     // goto
#define INT_N 8      // int
#define NUM_N 9      // Number
#define PLUS_N 10    // +
#define INC_N 11     // ++
#define MINUS_N 12   // -
#define DEC_N 13     // --
#define CDEC_N 14    // --'
#define MUL_N 15     // *
#define DIV_N 16     // /
#define MOD_N 17     // %
#define ASSIGN_N 18  // =
#define EQUAL_N 19   // ==
#define NOTEQ_N 20   // !=
#define LE_N 21      // <
#define LEEQ_N 22    // <=
#define RE_N 23      // >
#define REEQ_N 24    // >=
#define AND_N 25     // &&
#define OR_N 26      // ||
#define NOT_N 27     // !
#define LPAREN_N 28  // (
#define RPAREN_N 29  // )
#define LBRACE_N 30  // {
#define RBRACE_N 31  // }
#define COMMA_N 32   // ,
#define COLON_N 33   // :
#define SEMI_N 34    // ;
#define TOKEN_NUM 35


/* scan.c */
extern char str[MAXSTRLEN];
extern int num;
void initialize_scan(const char*);
int scan(void);
void finalize_scan(void);

/* parse.c */
void parse(void);

/* register.c */
void initialize_register(void);
void initialize_fprog_list(void);
void define_variable(void);
void reference_variable(void);
void define_label(void);
void reference_label(char*);
void check_label_link(void);

/* generate.c */
void initialize_generator(char*);
void generate(int);
void generate_str(char*);
void generate_deplabel(char*);
void generate_goto(char*);
void generate_ln_indent(void);
void generate_nl_outdent(void);
void finalize_generator(void);

// debug function
void print_list(void);
