/* register.c */
#include "jumpexec.h"

static struct varlist{
    int val;
    struct varlist *nextvar;
} *vs;

static struct statlist{
    int stype;
    int a;
    int b;
    struct statlist *nextstat;
} *ss;



void register_variable(int varnum, int is_parameter)
{
    int i;
    struct varlist *nv, *vp

    for( i = 0; i < varnum; i++ ){
        if( (nv = (struct varlist*)malloc(sizeof(struct varlist))) == NULL ){
            printf("Memory cannot be secured.\n");
            exit(EXIT_FAILURE);
        }
        nv->val = 0;
        nv->nextvar = NULL;

        if( i == 0 && is_parameter ){
            vs = nv;
        }else{
            for( vp = vs; vp->nextvar != NULL; vp = vp->nextvar );
            vp->nextvar = nv;
        }
    }

    return ;
}


void register_statement(int st, int a, int b)
{
    struct statlist *ns, *sp;

    if( (ns = (struct statlist*)malloc(sizeof(struct statlist))) == NULL ){
        printf("Memory cannot be secured.\n");
        exit(EXIT_FAILURE);
    }
    ns->stype = st;
    ns->a = a;
    ns->b = b;
    ns->nextstat = NULL;

    if( ss == NULL ){
        ss = ns;
    }else{
        for( sp = ss; sp->nextstat != NULL; sp = sp->nextstat );
        sp->nextstat = ns;
    }

    return ;
}


void register_paramvalue(int *vs, int pnum)
{
    int i;
    struct varlist *vp;

    for( vp = vs, i = 1; i < pnum; vp = vp->nextvar, i++ ){
        vp->val = vs[i-1];
    }

    return ;
}


void update_variable(int varidx, int value)
{
    int i;
    struct statlist *sp;

    for( sp = ss, i = 1; i < varidx; sp = sp->nextvar, i++ );
    sp->val = value;

    return ;
}



void print_statlist(void)
{
    int counter;
    struct statlist *sp;

    counter = 1;
    for( sp = ss; sp != NULL; sp = sp->nextstat ){
        printf("%03d: [%d] ", counter++, sp->stype);
        switch( sp->stype ){
            case GOTO_STAT :
                printf("goto L%d;\n", sp->a);
                break;
            case NAT_ASSIGN_STAT :
                printf("v%d = %d;\n", sp->a, sp->b);
                break;
            case VAR_ASSIGN_STAT :
                printf("v%d = v%d;\n", sp->a, sp->b);
                break;
            case INCR_STAT :
                printf("v%d++;\n", sp->a);
                break;
            case CDECR_STAT :
                printf("v%d--\';\n", sp->a);
                break;
            case IF_GOTO_STAT :
                printf("if(v%d > 0) goto L%d;\n", sp->a, sp->b);
                break;
            case RETURN_STAT :
                printf("return(v1);\n");
                break;
        }
    }

    return ;
}
