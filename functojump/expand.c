/* expand.c */
#include "functojump.h"
#include "scan.h"

#define MAXNEWNAME 7
#define FUNCNAME_N 36

static FILE *fp;
static fpos_t head, scanp, tail;

static char funcname[MAXSTRLEN];
static int fpnum;

static char c, uc;
static char exstr[MAXSTRLEN];
static int exnum;

static struct flags{
    int is_register_exec;
    int is_formal_parameter;
    int is_var_def;
    int is_label_dep;
    int is_exscan_buf_exist;
    int is_ungetc_exec;
} fs;

static struct sublist{
    char *origname;
    char *newname;
    struct sublist *nextsub;
} *ss;

static struct buffer{
    int nexttoken;
    int nextnum;
    char nextstr[MAXSTRLEN];
} esbuf;


static void initialize_expand(char*);
static void initialize_flag(void);
static struct sublist *find_subnode(char*);
static struct sublist *gen_subnode(char*);
static void exscanc(void);
static int exscan(void);
static void exungetc(char);
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
    fpnum = 0;

    initialize_flag();
    ss = NULL;

    exscanc();

    return ;
}


static void initialize_flag(void)
{
    fs.is_register_exec = False;
    fs.is_formal_parameter = False;
    fs.is_var_def = True;
    fs.is_label_dep = False;
    fs.is_exscan_buf_exist = False;
    fs.is_ungetc_exec = False;

    return ;
}


static struct sublist *find_subnode(char *oname)
{
    struct sublist *sp;

    for( sp = ss; sp != NULL; sp = sp->nextsub ){
        if( strcmp(sp->origname, oname) == 0 ){
            return sp;  // already exists in sublist
        }
    }

    return NULL;
}


static struct sublist *gen_subnode(char *oname)
{
    struct sublist *ns, *sp;
    int len_name;
    static int v = 0;
    static int l = 0;

    if( (sp = find_subnode(oname)) != NULL ){  // node already exists
        if( strncmp(sp->newname, "__L", 3) == 0 ){
            if( fs.is_label_dep ){
                define_label_explicitly(sp->newname);
            }else{
                reference_label(sp->newname);
            }
        }
        return sp;
    }

    // generate new node
    ns = (struct sublist*)Malloc(sizeof(struct sublist));
    len_name = strlen(oname) + 1;
    ns->origname = (char*)Malloc(sizeof(char)*len_name);
    strcpy(ns->origname, oname);
    ns->newname = (char*)Malloc(sizeof(char)*MAXNEWNAME);
    ns->nextsub = NULL;

    // register newname
    if( fs.is_var_def || strcmp(ns->origname, "_r") == 0 ){
        snprintf(ns->newname, MAXNEWNAME, "__v%d", v++);
        define_variable_explicitly(ns->newname);
        if( fs.is_formal_parameter ){
            fpnum++;
        }
    }else{
        snprintf(ns->newname, MAXNEWNAME, "__L%d", l++);
        if( fs.is_label_dep ){
            define_label_explicitly(ns->newname);
            fs.is_label_dep = False;
        }else{
            reference_label(ns->newname);
        }
    }

    // connect
    if( ss == NULL ){
        ss = ns;
    }else{
        for( sp = ss; sp->nextsub != NULL; sp = sp->nextsub );
        sp->nextsub = ns;
    }

    return ns;
}


void expand(char *funcname, char *retvar)
{
    initialize_expand(funcname);

    while( exscan() != FUNCNAME_N );

    fs.is_register_exec = True;
    inline_function();
    strcpy(retvar, exstr);

    // print_sublist();  // for debug
    finalize_expand();

    return ;
}


void expand_binope(char *funcname, char *arg_l, char *arg_r, char *retvar)
{
    initialize_arglist();
    register_arg(arg_l);
    register_arg(arg_r);
    expand(funcname, retvar);
    finalize_arglist(NULL);

    return ;
}


static void exscanc(void)
{
    if( fs.is_ungetc_exec ){
        fs.is_ungetc_exec = False;
        c = uc;
        return ;
    }

    fsetpos(fp, &scanp);
    c = fgetc(fp);
    fgetpos(fp, &scanp);

    return ;
}


static void exungetc(char c)
{
    uc = c;
    fs.is_ungetc_exec = True;

    return ;
}


static int exscan(void)
{
    int i;
    int token;
    struct sublist *s;
    static int prevtoken = UNKNOWN;

    if( fs.is_exscan_buf_exist ){
        fs.is_exscan_buf_exist = False;
        exnum = esbuf.nextnum;
        strcpy(exstr, esbuf.nextstr);
        return esbuf.nexttoken;
    }

    while( is_invalid_char(c) ){
        exscanc();
    }

    // Name, Keyword
    if( isalpha(c) || c == '_'){
        for( i = 0; isalnum(c) || c == '_'; i++ ){
            exstr[i] = c;
            exscanc();
        }
        exstr[i] = '\0';
        if( (token = str_to_tokennum(exstr)) == NAME_N ){
            if( prevtoken == GOTO_N ){
                fs.is_label_dep = True;
            }
            if( fs.is_register_exec ){
                if( strncmp(exstr, "sig_", 4) == 0 ){
                    s = find_subnode(exstr+4);
                    snprintf(exstr, MAXSTRLEN, "sig_%s", s->newname);
                }else{
                    s = gen_subnode(exstr);
                    strcpy(exstr, s->newname);
                }
            }
            if( strcmp(funcname, exstr) == 0 ){
                token = FUNCNAME_N;
            }
        }
    }

    // Number
    else if( isdigit(c) ){
        exnum = 0;
        for( i = 0; isdigit(c); i++ ){
            exnum = 10 * exnum + (c - '0');
            snprintf(exstr+i, 2, "%c", c);
            exscanc();
        }
        token = NUM_N;
    }

    // Symbol
    else{
        switch( c ){
            case '+' :
                exscanc();
                token = ( c == '+' )? INC_N : UNKNOWN;
                break;
            case '-' :
                exscanc();
                if( c == '-' ){
                    exscanc();
                    if( c == '\'' ){
                        token = CDEC_N;
                    }else{
                        token = DEC_N;
                        exungetc(c);
                    }
                    break;
                }else{
                    token = UNKNOWN;
                }
                break;
            case '=' : token = ASSIGN_N; break;
            case '>' : token = RE_N; break;
            case '&' :
                exscanc();
                token = ( c == '&' )? AND_N : UNKNOWN;
                break;
            case '|' :
                exscanc();
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
        exscanc();
    }

    prevtoken = token;

    return token;
}


static void inline_function(void)
{
    int token;
    int argnum;
    struct arglist *ap;
    struct sublist *sp;
    char strreg[MAXSTRLEN];

    // skip formal parameter and variable declaration
    fs.is_formal_parameter = True;
    while( exscan() != LBRACE_N );
    fs.is_formal_parameter = False;
    if( (token = exscan()) == INT_N ){
        while( exscan() != SEMI_N );
    }else{
        esbuf.nexttoken = token;
        esbuf.nextnum = exnum;
        strcpy(esbuf.nextstr, exstr);
        fs.is_exscan_buf_exist = True;
    }
    fs.is_var_def = False;

    strcpy(strreg, str);  // escape str for generate

    // generate assignment of argument
    argnum = 0;
    sp = ss;
    ap = get_args();
    while( ap != NULL && sp != NULL ){
        fsetpos(fp, &tail);
        generate_assign_with_sign(sp->newname, ap->argname);
        fgetpos(fp, &tail);
        sp = sp->nextsub;
        ap = ap-> nextarg;
        argnum++;
    }
    if( fpnum != argnum ){
        printf("Invalid number of arguments.\n");
        exit(EXIT_FAILURE);
    }

    // generate until return comes
    while( (token = exscan()) != RETURN_N ){
        strcpy(str, exstr);
        fsetpos(fp, &tail);
        generate(token);
        fgetpos(fp, &tail);
    }
    exscan();  // skip '('
    exscan();  // store retvar to exstr
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

    printf("Sublist of function \"%s\" =", funcname);
    for( sp = ss; sp != NULL; sp = sp->nextsub ){
        printf(" [%s->%s]", sp->origname, sp->newname);
    }
    printf("\n\n");

    return ;
}
