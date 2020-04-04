/* comp.c */
#include "univfunc.h"


static void *Malloc(int);


static void *Malloc(int size)
{
    void *p;

    if( (p = malloc(sizeof(int)*size)) == NULL ){
        printf("Memory cannot be secured.\n");
        exit(EXIT_FAILURE);
    }

    return p;
}


int comp(int p, int x)
{
    int i;
    int k, m, s, n;
    int pc, a, b;
    int *vs, *ss;

    if( ! executable(p, x) ){
        while(True);
    }

    k = element(p, 1);  // number of parameter
    m = element(p, 2);  // number of parameter + variable

    vs = (int*)Malloc(m+1);
    for( i = 1; i <= k; i++ ){
        vs[i] = element(x, i);
        // printf("debug:vs[%d] = %d\n", i, vs[i]);///debug
    }
    for( ; i <= m; i++ ){
        vs[i] = 0;
        // printf("debug:vs[%d] = 0\n", i);///debug
    }

    s = element(p, 3);  // code of statements
    n = length(s);  // number of statements

    ss = (int*)Malloc(n+1);
    for( i = 1; i <= n; i++ ){
        ss[i] = element(s, i);
        // printf("debug:ss[%d] = %d\n", i, ss[i]);///debug
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
