/* scan.c */
#include "funcprog.h"

// #define PRINTPROG
#define KEYWORDNUM 6


struct key{
    int keynum;
    char keyname[MAXSTRLEN];
};
static struct key ks[KEYWORDNUM] = {
    {IF_N,     "if"    },
    {ELSE_N,   "else"  },
    {WHILE_N,  "while" },
    {LOOP_N,   "loop"  },
    {RETURN_N, "return"},
    {INT_N,    "int"   }
};


char str[MAXSTRLEN];
int num;

static FILE *fp;
static char c;

static void scanc(void);
static int is_invalid_char(void);
static int str_to_tokennum(void);



void init_scan(const char *filename)
{
    if( (fp = fopen(filename, "r")) == NULL ){
        printf("File does not exist.\n");
        exit(EXIT_FAILURE);
    }

    scanc();

    return ;
}


static void scanc(void)
{
    c = fgetc(fp);

    #ifdef PRINTPROG
    if( c == EOF ){
        printf("\n");
    }else{
        printf("%c", c);
    }
    #endif

    return ;
}


int scan(void)
{
    int i;
    int token;

    while( is_invalid_char() ){
        scanc();
    }

    // end of file
    if( c == EOF ){
        token = END_OF_FILE;
    }

    // Name, Keyword
    else if( isalpha(c) || c == '_'){
        for( i = 0; isalnum(c) || c == '_'; i++ ){
            if( i >= MAXSTRLEN ){
                printf("Name length is too long.\n");
                exit(EXIT_FAILURE);
            }
            str[i] = c;
            scanc();
        }
        str[i] = '\0';
        token = str_to_tokennum();
    }

    // Number
    else if( isdigit(c) ){
        num = 0;
        for( i = 0; isdigit(c); i++ ){
            num = 10 * num + (c - '0');
            snprintf(str+i, 2, "%c", c);
            scanc();
        }
        token = NUM_N;
    }

    // Symbol
    else{
        switch( c ){
            case '+' :
                scanc();
                if( c == '+' ){
                    token = INC_N;
                }else{
                    token = PLUS_N;
                    ungetc(c, fp);
                }
                break;
            case '-' :
                scanc();
                if( c == '-' ){
                    token = DEC_N;
                }else{
                    token = MINUS_N;
                    ungetc(c, fp);
                }
                break;
            case '*' : token = MUL_N; break;
            case '/' : token = DIV_N; break;
            case '%' : token = MOD_N; break;
            case '=' :
                scanc();
                if( c == '=' ){
                    token = EQUAL_N;
                }else{
                    token = ASSIGN_N;
                    ungetc(c, fp);
                }
                break;
            case '!' :
                scanc();
                if( c == '=' ){
                    token = NOTEQ_N;
                }else{
                    token = NOT_N;
                    ungetc(c, fp);
                }
                break;
            case '<' :
                scanc();
                if( c == '=' ){
                    token = LEEQ_N;
                }else{
                    token = LE_N;
                    ungetc(c, fp);
                }
                break;
            case '>' :
                scanc();
                if( c == '=' ){
                    token = REEQ_N;
                }else{
                    token = RE_N;
                    ungetc(c, fp);
                }
                break;
            case '&' :
                scanc();
                if( c == '&' ){
                    token = AND_N;
                }else{
                    token = UNKNOWN;
                }
                break;
            case '|' :
                scanc();
                if( c == '|' ){
                    token = OR_N;
                }else{
                    token = UNKNOWN;
                }
                break;
            case '(' : token = LPAREN_N; break;
            case ')' : token = RPAREN_N; break;
            case '{' : token = LBRACE_N; break;
            case '}' : token = RBRACE_N; break;
            case ',' : token = COMMA_N; break;
            case ':' : token = COLON_N; break;
            case ';' : token = SEMI_N; break;
            default : token = UNKNOWN;
        }
        scanc();
    }

    return token;
}


static int is_invalid_char(void)
{
    switch( c ){
        case '\r' :
        case '\n' :
        case '\t' :
        case ' '  :
            return True;  // invalid char
    }

    return False;  // valid char
}


static int str_to_tokennum(void)
{
    int i;
    for( i = 0; i < KEYWORDNUM; i++ ){
        if( strcmp(ks[i].keyname, str) == 0 ){
            return ks[i].keynum;
        }
    }

    return NAME_N;
}


void end_scan(void)
{
    fclose(fp);

    return ;
}
