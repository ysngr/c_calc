/* iscode_generate.c */
#include "univfunc.h"
#include "iscode.h"

char filename[MAXSTRLEN] = "prog.c";

static FILE *fp;

static int dectoprog(Integer);
static Integer leftright(Integer n);
static int halt(void);


int is_code(Integer n)
{
    if( dectoprog(n) == False ){
        return False;
    }

    return parse();
}


static int dectoprog(Integer n)
{
    int i;
    Integer varnum, fpnum, stat;
    Integer a, b;
    char indent[5] = {' ', ' ', ' ', ' '};

    if( (fp = fopen(filename, "w")) == NULL ){
        printf("File cannot be generated.\n");
        exit(EXIT_FAILURE);
    }

    // program name
    fprintf(fp, "prog(");

    // formal parameter
    fpnum = left(n);
    n = right(n);
    for( i = 1; i <= fpnum; i++ ){
        fprintf(fp, "v%d", i);
        if( i != fpnum ){
            fprintf(fp, ", ");
        }
    }
    fprintf(fp, ") {\n");

    // variable
    if( (varnum = left(n)) < fpnum ){
        return halt();
    }
    if( varnum > fpnum ){
        fprintf(fp, "%sint ", indent);
    }
    n = right(n);
    for( ; i <= varnum; i++ ){
        fprintf(fp, "v%d", i);
        if( i != varnum ){
            fprintf(fp, ", ");
        }
    }
    if( varnum > fpnum ){
        fprintf(fp, ";\n");
    }

    // calc main
    if( left(n) == 0 ){
        fprintf(fp, "%sL1: return(v1);\n}\n", indent);
        return True;
    }
    for( i = 1; n != 0; i++ ){
        fprintf(fp, "%sL%d: ", indent, i);
        stat = left(n);
        n = right(n);
        switch( left(stat) ){
            case 1 :  // < 1, a >
                if( length(right(stat)) != 1 ){
                    return halt();
                }
                a = left(right(stat));
                fprintf(fp, "goto L%lld;\n", a);
                break;
            case 2 :  // < 2, a, b >
                if( length(right(stat)) != 2 ){
                    return halt();
                }
                a = left(right(stat));
                b = leftright(right(stat));
                fprintf(fp, "v%lld = %lld;\n", a, b);
                break;
            case 3 :  // < 3, a, b >
                if( length(right(stat)) != 2 ){
                    return halt();
                }
                a = left(right(stat));
                b = leftright(right(stat));
                fprintf(fp, "v%lld = v%lld;\n", a, b);
                break;
            case 4 :  // < 4, a >
                if( length(right(stat)) != 1 ){
                    return halt();
                }
                a = left(right(stat));
                fprintf(fp, "v%lld++;\n", a);
                break;
            case 5 :  // < 5, a >
                if( length(right(stat)) != 1 ){
                    return halt();
                }
                a = left(right(stat));
                fprintf(fp, "v%lld--\';\n", a);
                break;
            case 6 :  // < 6, a, b >
                if( length(right(stat)) != 2 ){
                    return halt();
                }
                a = left(right(stat));
                b = leftright(right(stat));
                fprintf(fp, "if( v%lld > 0 ) goto L%lld;\n", a, b);
                break;
            default :
                return halt();
        }
    }
    fprintf(fp, "%sL%d: return(v1);\n}\n", indent, i);

    fclose(fp);

    return True;
}


static Integer leftright(Integer n)
{
    return left(right(n));
}


static int halt(void)
{
    fclose(fp);

    return False;
}
