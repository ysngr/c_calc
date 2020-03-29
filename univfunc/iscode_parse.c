/* iscode_parse.c */
#include "iscode.h"
#include <setjmp.h>


static int token;

static struct counter{
    int v;
    int l;
} cnt;

static struct flags{
    int is_var_def;
    int is_label_dep;
} fs;

jmp_buf env;  // for error


static void get_token(void);
static int is_token_(int);
static void is_token_or_err(int);

static void init_parse(void);
static void init_counter(void);
static void init_flag(void);

static void variable_names(void);
static int is_variable(void);
static void variable(void);
static void variable_declaration(void);
static void label(void);
static void labelname(void);

static void program_name(void);
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
        error("Invalid token is scanned.");
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
        error("Expected token is not found.");
    }

    return ;
}


void error(char *errmsg)
{
    #ifdef PRINT_ERRMSG
        if( errmsg!= NULL ){
            printf("%s\n", errmsg);
        }
    #endif
    
    end_scan();
    longjmp(env, Error);

    return ;
}


static void init_parse(void)
{
    init_register();
    get_token();

    return ;
}


static void init_counter(void)
{
    cnt.v = 0;
    cnt.l = 0;

    return ;
}


static void init_flag(void)
{
    fs.is_var_def = True;
    fs.is_label_dep = False;

    return ;
}


int parse(void)
{
    init_scan();
    init_parse();

    if( setjmp(env) != 0 ){
        return False;
    }

    do{
        init_counter();
        init_flag();
        init_fprog_list();

        // prog-name formal-params '{' var-decl calc-main '}'
        program_name();
        fs.is_var_def = True;
        formal_parameters();
        is_token_or_err(LBRACE_N);
        variable_declaration();
        fs.is_var_def = False;
        calc_main();
        is_token_or_err(RBRACE_N);

        check_label_link();
    }while( is_token_(END_OF_FILE) == False );

    // print_list();  // for debug

    end_scan();

    return True;
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
            error("Invalid variable index.");
        }
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
        error("Variable is expected but not found.");
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
    char labelnum[MAXSTRLEN];

    if( token != NAME_N ){
        error("Label is not found.");
    }

    // 'L'Num
    if( str[0] != 'L' ){
        error("Invalid label name.");
    }
    for( i = 1; str[i] != '\0'; i++ ){
        if( isdigit(str[i]) == False ){
            error("Invalid label index.");
        }
        labelnum[i-1] = str[i];
    }

    if( fs.is_label_dep ){
        generate_label();
        fs.is_label_dep = False;
    }else{
        if( ++cnt.l != atoi(labelnum) ){
            error("Invalid label index.");
        }
        paste_label(str);
    }

    get_token();

    return ;
}


static void program_name(void)
{
    is_token_or_err(NAME_N);
    define_variable();

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
        error("Invalid conditional expression.");
    }
    is_token_or_err(RPAREN_N);

    // goto-stat
    if( is_goto_statement() == False ){
        error("Invalid format of if statement.");
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
        error("Invalid format of return statement.");
    }
    is_token_or_err(RPAREN_N);
    is_token_or_err(SEMI_N);

    return True;
}
