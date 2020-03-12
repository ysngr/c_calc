/* register.c */
#include "funcprog.h"

static struct varlist{
    char *varname;
    struct varlist *nextvar;
} *v;

static struct fproglist{
    char *fprogname;
    struct varlist *vars;
    struct fproglist *nextfprog;
} *fs, *f;


static void *Malloc(int);
static int is_funcprog_declared(void);
static int is_variable_declared(void);


static void *Malloc(int size)
{
    void *p;

    if( (p = malloc(size)) == NULL ){
        printf("Memory cannot be secured.\n");
        exit(EXIT_FAILURE);
    }

    return p;
}


void init_register(void)
{
    fs = NULL;
    f = NULL;
    v = NULL;

    return ;
}


void init_fprog_list(void)
{
    struct fproglist *nf;

    // generate
    nf = (struct fproglist*)Malloc(sizeof(struct fproglist));
    nf->fprogname = NULL;
    nf->vars = NULL;
    nf->nextfprog = NULL;

    // connect
    if( fs == NULL ){  // first function program
        f = nf;
        fs = nf;
    }else{  // otherwise
        f->nextfprog = nf;
        f = nf;
    }

    return ;
}


void define_variable(void)
{
    int len_str = strlen(str) + 1;
    struct varlist *nv;

    // function program name
    if( f->fprogname == NULL ){
        if( is_funcprog_declared() ){
            printf("\nFunction program \"%s\" is duplicated.\n", str);
            exit(EXIT_FAILURE);
        }

        f->fprogname = (char*)Malloc(sizeof(char)*len_str);
        strncpy(f->fprogname, str, len_str);
    }

    // variable name
    else{
        if( is_variable_declared() ){
            printf("\nVariable \"%s\" is duplicated.\n", str);
            exit(EXIT_FAILURE);
        }

        // generate new node
        nv = (struct varlist*)Malloc(sizeof(struct varlist));
        nv->varname = (char*)Malloc(sizeof(char)*len_str);
        strncpy(nv->varname, str, len_str);
        nv->nextvar = NULL;

        // connect
        if( f->vars == NULL ){  // first variable
            f->vars = nv;
        }else{  // otherwise
            v->nextvar = nv;
        }
        v = nv;
    }

    return ;
}


void reference_variable(void)
{
    if( is_variable_declared() == False ){
        printf("\nVariable \"%s\" is not declared.\n", str);
        exit(EXIT_FAILURE);
    }

    return ;
}


static int is_funcprog_declared(void)
{
    struct fproglist *p;

    // first function program
    if( fs->fprogname == NULL ){
        return False;
    }

    // otherwise
    for( p = fs; p != NULL; p = p->nextfprog ){
        if( p->fprogname != NULL && strcmp(p->fprogname, str) == 0 ){
            return True;
        }
    }

    return False;
}


static int is_variable_declared(void)
{
    struct varlist *p;
    struct fproglist *fp;

    // variable name
    for( p = f->vars; p != NULL; p = p->nextvar ){
        if( strcmp(p->varname, str) == 0 ){
            return True;
        }
    }

    // function program name
    for( fp = fs; fp != NULL; fp = fp->nextfprog ){
        if( strcmp(fp->fprogname, str) == 0 ){
            return True;
        }
    }

    return False;
}
