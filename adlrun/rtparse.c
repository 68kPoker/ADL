#include <stdio.h>

#include "adltypes.h"
#include "adlprog.h"
#include "adldef.h"
#include "adlrun.h"

#define FIN  100	/* Final state */
#define XXX -100	/* Default action in action table */
#define ERR -100	/* Error state in transit table */

int16
    state,		/* Current state for finite state machine */
    Tnoun,		/* Temporary storage for current noun. */
    Tmod,		/* Temporary storage for current adjective. */
    P1, P2,		/* Temporary storage for prepositions */
    C1;			/* Temporary storage for the last CONJ typed. */
char
    kludge[ SLEN ],	/* Static area for s */
    s_str[ SLEN ];	/* End-of-string save for TELLER */


int16	trantab[ 15 ][ 10 ] = {
/*
 *		Transit table for state machine
 *
 *				   NOUN
 *	  PREP VERB STR  ADJ  NOUN SYN  ART  CONJ ","  SEP
 */
/* 0 */ {   4,   1,   2,  12,  12,  12,  12, ERR, ERR, FIN },
/* 1 */ {   4,   2,   2,   2,   2,   2,   2, ERR, ERR, FIN },
/* 2 */ {   4,   3,   3,   3,   3,   3,   3,   5,   5, FIN },
/* 3 */ {  11, ERR, ERR, ERR, ERR, ERR, ERR,   6,   6, FIN },
/* 4 */ {   9,   7,   7,   7,   7,   7,   7, ERR, ERR, FIN },
/* 5 */ { ERR,   8,   8,   8,   8,   8,   8,   5,   5, ERR },
/* 6 */ { ERR,   3,   3,   3,   3,   3,   3,   6,   6, ERR },
/* 7 */ {   9, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, FIN },
/* 8 */ {   4, ERR, ERR, ERR, ERR, ERR, ERR,   5,   5, FIN },
/* 9 */ { ERR,  10,  10,  10,  10,  10,  10, ERR, ERR, FIN },
/*10 */ {  11, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, FIN },
/*11 */ { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, FIN },
/*12 */ {   4,   3,   3,   3,   3,   3,   3,   5,  13, FIN },
/*13 */ { ERR,  14,  14,   8,   8,   8,   8, ERR, ERR, ERR },
/*14 */ { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, FIN } };

int16	actions[ 15 ][ 10 ] = {
/*
 *		Action table for state machine
 *				   NOUN
 *	  PREP VERB STR  ADJ  NOUN SYN  ART  CONJ ","  SEP
 */
/* 0 */ {   2,   0,   1,   1,   1,   1,   1, 500, 500, XXX },
/* 1 */ {   3,   1,   1,   1,   1,   1,   1, 501, 501, XXX },
/* 2 */ {   7,   5,   5,   5,   5,   5,   5,   6,   6,   4 },
/* 3 */ {   2, 502, 502, 502, 502, 502, 502,   8,   8, XXX },
/* 4 */ {   9,   1,   1,   1,   1,   1,   1, 501, 501,  10 },
/* 5 */ { 501,  11,  11,  11,  11,  11,  11,   8,   8, 501 },
/* 6 */ { 501,  11,  11,  11,  11,  11,  11,   8,   8, 501 },
/* 7 */ {  13, 502, 502, 502, 502, 502, 502, 502, 502,  12 },
/* 8 */ {   2, 502, 502, 502, 502, 502, 502,   8,   8, XXX },
/* 9 */ { 501,  15,  15,  15,  15,  15,  15, 501, 501,  14 },
/*10 */ {   2, 502, 502, 502, 502, 502, 502, 502, 502, XXX },
/*11 */ { 503, 503, 503, 503, 503, 503, 503, 503, 503,  10 },
/*12 */ {   7,   5,   5,   5,   5,   5,   5,   6,  16,   4 },
/*13 */ { 501,  19,  18,  17,  17,  17,  17, 501, 501, 501 },
/*14 */ { 503, 503, 503, 503, 503, 503, 503, 503, 503, XXX } };

static
p_error( n )
int
    n;
{
    char
	str[ 200 ];

    switch( n ) {
	case -3  :
	    sprintf( str,
			"I got confused trying to expand \"%s\" to \"%s\"\n",
			 s, xp );
	    break;
	case -2  :
	    sprintf( str, "\"%s\" is not a valid abbreviation.\n", s );
	    break;
	case -1  :
	    sprintf( str, "I don't know the word \"%s\".\n", s );
	    break;
	case 500 :
	    sprintf( str, "\"%s\" is not a verb.\n", s );
	    break;
	case 501 :
	    sprintf( str, "\"%s\" is not part of a noun phrase.\n", s );
	    break;
	case 502 :
	    sprintf( str, "\"%s\" is not a preposition.\n", s );
	    break;
	case 503 :
	    sprintf( str, "End of sentence expected.\n" );
	    break;
	case 504 :
	    sprintf( str, "Illegal multiple word verb phrase.\n" );
	    break;
	case 505 :
	    sprintf( str, "Too many direct objects.\n" );
	    break;
	case 506 :
	    sprintf( str, "Illegal multiple word preposition.\n" );
	    break;
	default  :
	    sprintf( str, "I don't understand that.\n", n );
    }
    sayer( str );
    state = ERR;
}


static
Get_Noun()
{
    Tmod = 0;
    Tnoun = 0;

    /* Check to see whether the object is a string */
    if( t_type == STRING ) {
	Tnoun = t_val;
	return 1;
    }

    /* The object is not a string.  Find a noun phrase */

    /* Skip the article */
    if( t_type == ART )
	gettoken( 0 );

    /* Check to see whether we have a full-fledged object. */
    if( t_type == NOUN_SYN ) {
	Tnoun = objspace[ t_val ].noun;
	Tmod = objspace[ t_val ].adj;
	return 1;
    }

    /* Check for a modifier (adjective or verb) */
    if( t_type == ADJEC ) {
	Tmod = t_val;
	gettoken( 0 );
    }
    else if( t_type == VERB ) {
	Tmod = -t_val;
	gettoken( 0 );
    }

    /* Check for the noun */
    if( t_type == NOUN ) {
	Tnoun = t_val;
	return 1;
    }

    if( t_type < 0 ) {
	p_error( t_type );	/* Dictionary error */
	return 0;
    }
    else if( (Tmod == 0) && (Tnoun == 0) ) {
	p_error( 501 );		/* S is not a noun. */
	return 0;
    }
    else {
	read_t = 0;		/* Skip the next token */
	return 1;		/* Found adjective, but no noun, */
    }				/* so we'll leave it up to the dwimmer to figure
				   it out */
}


Find_PP( Prep1, Mod, Noun, Prep2 )
{
    int16
	i,
	obj;

    if( Noun || Mod ) {
	if( (obj = noun_exists( Mod, Noun )) < 0 )
	    return -1;
    }
    else
	obj = 0;
    for( i = 0; i < NUMPP; i++ )
	if(	prepspace[ i ].first == Prep &&
	    	prepspace[ i ].obj   == obj &&
	    	prepspace[ i ].last  == t_val )
	{
	    return prepspace[ i ].val;
        }
    return -1;
}


Find_VP( verb, prep )
int16
    verb, prep;
{
    int16
	i;

    for( i = 0; i < NUMVS; i++ )
	if( (verb == verbsyn[ i ].vrb) && (prep == verbsyn[ i ].prp) )
	    return verbsyn[ i ].val;
    return -1;
}


Perform_Action( which )
int
    which;
{
    int16
	x;			/* Temporary used for lookups */

    switch( which ) {
	case  0 :
	    Verb = t_val;
	    break;
	case  1 :
	    Get_Noun();
	    break;
	case  2 :
	    P1 = t_val;
	    break;
	case  3 :
	    if( (x = Find_VP( Verb, t_val )) >= 0 ) {
		Verb = x;
		state = 1;
	    }
	    else
		P1 = t_val;
	    break;
	case  4 :
	    Dmod[ 0 ] = Tmod;	Dnoun[ 0 ] = Tnoun;	Conj[ 0 ] = 0;
	    NumDobj = 1;
	    break;
	case  5 :
	    Imod = Tmod;	Inoun = Tnoun;
	    Get_Noun();
	    Dmod[ 0 ] = Tmod;	Dnoun[ 0 ] = Tnoun;	Conj[ 0 ] = 0;
	    NumDobj = 1;
	    break;
	case  6 :
	    Dmod[ 0 ] = Tmod;	Dnoun[ 0 ] = Tnoun;	Conj[ 1 ] = t_val;
	    NumDobj = 1;
	    break;
	case  7 :
	    Dmod[ 0 ] = Tmod;	Dnoun[ 0 ] = Tnoun;	Conj[ 0 ] = 0;
	    NumDobj = 1;
	    P1 = t_val;
	    break;
	case  8 :
	    Conj[ NumDobj ] = t_val;
	    break;
	case  9 :
	    P2 = t_val;	Tmod = Tnoun = 0;
	    break;
	case 10 :
	    if( (x = Find_VP( Verb, P1 )) >= 0 )
		Verb = x;
	    else
		p_error( 504 );		/* Illegal verb phrase */
	    break;
	case 11 :
	    if( NumDobj >= NUMDO ) {
		p_error( 505 );		/* Too many direct objects */
		break;
	    }
	    Get_Noun();
	    Dmod[ NumDobj ] = Tmod;	Dnoun[ NumDobj ] = Tnoun;
	    NumDobj++;
	    break;
	case 12 :
	    Imod = Tmod;		Inoun = Tnoun;
	    Prep = P1;
	    break;
	case 13 :
	    P2 = t_val;
	    break;
	case 14 :
	    if( (Tnoun != 0) || (Tmod != 0) ) {
		Imod = Tmod;	Inoun = Tnoun;	Prep = P1;
		if( (x = Find_VP( Verb, P2 )) >= 0 )
		    Verb = x;
		else
		    p_error( 504 );	/* Illegal verb phrase */
	    }
	    else {
		if( (x = Find_PP( P1, Tmod, Tnoun, P2 )) >= 0 ) {
		    P1 = x;
		    if( (x = Find_VP( Verb, P1 )) >= 0 )
			Verb = x;
		    else
			p_error( 504 );	/* Illegal verb phrase */
		}
		else
		    p_error( 506 );	/* Illegal prep phrase */
	    }
	    break;
	case 15 :
	    if( (x = Find_PP( P1, Tmod, Tnoun, P2)) >= 0 )
		Prep = x;
	    else {
		p_error( 506 );		/* Illegal prep phrase */
		break;
	    }
	    Get_Noun();
	    Imod = Tmod;	Inoun = Tnoun;
	    break;
	case 16 :
	    C1 = t_val;
	    strcpy( s_str, PSTRING );
	    break;
	case 17 :
	    Dmod[ 0 ] = Tmod;	Dnoun[ 0 ] = Tnoun;	Conj[ 0 ] = 0;
	    Conj[ 1 ] = C1;
	    Get_Noun();
	    Dmod[ 1 ] = Tmod;	Dnoun[ 1 ] = Tnoun;	NumDobj = 2;
	    break;
	case 18 :
	    Imod = Tmod;	Inoun = Tnoun;
	    Dmod[ 0 ] = 0;	Dnoun[ 0 ] = t_val;	NumDobj = 1;
	    Verb = _TELLER;
	    break;
	case 19 :
	    Imod = Tmod;	Inoun = Tnoun;
	    Dmod[ 0 ] = 0;	Dnoun[ 0 ] = newtstr( s_str );	NumDobj = 1;
	    Verb = _TELLER;
	    *PSTRING = '\0';
	    break;
    }
}


initvars()
{
    int
	i;

    read_t = 1;
    state = 1;	/* Initial state */
    s = kludge;
    NumDobj = Prep = Iobj = Inoun = Imod = Verb = 0;
    for( i=0; i < NUMDO; i++ )
	Dobj[ i ] = Conj[ i ] = Dnoun[ i ] = Dmod[ i ] = 0;
}


parse()
{
    int16
	done,
	act;

    done = 0;	read_t = 1;	state = 0;
    s = kludge;	Tmod = Tnoun = P1 = P2 = C1 = 0;
    while( !done ) {
	gettoken( 0 );
	if( t_type < 0 )
	    p_error( t_type );		/* Dictionary error */
	else {
	    act   = actions[ state ][ t_type - MIN_RT ];
	    state = trantab[ state ][ t_type - MIN_RT ];
	    if( state == ERR )
		p_error( act );
	    else
		Perform_Action( act );
	}
	if( state == FIN )
	    done = 2;
	else if( state == ERR )
	    done = 1;
    }	/* while */
    return done - 1;
}	/* parse */

/*** EOF rtparse.c ***/
