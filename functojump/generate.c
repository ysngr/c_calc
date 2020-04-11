/* generate.c */
#include "functojump.h"


static FILE *fp;
static fpos_t head;
static char midfile[MAXSTRLEN];
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

static void name_midfile(char*);
static void generate_basic_funcs(void);
static void generate_space_before(int);
static void generate_space_after(int);
static void generate_indent(void);



void initialize_generator(char *inputfile)
{
    name_midfile(inputfile);

    if( (fp = fopen(midfile, "w+")) == NULL ){
        printf("Output file cannot be generated.\n");
        exit(EXIT_FAILURE);
    }

    fgetpos(fp, &head);
    generate_basic_funcs();

    depth = 0;
    fs.is_gen_indent = False;

    return ;
}


static void generate_basic_funcs(void)
{
    FILE *lfp;
    char buffer[MAXSTRLEN];

    if( (lfp = fopen("basicfuncs.c", "r")) == NULL ){
        printf("Lirary file cannot be opened.\n");
        exit(EXIT_FAILURE);
    }

    while( fgets(buffer, MAXSTRLEN, lfp) != NULL ){
        fprintf(fp, "%s", buffer);
    }

    return ;
}


FILE *get_generate_fp(void)
{
    return fp;
}


fpos_t get_generate_head(void)
{
    return head;
}


static void name_midfile(char *inputfile)
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

    snprintf(midfile, MAXSTRLEN, "%s_jump_mid%s", inputfiledelext, extension);

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
    increment_arrlabel_counter();

    return ;
}


void generate_if(char *var, char *label)
{
    generate(IF_N);
    generate(LPAREN_N);
    generate_str(var);
    generate(RE_N);
    generate_str("0");
    generate(RPAREN_N);
    generate_str("  ");
    generate_goto(label);

    return ;
}


void generate_assign(char *lv, char *rv)
{
    generate_indent_str(lv);
    generate(ASSIGN_N);
    generate_str(rv);
    generate(SEMI_N);
    increment_arrlabel_counter();

    return ;
}


void generate_assign_with_sign(char *lv, char *rv)
{
    int i;
    char signlv[MAXSTRLEN], signrv[MAXSTRLEN];

    generate_assign(lv, rv);

    snprintf(signlv, MAXSTRLEN, "sig_%s", lv);
    snprintf(signrv, MAXSTRLEN, "1");
    for( i = 0; rv[i] != '\0'; i++ ){
        if( isdigit(rv[i]) == False ){
            snprintf(signrv, MAXSTRLEN, "sig_%s", rv);
            break;
        }
    }
    generate_assign(signlv, signrv);

    return ;
}


void generate_incr(char *var)
{
    generate_indent_str(var);
    generate(INC_N);
    generate(SEMI_N);
    increment_arrlabel_counter();

    return ;
}


void generate_cdecr(char *var)
{
    generate_indent_str(var);
    generate(CDEC_N);
    generate(SEMI_N);
    increment_arrlabel_counter();

    return ;
}


void generate_expand_incr(char *var)
{
    int i;
    char signvar[MAXSTRLEN];
    char labels[6][MAXSTRLEN];

    snprintf(signvar, MAXSTRLEN, "sig_%s", var);
    for( i = 0; i < 6; i++ ){
        create_newlabel(labels[i]);
    }

    generate_if(signvar, labels[0]);
    generate_goto(labels[1]);
    generate_arrlabel(labels[0]);
    generate_incr(var);
    generate_goto(labels[2]);
    generate_arrlabel(labels[1]);
    generate_if(var, labels[3]);
    generate_goto(labels[4]);
    generate_arrlabel(labels[3]);
    generate_cdecr(var);
    generate_goto(labels[5]);
    generate_arrlabel(labels[4]);
    generate_assign_with_sign(var, "1");
    generate_arrlabel(labels[5]);
    generate_arrlabel(labels[2]);

    return ;
}


void generate_expand_decr(char *var)
{
    int i;
    char signvar[MAXSTRLEN];
    char labels[6][MAXSTRLEN];

    snprintf(signvar, MAXSTRLEN, "sig_%s", var);
    for( i = 0; i < 6; i++ ){
        create_newlabel(labels[i]);
    }

    generate_if(signvar, labels[0]);
    generate_goto(labels[1]);
    generate_arrlabel(labels[0]);
    generate_if(var, labels[2]);
    generate_goto(labels[3]);
    generate_arrlabel(labels[2]);
    generate_cdecr(var);
    generate_goto(labels[4]);
    generate_arrlabel(labels[3]);
    generate_assign(var, "1");
    generate_assign(signvar, "0");
    generate_arrlabel(labels[4]);
    generate_goto(labels[5]);
    generate_arrlabel(labels[1]);
    generate_incr(var);
    generate_arrlabel(labels[5]);

    return ;
}


void finalize_generator(void)
{
    fclose(fp);

    return ;
}


void get_filenames(char *midfbuf, char *outfbuf)
{
    int i, j;

    strcpy(midfbuf, midfile);

    for( i = 0, j = 0; midfile[j] != 0; i++, j++ ){  // kill "_mid"
        if( strncmp(midfile+j, "_mid", 4) == 0 ){
            i--;
            j += 3;
        }else{
            outfbuf[i] = midfile[j];
        }
    }
    outfbuf[i] = '\0';

    return ;
}
