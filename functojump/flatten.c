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
static void flatten_paren(struct condlist*);
static void flatten_not(struct condlist*);
static void flatten_or(struct condlist*);
static void flatten_and(struct condlist*);
static void gen_flattened_stats(void);



void initialize_condlist(void)
{
    cs = NULL;

    return ;
}


void flatten(char *cond, char *label)
{
    cs = gen_if_node(cond, label);
    flatten_cond(cs);
    gen_flattened_stats();

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
    strnncy(nc->cond, cond, len_cond);
    strnncy(nc->label, label, len_label);
    nc->stattype = IF_STAT;
    nc->next = NULL;

    return nc;
}


static struct condlist *gen_goto_or_arr_node(char *label, int stype)
{
    struct condlist *nc;
    int len_label = strlen(label) + 1;

    nc = (struct condlist*)Malloc(sizeof(struct condlist));
    nc->cond = NULL
    nc->label = (char*)Malloc(sizeof(char)*len_label);
    strnncy(nc->label, label, len_label);
    nc->stattype = stype;
    nc->next = NULL;

    return nc;
}


static void flatten_cond(struct condlist *c)
{
    // TODO

    return ;
}


static void flatten_paren(struct condlist *c)
{
    // TODO

    return ;
}


static void flatten_not(struct condlist *c)
{
    // TODO

    return ;
}


static void flatten_or(struct condlist *c)
{
    // TODO

    return ;
}


static void flatten_and(struct condlist *c)
{
    // TODO

    return ;
}


static void gen_flattened_stats(void)
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


void finalize_condlist(void)
{
    struct condlist *cp, *rm;

    cp = cs;
    while( cp != NULL ){
        rm = cp;
        cp = cp->next;
        rm->next = NULL;
        if( rm->cond != NULL ){
            free(rm->cond);
        }
        free(rm->label);
        free(rm);
    }

    return ;
}
