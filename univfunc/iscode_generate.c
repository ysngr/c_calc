/* iscode_generate.c */
#include "univfunc.h"
#include "iscode.h"

char filename[MAXSTRLEN] = "prog.c";

static FILE *fp;

static int dectoprog(int);
static int halt(void);


int is_code(int n)
{
    if( dectoprog(n) == False ){
        return False;
    }

    return parse();
}


static int dectoprog(int n)
{
    int i;
    int varnum, fpnum, stat;
    int a, b;
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
    fprintf(fp, ") {\n%s", indent);

    // variable
    varnum = left(n);
    n = right(n);
    fprintf(fp, "int ");
    for( ; i <= varnum; i++ ){
        fprintf(fp, "v%d", i);
        if( i != varnum ){
            fprintf(fp, ", ");
        }
    }
    fprintf(fp, ";\n");

    // calc main
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
                fprintf(fp, "goto L%d;\n", a);
                break;
            case 2 :  // < 2, a, b >
                if( length(right(stat)) != 2 ){
                    return halt();
                }
                a = left(right(stat));
                b = right(right(stat));
                fprintf(fp, "v%d = %d;\n", a, b);
                break;
            case 3 :  // < 3, a, b >
                if( length(right(stat)) != 2 ){
                    return halt();
                }
                a = left(right(stat));
                b = right(right(stat));
                fprintf(fp, "v%d = v%d;\n", a, b);
                break;
            case 4 :  // < 4, a >
                if( length(right(stat)) != 1 ){
                    return halt();
                }
                a = left(right(stat));
                fprintf(fp, "v%d++;\n", a);
                break;
            case 5 :  // < 5, a >
                if( length(right(stat)) != 1 ){
                    return halt();
                }
                a = left(right(stat));
                fprintf(fp, "v%d--\';\n", a);
                break;
            case 6 :  // < 6, a, b >
                if( length(right(stat)) != 2 ){
                    return halt();
                }
                a = left(right(stat));
                b = right(right(stat));
                fprintf(fp, "if( v%d > 0 ) goto L%d;\n", a, b);
                break;
            default :
                return halt();
        }
    }
    fprintf(fp, "%sL%d: return(v1);\n}\n", indent, i);

    fclose(fp);

    return True;
}


static int halt(void)
{
    fclose(fp);

    return False;
}
