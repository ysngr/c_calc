/* jumpprog.c */
#include "jumpprog.h"


int main(int argc, char *argv[])
{
    if( argc != 2 ){
        printf("Invalid argument.\n");
        return -1;
    }

    init_scan(argv[1]);
    parse();
    end_scan();
    printf("Valid program.\n");

    return 0;
}
