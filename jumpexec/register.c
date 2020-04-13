/* register.c */
#include "jumpexec.h"

static int statnum;

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



void initialize_register(void)
{
    statnum = 0;
    vs = NULL;
    ss = NULL;

    return ;
}


void register_variable(int varnum)
{
    int i;
    struct varlist *nv, *vp;

    for( i = 0; i < varnum; i++ ){
        if( (nv = (struct varlist*)malloc(sizeof(struct varlist))) == NULL ){
            printf("Memory cannot be secured.\n");
            exit(EXIT_FAILURE);
        }
        nv->val = 0;
        nv->nextvar = NULL;

        if( vs == NULL ){
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

    statnum++;

    return ;
}


void check_label_link(void)
{
    struct statlist *sp;

    for( sp = ss; sp != NULL; sp = sp->nextstat ){
        switch( sp->stype ){
            case GOTO_STAT :
                if( sp->a > statnum ){
                    printf("Invalid label:L%d.\n", sp->a);
                    exit(EXIT_FAILURE);
                }
            case IF_GOTO_STAT :
                if( sp->b > statnum ){
                    printf("Invalid label:L%d.\n", sp->b);
                    exit(EXIT_FAILURE);
                }
        }
    }

    return ;
}


void set_variable(int varidx, int value)
{
    int i;
    struct varlist *vp;

    for( vp = vs, i = 1; i < varidx; vp = vp->nextvar, i++ );
    vp->val = value;

    return ;
}


int get_variable(int varidx)
{
    int i;
    struct varlist *vp;

    for( vp = vs, i = 1; i < varidx; vp = vp->nextvar, i++ );

    return vp->val;
}


void get_statement(int pc, int *reg)
{
    int i;
    struct statlist *sp;

    for( sp = ss, i = 1; i < pc; sp = sp->nextstat, i++ );
    reg[STATELEM_STYPE] = sp->stype;
    reg[STATELEM_A] = sp->a;
    reg[STATELEM_B] = sp->b;

    return ;
}


void finalize_register(void)
{
    struct varlist *vp;
    struct statlist *sp;
    void *rm;

    vp = vs;
    while( vp != NULL ){
        rm = vp;
        vp = vp->nextvar;
        free(rm);
    }

    sp = ss;
    while( sp != NULL ){
        rm = sp;
        sp = sp->nextstat;
        free(rm);
    }

    return ;
}



void print_variables(void)  // debug function
{
    int varidx;
    struct varlist *vp;

    varidx = 1;
    for( vp = vs; vp != NULL; vp = vp->nextvar ){
        printf("v%d = %d", varidx++, vp->val);
        if( vp->nextvar != NULL ){
            printf(", ");
        }
    }
    printf("\n");

    return ;
}
