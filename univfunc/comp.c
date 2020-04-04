/* comp.c */
#include "univfunc.h"


static void *Malloc(Integer);


static void *Malloc(Integer size)
{
    void *p;

    if( (p = malloc(sizeof(Integer)*size)) == NULL ){
        printf("Memory cannot be secured.\n");
        exit(EXIT_FAILURE);
    }

    return p;
}


Integer comp(Integer p, Integer x)
{
    int i;
    Integer k, m, s, n;
    Integer pc, a, b;
    Integer *vs, *ss;

    if( ! executable(p, x) ){
        while(True);
    }

    k = left(p);  // number of parameter
    p = right(p);
    m = left(p);  // number of parameter + variable
    p = right(p);

    vs = (Integer*)Malloc(m+1);
    for( i = 1; i <= k; i++ ){
        vs[i] = element(x, i);
    }
    for( ; i <= m; i++ ){
        vs[i] = 0;
    }

    s = p;  // code of statements
    n = length(s);  // number of statements

    ss = (Integer*)Malloc(n+1);
    for( i = 1; i <= n; i++ ){
        ss[i] = element(s, i);
    }

    for( pc = 1; pc <= n; ){
        switch( element(ss[pc], 1) ){
            case 1 :  // < 1, a > : goto La;
                pc = element(ss[pc], 2);
                break;
            case 2 :  // < 2, a, b > : va = b;
                a = element(ss[pc], 2);
                b = element(ss[pc], 3);
                vs[a] = b;
                pc++;
                break;
            case 3 :  // < 3, a, b > : va = vb;
                a = element(ss[pc], 2);
                b = element(ss[pc], 3);
                vs[a] = vs[b];
                pc++;
                break;
            case 4 :  // < 4, a > : va++;
                a = element(ss[pc], 2);
                vs[a]++;
                pc++;
                break;
            case 5 :  // < 5, a > : va--';
                a = element(ss[pc], 2);
                if( vs[a] != 0 ){
                    vs[a]--;
                }
                pc++;
                break;
            case 6 :  // < 6, a, b > : if(va > 0) goto Lb;
                a = element(ss[pc], 2);
                b = element(ss[pc], 3);
                if( vs[a] > 0 ){
                    pc = b;
                }else{
                    pc++;
                }
                break;
        }
    }

    return vs[1];
}
