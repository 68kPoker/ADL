	/***************************************************************\
	*								*
	*	adlobj.c - routines dealing with the compilation of	*
	*	object declaractions and assigments.			*
	*	Copyright 1987 by Ross Cunniff.				*
	*								*
	\***************************************************************/

#include <stdio.h>

#include "adltypes.h"
#include "adlprog.h"
#include "adldef.h"
#include "adlcomp.h"


	/***************************************************************\
	*								*
	*	noun_exists( adj, noun ) - returns an object ID iff	*
	*	an object exists with noun ID noun and adjective id	*
	*	adj.  Returns -1 otherwise.				*
	*								*
	\***************************************************************/

int16
noun_exists( adj, noun )
int16
    adj,
    noun;
{
    int16
	t;

    for( t = nounspace[ noun ]; t != 0; t = objspace[ t ].others )
	if( objspace[ t ].adj == adj )
	    return t;
    return -1;
}


	/***************************************************************\
	*								*
	*	move_obj( obj, dest ) - moves object obj to object	*
	*	dest, taking care to put obj at the END of the list	*
	*	of objects contained in dest.				*
	*								*
	\***************************************************************/

move_obj( obj, dest )
int16
    obj, dest;
{
    int16
	t;

    /* Find the object whose link is obj (if any) */
    t = objspace[ obj ].loc;
    if( objspace[ t ].cont != obj ) {
	/* Obj must be a link */
	t = objspace[ t ].cont;
	while( objspace[ t ].link != obj )
	    t = objspace[ t ].link;
	objspace[ t ].link = objspace[ objspace[ t ].link ].link;
    }
    else
	/* Obj is the head of the list. */
	objspace[ t ].cont = objspace[ objspace[ t ].cont ].link;

    /* Seek to the end of the list and place obj there. */
    objspace[ obj ].loc = dest;
    t = objspace[ dest ].cont;
    objspace[ obj ].link = 0;
    if( t ) {
	/* Jump to the end of the list */
	while( objspace[ t ].link )
	    t = objspace[ t ].link;
	objspace[ t ].link = obj;
    }
    else
	/* There is nothing in the list. */
	objspace[ dest ].cont = obj;
}


	/***************************************************************\
	*								*
	*	new_noun( mod, noun ) - create a new object with	*
	*	modifier list mod and noun ID noun.  The object is	*
	*	initially placed in .ALL.				*
	*								*
	\***************************************************************/

int16
new_noun( mod, noun )
int16
    mod, noun;
{
    int16
	obj;

    obj = NUMOBJ++;
    objspace[ obj ].others = nounspace[ noun ];
    nounspace[ noun ] = obj;
    objspace[ obj ].loc = _ALL;
    objspace[ obj ].link = objspace[ _ALL ].cont;
    objspace[ _ALL ].cont = obj;
    objspace[ obj ].adj = mod;
    objspace[ obj ].noun = noun;
    return obj;
} 


	/***************************************************************\
	*								*
	*	getnew( t_read ) - read a new modified object from the	*
	*	token stream and return its id.	t_read is nonzero	*
	*	iff the first token in the list has already been read.	*
	*								*
	\***************************************************************/

int16
getnew( t_read )
int16
    t_read;
{
    int16
	t_save;

    if( !t_read )
	lexer();
    if( t_type == VERB ) {
	t_val = -t_val;
	t_type = ADJEC;
    }
    if( t_type == ADJEC ) {
	t_save = t_val;
	lexer();
	if( t_type == UNDECLARED ) {
	    insertkey( token, NOUN, NUMNOUN, 1 );
	    return new_noun( t_save, NUMNOUN++ );
	}
	else if( t_type == NOUN ) {
	    if(		(noun_exists( t_save, t_val ) >= 0) ||
			(noun_exists( 0, t_val ) >= 0) )
		error( "Attempt to redeclare a noun.\n" );
	    else
		return new_noun( t_save, t_val );
	}
	else
	    error( NOUN_WANTED );
    }
    else if( t_type == UNDECLARED ) {
	insertkey( token, NOUN, NUMNOUN, 1 );
	return new_noun( 0, NUMNOUN++ );
    }
    else
	error( ILLEGAL_SYMBOL );
    return -1;
}


	/***************************************************************\
	*								*
	*	getold( t_read, t_save ) - read a previously declared	*
	*	object from the token string.  t_read is the number	*
	*	of tokens already read, and t_save is the value of the	*
	*	previous token (if such exists).			*
	*								*
	\***************************************************************/

int16
getold( t_read, t_save )
int16
    t_read,
    t_save;
{
    if( t_read == 0 )
	lexer();
    if( t_read != 2 ) {
	if( t_type == NOUN_SYN )
	    return t_val;
	if( t_type == VERB ) {
	    t_type = ADJEC;
	    t_val = -t_val;
	}
	if( t_type == ADJEC ) {
	    t_save = t_val;
	    lexer();
	}
    }
    if( t_type != NOUN ) {
	error( ILLEGAL_SYMBOL );
	return -1;
    }
    if( (t_save = noun_exists( t_save, t_val )) < 0 )
	error( ATTEMPT );
    return t_save;
}


	/***************************************************************\
	*								*
	*	setprop( obj, which, val ) - set the which'th property	*
	*	of object obj to be val.				*
	*								*
	\***************************************************************/

setprop( obj, which, val )
int16
    obj,
    which,
    val;
{
    static char
	*ALREADY = "Noun property already assigned.\n";

    if( (which >=1) && (which <= 16) ) {
	/* Boolean property */
	if( objspace[ obj ].props1to16 & bitpat[ which - 1 ] )
	    warning( ALREADY );
	if( val )
	    objspace[ obj ].props1to16 |= bitpat[ which - 1 ];
	else
	    objspace[ obj ].props1to16 &= ibitpat[ which - 1 ];
    }
    else if( (which >= 17) && (which <= _ACT ) ) {
	if( objspace[ obj ].props[ which - 17 ] )
	    warning( ALREADY );
	objspace[ obj ].props[ which - 17 ] = val;
    }
    else
	error( "Invalid object property number.\n" );
}
  

/***************************************************************\
*								*
*	nounassign( t_read, t_save ) - parse and interpret a	*
*	noun property assignment.				*
*								*
\***************************************************************/

nounassign( t_read, t_save )
int16
    t_read,
    t_save;
{
    int16
	obj,
	which,
	getassign();

    obj = getold( t_read, t_save );
    lexer();
    if( t_type != '(' )
	_ERR_FIX( LEFT_EXPECTED, ';' );
    lexer();
    if( t_type != CONST )
	_ERR_FIX( CONST_EXPECTED, ';' );
    which = t_val;
    lexer();
    if( t_type != ')' )
	_ERR_FIX( RIGHT_EXPECTED, ';' );
    setprop( obj, which, getassign( 0 ) );
}


	/***************************************************************\
	*								*
	*	getnouns() - parse and interpret a NOUN declaration.	*
	*								*
	\***************************************************************/

getnouns()
{
    int16
	obj, loc;

    while( t_type != ';' ) {
	if( (obj = getnew( 0 )) >= 0 ) {
	    lexer();
	    if( t_type == '(' ) {
		loc = getold( 0, 0 );
		move_obj( obj, loc );
		lexer();
		if( t_type != ')' )
		    _ERR_FIX( RIGHT_EXPECTED, ';' );
		lexer();
	    }
	    else if( (t_type != ',') && (t_type != ';') )
		_ERR_FIX( COMMA_EXPECTED, ';' );
	}
	else
	    eatuntil( ';' );
    }
}

/*** EOF adlobj.c ***/
