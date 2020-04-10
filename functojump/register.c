/* register.c */
#include "functojump.h"

#define MAXREPNAMELEN 10

static struct varlist{
    char *varname;
    int repvaridx;
    int is_formal_parameter;
    struct varlist *nextvar;
} *v;

static struct labellist{
    char *labelname;
    char replabelname[MAXREPNAMELEN];///TODO
    int is_used_for_dep;
    int is_used_for_arr;
    struct labellist *nextlabel;
} *l;

static struct fproglist{
    char *fprogname;
    struct varlist *vars;
    struct labellist *labels;
    struct fproglist *nextfprog;
} *fs, *f;

static struct arglist *as;


static int is_funcprog_declared(void);
static int is_variable_declared(void);
static struct labellist *find_label(char*);



void *Malloc(int size)
{
    void *p;

    if( (p = malloc(size)) == NULL ){
        printf("Memory cannot be secured.\n");
        exit(EXIT_FAILURE);
    }

    return p;
}


void initialize_register(void)
{
    fs = NULL;
    f = NULL;
    v = NULL;

    return ;
}


void initialize_fprog_list(void)
{
    struct fproglist *nf;

    // generate
    nf = (struct fproglist*)Malloc(sizeof(struct fproglist));
    nf->fprogname = NULL;
    nf->vars = NULL;
    nf->labels = NULL;
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


void define_variable(int is_formal_parameter)
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
        nv->is_formal_parameter = is_formal_parameter;
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


void define_variable_explicitly(char *var)
{
    int len_var = strlen(var) + 1;
    struct varlist *nv;

    if( is_variable_declared() ){
        return ;
    }

    // generate new node
    nv = (struct varlist*)Malloc(sizeof(struct varlist));
    nv->varname = (char*)Malloc(sizeof(char)*len_var);
    strncpy(nv->varname, var, len_var);
    nv->is_formal_parameter = False;
    nv->nextvar = NULL;

    // connect
    if( f->vars == NULL ){  // first variable
        f->vars = nv;
    }else{  // otherwise
        v->nextvar = nv;
    }
    v = nv;

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


void define_label(void)
{
    int len_str = strlen(str) + 1;
    struct labellist *nl;

    if( (nl = find_label(str)) != NULL ){
        nl->is_used_for_dep = True;
        return ;
    }

    // generate new node
    nl = (struct labellist*)Malloc(sizeof(struct labellist));
    nl->labelname = (char*)Malloc(sizeof(char)*len_str);
    strncpy(nl->labelname, str, len_str);
    nl->is_used_for_dep = True;
    nl->is_used_for_arr = False;
    nl->nextlabel = NULL;

    // connect
    if( f->labels == NULL ){  // first label
        f->labels = nl;
    }else{  // otherwise
        l->nextlabel = nl;
    }
    l = nl;

    return ;
}


void define_label_explicitly(char *label)
{
    int len_label = strlen(label) + 1;
    struct labellist *nl;

    if( (nl = find_label(label)) != NULL ){
        nl->is_used_for_dep = True;
        return ;
    }

    // generate new node
    nl = (struct labellist*)Malloc(sizeof(struct labellist));
    nl->labelname = (char*)Malloc(sizeof(char)*len_label);
    strncpy(nl->labelname, label, len_label);
    nl->is_used_for_dep = True;
    nl->is_used_for_arr = False;
    nl->nextlabel = NULL;

    // connect
    if( f->labels == NULL ){  // first label
        f->labels = nl;
    }else{  // otherwise
        l->nextlabel = nl;
    }
    l = nl;

    return ;
}


void reference_label(char *label)
{
    int len_str = strlen(label) + 1;
    struct labellist *nl;

    if( (nl = find_label(label)) != NULL ){
        if( nl->is_used_for_arr == True ){
            printf("Label \"%s\" is used multiple times.\n", label);
            exit(EXIT_FAILURE);
        }
        nl->is_used_for_arr = True;
        return ;
    }

    // generate new node
    nl = (struct labellist*)Malloc(sizeof(struct labellist));
    nl->labelname = (char*)Malloc(sizeof(char)*len_str);
    strncpy(nl->labelname, label, len_str);
    nl->is_used_for_dep = False;
    nl->is_used_for_arr = True;
    nl->nextlabel = NULL;

    // connect
    if( f->labels == NULL ){  // first label
        f->labels = nl;
    }else{  // otherwise
        l->nextlabel = nl;
    }
    l = nl;

    return ;
}


static struct labellist *find_label(char *label)
{
    struct labellist *p;

    for( p = f->labels; p != NULL; p = p->nextlabel ){
        if( strcmp(p->labelname, label) == 0 ){
            return p;
        }
    }

    return NULL;
}


void check_label_link(void)
{
    struct labellist *lp;

    for( lp = f->labels; lp != NULL; lp = lp->nextlabel ){
        if( lp->is_used_for_dep && ! lp->is_used_for_arr ){
            printf("Invalid correspondence of label \"%s\".\n", lp->labelname);
            exit(EXIT_FAILURE);
        }
    }

    return ;
}


struct arglist *initialize_arglist(void)
{
    struct arglist *pas;

    if( as == NULL ){
        return NULL;
    }

    pas = as;
    as = NULL;

    return pas;
}


void finalize_arglist(struct arglist *pas)
{
    struct arglist *ap, *rm;

    if( as != NULL ){
        while( as->nextarg != NULL ){
            for( ap = as; ap->nextarg->nextarg != NULL; ap = ap->nextarg );
            rm = ap->nextarg;
            ap->nextarg = NULL;
            free(rm->argname);
            free(rm);
        }
        free(as);
    }

    as = pas;

    return ;
}


void register_arg(char *argname)
{
    int len_arg = strlen(argname) + 1;
    struct arglist *na, *ap;

    // generate new node
    na = (struct arglist*)Malloc(sizeof(struct arglist));
    na->argname = (char*)Malloc(sizeof(char)*len_arg);
    strncpy(na->argname, argname, len_arg);
    na->nextarg = NULL;

    // connect
    if( as == NULL ){  // first arg
        as = na;
    }else{  // otherwise
        for( ap = as; ap->nextarg != NULL; ap = ap->nextarg );
        ap->nextarg = na;
    }

    return ;
}


struct arglist *get_args(void)
{
    return as;
}


int register_repvaridx(void)
{
    struct fproglist *fp;
    struct varlist *vp;
    int counter;

    counter = 1;
    for( vp = f->vars; vp != NULL; vp = vp->nextvar ){
        vp->repvaridx = counter++;
    }

    return counter-2;
}


void register_replabelname(char *oname, char *nname)
{
    struct fproglist *fp;
    struct labellist *lp;

    for( lp = f->labels; strcmp(lp->labelname, oname) != 0; lp = lp->nextlabel );
    strcpy(lp->replabelname, nname);

    return ;
}


void get_mainfuncname(char *buf)
{
    struct fproglist *fp;

    strcpy(buf, f->fprogname);

    return ;
}


int get_repvaridx(char *oname)
{
    struct varlist *vp;

    for( vp = f->vars; vp != NULL && strcmp(vp->varname, oname) != 0; vp = vp->nextvar );

    return ( vp == NULL )? False : vp->repvaridx;
}



// debug function
void print_list(void)
{
    struct fproglist *fp;
    struct varlist *vp;
    struct labellist *lp;

    // funcprog
    for( fp = fs; fp != NULL; fp = fp->nextfprog ){
        printf("Funcprog = %s\n", fp->fprogname);
        // formal parameters
        printf("Formal parameters =");
        for( vp = fp->vars; vp != NULL && vp->is_formal_parameter; vp = vp->nextvar ){
            printf(" %s(%d)", vp->varname, vp->repvaridx);
        }
        printf("\n");
        // vars
        printf("Vars =");
        for( ; vp != NULL; vp = vp->nextvar ){
            printf(" %s(%d)", vp->varname, vp->repvaridx);
        }
        printf("\n");
        // labels
        printf("Labels =");
        for( lp = fp->labels; lp != NULL; lp = lp->nextlabel ){
            printf(" %s", lp->labelname);
        }
        printf("\n\n");
    }
}
