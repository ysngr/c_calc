/* parse.c */
#include "functojump.h"


static int token;
static char expr_r[MAXSTRLEN];
static char cond_expr[MAXSTRLEN];

static struct buffer{
    int nexttoken;
    int nextnum;
    char nextstr[MAXSTRLEN];
} tbuf;

static struct flags{
    int is_var_def;
    int is_formal_parameter;
    int is_token_buf_exist;
    int is_argument;
} fs;

static struct counter{
    int v;
    int l;
} cnt;


static void get_token(void);
static void get_token_from_buf(void);
static int is_token_(int);
static void is_token_or_err(int);
static int generate_is_token_(int);
static void generate_is_token_or_err(int);
static void error(void);

static void initialize_parse(void);
static void initialize_flag(void);
static void initialize_counter(void);

static void formal_parameters(void);
static void variable_names(void);
static void variable_declaration(void);
static void calc_main(void);

static void statements(void);
static int is_label(void);
static int is_statement(void);
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
static int relational_operator(void);
static void numerical_expression(void);
static int is_additive_operator(char*);
static void simple_numerical_expression(void);
static int is_multiplicative_operator(char*);
static void numerical_term(void);
static void atom_numerical_expression(void);

static void create_newvariable(char*);



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
    if( cmptoken == LABEL_N ){
        if( token != NAME_N ){
            return False;
        }
    }else{
        if( token != cmptoken ){
            return False;
        }
    }

    if( cmptoken == NAME_N ){
        // variable, function
        if( fs.is_var_def ){
            define_variable(fs.is_formal_parameter);
        }else{
            reference_variable();
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


static int generate_is_token_(int cmptoken)
{
    int prevtoken;
    char prevstr[MAXSTRLEN];

    prevtoken = token;
    strcpy(prevstr, str);

    if( is_token_(cmptoken) == False ){
        return False;
    }

    if( prevtoken == NAME_N || prevtoken == NUM_N ){
        generate_str(prevstr);
    }else{
        generate(prevtoken);
    }

    return True;
}


static void generate_is_token_or_err(int cmptoken)
{
    if( generate_is_token_(cmptoken) == False ){
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
    fs.is_formal_parameter = False;
    fs.is_token_buf_exist = False;
    fs.is_argument = False;

    return ;
}


static void initialize_counter(void)
{
    cnt.v = 0;
    cnt.l = 0;

    return ;
}


void parse(void)
{
    initialize_parse();

    do{
        initialize_flag();
        initialize_counter();
        initialize_fprog_list();

        // func-name formal-params '{' var-decl calc-main '}'
        generate_is_token_or_err(NAME_N);
        formal_parameters();
        generate_is_token_or_err(LBRACE_N);
        variable_declaration();
        calc_main();
        generate_is_token_or_err(RBRACE_N);

        check_label_link();
    }while( is_token_(END_OF_FILE) == False );

    // print_list();  // for debug

    return ;
}


static void formal_parameters(void)
{
    // '(' var-names ')'
    generate_is_token_or_err(LPAREN_N);
    fs.is_formal_parameter = True;
    variable_names();
    fs.is_formal_parameter = False;
    generate_is_token_or_err(RPAREN_N);

    return ;
}


static void variable_names(void)
{
    // var { ',' var }
    if( generate_is_token_(NAME_N) ){
        while( generate_is_token_(COMMA_N) ){
            generate_is_token_or_err(NAME_N);
        }
    }

    return ;
}


static void variable_declaration(void)
{
    // 'int' var-names ';'
    if( generate_is_token_(INT_N) ){
        variable_names();
        generate_is_token_or_err(SEMI_N);
    }

    return ;
}


static void calc_main(void)
{
    char buffer[MAXSTRLEN];

    fs.is_var_def = False;
    statements();

    get_fstvar(buffer);
    generate_arrlabel("_L");
    generate_assign(buffer, "_r");
    generate(RETURN_N);
    generate(LPAREN_N);
    generate_str(buffer);
    generate(RPAREN_N);
    generate(SEMI_N);

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
        reference_label(prevstr);
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


static int is_val_update_statement(void)
{
    char varname[MAXSTRLEN];

    // var
    strcpy(varname, str);
    if( is_token_(NAME_N) == False ){
        return False;
    }

    // assignment statement : '=' num-expr
    if( is_token_(ASSIGN_N) ){
        numerical_expression();
        generate_assign_with_sign(varname, expr_r);
    }
    // increment statement : '++'
    else if( is_token_(INC_N) ){
        generate_expand_incr(varname);
    }
    // decrement statement : '--'
    else if ( is_token_(DEC_N) ){
        generate_expand_decr(varname);
    }
    // conditional decrement statement : '--''
    else if( is_token_(CDEC_N) ){
        generate_cdecr(varname);
    }
    // otherwise
    else{
        error();
    }

    // ';'
    is_token_or_err(SEMI_N);

    return True;
}


static int is_if_statement(void)
{
    char cond[MAXSTRLEN];
    char thenlabel[MAXSTRLEN], elselabel[MAXSTRLEN];

    // 'if'
    if( is_token_(IF_N) == False ){
        return False;
    }

    // '(' cond-expr ')'
    is_token_or_err(LPAREN_N);
    conditional_expression();
    is_token_or_err(RPAREN_N);
    create_newlabel(thenlabel);
    snprintf(cond, MAXSTRLEN, "!(%s)", cond_expr);
    flatten(cond, thenlabel);

    // '{' stats '}' | stat
    if( is_token_(LBRACE_N) ){
        statements();
        is_token_or_err(RBRACE_N);
    }else{
        is_statement();
    }

    // 'else'
    if( is_token_(ELSE_N) ){
        create_newlabel(elselabel);
        generate_goto(elselabel);
        generate_arrlabel(thenlabel);
        // '{', stats, '}' | stat
        if( is_token_(LBRACE_N) ){
            statements();
            is_token_or_err(RBRACE_N);
        }else{
            is_statement();
        }
        generate_arrlabel(elselabel);
    }else{  // case : else does not exist (only if)
        generate_arrlabel(thenlabel);
    }

    return True;
}


static int is_while_statement(void)
{
    char looplabel[MAXSTRLEN], condlabel[MAXSTRLEN];
    char cond[MAXSTRLEN];

    // 'while'
    if( is_token_(WHILE_N) == False ){
        return False;
    }
    create_newlabel(looplabel);
    create_newlabel(condlabel);
    generate_arrlabel(looplabel);

    // '(' cond-expr ')'
    is_token_or_err(LPAREN_N);
    conditional_expression();
    is_token_or_err(RPAREN_N);
    snprintf(cond, MAXSTRLEN, "!(%s)", cond_expr);
    flatten(cond, condlabel);

    // '{' stats '}' | stat
    if( is_token_(LBRACE_N) ){
        statements();
        is_token_or_err(RBRACE_N);
    }else{
        is_statement();
    }
    generate_goto(looplabel);
    generate_arrlabel(condlabel);

    return True;
}


static int is_loop_statement(void)
{
    char var[MAXSTRLEN];
    char looplabel[MAXSTRLEN], condlabel[MAXSTRLEN];
    char cond[MAXSTRLEN];

    // 'loop'
    if( is_token_(LOOP_N) == False ){
        return False;
    }

    // '(' num-expr ')'
    is_token_or_err(LPAREN_N);
    create_newvariable(var);
    numerical_expression();
    generate_assign_with_sign(var, expr_r);

    create_newlabel(looplabel);
    create_newlabel(condlabel);
    generate_arrlabel(looplabel);
    is_token_or_err(RPAREN_N);
    snprintf(cond, MAXSTRLEN, "!((%s > 0) && (sig_%s > 0))", var, var);
    flatten(cond, condlabel);

    // '{' stats '}' | stat
    if( is_token_(LBRACE_N) ){
        statements();
        is_token_or_err(RBRACE_N);
    }else{
        is_statement();
    }
    generate_expand_decr(var);
    generate_goto(looplabel);
    generate_arrlabel(condlabel);

    return True;
}


static int is_return_statement(void)
{
    char var[MAXSTRLEN] = "_r";
    char label[MAXSTRLEN] = "_L";

    // 'return'
    if( is_token_(RETURN_N) == False ){
        return False;
    }

    // '(' num-expr ')' ';'
    is_token_or_err(LPAREN_N);
    numerical_expression();
    is_token_or_err(RPAREN_N);
    is_token_or_err(SEMI_N);
    define_variable_explicitly(var);
    generate_assign_with_sign(var, expr_r);
    generate_goto(label);

    return True;
}


static int is_goto_statement(void)
{
    // 'goto'
    if( is_token_(GOTO_N) == False ){
        return False;
    }

    // label ';'
    generate_goto(str);
    is_token_or_err(LABEL_N);
    is_token_or_err(SEMI_N);

    return True;
}


static void conditional_expression(void)
{
    int is_multiple_simple_cond_expr = False;
    char expr[MAXSTRLEN];

    // simple-cond-expr { '||' simple-cond_expr }
    simple_conditional_expression();
    strcpy(expr, cond_expr);
    while( is_token_(OR_N) ){
        is_multiple_simple_cond_expr = True;
        strcat(expr, ") || (");
        simple_conditional_expression();
        strcat(expr, cond_expr);
    }
    if( is_multiple_simple_cond_expr ){
        snprintf(cond_expr, MAXSTRLEN, "(%s)", expr);
    }else{
        strcpy(cond_expr, expr);
    }

    return ;
}


static void simple_conditional_expression(void)
{
    int is_muptiple_cond_expr = False;
    char expr[MAXSTRLEN];

    // cond-term { '&&' cond-term }
    conditional_term();
    strcpy(expr, cond_expr);
    while( is_token_(AND_N) ){
        is_muptiple_cond_expr = True;
        strcat(expr, ") && (");
        conditional_term();
        strcat(expr, cond_expr);
    }
    if( is_muptiple_cond_expr ){
        snprintf(cond_expr, MAXSTRLEN, "(%s)", expr);
    }else{
        strcpy(cond_expr, expr);
    }

    return ;
}


static void conditional_term(void)
{
    char expr[MAXSTRLEN];

    // '(' cond-expr ')'
    if( is_token_(LPAREN_N) ){
        conditional_expression();
        is_token_or_err(RPAREN_N);
    }
    // '!' cond-term
    else if( is_token_(NOT_N) ){
        conditional_term();
        snprintf(expr, MAXSTRLEN, "!(%s)", cond_expr);
        strcpy(cond_expr, expr);
    }
    // atom-cond-expr
    else{
        atom_conditional_expression();
    }

    return ;
}


static void atom_conditional_expression(void)
{
    int ope;
    char var1[MAXSTRLEN], var2[MAXSTRLEN];
    char expr1[MAXSTRLEN], expr2[MAXSTRLEN];
    char subretvar[MAXSTRLEN];

    // num-expr rel-ope num-expr { rel-ope num-expr }
    numerical_expression();
    strcpy(expr1, expr_r);
    ope = relational_operator();
    numerical_expression();
    strcpy(expr2, expr_r);

    switch( ope ){
        case EQUAL_N :
        case NOTEQ_N :
            create_newvariable(var1);
            create_newvariable(var2);

            expand_binope("_sub", expr1, expr2, subretvar);
            generate_assign_with_sign(var1, subretvar);
            expand_binope("_sub", expr2, expr1, subretvar);
            generate_assign_with_sign(var2, subretvar);

            if( ope == EQUAL_N ){
                snprintf(cond_expr, MAXSTRLEN, "((!(%s > 0)) || (!(sig_%s > 0))) && ((!(%s > 0)) || (!(sig_%s > 0)))", var1, var1, var2, var2);
            }else{
                snprintf(cond_expr, MAXSTRLEN, "((%s > 0) && (sig_%s > 0)) || ((%s > 0) && (sig_%s > 0))", var1, var1, var2, var2);
            }
            break;
        case LE_N :
        case LEEQ_N :
        case RE_N :
        case REEQ_N :
            create_newvariable(var1);

            if( ope == RE_N || ope == REEQ_N ){
                expand_binope("_sub", expr1, expr2, subretvar);
            }else{  // ope == LE_N || ope == LEEQ_N
                expand_binope("_sub", expr2, expr1, subretvar);
            }
            generate_assign_with_sign(var1, subretvar);

            if( ope == LEEQ_N || ope == REEQ_N ){
                generate_expand_incr(var1);
            }
            snprintf(cond_expr, MAXSTRLEN, "(%s > 0) && (sig_%s > 0)", var1, var1);
            break;
    }

    return ;
}


static int relational_operator(void)
{
    // '=='
    if( is_token_(EQUAL_N) ){
        return EQUAL_N;
    }
    // '!='
    else if( is_token_(NOTEQ_N) ){
        return NOTEQ_N;
    }
    // '<'
    else if( is_token_(LE_N) ){
        return LE_N;
    }
    // '<='
    else if( is_token_(LEEQ_N) ){
        return LEEQ_N;
    }
    // '>'
    else if( is_token_(RE_N) ){
        return RE_N;
    }
    // '>='
    else if( is_token_(REEQ_N) ){
        return REEQ_N;
    }
    // otherwise
    else{
        error();
    }

    return True;  // never reached
}


static void numerical_expression(void)
{
    int is_minus_exist;
    char expr_l[MAXSTRLEN], expr_o[MAXSTRLEN];

    // ['-'] simple-num-expr { add-ope simple-num-ope }
    if( is_token_(MINUS_N) ){
        is_minus_exist = True;
    }else{
        is_minus_exist = False;
    }

    simple_numerical_expression();
    if( is_minus_exist ){
        expand_binope("_sub", "0", expr_r, expr_l);
    }else{
        strcpy(expr_l, expr_r);
    }

    while( is_additive_operator(expr_o) ){
        simple_numerical_expression();
        expand_binope(expr_o, expr_l, expr_r, expr_l);
    }

    strcpy(expr_r, expr_l);

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
    char expr_l[MAXSTRLEN], expr_o[MAXSTRLEN];

    // num-term { mul-ope num-term }
    numerical_term();
    strcpy(expr_l, expr_r);

    while( is_multiplicative_operator(expr_o) ){
        numerical_term();
        expand_binope(expr_o, expr_l, expr_r, expr_l);
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
    // '(' num-expr ')'
    if( is_token_(LPAREN_N) ){
        numerical_expression();
        is_token_or_err(RPAREN_N);
    }
    // atom-num-expr
    else{
        atom_numerical_expression();
    }

    return ;
}


static void atom_numerical_expression(void)
{
    char funcname[MAXSTRLEN];
    char exprstr[MAXSTRLEN];
    char var[MAXSTRLEN];
    int is_func_in_arg;
    struct arglist *pas;
    char retvar[MAXSTRLEN];

    // var | func
    if( is_token_(NAME_N) ){
        strcpy(exprstr, str);
        strcpy(funcname, str);
        // funcname ('()' | '(' num-exprs, ')')
        if( is_token_(LPAREN_N) ){  // '('
            pas = initialize_arglist();
            is_func_in_arg = ( fs.is_argument )? True : False;
            strcat(exprstr, "(");
            // funcname()
            if( is_token_(RPAREN_N) ){  // ')'
                strcat(exprstr, ")");
            }
            // funcname(fp, ... ,fp)
            else{
                fs.is_argument = True;
                numerical_expression();
                register_arg(expr_r);
                strcat(exprstr, expr_r);
                while( is_token_(COMMA_N) ){  // ','
                    strcat(exprstr, ", ");
                    numerical_expression();
                    register_arg(expr_r);
                    strcat(exprstr, expr_r);
                }
                is_token_or_err(RPAREN_N);  // ')'
                strcat(exprstr, ")");
                if( ! is_func_in_arg ){
                    fs.is_argument = False;
                }
            }
            expand(funcname, retvar);
            strcpy(exprstr, retvar);
            if( fs.is_argument ){
                create_newvariable(var);
                generate_assign_with_sign(var, exprstr);
                strcpy(expr_r, var);
            }else{
                strcpy(expr_r, exprstr);
            }
            finalize_arglist(pas);
        }
        // var
        else{
            strcpy(expr_r, exprstr);
        }
        return ;
    }

    // int-const
    if( is_token_(NUM_N) ){
        strcpy(expr_r, str);
    }

    // otherwise
    else{
        error();
    }

    return ;
}


static void create_newvariable(char *var)
{
    snprintf(var, MAXSTRLEN, "_v%d", cnt.v++);
    define_variable_explicitly(var);

    return ;
}


void create_newlabel(char *label)
{
    snprintf(label, MAXSTRLEN, "_L%d", cnt.l++);

    return ;
}
