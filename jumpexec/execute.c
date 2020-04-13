/* execute.c */
#include "jumpexec.h"

// #define DEBUG_PRINT_EXECSTATE

static int execute_main_loop(void);



int execute(int argc, char *args[])
{
    int i;
    for( i = 0; i < argc; i++ ){
        set_variable(i+1, atoi(args[i]));
    }

    return execute_main_loop();
}


static int execute_main_loop(void)
{
    int pc;
    int statelem[3];
    int rvalue;
    int result;

    pc = 1;
    result = -1;

    while( True ){
        #ifdef DEBUG_PRINT_EXECSTATE
            printf("pc = %03d : ", pc);
            print_variables();
        #endif

        get_statement(pc, statelem);

        switch( statelem[STATELEM_STYPE] ){
            case GOTO_STAT :  // goto La;
                pc = statelem[STATELEM_A];
                break;
            case NAT_ASSIGN_STAT :  // va = b;
                set_variable(statelem[STATELEM_A], statelem[STATELEM_B]);
                pc++;
                break;
            case VAR_ASSIGN_STAT :  // va = vb;
                rvalue = get_variable(statelem[STATELEM_B]);
                set_variable(statelem[STATELEM_A], rvalue);
                pc++;
                break;
            case INCR_STAT :  // va++;
                rvalue = get_variable(statelem[STATELEM_A]) + 1;
                set_variable(statelem[STATELEM_A], rvalue);
                pc++;
                break;
            case CDECR_STAT :  // va--';
                if( (rvalue = get_variable(statelem[STATELEM_A]) - 1) >= 0 ){
                    set_variable(statelem[STATELEM_A], rvalue);
                }
                pc++;
                break;
            case IF_GOTO_STAT :  // if(va > 0) goto Lb;
                if( get_variable(statelem[STATELEM_A]) > 0 ){
                    pc = statelem[STATELEM_B];
                }else{
                    pc++;
                }
                break;
            case RETURN_STAT :  // return(v1);
                result = get_variable(1);
                goto EXIT;
        }
    }
    EXIT:

    return result;
}
