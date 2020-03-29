/* scan.c */
#include "functojump.h"
#include "scan.h"

// #define PRINTPROG

struct key ks[KEYWORDNUM] = {
    {IF_N    , "if"    },
    {ELSE_N  , "else"  },
    {WHILE_N , "while" },
    {LOOP_N  , "loop"  },
    {RETURN_N, "return"},
    {GOTO_N  , "goto"  },
    {INT_N   , "int"   }
};


char str[MAXSTRLEN];
int num;

static FILE *fp;
static char c;

static struct flags{
    int is_ungetc_exec;
} fs;

static void initialize_flag(void);
static void scanc(void);
static int Ungetc(int, FILE*);



void initialize_scan(const char *filename)
{
    if( (fp = fopen(filename, "r")) == NULL ){
        printf("File does not exist.\n");
        exit(EXIT_FAILURE);
    }

    initialize_flag();
    scanc();

    return ;
}


static void initialize_flag(void)
{
    fs.is_ungetc_exec = False;

    return ;
}


static void scanc(void)
{
    c = fgetc(fp);

    #ifdef PRINTPROG
        if( fs.is_ungetc_exec ){
            fs.is_ungetc_exec = False;
        }else if( c == EOF ){
            printf("\n");
        }else{
            printf("%c", c);
        }
    #endif

    return ;
}


static int Ungetc(int c, FILE *stream)
{
    ungetc(c, stream);
    fs.is_ungetc_exec = True;

    return (unsigned int)c;
}


int scan(void)
{
    int i;
    int token;

    BEGIN_SCAN:
    while( is_invalid_char(c) ){
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
        token = str_to_tokennum(str);
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
                    Ungetc(c, fp);
                }
                break;
            case '-' :
                scanc();
                if( c == '-' ){
                    scanc();
                    if( c == '\'' ){
                        token = CDEC_N;
                    }else{
                        token = DEC_N;
                        Ungetc(c, fp);
                    }
                    break;
                }else{
                    token = MINUS_N;
                    Ungetc(c, fp);
                }
                break;
            case '*' : token = MUL_N; break;
            case '/' :
                scanc();
                if( c == '*' ){  // comment
                    char pc;
                    scanc();
                    while( pc != '*' || c != '/' ){
                        pc = c;
                        scanc();
                        if( c == EOF ){
                            printf("Expected \'*/\' but not found.\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                    scanc();
                    goto BEGIN_SCAN;
                }else {
                    token = DIV_N;
                    Ungetc(c, fp);
                }
                break;
            case '%' : token = MOD_N; break;
            case '=' :
                scanc();
                if( c == '=' ){
                    token = EQUAL_N;
                }else{
                    token = ASSIGN_N;
                    Ungetc(c, fp);
                }
                break;
            case '!' :
                scanc();
                if( c == '=' ){
                    token = NOTEQ_N;
                }else{
                    token = NOT_N;
                    Ungetc(c, fp);
                }
                break;
            case '<' :
                scanc();
                if( c == '=' ){
                    token = LEEQ_N;
                }else{
                    token = LE_N;
                    Ungetc(c, fp);
                }
                break;
            case '>' :
                scanc();
                if( c == '=' ){
                    token = REEQ_N;
                }else{
                    token = RE_N;
                    Ungetc(c, fp);
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


int is_invalid_char(char ch)
{
    switch( ch ){
        case '\r' :
        case '\n' :
        case '\t' :
        case ' '  :
            return True;  // invalid char
    }

    return False;  // valid char
}


int str_to_tokennum(char *s)
{
    int i;
    for( i = 0; i < KEYWORDNUM; i++ ){
        if( strcmp(ks[i].keyname, s) == 0 ){
            return ks[i].keynum;
        }
    }

    return NAME_N;
}


void finalize_scan(void)
{
    fclose(fp);

    return ;
}
