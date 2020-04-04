/* univfunc.c */
#include "univfunc.h"


int main(void)
{
    int a[3] = {0, 1, EMPTY};
    printf("executable(p,x) = %d\n", executable(encode(a, 3),0));

    return 0;
}



int pair(int x, int y)
{
    if( x == EMPTY ){
        x = 0;
    }

    if( y == EMPTY ){
        y = 0;
    }

    return((x+y)*(x+y+1)/2 + x + 1);
}


int left(int z)
{
    int x, y;

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


int right(int z)
{
    int x, y;

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


int encode(int *as, int n)
{
    int i;
    int a;

    a = pair(as[n-1], 0);
    for( i = n-2; i >= 0; i-- ){
        a = pair(as[i], a);
    }

    return a;
}


void decode(int a, int *as)
{
    int i;

    for( i = 0; a != 0; i++ ){
        as[i] = left(a);
        a = right(a);
    }
    as[i] = EMPTY;

    return ;
}


int element(int a, int i)
{
    int j;

    for( j = 0; j < i-1; j++ ){
        a = right(a);
    }

    return left(a);
}


int length(int a)
{
    int n;

    if( a == 0 ){
        return 0;
    }

    for( n = 0; a != 0; n++ ){
        a = right(a);
    }

    return n;
}


int replace(int a, int i, int x)
{
    int j, k;
    int b;

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


int sequence(int x, int k)
{
    int i;
    int a;

    a = 0;
    for( i = 0; i < k; i++ ){
        a = pair(x, a);
    }

    return a;
}


int executable(int p, int x)
{
    int k;

    if( ! is_code(p) ){
        return False;
    }

    if( left(p) == length(x) ){
        return True;
    }

    return False;
}
