#include <stdio.h>

#include "adltypes.h"
#include "adlprog.h"
#include "builtins.h"
#include "vstring.h"
#include "adlrun.h"


varconts( n )
int16	n;
{
    switch( n ) {
	case _VERB_G : RETVAL = Verb;		break;
	case _DOBJ_G : RETVAL = Dobj[ 0 ];	break;
	case _IOBJ_G : RETVAL = Iobj;		break;
	case _PREP_G : RETVAL = Prep;		break;
	case _CONJ_G : RETVAL = Conj[ 0 ];	break;
	case _NUMD_G : RETVAL = NumDobj;	break;
	default      :
#if DEBUG
	    if( (n < 0) || (n > sp) )
		error( 13 );
#endif
	    RETVAL = stack[ n ];
    }
}


setg()
{
    assertargs( "$setg", 2 );
    switch( ARG( 1 ) ) {
	case _VERB_G : Verb = ARG( 2 );		break;
	case _IOBJ_G : Iobj = ARG( 2 );		break;
	case _DOBJ_G : Dobj[ 0 ] = ARG( 2 ); 	break;
	case _PREP_G : Prep = ARG( 2 );		break;
	case _CONJ_G : Conj[ 0 ] = ARG( 2 );	break;
	case _NUMD_G : NumDobj = ARG( 2 );	break;
	default	 :
	    if( (ARG( 1 ) < 0)||(ARG( 1 ) > sp) )
		error( 14 );
	    stack[ ARG( 1 ) ] = ARG( 2 );
    }
    RETVAL = ARG( 2 );
}


saystr()
{
  int16 i;

  for( i = 1; i < RETVAL; i++ )
    sayer( virtstr( ARG( i ) ) );
}

/*** EOF adlmiscr.c ***/
