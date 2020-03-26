/* generate.c */
#include "functojump.h"


static FILE *fp;
static fpos_t *head;
static char outputfile[MAXSTRLEN];
static int depth;

static struct flags{
    int is_gen_indent;
} fs;

static char token_to_str[TOKEN_NUM][MAXSTRLEN] = {
    ""      , ""      , "if"    , "else"  , "while" ,
    "loop"  , "return", "goto"  , "int"   , ""      ,
    "+"     , "++"    , "-"     , "--"    , "--\'"  ,
    "*"     , "/"     , "%"     , "="     , "=="    ,
    "!="    , "<"     , "<="    , ">"     , ">="    ,
    "&&"    , "||"    , "!"     , "("     , ")"     ,
    "{"     , "}"     , ","     , ":"     , ";"
};

static void name_outputfile(char*);
static void generate_space_before(int);
static void generate_space_after(int);
static void generate_indent(void);



void initialize_generator(char *inputfile)
{
    name_outputfile(inputfile);

    if( (fp = fopen(outputfile, "w+")) == NULL ){
        printf("Output file cannot be generated.\n");
        exit(EXIT_FAILURE);
    }
    fgetpos(fp, head);

    depth = 0;
    fs.is_gen_indent = False;

    return ;
}


FILE *get_generate_fp(void)
{
    return fp;
}


fpos_t *get_generate_head(void)
{
    return head;
}


static void name_outputfile(char *inputfile)
{
    int i, j;
    char inputfiledelext[MAXSTRLEN];
    char extension[MAXSTRLEN];

    for( i = 0; inputfile[i] != '.'; i++ ){
        inputfiledelext[i] = inputfile[i];
    }
    inputfiledelext[i] = '\0';

    for( j = 0; inputfile[i] != '\0'; i++, j++ ){
        extension[j] = inputfile[i];
    }
    extension[j] = '\0';

    snprintf(outputfile, MAXSTRLEN, "%s_jump%s", inputfiledelext, extension);

    return ;
}


void generate(int token)
{
    switch( token ){
        case RBRACE_N :
            depth--;
            break;
    }

    generate_space_before(token);
    generate_indent();
    fprintf(fp, "%s", token_to_str[token]);

    switch( token ){
        case NAME_N :
        case NUM_N :
            fprintf(fp, "%s", str);
            break;
        case LBRACE_N :
            depth++;
        case RBRACE_N :
        case COLON_N :
        case SEMI_N :
            fprintf(fp, "\n");
            fs.is_gen_indent = True;
            break;
    }

    generate_space_after(token);

    return ;
}


static void generate_space_before(int token)
{
    switch( token ){
        case ASSIGN_N :
        case EQUAL_N :
        case NOTEQ_N :
        case LE_N :
        case LEEQ_N :
        case RE_N :
        case REEQ_N :
        case AND_N :
        case OR_N :
            fprintf(fp, " ");
            break;
    }

    return ;
}


static void generate_space_after(int token)
{
    switch( token ){
        case GOTO_N :
        case INT_N :
        case ASSIGN_N :
        case EQUAL_N :
        case LE_N :
        case LEEQ_N :
        case RE_N :
        case REEQ_N :
        case AND_N :
        case OR_N :
        case COMMA_N :
            fprintf(fp, " ");
            break;
    }

    return ;
}


static void generate_indent(void)
{
    int i;

    if( fs.is_gen_indent ){
        for( i = 0; i < depth; i++ ){
            fprintf(fp, "    ");
        }
        fs.is_gen_indent = False;
    }

    return ;
}


void generate_str(char *s)
{
    fprintf(fp, "%s", s);

    return ;
}


void generate_indent_str(char *s)
{
    generate_indent();
    fprintf(fp, "%s", s);

    return ;
}


void generate_arrlabel(char *label)
{
    generate_indent();
    generate_str(label);
    reference_label(label);
    generate(COLON_N);

    return ;
}


void generate_goto(char *label)
{
    generate(GOTO_N);
    generate_str(label);
    define_label_explicitly(label);
    generate(SEMI_N);

    return ;
}


void finalize_generator(void)
{
    fclose(fp);

    return ;
}
