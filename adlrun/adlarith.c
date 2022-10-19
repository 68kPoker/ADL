#include <stdio.h>

#include "adltypes.h"
#include "adlprog.h"
#include "builtins.h"
#include "adlrun.h"


int16
myrand( n )
int16
    n;
{
    return 1 + ( RAND % n );
}


do_div()
{
    assertargs( "$div", 2 );
    if( ARG(2) == 0 )
	error( 1 );				/* Divide by zero */
    RETVAL = ARG(1) / ARG(2);
}


do_mod()
{
    assertargs( "$mod", 2 );
    if( ARG(2) == 0 )
	error( 1 );				/* Divide by zero */
    RETVAL = ARG(1) % ARG(2);
}


do_and()
{
    int16
	i,		/* Loop counter */
	tval;		/* Temporary save area */

    tval = -1;		/* Bit pattern of all ones */
    for( i = 1; i < RETVAL; i++ )
	tval &= ARG( i );
    RETVAL = tval;
}


do_or()
{
    int16
	i,		/* Loop counter */
	tval;		/* Storage area */

    tval = 0;		/* Bit pattern of all zeros */
    for( i = 1; i < RETVAL; i++ )
	tval |= ARG( i );
    RETVAL = tval;
}

/*** EOF adlarith.c ***/
