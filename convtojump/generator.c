/* generator.c */
#include "convtojump.h"


static FILE *fp;

static void separate_extension(char*);



void initialize_generator(char *inputfile)
{
    char inputfiledelext[MAXSTRLEN];
    char extension[MAXSTRLEN];
    char outputfile[MAXSTRLEN];

    separate_extension(inputfile, inputfiledelext, extension);
    snprintf(outputfile, MAXSTRLEN, "%s_jump.%s", inputfiledelext, extension);

    if( (fp = fopen(outputfile, "w")) == NULL ){
        printf("Output file cannot be generated.\n");
        exit(EXIT_FAILURE);
    }

    return ;
}


static void separate_extension(char *file, char *name, char *ext)
{
    int i, j;

    for( i = 0; file[i] != '.'; i++ ){
        name[i] = file[i];
    }
    name[i] = '\0';

    for( j = 0; file[i] != '\0'; i++, j++ ){
        ext[j] = file[i];
    }
    ext[j] = '\0';

    return ;
}


void generate(int token)
{
    // TODO

    return ;
}


void finalize_generator(void)
{
    fclose(fp);

    return ;
}
