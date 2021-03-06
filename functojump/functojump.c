/* functojump.c */
#include "functojump.h"


int main(int argc, char *argv[])
{
    if( argc != 2 ){
        printf("Invalid argument.\n");
        return -1;
    }

    initialize_scan(argv[1]);
    initialize_generator(argv[1]);
    parse();
    finalize_generator();
    finalize_scan();
    printf("Valid program.\n");
    replace();

    return 0;
}
