/* parse.c */
#include "jumpexec.h"


static int token;

static int labelidx;
static int varidx;
static int num;


static struct counter{
    int v;
    int l;
} cnt;

static struct statreg{
    int stype;
    int a;
    int b;
} s;

static struct flags{
    int is_var_def;
    int is_label_dep;
} fs;


static void get_token(void);
static int is_token_(int);
static void is_token_or_err(int);
static void error(void);

static void initialize_parse(void);
static void initialize_counter(void);
static void initialize_flag(void);
static void initialize_statreg(void);

static int variable_names(void);
static int is_variable(void);
static void variable(void);
static void variable_declaration(void);
static void label(void);
static void labelname(void);

static int formal_parameters(void);
static void calc_main(void);
static void statements(void);
static int is_statement(void);
static int is_val_update_statement(void);
static int is_if_statement(void);
static int is_goto_statement(void);
static int return_statement(void);



static void get_token(void)
{
    if( (token = scan()) < 0 ){
        error();
    }

    return ;
}


static int is_token_(int cmptoken)
{
    int i;

    if( cmptoken == VARIABLE_N ){
        if( str[0] != 'v' ){
            return False;
        }
        for( i = 1; str[i] != '\0'; i++ ){
            if( ! isdigit(str[i]) ){
                return False;
            }
        }
        varidx = atoi(str+1);
    }else{
        if( token != cmptoken ){
            return False;
        }
        if( cmptoken == NUM_N ){
            num = atoi(str);
        }
    }

    get_token();

    return True;
}


static void is_token_or_err(int cmptoken)
{
    if( is_token_(cmptoken) == False ){
        error();
    }

    return ;
}


static void error(void)
{
    printf("\nInvalid program.\n");
    exit(EXIT_FAILURE);

    return ;
}


static void initialize_parse(void)
{
    initialize_counter();
    initialize_flag();
    get_token();

    return ;
}


static void initialize_counter(void)
{
    cnt.v = 0;
    cnt.l = 0;

    return ;
}


static void initialize_flag(void)
{
    fs.is_var_def = True;
    fs.is_label_dep = False;

    return ;
}


static void initialize_statreg(void)
{
    s.stype = Empty;
    s.a = Empty;
    s.b = Empty;

    labelidx = Empty;
    varidx = Empty;
    num = Empty;

    return ;
}


int parse(void)
{
    int fpnum;

    initialize_parse();

    // prog-name formal-params '{' var-decl calc-main '}'
    is_token_or_err(NAME_N);
    fs.is_var_def = True;
    fpnum = formal_parameters();
    is_token_or_err(LBRACE_N);
    variable_declaration();
    fs.is_var_def = False;
    calc_main();
    is_token_or_err(RBRACE_N);
    is_token_or_err(END_OF_FILE);

    check_label_link();

    return fpnum;
}


static int variable_names(void)
{
    int varnum;

    // var { ',' var }
    if( is_variable() ){
        varnum = 1;
        while( is_token_(COMMA_N) ){
            variable();
            varnum++;
        }
    }else{
        varnum = 0;
    }

    return varnum;
}


static int is_variable(void)
{
    int i;
    char varnum[MAXSTRLEN];

    if( token != NAME_N ){
        return False;
    }

    // 'v'Num
    if( str[0] != 'v' ){
        return False;
    }
    for( i = 1; str[i] != '\0'; i++ ){
        if( isdigit(str[i]) == False ){
            return False;
        }
        varnum[i-1] = str[i];
    }

    if( fs.is_var_def ){
        if( ++cnt.v != atoi(varnum) ){
            error();
        }
    }

    get_token();

    return True;
}


static void variable(void)
{
    if( is_variable() == False ){
        error();
    }

    return ;
}


static void label(void)
{
    // label
    labelname();

    // ':'
    is_token_or_err(COLON_N);

    return ;
}


static void labelname(void)
{
    int i;

    if( token != NAME_N ){
        error();
    }

    // 'L'Num
    if( str[0] != 'L' ){
        error();
    }
    for( i = 1; str[i] != '\0'; i++ ){
        if( isdigit(str[i]) == False ){
            error();
        }
    }

    labelidx = atoi(str+1);
    if( ! fs.is_label_dep ){
        if( ++cnt.l != labelidx ){
            error();
        }
    }

    get_token();

    return ;
}


static int formal_parameters(void)
{
    int fpnum;

    // '(' var-names ')'
    is_token_or_err(LPAREN_N);
    fpnum = variable_names();
    is_token_or_err(RPAREN_N);

    register_variable(fpnum);

    return fpnum;
}


static void variable_declaration(void)
{
    int varnum = 0;

    // 'int' var-names ';'
    if( is_token_(INT_N) ){
        varnum = variable_names();
        is_token_or_err(SEMI_N);
    }

    register_variable(varnum);

    return ;
}


static void calc_main(void)
{
    statements();

    return ;
}


static void statements(void)
{
    do{
        label();
    }while( is_statement() );

    return_statement();

    return ;
}


static int is_statement(void)
{
    initialize_statreg();

    if( is_val_update_statement() || is_if_statement() || is_goto_statement() ){
        register_statement(s.stype, s.a, s.b);
    }else{
        return False;
    }

    return True;
}


static int is_val_update_statement(void)
{
    // var
    if( is_token_(VARIABLE_N) == False ){
        return False;
    }
    s.a = varidx;

    // assignment statement : '=' (Num | var)
    if( is_token_(ASSIGN_N) ){
        if( is_token_(NUM_N) ){
            s.stype = NAT_ASSIGN_STAT;
            s.b = num;
        }else{
            is_token_or_err(VARIABLE_N);
            s.stype = VAR_ASSIGN_STAT;
            s.b = varidx;
        }
    }
    // increment statement : '++'
    else if( is_token_(INC_N) ){
        s.stype = INCR_STAT;
    }
    // conditional decrement statement : '--''
    else{
        is_token_or_err(CDEC_N);
        s.stype = CDECR_STAT;
    }

    // ';'
    is_token_or_err(SEMI_N);

    return True;
}


static int is_if_statement(void)
{
    // 'if'
    if( is_token_(IF_N) == False ){
        return False;
    }
    s.stype = IF_GOTO_STAT;

    // '(' var '>' '0' ')'
    is_token_or_err(LPAREN_N);
    is_token_or_err(VARIABLE_N);
    s.a = varidx;
    is_token_or_err(RE_N);
    is_token_or_err(NUM_N);
    s.b = num;
    if( strcmp(str, "0") != 0 ){
        error();
    }
    is_token_or_err(RPAREN_N);

    // goto-stat
    if( is_goto_statement() == False ){
        error();
    }

    return True;
}


static int is_goto_statement(void)
{
    // 'goto'
    if( is_token_(GOTO_N) == False ){
        return False;
    }
    if( s.stype != IF_GOTO_STAT ){
        s.stype = GOTO_STAT;
    }

    // label ';'
    fs.is_label_dep = True;
    labelname();
    fs.is_label_dep = False;
    is_token_or_err(SEMI_N);
    if( s.stype == IF_GOTO_STAT ){
        s.b = labelidx;
    }else{
        s.a = labelidx;
    }

    return True;
}


static int return_statement(void)
{
    // 'return' '(' 'v1' ')' ';'
    is_token_or_err(RETURN_N);
    is_token_or_err(LPAREN_N);
    variable();
    if( strcmp(str, "v1") != 0 ){
        error();
    }
    is_token_or_err(RPAREN_N);
    is_token_or_err(SEMI_N);

    register_statement(RETURN_STAT, Empty, Empty);

    return True;
}
