/* parse.c */
#include "funcprog.h"


static int token;


static void get_token(void);
static int is_token_(int);
static void is_token_or_err(int);
static void formal_parameters(void);
static void variable_names(void);
static void variable_declaration(void);
static void calc_main(void);

static int is_statement(void);
static void statements(void);
static int is_assign_statement(void);
static int is_increment_statement(void);
static int is_decrement_statement(void);
static int is_if_statement(void);
static int is_while_statement(void);
static int is_loop_statement(void);
static int is_return_statement(void);

static void condition_statements(void);
static void atom_condition(void);
static int is_logic_operator(void);
static void expression(void);
static int is_compare_operator(void);
static void int_operand(void);
static int is_int_operator(void);

static void error(void);



static void get_token(void)
{
    if( (token = scan()) <= 0 ){
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


void parse(void)
{
    is_token_or_err(NAME_N);  // function name
    formal_parameters();
    is_token_or_err(LBRACE_N);  // '{'
    variable_declaration();
    calc_main();
    is_token_or_err(RBRACE_N);  // '}'
    is_token_or_err(END_OF_FILE);

    return ;
}


static void formal_parameters(void)
{
    is_token_or_err(LPAREN_N);  // '('
    variable_names();
    is_token_or_err(RPAREN_N);  // ')'

    return ;
}


static void variable_names(void)
{
    if( is_token_(NAME_N) == False ){  // variable name
        return ;
    }

    while( is_token_(COMMA_N) ){  // ','
        is_token_or_err(NAME_N);  // variable name
    }

    return ;
}


static void variable_declaration(void)
{
    if( is_token_(INT_N) == False ){  // int
        return ;
    }

    variable_names();
    is_token_or_err(SEMI_N);  // ';'

    return ;
}


static void calc_main(void)
{
    statements();

    return ;
}


static int is_statement(void)
{
    if( is_assign_statement() ){
        return True;
    }else if( is_increment_statement() ){
        return True;
    }else if( is_decrement_statement() ){
        return True;
    }else if( is_if_statement() ){
        return True;
    }else if( is_while_statement() ){
        return True;
    }else if( is_loop_statement() ){
        return True;
    }else if( is_return_statement() ){
        return True;
    }

    return EMPTY;  // empty statement
}


static void statements(void)
{
    while( is_statement() != EMPTY );

    return ;
}


static int is_assign_statement(void)
{
    if( is_token_(NAME_N) == False ){
        return False;
    }

    is_token_or_err(EQUAL_N);
    expression();

    return True;
}


static int is_increment_statement(void)
{
    if( is_token_(NAME_N) == False ){  // variable name
        return False;
    }

    is_token_or_err(INC_N);  // '++'
    is_token_or_err(SEMI_N);  // ';'

    return True;
}


static int is_decrement_statement(void)
{
    if( is_token_(NAME_N) == False ){  // variable name
        return False;
    }

    is_token_or_err(DEC_N);  // '--'
    is_token_or_err(SEMI_N);  // ';'

    return True;
}


static int is_if_statement(void)
{
    if( is_token_(IF_N) == False ){  // if
        return False;
    }

    is_token_or_err(LPAREN_N);  // '('
    condition_statements();
    is_token_or_err(RPAREN_N);  // ')'
    is_token_or_err(LBRACE_N);  // '{'
    statements();
    is_token_or_err(RBRACE_N);  // '}'

    if( is_token_(ELSE_N) == False ){  // else
        return True;
    }

    is_token_or_err(LBRACE_N);  // '{'
    statements();
    is_token_or_err(RBRACE_N);  // '}'

    return True;
}


static int is_while_statement(void)
{
    if( is_token_(WHILE_N) == False ){
        return False;
    }

    condition_statements();
    is_token_or_err(LBRACE_N);  // '{'
    statements();
    is_token_or_err(RBRACE_N);  // '}'

    return True;
}


static int is_loop_statement(void)
{
    if( is_token_(LOOP_N) == False ){
        return False;
    }

    expression();
    is_token_or_err(LBRACE_N);  // '{'
    statements();
    is_token_or_err(RBRACE_N);  // '}'

    return True;
}


static int is_return_statement(void)
{
    if( is_token_(RETURN_N) == False ){
        return False;
    }

    is_token_or_err(LPAREN_N);  // '('
    expression();
    is_token_or_err(RPAREN_N);  // ')'

    return True;
}


static void condition_statements(void)
{
    atom_condition();
    while( is_logic_operator() ){
        atom_condition();
    }

    return ;
}


static int is_logic_operator(void)
{
    if( is_token_(AND_N) ){  // '&&'
        return True;
    }else if( is_token_(OR_N) ){  // '||'
        return True;
    }else if( is_token_(NOT_N) ){  // '!'
        return True;
    }

    return False;
}


static void atom_condition(void)
{
    expression();
    while( is_compare_operator() ){
        expression();
    }

    return ;
}


static int is_compare_operator(void)
{
    if( is_token_(EQUAL_N) ){  // '=='
        return True;
    }else if( is_token_(NOTEQ_N) ){  // '!='
        return True;
    }else if( is_token_(LE_N) ){  // '<'
        return True;
    }else if( is_token_(LEEQ_N) ){  // '<='
        return True;
    }else if( is_token_(RE_N) ){  // '>'
        return True;
    }else if( is_token_(REEQ_N) ){  // '>='
        return True;
    }

    return False;
}


static void expression(void)
{
    int_operand();
    while( is_int_operator() ){
        int_operand();
    }

    return ;
}


static void int_operand(void)
{
    if( is_token_(NAME_N) ){  // variable | function
        if( is_token_(LPAREN_N) ){  // '('
            variable_names();
            is_token_or_err(RPAREN_N);  // ')'
            is_token_or_err(SEMI_N);  // ';'
        }
    }else if( is_token_(NUM_N) ){  // Number
        // do nothing
    }else{
        error();
    }

    return ;
}


static int is_int_operator(void)
{
    if( is_token_(PLUS_N) ){  // '+'
        return True;
    }else if( is_token_(MINUS_N) ){  // '-'
        return True;
    }else if( is_token_(TIMES_N) ){  // '*'
        return True;
    }else if( is_token_(DIV_N) ){  // '/'
        return True;
    }else if( is_token_(MOD_N) ){  // '%'
        return True;
    }

    return False;
}


static void error(void)
{
    printf("Invalid program.\n");
    exit(EXIT_FAILURE);
}
