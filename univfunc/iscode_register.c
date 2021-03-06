/* iscode_register.c */
#include "iscode.h"

static struct varlist{
    char *varname;
    struct varlist *nextvar;
} *v;

static struct labellist{
    char *labelname;
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

static char errmsg[MAXSTRLEN];  // for error


static void *Malloc(int);
static int is_funcprog_declared(void);
static int is_variable_declared(void);
static struct labellist *find_label(char*);



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


void define_variable(void)
{
    int len_str = strlen(str) + 1;
    struct varlist *nv;

    // function program name
    if( f->fprogname == NULL ){
        if( is_funcprog_declared() ){
            snprintf(errmsg, MAXSTRLEN, "Function program \"%s\" is duplicated.", str);
            error(errmsg);
        }

        f->fprogname = (char*)Malloc(sizeof(char)*len_str);
        strncpy(f->fprogname, str, len_str);
    }

    // variable name
    else{
        if( is_variable_declared() ){
            snprintf(errmsg, MAXSTRLEN, "Variable \"%s\" is duplicated.", str);
            error(errmsg);
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
        snprintf(errmsg, MAXSTRLEN, "Variable \"%s\" is not declared.", str);
        error(errmsg);
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


void generate_label(void)
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


void paste_label(char *label)
{
    int len_str = strlen(label) + 1;
    struct labellist *nl;

    if( (nl = find_label(label)) != NULL ){
        if( nl->is_used_for_arr == True ){
            snprintf(errmsg, MAXSTRLEN, "Label \"%s\" is used multiple times.", label);
            error(errmsg);
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
            snprintf(errmsg, MAXSTRLEN, "Invalid correspondence of label \"%s\".", lp->labelname);
            error(errmsg);
        }
    }

    return ;
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
        // var
        printf("Vars =");
        for( vp = fp->vars; vp != NULL; vp = vp->nextvar ){
            printf(" %s", vp->varname);
        }
        printf("\n");
        // label
        printf("Labels =");
        for( lp = fp->labels; lp != NULL; lp = lp->nextlabel ){
            printf(" %s", lp->labelname);
        }
        printf("\n");
    }
}
