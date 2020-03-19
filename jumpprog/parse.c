/* parse.c */
#include "jumpprog.h"


static int token;

static struct flags{
    int is_var_def;
    int is_label_dep;
} fs;


static void get_token(void);
static int is_token_(int);
static void is_token_or_err(int);
static void error(void);

static void init_parse(void);
static void init_flag(void);

static void variable_names(void);
static int is_variable(void);
static void variable(void);
static void variable_declaration(void);
static void label(void);
static void labelname(void);

static void formal_parameters(void);
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
    if( token != cmptoken ){
        return False;
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


static void init_parse(void)
{
    init_register();
    get_token();

    return ;
}


static void init_flag(void)
{
    fs.is_var_def = True;
    fs.is_label_dep = False;

    return ;
}


void parse(void)
{
    init_parse();

    do{
        init_flag();
        init_fprog_list();

        // prog-name formal-params '{' var-decl calc-main '}'
        is_token_or_err(NAME_N);
        formal_parameters();
        is_token_or_err(LBRACE_N);
        variable_declaration();
        calc_main();
        is_token_or_err(RBRACE_N);

        check_label_link();
    }while( is_token_(END_OF_FILE) == False );

    print_list();  // for debug

    return ;
}


static void variable_names(void)
{
    // var { ',' var }
    if( is_variable() ){
        while( is_token_(COMMA_N) ){
            variable();
        }
    }

    return ;
}


static int is_variable(void)
{
    int i;

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
    }

    if( fs.is_var_def ){
        define_variable();
    }else{
        reference_variable();
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

    if( fs.is_label_dep ){
        generate_label();
        fs.is_label_dep = False;
    }else{
        paste_label(str);
    }

    get_token();

    return ;
}


static void formal_parameters(void)
{
    // '(' var-names ')'
    is_token_or_err(LPAREN_N);
    variable_names();
    is_token_or_err(RPAREN_N);

    return ;
}


static void variable_declaration(void)
{
    // 'int' var-names ';'
    if( is_token_(INT_N) ){
        variable_names();
        is_token_or_err(SEMI_N);
    }

    return ;
}


static void calc_main(void)
{
    fs.is_var_def = False;
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
    if( is_val_update_statement() ){
        return True;
    }
    else if( is_if_statement() ){
        return True;
    }
    else if( is_goto_statement() ){
        return True;
    }

    return False;
}


static int is_val_update_statement(void)
{
    // var
    if( is_token_(NAME_N) == False ){
        return False;
    }

    // assignment statement : '=' (Num | var)
    if( is_token_(ASSIGN_N) ){
        if( is_token_(NUM_N) ){
            // do nothing
        }else{
            variable();
        }
    }
    // increment statement : '++'
    else if( is_token_(INC_N) ){
        // do nothing
    }
    // conditional decrement statement : '--''
    else{
        is_token_or_err(CDEC_N);
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

    // '(' var '>' '0' ')'
    is_token_or_err(LPAREN_N);
    variable();
    is_token_or_err(RE_N);
    is_token_or_err(NUM_N);
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

    // label ';'
    fs.is_label_dep = True;
    labelname();
    is_token_or_err(SEMI_N);

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

    return True;
}
