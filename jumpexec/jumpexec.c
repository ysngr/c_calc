/* jumpexec.c */
#include "jumpexec.h"

int main(int argc, char *argv[])
{
    if( argc < 2 ){
        printf("Invalid arguments.\n");
        return -1;
    }

    if( parse(argv[1]) != argc - 2 ){
        printf("Invalid number of arguments.\n");
        return -1;
    }
    print_statlist();///debug

    // execute();

    return 0;
}
