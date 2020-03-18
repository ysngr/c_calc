/* parse.c */
#include "convtojump.h"


static int token;
static char expr_r[MAXSTRLEN];

static struct buffer{
    int nexttoken;
    int nextnum;
    char nextstr[MAXSTRLEN];
} tbuf;

static struct flags{
    int is_var_def;
    int is_label_dep;
    int is_token_buf_exist;
    int is_generate_token;
    int is_argument;
} fs;


static void get_token(void);
static void get_token_from_buf(void);
static int is_token_(int);
static void is_token_or_err(int);
static void error(void);

static void initialize_parse(void);
static void initialize_flag(void);

static void formal_parameters(void);
static void variable_names(void);
static void variable_declaration(void);
static void calc_main(void);

static void statements(void);
static int is_label(void);
static int is_statement(void);
static int is_single_statement(void);
static int is_val_update_statement(void);
static int is_if_statement(void);
static int is_while_statement(void);
static int is_loop_statement(void);
static int is_return_statement(void);
static int is_goto_statement(void);

static void conditional_expression(void);
static void simple_conditional_expression(void);
static void conditional_term(void);
static void atom_conditional_expression(void);
static int is_relational_operator(void);
static void numerical_expression(void);
static int is_additive_operator(char*);
static void simple_numerical_expression(void);
static int is_multiplicative_operator(char*);
static void numerical_term(void);
static void atom_numerical_expression(void);

static void exprcat(char*, char*, char*, char*);



static void get_token(void)
{
    if( fs.is_token_buf_exist ){
        get_token_from_buf();
        fs.is_token_buf_exist = False;
    }

    else if( (token = scan()) < 0 ){
        error();
    }

    return ;
}


static void get_token_from_buf(void)
{
    token = tbuf.nexttoken;
    strcpy(str, tbuf.nextstr);
    num = tbuf.nextnum;

    return ;
}


static int is_token_(int cmptoken)
{
    if( token != cmptoken ){
        return False;
    }

    if( cmptoken == NAME_N ){
        // label
        if( fs.is_label_dep ){
            generate_label();
            fs.is_label_dep = False;
        }
        // variable, function
        else if( fs.is_var_def ){
            define_variable();
        }else{
            reference_variable();
        }
    }

    if( fs.is_generate_token ){
        generate(token);
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
    printf("Invalid program.\n");
    exit(EXIT_FAILURE);

    return ;
}


static void initialize_parse(void)
{
    initialize_register();
    get_token();

    return ;
}


static void initialize_flag(void)
{
    fs.is_var_def = True;
    fs.is_label_dep = False;
    fs.is_token_buf_exist = False;
    fs.is_generate_token = True;
    fs.is_argument = False;

    return ;
}


void parse(void)
{
    initialize_parse();

    do{
        initialize_flag();
        initialize_fprog_list();

        // func-name formal-params '{' var-decl calc-main '}'
        is_token_or_err(NAME_N);
        formal_parameters();
        is_token_or_err(LBRACE_N);
        variable_declaration();
        calc_main();
        is_token_or_err(RBRACE_N);

        check_label_link();
    }while( is_token_(END_OF_FILE) == False );

    // print_list();  // for debug

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


static void variable_names(void)
{
    // var { ',' var }
    if( is_token_(NAME_N) ){
        while( is_token_(COMMA_N) ){
            is_token_or_err(NAME_N);
        }
    }

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
        while( is_label() );
    }while( is_statement() != Empty );

    return ;
}


static int is_label(void)
{
    int prevtoken;
    char prevstr[MAXSTRLEN];
    int prevnum;

    // keep prev state
    prevtoken = token;
    strcpy(prevstr, str);
    prevnum = num;

    // label
    if( token != NAME_N ){
        return False;
    }
    get_token();

    // ':'
    if( token == COLON_N ){
        generate(prevtoken);  // generate labelname
        generate(token);  // generate ':'
        get_token();
        paste_label(prevstr);
        return True;
    }

    // current state -> next state (buffer)
    tbuf.nexttoken = token;
    strcpy(tbuf.nextstr, str);
    tbuf.nextnum = num;
    fs.is_token_buf_exist = True;

    // prev state -> current state
    token = prevtoken;
    strcpy(str, prevstr);
    num = prevnum;

    return False;
}


static int is_statement(void)
{
    if( is_val_update_statement() ){
        return True;
    }
    else if( is_if_statement() ){
        return True;
    }
    else if( is_while_statement() ){
        return True;
    }
    else if( is_loop_statement() ){
        return True;
    }
    else if( is_return_statement() ){
        return True;
    }
    else if( is_goto_statement() ){
        return True;
    }

    return Empty;  // empty statement
}


static int is_single_statement(void)
{
    generate_ln_indent();
    is_statement();
    generate_nl_outdent();

    return True;
}


static int is_val_update_statement(void)
{
    // var
    if( is_token_(NAME_N) == False ){
        return False;
    }

    // assignment statement : '=' num-expr
    if( is_token_(ASSIGN_N) ){
        numerical_expression();
    }
    // increment statement : '++'
    else if( is_token_(INC_N) ){
        // do nothing
    }
    // decrement statement : '--'
    else if( is_token_(DEC_N) ){
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

    // '(' cond-expr ')'
    is_token_or_err(LPAREN_N);
    conditional_expression();
    is_token_or_err(RPAREN_N);

    // '{' stats '}' | stat
    if( is_token_(LBRACE_N) ){
        statements();
        is_token_or_err(RBRACE_N);
    }else{
        is_single_statement();
    }

    // 'else'
    if( is_token_(ELSE_N) ){
        // '{', stats, '}' | stat
        if( is_token_(LBRACE_N) ){
            statements();
            is_token_or_err(RBRACE_N);
        }else{
            is_single_statement();
        }
    }

    return True;
}


static int is_while_statement(void)
{
    // 'while'
    if( is_token_(WHILE_N) == False ){
        return False;
    }

    // '(' cond-expr ')'
    is_token_or_err(LPAREN_N);
    conditional_expression();
    is_token_or_err(RPAREN_N);

    // '{' stats '}' | stat
    if( is_token_(LBRACE_N) ){
        statements();
        is_token_or_err(RBRACE_N);
    }else{
        is_single_statement();
    }

    return True;
}


static int is_loop_statement(void)
{
    // 'loop'
    if( is_token_(LOOP_N) == False ){
        return False;
    }

    // '(' num-expr ')'
    is_token_or_err(LPAREN_N);
    numerical_expression();
    is_token_or_err(RPAREN_N);

    // '{' stats '}' | stat
    if( is_token_(LBRACE_N) ){
        statements();
        is_token_or_err(RBRACE_N);
    }else{
        is_single_statement();
    }

    return True;
}


static int is_return_statement(void)
{
    // 'return'
    if( is_token_(RETURN_N) == False ){
        return False;
    }

    // '(' num-expr ')' ';'
    is_token_or_err(LPAREN_N);
    numerical_expression();
    is_token_or_err(RPAREN_N);
    is_token_or_err(SEMI_N);

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
    is_token_or_err(NAME_N);
    is_token_or_err(SEMI_N);

    return True;
}


static void conditional_expression(void)
{
    // simple-cond-expr { '||' simple-cond_expr }
    simple_conditional_expression();
    while( is_token_(OR_N) ){
        simple_conditional_expression();
    }

    return ;
}


static void simple_conditional_expression(void)
{
    // cond-term { '&&' cond-term }
    conditional_term();
    while( is_token_(AND_N) ){
        conditional_term();
    }

    return ;
}


static void conditional_term(void)
{
    // '(' cond-expr ')'
    if( is_token_(LPAREN_N) ){
        conditional_expression();
        is_token_or_err(RPAREN_N);
    }
    // '!' cond-term
    else if( is_token_(NOT_N) ){
        conditional_term();
    }
    // atom-cond-expr
    else{
        atom_conditional_expression();
    }

    return ;
}


static void atom_conditional_expression(void)
{
    // num-expr rel-ope num-expr { rel-ope num-expr }
    numerical_expression();
    if( is_relational_operator() == False ){
        error();
    }
    numerical_expression();
    while( is_relational_operator() ){
        numerical_expression();
    }

    return ;
}


static int is_relational_operator(void)
{
    // '=='
    if( is_token_(EQUAL_N) ){
        return True;
    }
    // '!='
    else if( is_token_(NOTEQ_N) ){
        return True;
    }
    // '<'
    else if( is_token_(LE_N) ){
        return True;
    }
    // '<='
    else if( is_token_(LEEQ_N) ){
        return True;
    }
    // '>'
    else if( is_token_(RE_N) ){
        return True;
    }
    // '>='
    else if( is_token_(REEQ_N) ){
        return True;
    }

    return False;
}


static void numerical_expression(void)
{
    int is_minus_exist;
    char expr_l[MAXSTRLEN], expr_o[MAXSTRLEN], exprstr[MAXSTRLEN];

    fs.is_generate_token = False;

    // ['-'] simple-num-expr { add-ope simple-num-ope }
    if( is_token_(MINUS_N) ){
        is_minus_exist = True;
    }else{
        is_minus_exist = False;
    }

    simple_numerical_expression();
    if( is_minus_exist ){
        strcpy(expr_l, "0");
        strcpy(expr_o, "_sub");
        exprcat(exprstr, expr_l, expr_o, expr_r);
        strcpy(expr_l, exprstr);
    }else{
        strcpy(expr_l, expr_r);
    }

    while( is_additive_operator(expr_o) ){
        simple_numerical_expression();
        exprcat(exprstr, expr_l, expr_o, expr_r);
        strcpy(expr_l, exprstr);
    }

    if( fs.is_argument ){
        strcpy(expr_r, expr_l);
    }else{
        generate_expr(expr_l);
        fs.is_generate_token = True;
    }

    return ;
}


static int is_additive_operator(char *expr_o)
{
    // '+'
    if( is_token_(PLUS_N) ){
        strcpy(expr_o, "_add");
        return True;
    }
    // '-'
    else if( is_token_(MINUS_N) ){
        strcpy(expr_o, "_sub");
        return True;
    }

    return False;
}


static void simple_numerical_expression(void)
{
    char expr_l[MAXSTRLEN], expr_o[MAXSTRLEN], exprstr[MAXSTRLEN];

    // num-term { mul-ope num-term }
    numerical_term();
    strcpy(expr_l, expr_r);

    while( is_multiplicative_operator(expr_o) ){
        numerical_term();
        exprcat(exprstr, expr_l, expr_o, expr_r);
        strcpy(expr_l, exprstr);
    }

    strcpy(expr_r, expr_l);

    return ;
}


static int is_multiplicative_operator(char *expr_o)
{
    // '*'
    if( is_token_(MUL_N) ){
        strcpy(expr_o, "_mul");
        return True;
    }
    // '/'
    else if( is_token_(DIV_N) ){
        strcpy(expr_o, "_div");
        return True;
    }
    // '%'
    else if( is_token_(MOD_N) ){
        strcpy(expr_o, "_mod");
        return True;
    }

    return False;
}


static void numerical_term(void)
{
    char exprstr[MAXSTRLEN];

    // '(' num-expr ')'
    if( is_token_(LPAREN_N) ){
        numerical_expression();
        strcpy(exprstr, expr_r);
        is_token_or_err(RPAREN_N);
        snprintf(expr_r, MAXSTRLEN, "(%s)", exprstr);
    }
    // atom-num-expr
    else{
        atom_numerical_expression();
    }

    return ;
}


static void atom_numerical_expression(void)
{
    char exprstr[MAXSTRLEN];

    // var | func
    if( is_token_(NAME_N) ){
        strcpy(exprstr, str);
        // '()' | '(' num-exprs, ')'
        if( is_token_(LPAREN_N) ){  // '('
            strcat(exprstr, "(");
            // func()
            if( is_token_(RPAREN_N) ){  // ')'
                strcat(exprstr, ")");
            }
            // func(fp, ... ,fp)
            else{
                fs.is_argument = True;
                numerical_expression();
                strcat(exprstr, expr_r);
                while( is_token_(COMMA_N) ){  // ','
                    strcat(exprstr, ", ");
                    numerical_expression();
                    strcat(exprstr, expr_r);
                }
                is_token_or_err(RPAREN_N);  // ')'
                strcat(exprstr, ")");
                fs.is_argument = False;
            }
        }
        // var
        strcpy(expr_r, exprstr);
    }

    // int-const
    else if( is_token_(NUM_N) ){
        strcpy(expr_r, str);
    }

    // otherwise
    else{
        error();
    }

    return ;
}


static void exprcat(char *expr, char *l, char *o, char *r)
{
    // str <- o(l,r)
    snprintf(expr, MAXSTRLEN, "%s(%s, %s)", o, l, r);

    // initialize
    memset(l, '\0', MAXSTRLEN);
    memset(o, '\0', MAXSTRLEN);
    memset(r, '\0', MAXSTRLEN);

    return ;
}
