#include <stdio.h>

#include "adltypes.h"
#include "adlprog.h"
#include "builtins.h"
#include "adlrun.h"


setp()
{
    int16
	a, b;		/* Arg1, arg2 */

    assertargs( "$setp", 3 );
    a = ARG( 1 );
    b = ARG( 2 );
    if( (a < 0) || (a > NUMOBJ) )
	error( 15 );	/* Illegal object for $setp */
    if( (b < 1) || (b > _ACT) )
	error( 16 );	/* Illegal propnum for $setp */
    if( (b >= 1) && (b <= 16) ) {
	if( ARG( 3 ) )
	    objspace[ a ].props1to16 |= bitpat[ b - 1 ];
	else
	    objspace[ a ].props1to16 &= ibitpat[ b - 1 ];
    }
    else if( (b >= 17) && (b <= _ACT) )
	objspace[ a ].props[ b - 17 ] = ARG( 3 );
}


move_obj()
{
    int16
	a, b, t;	/* Arg1, arg2, temp */

    assertargs( "$move", 2 );
    a = ARG( 1 );
    b = ARG( 2 );
    if( (a < 0) || (b < 0) || (a > NUMOBJ) || (b > NUMOBJ) )
	error( 17 );	/* Illegal object for $move */
    t = objspace[ a ].loc;
    if( objspace[ t ].cont != a ) {
	t = objspace[ t ].cont;
	while( objspace[ t ].link != a )
	    t = objspace[ t ].link;
	objspace[ t ].link = objspace[ objspace[ t ].link ].link;
    }
    else
	objspace[ t ].cont = objspace[ objspace[ t ].cont ].link;
    objspace[ a ].loc = b;
    objspace[ a ].link = 0;
    t = objspace[ b ].cont;
    if( t ) {
	while( objspace[ t ].link )
	    t = objspace[ t ].link;
	objspace[ t ].link = a;
    }
    else
	objspace[ b ].cont = a;
}


objprop( n )
int16
    n;
{
    assertargs( "$prop", 1 );
    if( (ARG( 1 ) < 0) || (ARG( 1 ) > NUMOBJ) )
	error( 18 );	/* Illegal object for $prop */
    if( (n >= 1) && (n <= 16) ) {
	if( objspace[ ARG( 1 ) ].props1to16 & bitpat[ n - 1 ] )
	    RETVAL = 1;
	else
	    RETVAL = 0;
    }
    else if( (n >= 17) && (n <= _ACT) )
	RETVAL = objspace[ ARG( 1 ) ].props[ n - 17 ];
    else
	switch( n ) {
	    case _LOC :
		RETVAL = objspace[ ARG( 1 ) ].loc;
		break;
	    case _CONT :
		RETVAL = objspace[ ARG( 1 ) ].cont;
		break;
	    case _LINK :
		RETVAL = objspace[ ARG( 1 ) ].link;
		break;
	    case _MODIF :
		RETVAL = objspace[ ARG( 1 ) ].adj;
		break;
	    default :
		error( 19 );		/* Illegal promnum for $prop */
	}
}


vset()
{
    assertargs( "$vset", 3 );
    if( (ARG( 1 ) < 0) || (ARG( 1 ) > NUMVERB) )
	error( 20 );		/* Illegal verb for $vset */
    switch( ARG( 2 ) ) {
	case _PREACT :
	    verbspace[ ARG( 1 ) ].preact = ARG( 3 );
	    break;
	case _ACT :
	    verbspace[ ARG( 1 ) ].postact = ARG( 3 );
	    break;
	default	 :
	    error( 21 );		/* Illegal propnum for $vset */
    }
}


vprop()
{
    assertargs( "$vprop", 2 );
    if( (ARG( 1 ) < 0) || (ARG( 1 ) > NUMVERB) )
	error( 22 );		/* Illegal verb for $vprop */
    switch( ARG( 2 ) ) {
	case _PREACT :
	    RETVAL = verbspace[ ARG( 1 ) ].preact;
	    break;
	case _ACT :
	    RETVAL = verbspace[ ARG( 1 ) ].postact;
	    break;
	default :
	    error( 23 );		/* Illegal propnum for $vprop */
    }
}

/*** EOF adlobjr.c ***/
