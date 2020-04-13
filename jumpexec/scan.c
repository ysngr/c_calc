/* scan.c */
#include "jumpexec.h"

#define KEYWORDNUM 4

char str[MAXSTRLEN];

static FILE *fp;
static char c;

static void scanc(void);
static int is_invalid_char(void);
static int str_to_tokennum(void);



void initialize_scan(char *filename)
{
    if( (fp = fopen(filename, "r")) == NULL ){
        printf("File cannot be opened.\n");
        exit(EXIT_FAILURE);
    }

    scanc();

    return ;
}


static void scanc(void)
{
    c = fgetc(fp);

    return ;
}


int scan(void)
{
    int i;
    int token;

    BEGIN_SCAN:
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
        for( i = 0; isdigit(c); i++ ){
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
                token = ( c == '+' )? INC_N : UNKNOWN;
                break;
            case '-' :
                scanc();
                if( c == '-' ){
                    scanc();
                    token = ( c == '\'' )? CDEC_N : UNKNOWN;
                }else{
                    token = UNKNOWN;
                }
                break;
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
                    token = UNKNOWN;
                }
                break;
            case '=' : token = ASSIGN_N; break;
            case '>' : token = RE_N; break;
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
        case ' ' :
        case '\n' :
        case '\r' :
        case '\t' :
            return True;
    }

    return False;
}



static int str_to_tokennum(void)
{
    int i;
    int keynum[KEYWORDNUM] = {IF_N, RETURN_N, GOTO_N, INT_N};
    char keystr[KEYWORDNUM][MAXSTRLEN] = {"if", "return", "goto", "int"};

    for( i = 0; i < KEYWORDNUM; i++ ){
        if( strcmp(keystr[i], str) == 0 ){
            return keynum[i];
        }
    }

    return NAME_N;
}


void finalize_scan(void)
{
    fclose(fp);

    return ;
}
