#include <stdio.h>

#include "adltypes.h"
#include "adlprog.h"
#include "adldef.h"
#include "builtins.h"
#include "vstring.h"
#include "adlrun.h"

extern char
    *malloc();

eqstring()
{
    char
	*s;

    assertargs( "$eqst", 2 );
    s = malloc( strlen( virtstr( ARG( 1 ) ) ) + 1 );
    if( s == (char *)0 )
	error( 27 );				/* Out of memory */
    strcpy( s, virtstr( ARG( 1 ) ) );
    if( strcmp( s, virtstr( ARG( 2 ) ) ) )
	RETVAL = 0;
    else
	RETVAL = 1;
    free( s );
}


substring()
{
    char
	*s;
    int
	t;

    assertargs( "$subs", 3 );
    t = strlen( virtstr( ARG( 1 ) ) );
    s = malloc( t + 1 );
    if( s == (char *)0 )
	error( 27 );				/* Out of memory */
    strcpy( s, virtstr( ARG( 1 ) ) );
    if( ARG( 2 ) >= t ) {
	ARG( 2 ) = t;
	ARG( 3 ) = 1;
    }
    else if( ((ARG( 2 ) + ARG( 3 )) >= t) || (!ARG( 3 )) )
	ARG( 3 ) = t - ARG( 2 );
    strncpy( s, &s[ ARG( 2 ) ], ARG( 3 ) );
    s[ ARG( 3 ) ] = '\0';
    RETVAL = newtstr( s );
    free( s );
}


lengstring()
{
    RETVAL = strlen( virtstr( ARG( 1 ) ) );
}


readstring()
{
    char
	s[ SLEN ];

    getstring( s );
    RETVAL = newtstr( s );
}


catstring()
{
    char
	*s;

    assertargs( "$cat", 2 );
    s = malloc( strlen( virtstr( ARG(1) ) ) + strlen( virtstr( ARG(2) ) ) + 1 );
    if( s == (char *)0 )
	error( 27 );				/* Out of memory */
    strcpy( s, virtstr( ARG(1) ) );
    strcat( s, virtstr( ARG(2) ) );
    RETVAL = newtstr( s );
    free( s );
}


chrstring()
{
    char
	s[ 2 ];

    assertargs( "$chr", 1 );
    s[ 0 ] = (char) ARG( 1 );
    s[ 1 ] = '\0';
    RETVAL = newtstr( s );
}


ordstring()
{
    char
	temp;

    assertargs( "$ord", 1 );
    temp = *( virtstr( ARG( 1 ) ) );
    RETVAL = (int16) temp;
}


int16
strpos( s1, s2 )
char
    *s1, *s2;
{
    char
	*t0,
	*t1,
	*t2;

    t0 = s2;
    while( *s2 ) {
	if( *s1 == *s2 ) {
	    t1 = s1;
	    t2 = s2;
	    while( *s1 && *s1 == *s2 ) {
		s1++;
		s2++;
	    }
	    if( !*s1 )	/* Found it! */
	        return (int16) (s2 - t0);
	    s1 = t1;
	    s2 = t2;
	}
	s2++;
    }
    return -1;
}


posstring()
{
    char
	*s;

    assertargs( "$pos", 2 );
    s = malloc( strlen( virtstr( ARG( 1 ) ) ) + 1 );
    if( s == (char *)0 )
	error( 27 );				/* Out of memory */
    strcpy( s, virtstr( ARG( 1 ) ) );
    RETVAL = strpos( s, virtstr( ARG( 2 ) ) );
    free( s );
}


savestr()
{
    assertargs( "$savestr", 1 );
    RETVAL = vs_save( ARG( 1 ) );
}


do_str()
{
    char s[ SLEN ];

    assertargs( "$str", 1 );
    (void)sprintf( s, "%d", ARG( 1 ) );
    RETVAL = newtstr( s );
}


do_num()
{
    assertargs( "$num", 1 );
    RETVAL = atoi( virtstr( ARG( 1 ) ) );
}


char	bletch[ 10 ];	/* Static store area for xlate */

char	*
xlate( s )
char
    *s;
{
    int
	i;

    for( i = 0; s[ i ]; i++ )
	bletch[ i ] = ~s[ i ];
    bletch[ i ] = '\0';
    return bletch;
}


do_name()
{
    char
	s[ 80 ];
    int16
	t;

    assertargs( "$name", 1 );
#if DEBUG
    if( (ARG( 1 ) < 0) || (ARG( 1 ) > NUMOBJ) )
	error( 25 );	/* Illegal object for $name */
#endif
    t = objspace[ ARG( 1 ) ].adj;
    if( t < 0 ) {
	strcpy( s, findone( VERB, -t ) );
	strcat( s, " " );
    }
    else if( t ) {
	strcpy( s, findone( ADJEC, t ) );
	strcat( s, " " );
    }
    else
	*s = '\0';
    strcat( s, findone( NOUN, objspace[ ARG( 1 ) ].noun ) );
    RETVAL = newtstr( s );
}


do_vname()
{
    assertargs( "$vname", 1 );
    RETVAL = newtstr( findone( VERB, ARG( 1 ) ) );
}


do_mname()
{
    assertargs( "$mname", 1 );
    if( ARG( 1 ) < 0 ) {
	ARG( 1 ) = -ARG( 1 );
	do_vname();
    }
    else if( ARG( 1 ) )
	RETVAL = newtstr( findone( ADJEC, ARG( 1 ) ) );
    else
	RETVAL = newtstr( "" );
}


do_pname()
{
    assertargs( "$pname", 1 );
    RETVAL = newtstr( findone( PREP, ARG( 1 ) ) );
}


do_define()
{
    char
	s1[ 80 ],
	s2[ 80 ];

    assertargs( "$define", 2 );
    strcpy( s1, virtstr( ARG( 1 ) ) );
    strcpy( s2, virtstr( ARG( 2 ) ) );
    define( s1, s2 );
}


do_undef()
{
    char
	s[ 80 ];

    assertargs( "$undef", 1 );
    strcpy( s, xlate( virtstr( ARG( 1 ) ) ) );
    undef( s );
}

/*** EOF adlstr.c ***/
