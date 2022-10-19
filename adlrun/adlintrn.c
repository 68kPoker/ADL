#include <stdio.h>

#include "adltypes.h"
#include "adlprog.h"
#include "builtins.h"
#include "vstring.h"
#include "adlrun.h"


setdemon()
{
    assertargs( "$sdem", 1 );
    if( (ARG( 1 ) < 0) || (ARG( 1 ) > NUMROUT) )
	error( 4 );			/* Illegal routine for $sdem */
    demons[ numd++ ] = ARG( 1 );
}


deldemon()
{
    int16
	i;	/* Loop counter */

    assertargs( "$ddem", 1 );
    for( i = 0; i < numd; i++ )
	if( demons[ i ] == ARG( 1 ) )
	    break;
    if( i < numd ) {
	numd--;
	for( ; i < numd; i++ )
	    demons[ i ] = demons[ i + 1 ];
    }
}


setfuse()
{
    assertargs( "$sfus", 3 );	/* ($sfus actor rout nturns) */
    if( (ARG( 1 ) < 0) || (ARG( 1 ) > NUMOBJ) )
	error( 30 );			/* Illegal actor for $sfus */
    if( (ARG( 2 ) < 0) || (ARG( 2 ) > NUMROUT) )
	error( 5 );			/* Illegal routine for $sfus */
    if( !routspace[ ARG( 2 ) ] )
	return;				/* Don't bother with a null fuse */
    f_actors[ numf ] = ARG( 1 );
    fuses[ numf ] = ARG( 2 );
    ftimes[ numf++ ] = currturn + ARG( 3 );
}


delfuse( act, which )
int16
    act,
    which;
{
    int16
	i;	/* Loop counter */

    for( i = 0; i < numf; i++ )
	if( (fuses[ i ] == which) && (f_actors[ i ] == act) )
	    break;
    if( i < numf ) {
	numf--;
	for( ; i < numf; i++ ) {
	    fuses[ i ] = fuses[ i + 1 ];
	    ftimes[ i ] = ftimes[ i + 1 ];
	    f_actors[ i ] = f_actors[ i + 1 ];
	}
    }
}


incturn()
{
    if( ARG( 0 ) == 2 )
	/* The programmer specified an increment */
	currturn += ARG( 1 );
    else
	/* The default increment is 1 */
	currturn++;
    execfuses();
}


retturn()
{
    RETVAL = currturn;
}


doprompt()
{
    assertargs( "$prompt", 1 );
    if( (ARG( 1 ) < 0) || (ARG( 1 ) > NUMROUT) )
	error( 6 );		/* Illegal routine for $prompt */
    prompter = ARG( 1 );
}


setactor()
{
    assertargs( "$actor", 3 );
    if( (ARG( 1 ) < 2) || (ARG( 1 ) > NUMOBJ) )
	error( 7 );		/* Illegal object for $actor */
    if( numact <  10 ) {
	actlist[ numact ].actor = ARG( 1 );
	actlist[ numact ].linebuf = actlist[ numact ].savebuf;
	actlist[ numact ].interact = ARG( 3 );
	if( ARG( 2 ) ) {
	    strncpy( actlist[ numact ].linebuf, virtstr( ARG( 2 ) ), SLEN - 1 );
	    actlist[ numact ].savebuf[ 79 ] = '\0';
	}
	else
	    *actlist[ numact ].linebuf = '\0';
	numact++;
    }
    else {
	printf( "Too many actors in actlist, ip = %ld.\n", ip );
	head_term();
	exit( -1 );
    }
}


delactor( n )
int16
    n;
{
    int16
	i;	/* Loop counter */

    for( i = 0; i < numact; i++ ) {
	if( actlist[ i ].actor == n ) {
	    numact--;
#if MULTIPLEX
	    if( actlist[ i ].ttyfile != (FILE *)0 ) {
		fclose( actlist[ i ].ttyfile );
		actlist[ i ].ttyfile = (FILE *)0;
	    }
#endif
	    for( ; i < numact; i++ ) {
		actlist[ i ].actor = actlist[ i + 1 ].actor;
		strcpy( actlist[ i ].savebuf, actlist[ i + 1 ].linebuf );
		actlist[ i ].linebuf = actlist[ i ].savebuf;
#if MULTIPLEX
		actlist[ i ].ttyfile = actlist[ i + 1 ].ttyfile;
		strcpy( actlist[ i ].ttyname, actlist[ i + 1 ].ttyname );
#endif
	    }
	    return;
	}
    }
}

/*** EOF adlintrn.c ***/
