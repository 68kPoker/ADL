#include <stdio.h>

#include "adltypes.h"
#include "adlprog.h"
#include "adlrun.h"


int16
dwimmer( rp, obj )
int16
    rp,
    obj;
{
    bp = sp = NUMVAR;			/* Trim the stack */
    push( 2 );				/* Push the stackdepth */
    push( obj );			/* Push the object */
    push( NUMVAR );			/* Push the bp */
    ip = 0;
    puship();				/* Push the ip */
    if( !(ip = routspace[ rp ]) ) {
	/* No dwimmer, so we fail. */
	bp = sp = NUMVAR;
	return 0;
    }
    runprog();				/* Run the dwimmer */
    return stack[ NUMVAR ];		/* Return the retval */
}


dwimerror( n )
int16
    n;
{
    switch( n ) {
	case 1 :
	    sayer( "You will have to be more specific.\n" );
	    break;
	case 2 :
	    sayer( "You don't see anything like that.\n" );
	    break;
    }
}


int16
dwimadj( which, adj )
int16
    which,
    adj;
{
    int16
	i, tsave;

    tsave = 0;
    for( i = 0; i < NUMOBJ; i++ ) {
	if( objspace[ i ].adj == adj ) {
	    if( dwimmer( which, i ) ) {
		if( tsave ) {
		    dwimerror( 1 );
		    return -1;
		}
		else
		    tsave = i;
	    }
	}
    }

    if( !tsave ) {
	dwimerror( 2 );
	return -1;
    }
    else
	return tsave;
}


int16
dwimnoun( which, noun )
int16
    which,
    noun;
{
    int16
	i, tsave;

    tsave = 0;
    for( i = nounspace[ noun ]; i != 0; i = objspace[ i ].others ) {
	if( dwimmer( which, i ) ) {
	    if( tsave ) {
		dwimerror( 1 );
		return -1;
	    }
	    else
		tsave = i;
	}
    }

    if( !tsave ) {
	dwimerror( 2 );
	return -1;
    }
    else
	return tsave;
}


dwimi()
{
    int16
	t;

    if( SET_EXIT( 0 ) != 0 )
	return -1;

    if( Inoun < 0 ) {
	/* Copy the string index */
	Iobj = Inoun;
	return 0;
    }
    if( (Inoun == 0) && (Imod == 0) )
	/* Preserve the old Iobj */
	return 0;
    if( Imod != 0 ) {
	/* The player typed a modifier */
	if( Inoun ) {
	    /* The player also typed a noun */
	    if( (Iobj = noun_exists( Imod, Inoun )) < 0 ) {
		/* Oops - no such mod-noun pair */
		dwimerror( 2 );
		return -1;
	    }
	}
	else {
	    /* The player typed no noun */
	    if( (Iobj = dwimadj( _DWIMI, Imod )) < 0 )
		/* Oops - the modifier wasn't sufficient */
		return -1;
	}
    }
    else {
	/* The player typed a noun, but no modifier */
	if( (t = noun_exists( 0, Inoun )) < 0 ) {
	    /* There is no unmodified object with noun Inoun */
	    if( (Iobj = dwimnoun( _DWIMI, Inoun )) < 0 )
		/* Oops - the noun wasn't sufficient */
		return -1;
	}
	else
	    /* Everything is fine. */
	    Iobj = t;
    }
    return 0;
}


dwimd()
{
    int16
	i, t;

    if( SET_EXIT( 0 ) != 0 )
	return -1;

    for( i = 0; i < NumDobj; i++ ) {
	if( Dnoun[ i ] < 0 ) {		/* User typed a string */
	    Dobj[ i ] = Dnoun[ i ];
	    continue;
	}
	if( Dmod[ i ] != 0 ) {
	    /* The player typed a modifier */
	    if( Dnoun[ i ] ) {
		/* The player typed a noun */
		if( (Dobj[ i ] = noun_exists( Dmod[ i ], Dnoun[ i ] )) < 0 ) {
		    /* No such mod-noun pair exists */
		    dwimerror( 2 );
		    return -1;
		}
	    }
	    else if( (Dobj[ i ] = dwimadj( _DWIMD, Dmod[ i ] )) < 0 ) {
		return -1;
	    }
	}
	else if( (t = noun_exists( 0, Dnoun[ i ] )) < 0 ) {
	    /* An object with noun Dnoun[ i ] and mod 0 doesn't exist */
	    if( (Dobj[ i ] = dwimnoun( _DWIMD, Dnoun[ i ] )) < 0 ) {
		return -1;
	    }
	}
	else
	    /* An object with noun Dnoun[ i ] and mod 0 DOES exist */
	    Dobj[ i ] = t;
    }
    return 0;
}

/*** EOF adldwim.c ***/
