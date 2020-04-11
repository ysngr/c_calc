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
