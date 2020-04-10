/* flatten.c */
#include "functojump.h"

#define IF_STAT 1
#define GOTO_STAT 2
#define ARR_LABEL 3

static struct condlist{
    char *cond;
    char *label;
    int stattype;
    struct condlist *next;
} *cs;


static struct condlist *gen_if_node(char*, char*);
static struct condlist *gen_goto_or_arr_node(char*, int);
static void flatten_cond(struct condlist*);
static struct condlist *flatten_paren(struct condlist*);
static struct condlist *flatten_not(struct condlist*);
static struct condlist *flatten_or(struct condlist*);
static struct condlist *flatten_and(struct condlist*);
static void flatten_re(struct condlist*);
static void generate_flattened_stats(void);
static void free_condlist(struct condlist*);

static void print_condlist(void);


void initialize_condlist(void)
{
    cs = NULL;

    return ;
}


void flatten(char *cond, char *label)
{
    cs = gen_if_node(cond, label);
    flatten_cond(cs);
    flatten_re(cs);
    generate_flattened_stats();

    return ;
}


static struct condlist *gen_if_node(char *cond, char *label)
{
    struct condlist *nc;
    int len_cond = strlen(cond) + 1;
    int len_label = strlen(label) + 1;

    nc = (struct condlist*)Malloc(sizeof(struct condlist));
    nc->cond = (char*)Malloc(sizeof(char)*len_cond);
    nc->label = (char*)Malloc(sizeof(char)*len_label);
    strncpy(nc->cond, cond, len_cond);
    strncpy(nc->label, label, len_label);
    nc->stattype = IF_STAT;
    nc->next = NULL;

    return nc;
}


static struct condlist *gen_goto_or_arr_node(char *label, int stype)
{
    struct condlist *nc;
    int len_label = strlen(label) + 1;

    nc = (struct condlist*)Malloc(sizeof(struct condlist));
    nc->cond = NULL;
    nc->label = (char*)Malloc(sizeof(char)*len_label);
    strncpy(nc->label, label, len_label);
    nc->stattype = stype;
    nc->next = NULL;

    return nc;
}


static void flatten_cond(struct condlist *c)
{
    int i;
    struct condlist *nc;
    struct condlist *(*flatten_func[])(struct condlist*) = {
        flatten_paren, flatten_not, flatten_or, flatten_and
    };

    if( c == NULL ){
        return ;
    }

    if( c->stattype == IF_STAT ){
        for( i = 0; i < 4; i++ ){
            if( (nc = (*flatten_func[i])(c)) != NULL ){
                flatten_cond(nc);
                return ;
            }
        }
    }
    flatten_cond(c->next);

    // print_condlist();///debug

    return ;
}


static struct condlist *flatten_paren(struct condlist *c)
{
    int i, d;
    int ps, pe;
    char rm_paren_cond[MAXSTRLEN];
    struct condlist *nc, *pc;

    // find '('
    for( i = 0; c->cond[i] == ' '; i++ );  // skip space
    if( c->cond[i] != '(' ){
        return NULL;
    }
    ps = i;
    // find ')'
    d = 1;
    for( i++; c->cond[i] != '\0'; i++ ){
        switch( c->cond[i] ){
            case '(' : d++; break;
            case ')' : d--; break;
        }
        if( d <= 0 ){
            pe = i;
            break;
        }
    }
    for( i++; c->cond[i] != '\0'; i++ ){
        if( c->cond[i] != ' ' ){
            return NULL;
        }
    }
    // remove most outside paren
    strncpy(rm_paren_cond, (c->cond)+ps+1, pe-ps-1);
    rm_paren_cond[pe-ps] = '\0';

    // generate new node
    nc = gen_if_node(rm_paren_cond, c->label);

    // connect
    nc->next = c->next;
    if( c == cs ){
        cs = nc;
    }else{
        for( pc = cs; pc->next != c; pc = pc->next );
        pc->next = nc;
    }
    free_condlist(c);

    return nc;
}


static struct condlist *flatten_not(struct condlist *c)
{
    int i;
    char rm_not_cond[MAXSTRLEN];
    char newlabel[MAXSTRLEN];
    struct condlist *nci, *ncg, *nca;
    struct condlist *pc;

    for( i = 0; c->cond[i] == ' '; i++ );  // skip space
    if( c->cond[i] != '!' ){
        return NULL;
    }
    strncpy(rm_not_cond, c->cond+i+1, MAXSTRLEN);
    create_newlabel(newlabel);

    // generate new node
    nci = gen_if_node(rm_not_cond, newlabel);
    ncg = gen_goto_or_arr_node(c->label, GOTO_STAT);
    nca = gen_goto_or_arr_node(newlabel, ARR_LABEL);

    // connect
    nci->next = ncg;
    ncg->next = nca;
    nca->next = c->next;
    if( c == cs ){
        cs = nci;
    }else{
        for( pc = cs; pc->next != c; pc = pc->next );
        pc->next = nci;
    }
    free_condlist(c);

    return nci;
}


static struct condlist *flatten_or(struct condlist *c)
{
    int i, d;
    int is_flatten_exec;
    char fst_cond[MAXSTRLEN], snd_cond[MAXSTRLEN];
    struct condlist *ncf, *ncs;
    struct condlist *pc;

    d = 0;
    is_flatten_exec = False;
    for( i = 0; c->cond[i] != '\0'; i++ ){
        switch( c->cond[i] ){
            case '(' : d++; break;
            case ')' : d--; break;
        }
        if( d == 0 && strncmp((c->cond)+i, "||", 2) == 0 ){
            strncpy(fst_cond, c->cond, i);
            fst_cond[i] = '\0';
            strcpy(snd_cond, (c->cond)+i+2);
            is_flatten_exec = True;
            break;
        }
    }
    if( ! is_flatten_exec ){
        return NULL;
    }

    // generate new node
    ncf = gen_if_node(fst_cond, c->label);
    ncs = gen_if_node(snd_cond, c->label);

    // connect
    ncf->next = ncs;
    ncs->next = c->next;
    if( c == cs ){
        cs = ncf;
    }else{
        for( pc = cs; pc->next != c; pc = pc->next );
        pc->next = ncf;
    }
    free_condlist(c);

    return ncf;
}


static struct condlist *flatten_and(struct condlist *c)
{
    int i, d;
    int is_flatten_exec;
    char fst_cond[MAXSTRLEN], snd_cond[MAXSTRLEN];
    char truelabel[MAXSTRLEN], falselabel[MAXSTRLEN];
    struct condlist *ncif, *ncg, *ncat, *ncis, *ncaf;
    struct condlist *pc;

    d = 0;
    is_flatten_exec = False;
    for( i = 0; c->cond[i] != '\0'; i++ ){
        switch( c->cond[i] ){
            case '(' : d++; break;
            case ')' : d--; break;
        }
        if( d == 0 && strncmp((c->cond)+i, "&&", 2) == 0 ){
            strncpy(fst_cond, c->cond, i);
            fst_cond[i] = '\0';
            strcpy(snd_cond, (c->cond)+i+2);
            is_flatten_exec = True;
            break;
        }
    }
    if( ! is_flatten_exec ){
        return NULL;
    }
    create_newlabel(truelabel);
    create_newlabel(falselabel);

    // generate new node
    ncif = gen_if_node(fst_cond, truelabel);
    ncg = gen_goto_or_arr_node(falselabel, GOTO_STAT);
    ncat = gen_goto_or_arr_node(truelabel, ARR_LABEL);
    ncis = gen_if_node(snd_cond, c->label);
    ncaf = gen_goto_or_arr_node(falselabel, ARR_LABEL);

    // connect
    ncif->next = ncg;
    ncg->next = ncat;
    ncat->next = ncis;
    ncis->next = ncaf;
    ncaf->next = c->next;
    if( c == cs ){
        cs = ncif;
    }else{
        for( pc = cs; pc->next != c; pc = pc->next );
        pc->next = ncif;
    }
    free_condlist(c);

    return ncif;
}


static void flatten_re(struct condlist *c)
{
    int i;

    if( c == NULL ){
        return ;
    }

    if( c->stattype == IF_STAT ){
        for( i = 0; c->cond[i] != '\0'; i++ ){
            if( c->cond[i] == '>' ){
                while( c->cond[--i] == ' ' );
                c->cond[i+1] = '\0';
                break;
            }
        }
    }

    flatten_re(c->next);

    return ;
}


static void generate_flattened_stats(void)
{
    struct condlist *cp;

    for( cp = cs; cp != NULL; cp = cp->next ){
        switch( cp->stattype ){
            case IF_STAT :
                generate_if(cp->cond, cp->label);
                break;
            case GOTO_STAT :
                generate_goto(cp->label);
                break;
            case ARR_LABEL :
                generate_arrlabel(cp->label);
                break;
        }
    }

    return ;
}


static void free_condlist(struct condlist *p)
{
    if( p->cond != NULL ){
        free(p->cond);
    }
    free(p->label);
    free(p);

    return ;
}


void finalize_condlist(void)
{
    struct condlist *cp, *rm;

    cp = cs;
    while( cp != NULL ){
        rm = cp;
        cp = cp->next;
        free_condlist(rm);
    }

    return ;
}



// debug function
static void print_condlist(void)
{
    struct condlist *cp;

    for( cp = cs; cp != NULL; cp = cp->next ){
        if( cp->stattype == IF_STAT ){
            printf("type=if : cond = %s, label = %s\n", cp->cond, cp->label);
        }else if( cp->stattype == GOTO_STAT ){
            printf("type=goto : label = %s\n", cp->label);
        }else{
            printf("type=arr : label = %s\n", cp->label);
        }
    }
    printf("\n");

    return ;
}
