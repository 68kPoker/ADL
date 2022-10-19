#include <stdio.h>

#include "adltypes.h"
#include "adlprog.h"
#include "builtins.h"
#include "adlrun.h"


setverb()
{
    int16
	i;

    assertargs( "$setv", 10 );
    for( i = 0; i <= 9; i++ )
	vecverb[ i ] = ARG( i + 1 );
    RETVAL = 0;
}


hitverb()
{
    int16
	i;

    assertargs( "$hit", 11 );
    for( i = 0; i <= 9; i++ )
	if( vecverb[ i ] == Verb ) {
	    ARG( 2 ) = ARG( i + 2 );
	    if( ARG( 2 ) )
		move_obj();
	    RETVAL = 0;
	    return;
	}
    RETVAL = 0;
}


missverb()
{
    int16
	i,
	oldbp,
	which;

    assertargs( "$miss", 10 );
    for( i = 0; i <= 9; i++ ) {
	if( vecverb[ i ] == Verb ) {
	    popip();
	    oldbp = pop();
	    which = ARG( i + 1 );
#if DEBUG
	    if( (which < 0) || (which > NUMROUT) )
		error( 26 );	/* Illegal rout for $miss */
#endif
	    sp = bp;		/* Cut args off stack */
	    if( which && routspace[ which ] ) {
		push( 1 );	/* stackdepth */
		push( oldbp );	/* bp */
		puship();	/* ip */
		ip = routspace[ which ];
	    }
	    else {
		push( 0 );
		bp = oldbp;
	    }
	    return;
	}
    }
    popip();
    oldbp = pop();
    sp = bp + 1;
    stack[ bp ] = 0;
    bp = oldbp;
}

/*** EOF adltrans.c ***/
