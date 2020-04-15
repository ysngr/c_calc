/* replace.c */
#include "functojump.h"
#include "scan.h"


static FILE *rfp, *wfp;

static char c;
static char restr[MAXSTRLEN];
static int renum;

static int genidx;
static char genbuf[MAXSTRLEN];
static char ngencbuf;

static int fpnum;
static int signbase;
static int labelcounter;

static struct flags{
    int is_generate_arrlabel;
} fs;


static void initialize_replace(void);
static void initialize_rescan(void);
static void reset_genbuf(void);
static void rescanc(void);
static int rescan(void);
static void regenerate(void);
static void regenerate_repname(void);
static void finalize_replace(void);



static void initialize_replace(void)
{
    char midfile[MAXSTRLEN], outputfile[MAXSTRLEN];

    fpnum = get_fpnum();
    signbase = register_repvaridx();
    get_filenames(midfile, outputfile);

    if( (rfp = fopen(midfile, "r")) == NULL ){
        printf("File cannot be opened.\n");
        exit(EXIT_FAILURE);
    }
    if( (wfp = fopen(outputfile, "w")) == NULL ){
        printf("Output file cannot be generated.\n");
        exit(EXIT_FAILURE);
    }

    initialize_rescan();

    return ;
}


static void initialize_rescan(void)
{
    char mainfuncname[MAXSTRLEN];

    get_mainfuncname(mainfuncname);
    reset_genbuf();
    ngencbuf = '\0';
    fs.is_generate_arrlabel = False;

    rescanc();
    while( rescan() != NAME_N || strcmp(restr,mainfuncname) != 0 ){
        reset_genbuf();
    }

    regenerate();
    regenerate_repname();

    labelcounter = 1;

    return ;
}


static void reset_genbuf(void)
{
    memset(genbuf, '\0', MAXSTRLEN);
    genidx = 0;

    return ;
}


void replace(void)
{
    int i;

    initialize_replace();

    while( True ){
        switch( rescan() ){
            case NAME_N :
                regenerate();
                regenerate_repname();
                break;
            case LBRACE_N :
                if( rescan() != INT_N ){
                    strcat(genbuf+genidx-1, "int ");
                    genidx += 5;
                    genbuf[genidx] = '\0';
                }
            case INT_N :
                regenerate();
                for( i = fpnum+1; i <= 2*signbase; i++ ){
                    fprintf(wfp, "v%d", i);
                    if( i != 2 * signbase ){
                        fprintf(wfp, ", ");
                    }else{
                        fprintf(wfp, ";");
                    }
                }
                while( rescan() != SEMI_N ){
                    reset_genbuf();
                }
                for( i = signbase; i <= 2*signbase; i++ ){
                    fprintf(wfp, "\n    L%d: v%d = 1;", labelcounter++, i);
                }
                break;
            case LABEL_N :
                for( i = strlen(genbuf)-1; genbuf[i] != '\n'; i-- );
                genbuf[i] = '\0';
                labelcounter--;
                fs.is_generate_arrlabel = True;
                regenerate();
                break;
            case RETURN_N :
                regenerate();
                fprintf(wfp, "v1);\n}\n");
                goto EXIT;
        }
    }
    EXIT:

    finalize_replace();

    return ;
}


static void rescanc(void)
{
    c = fgetc(rfp);
    genbuf[genidx++] = c;

    return ;
}


static int rescan(void)
{
    int i;
    int token;
    char cbuf;
    char labelbuf[MAXREPNAMELEN];

    while( is_invalid_char(c) ){
        rescanc();
    }

    // paste arrlabel
    if( fs.is_generate_arrlabel ){
        cbuf = genbuf[genidx-1];
        genbuf[genidx-1] = '\0';
        snprintf(labelbuf, MAXREPNAMELEN, "L%d: ", labelcounter++);
        strcat(genbuf, labelbuf);
        genidx += strlen(labelbuf);
        genbuf[genidx-1] = cbuf;
        fs.is_generate_arrlabel = False;
    }

    // end of file
    if( c == EOF ){
        token = END_OF_FILE;
    }

    // Name, Keyword
    else if( isalpha(c) || c == '_' ){
        for( i = 0; isalnum(c) || c == '_' || c == ':'; i++ ){
            restr[i] = c;
            rescanc();
        }
        restr[i] = '\0';
        if( (token = str_to_tokennum(restr)) == NAME_N ){
            ngencbuf = genbuf[genidx-1];
            genbuf[genidx-i-1] = '\0';
            genidx -= i + 1;
            if( restr[i-1] == ':' ){
                token = LABEL_N;
            }
        }
    }

    // Number
    else if( isdigit(c) ){
        renum = 0;
        for( i = 0; isdigit(c); i++ ){
            renum = 10 * renum + (c - '0');
            snprintf(restr+i, 2, "%c", c);
            rescanc();
        }
        token = NUM_N;
    }

    // Symbol
    else{
        switch( c ){
            case '+' :
                rescanc();
                token = ( c == '+' )? INC_N : UNKNOWN;
                break;
            case '-' :
                rescanc();
                if( c == '-' ){
                    rescanc();
                    token = ( c == '\'' )? CDEC_N : UNKNOWN;
                    break;
                }else{
                    token = UNKNOWN;
                }
                break;
            case '=' : token = ASSIGN_N; break;
            case '>' : token = RE_N; break;
            case '(' : token = LPAREN_N; break;
            case ')' : token = RPAREN_N; break;
            case '{' : token = LBRACE_N; break;
            case '}' : token = RBRACE_N; break;
            case ';' :
                token = SEMI_N;
                fs.is_generate_arrlabel = True;
                break;
            default : token = UNKNOWN;
        }
        rescanc();
    }

    return token;
}


static void regenerate(void)
{
    fprintf(wfp, "%s", genbuf);
    reset_genbuf();
    if( ngencbuf != '\0' ){
        genbuf[genidx++] = ngencbuf;
        ngencbuf = '\0';
    }

    return ;
}


static void regenerate_repname(void)
{
    int is_signvar;
    char varname[MAXSTRLEN];
    int idx;

    if( strncmp(restr, "sig_", 4) == 0 ){
        is_signvar = True;
        strcpy(varname, restr+4);
    }else{
        is_signvar = False;
        strcpy(varname, restr);
    }

    // variable
    if( (idx = get_repvaridx(varname)) != False ){
        idx += ( is_signvar )? signbase : 0;
        fprintf(wfp, "v%d", idx);
    }
    // label
    else if( (idx = get_replabelidx(varname)) != False ){
        fprintf(wfp, "L%d", idx);
    }
    // otherwise
    else{
        fprintf(wfp, "%s", restr);
    }

    return ;
}


static void finalize_replace(void)
{
    genbuf[genidx-1] = '\0';  // kill EOF
    regenerate();  // flush buffer

    fclose(rfp);
    fclose(wfp);

    return ;
}
