/* univfunc.c */
#include "univfunc.h"


int main(void)
{
    printf("c = %lld\n", comp(161622373321537, 0));

    return 0;
}



Integer pair(Integer x, Integer y)
{
    return((x+y)*(x+y+1)/2 + x + 1);
}


Integer left(Integer z)
{
    Integer x, y;

    if( z == 0 ){
        return 0;
    }

    for( x = 0; x < z; x++ ){
        for( y = 0; y < z; y++ ){
            if( pair(x, y) == z ){
                return x;
            }
        }
    }

    return 0;  // never reached
}


Integer right(Integer z)
{
    Integer x, y;

    if( z == 0 ){
        return 0;
    }

    for( x = 0; x < z; x++ ){
        for( y = 0; y < z; y++ ){
            if( pair(x, y) == z ){
                return y;
            }
        }
    }

    return 0;  // never reached
}


Integer encode(Integer *as, Integer n)
{
    int i;
    Integer a;

    a = pair(as[n-1], 0);
    for( i = n-2; i >= 0; i-- ){
        a = pair(as[i], a);
    }

    return a;
}


void decode(Integer a, Integer *as)
{
    int i;

    for( i = 0; a != 0; i++ ){
        as[i] = left(a);
        a = right(a);
    }

    return ;
}


Integer element(Integer a, Integer i)
{
    Integer j;

    for( j = 0; j < i-1; j++ ){
        a = right(a);
    }

    return left(a);
}


Integer length(Integer a)
{
    Integer n;

    if( a == 0 ){
        return 0;
    }

    for( n = 0; a != 0; n++ ){
        a = right(a);
    }

    return n;
}


Integer replace(Integer a, Integer i, Integer x)
{
    Integer j, k;
    Integer b;

    b = 0;
    for( k = 0, j = length(a); k < length(a); k++, j-- ){
        if( j == i ){
            b = pair(x, b);
        }else{
            b = pair(element(a, j), b);
        }
    }

    return b;
}


Integer sequence(Integer x, Integer k)
{
    Integer i;
    Integer a;

    a = 0;
    for( i = 0; i < k; i++ ){
        a = pair(x, a);
    }

    return a;
}


Integer executable(Integer p, Integer x)
{
    Integer k;

    if( ! is_code(p) ){
        return False;
    }

    if( left(p) == length(x) ){
        return True;
    }

    return False;
}
