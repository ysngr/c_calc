/* jumpexec.c */
#include "jumpexec.h"

static void initialize(char*);
static void finalize(void);


int main(int argc, char *argv[])
{
    if( argc < 2 ){
        printf("Invalid arguments.\n");
        return -1;
    }

    initialize(argv[1]);

    if( parse() != argc-2 ){
        printf("Invalid number of arguments.\n");
        return -1;
    }
    printf("success: return-value = %d\n", execute(argc-2, argv+2));

    finalize();

    return 0;
}



static void initialize(char *filename)
{
    initialize_scan(filename);
    initialize_register();

    return ;
}


static void finalize(void)
{
    finalize_scan();
    finalize_register();

    return ;
}
