/* generate.c */
#include "convtojump.h"


static FILE *fp;
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

static void separate_extension(char*, char*, char*);
static void generate_space_before(int);
static void generate_space_after(int);
static void generate_indent(void);



void initialize_generator(char *inputfile)
{
    char inputfiledelext[MAXSTRLEN];
    char extension[MAXSTRLEN];
    char outputfile[MAXSTRLEN];

    separate_extension(inputfile, inputfiledelext, extension);
    snprintf(outputfile, MAXSTRLEN, "%s_jump%s", inputfiledelext, extension);

    if( (fp = fopen(outputfile, "w")) == NULL ){
        printf("Output file cannot be generated.\n");
        exit(EXIT_FAILURE);
    }

    depth = 0;
    fs.is_gen_indent = False;

    return ;
}


static void separate_extension(char *file, char *name, char *ext)
{
    int i, j;

    for( i = 0; file[i] != '.'; i++ ){
        name[i] = file[i];
    }
    name[i] = '\0';

    for( j = 0; file[i] != '\0'; i++, j++ ){
        ext[j] = file[i];
    }
    ext[j] = '\0';

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


void generate_ln_indent(void)
{
    fprintf(fp, "\n");
    depth++;
    fs.is_gen_indent = True;
    // generate_indent();

    return ;
}


void generate_nl_outdent(void)
{
    if( ! fs.is_gen_indent ){
        fprintf(fp, "\n");
    }
    depth--;
    fs.is_gen_indent = True;

    return ;
}


void generate_expr(char *expr)
{
    fprintf(fp, "%s", expr);

    return ;
}


void finalize_generator(void)
{
    fclose(fp);

    return ;
}
