/* expand.c */
#include "functojump.h"

#define MAXNEWNAME 7

struct key{  // TODO : integrate to scan.c
    int keynum;
    char keyname[MAXSTRLEN];
};
static struct key ks[7] = {
    {IF_N    , "if"    },
    {ELSE_N  , "else"  },
    {WHILE_N , "while" },
    {LOOP_N  , "loop"  },
    {RETURN_N, "return"},
    {GOTO_N  , "goto"  },
    {INT_N   , "int"   }
};

static FILE *fp;
static char c;
static char restr[MAXSTRLEN];
static int renum;

static fpos_t *head, *tail;

static int counter;

static struct sublist{
    char *origname;
    char *newname;
    struct sublist *nextsub;
} *ss;


static void initialize_expand(void);
static struct sublist *find_subnode(char*);
static void gen_subnode(char*);
static void rescanc(void);
static int rescan(void);
static int is_invalid_char(void);
static int str_to_tokennum(void);
static void inline_function(void);



static void initialize_expand(void)
{
    struct arglist *ap;

    fp = get_generate_fp();
    head = get_generate_head();
    fgetpos(fp, tail);

    ss = NULL;
    counter = 0;

    for( ap = get_args(); ap != NULL; ap = ap->nextarg ){
        gen_subnode(ap->argname);
    }

    rescanc();

    return ;
}


static struct sublist *find_subnode(char *name)
{
    struct sublist *sp;

    for( sp = ss; sp != NULL; sp = sp->nextsub ){
        if( strcmp(sp->origname, name) == 0 ){
            return sp;  // already exists in sublist
        }
    }

    return NULL;
}


static void gen_subnode(char *name)
{
    struct sublist *ns, *sp;
    int len_name;

    if( find_subnode(name) != NULL ){
        return ;
    }

    // generate new node
    ns = (struct sublist*)Malloc(sizeof(struct sublist));
    len_name = strlen(name) + 1;
    ns->origname = (char*)Malloc(sizeof(char)*len_name);
    strcpy(ns->origname, name);
    ns->newname = (char*)Malloc(sizeof(char)*MAXNEWNAME);
    snprintf(ns->newname, MAXNEWNAME, "_vl%d", counter++);
    ns->nextsub = NULL;

    // connect
    if( ss == NULL ){
        ss = ns;
    }else{
        for( sp = ss; sp->nextsub != NULL; sp = sp->nextsub );
        sp->nextsub = ns;
    }

    return ;
}


void expand(char *funcname, char *retvar)
{
    initialize_expand();

    fsetpos(fp, head);

    do{
        rescan();
    }while( strcmp(funcname, restr) != 0 );

    inline_function();
    strcpy(retvar, restr);

    return ;
}


static void rescanc(void)
{
    fsetpos(fp, head);
    c = fgetc(fp);
    fgetpos(fp, head);

    return ;
}


static int rescan(void)
{
    int i;
    int token;

    while( is_invalid_char() ){
        rescanc();
    }

    // Name, Keyword
    if( isalpha(c) || c == '_'){
        for( i = 0; isalnum(c) || c == '_'; i++ ){
            restr[i] = c;
            rescanc();
        }
        restr[i] = '\0';
        if( (token = str_to_tokennum()) == NAME_N ){
            gen_subnode(restr);
            strcpy(restr, find_subnode(restr)->newname);
        }
    }

    // Number
    else if( isdigit(c) ){
        renum = 0;
        for( i = 0; isdigit(c); i++ ){
            renum = 10 * renum + (c - '0');
            snprintf(restr+i, 2, "%c", c);
            rescanc();
        }
        token = NUM_N;
    }

    // Symbol
    else{
        switch( c ){
            case '+' :
                rescanc();
                if( c == '+' ){
                    token = INC_N;
                }else{
                    token = PLUS_N;
                    ungetc(c, fp);
                }
                break;
            case '-' :
                rescanc();
                if( c == '-' ){
                    rescanc();
                    if( c == '\'' ){
                        token = CDEC_N;
                    }else{
                        token = DEC_N;
                        ungetc(c, fp);
                    }
                    break;
                }else{
                    token = MINUS_N;
                    ungetc(c, fp);
                }
                break;
            case '*' : token = MUL_N; break;
            case '/' : token = DIV_N; break;
            case '%' : token = MOD_N; break;
            case '=' : token = ASSIGN_N; break;
            case '>' : token = RE_N; break;
            case '&' :
                rescanc();
                if( c == '&' ){
                    token = AND_N;
                }else{
                    token = UNKNOWN;
                }
                break;
            case '|' :
                rescanc();
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
        rescanc();
    }
    // printf("[debug:token=%d]\n", token);///debug
    // if( token != UNKNOWN ) printf("[debug:token=%d]\n", token);///debug

    return token;
}


static int is_invalid_char(void)  // TODO : integrate to scan.c
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


static int str_to_tokennum(void)  // TODO : integrate to scan.c
{
    int i;
    for( i = 0; i < 7; i++ ){
        if( strcmp(ks[i].keyname, restr) == 0 ){
            return ks[i].keynum;
        }
    }

    return NAME_N;
}


static void inline_function(void)
{
    int token;
    char strreg[MAXSTRLEN];

    // skip formal parameter
    while( rescan() != LBRACE_N );

    // generate until return comes
    strcpy(strreg, str);  // escape str
    while( (token = rescan()) != RETURN_N ){
        strcpy(str, restr);
        fsetpos(fp, tail);
        generate(token);
        fgetpos(fp, tail);
    }
    rescan();  // skip '('
    rescan();  // store retvar to restr
    fsetpos(fp, tail);
    strcpy(str, strreg);  // restore str

    return ;
}
