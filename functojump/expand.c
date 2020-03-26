/* expand.c */
#include "functojump.h"
#include "scan.h"

#define MAXNEWNAME 7
#define FUNCNAME_N 36

static FILE *fp;
static fpos_t head, scanp, tail;

static char funcname[MAXSTRLEN];
static int counter = 0;

static char c;
static char restr[MAXSTRLEN];
static int renum;

static struct sublist{
    char *origname;
    char *newname;
    struct sublist *nextsub;
} *ss;


static void initialize_expand(char*);
static void initialize_sublist(void);
static struct sublist *find_subnode(char*);
static void gen_subnode(char*);
static void rescanc(void);
static int rescan(void);
static void inline_function(void);
static void finalize_expand(void);

static void print_sublist(void);



static void initialize_expand(char *fname)
{
    fp = get_generate_fp();
    head = get_generate_head();
    scanp = head;
    fgetpos(fp, &tail);

    strcpy(funcname, fname);

    initialize_sublist();

    rescanc();

    return ;
}


static void initialize_sublist(void)
{
    struct arglist *ap;

    ss = NULL;

    for( ap = get_args(); ap != NULL; ap = ap->nextarg ){
        gen_subnode(ap->argname);
    }

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

    if( find_subnode(name) != NULL ){  // node already exists
        return ;
    }
    if( strcmp(funcname, name) == 0 ){  // function name is not registered
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
    initialize_expand(funcname);

    while( rescan() != FUNCNAME_N );

    inline_function();
    strcpy(retvar, restr);

    print_sublist();  // for debug
    finalize_expand();

    return ;
}


static void rescanc(void)
{
    fsetpos(fp, &scanp);
    c = fgetc(fp);
    fgetpos(fp, &scanp);

    return ;
}


static int rescan(void)
{
    int i;
    int token;
    struct sublist *s;

    while( is_invalid_char(c) ){
        rescanc();
    }

    // Name, Keyword
    if( isalpha(c) || c == '_'){
        for( i = 0; isalnum(c) || c == '_'; i++ ){
            restr[i] = c;
            rescanc();
        }
        restr[i] = '\0';
        if( (token = str_to_tokennum(restr)) == NAME_N ){
            gen_subnode(restr);
            if( (s = find_subnode(restr)) == NULL ){
                token = FUNCNAME_N;
            }else{
                strcpy(restr, s->newname);
            }
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
                token = ( c == '+' )? INC_N : UNKNOWN;
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
                    token = UNKNOWN;
                }
                break;
            case '=' : token = ASSIGN_N; break;
            case '>' : token = RE_N; break;
            case '&' :
                rescanc();
                token = ( c == '&' )? AND_N : UNKNOWN;
                break;
            case '|' :
                rescanc();
                token = ( c == '|' )? OR_N : UNKNOWN;
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

    return token;
}


static void inline_function(void)
{
    int token;
    char strreg[MAXSTRLEN];

    // skip formal parameter
    while( rescan() != LBRACE_N );

    // generate until return comes
    strcpy(strreg, str);  // escape str for generate
    while( (token = rescan()) != RETURN_N ){
        strcpy(str, restr);
        fsetpos(fp, &tail);
        generate(token);
        fgetpos(fp, &tail);
    }
    rescan();  // skip '('
    rescan();  // store retvar to restr
    fsetpos(fp, &tail);
    strcpy(str, strreg);  // restore str

    return ;
}


static void finalize_expand(void)
{
    struct sublist *sp, *rm;

    if( ss != NULL ){
        while( ss->nextsub != NULL ){
            for( sp = ss; sp->nextsub->nextsub != NULL; sp = sp->nextsub );
            rm = sp->nextsub;
            sp->nextsub = NULL;
            free(rm->origname);
            free(rm->newname);
            free(rm);
        }
        free(ss);
    }

    return ;
}



// debug function
static void print_sublist(void)
{
    struct sublist *sp;

    printf("Sublist =");
    for( sp = ss; sp != NULL; sp = sp->nextsub ){
        printf(" [%s->%s]", sp->origname, sp->newname);
    }
    printf("\n");

    return ;
}
