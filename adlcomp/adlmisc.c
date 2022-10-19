	/***************************************************************\
	*								*
	*	adlmisc.c - miscellaneous compiling routines.		*
	*	Copyright 1987 by Ross Cunniff.				*
	*								*
	\***************************************************************/

#include <stdio.h>

#include "adltypes.h"
#include "adlprog.h"
#include "adldef.h"
#include "adlcomp.h"

static	int16
    NUMADJ = 1,			/* Number of ADJECs found.		*/
    num_local;			/* Number of LOCALs found.		*/
extern	int16
    numprep;			/* Number of prep synonyms found.	*/


	/***************************************************************\
	*								*
	*	get_local() - read and process a possible LOCAL		*
	*	declaration.						*
	*								*
	\***************************************************************/

get_local()
{
    int16
	i;

    num_local = 0;
    if( t_type == LOCAL_D ) {
	/* We found the LOCAL token - get a LOCAL list */
	do {
	    /* Get the name of a local */
	    lexer();
	    if( t_type < 256 )
		_ERR_FIX( ILLEGAL_SYMBOL, '(' );
	    insert_local( token, LOCAL, num_local );

	    lexer();
	    if( t_type == '[' ) {
		/* The user wants an array.  Get the size. */
		lexer();
		if( t_type != CONST )
		    _ERR_FIX( CONST_EXPECTED, ';' );
		if( t_val < 1 )
		    _ERR_FIX( BAD_ARRAY, ';' );

		/* Create space on the stack for the locals */
		for( i = 0; i < t_val; i++ )
		    newcode( PUSH, 0L );
		num_local += t_val;

		/* Pick up the closing bracket */
		lexer();
		if( t_type != ']' )
		    _ERR_FIX( BRACKET_EXPECTED, ';' );

		/* Pick up the next token */
		lexer();
	    }
	    else {
		newcode( PUSH, 0L );	/* Allocate space for this local */
		num_local += 1;
	    }
	} while( t_type == ',' );

	/* The list should be terminated by a semicolon */
	if( t_type != ';' )
	    _ERR_FIX( SEMI_EXPECTED, '(' );

	/* Skip to the beginning of the routine */
	lexer();
    }
    if( num_local > 31 )
	error( "Number of locals must be less than 32.\n" );
}


	/***************************************************************\
	*								*
	*	unget_local() - forget all LOCALS declared in this	*
	*	routine.						*
	*								*
	\***************************************************************/

unget_local()
{
    del_locals();
}


	/***************************************************************\
	*								*
	*	getvars() - get a VAR declaration list.			*
	*								*
	\***************************************************************/
getvars()
{
    do {
	lexer();
	if( t_type != UNDECLARED )
	    _ERR_FIX( ILLEGAL_SYMBOL, ';' );
	insert_sys( token, VAR, NUMVAR );
	lexer();
	if( t_type == '[' ) {
	    /* The user wants an array.  Pick up the size. */
	    lexer();
	    if( t_type != CONST )
		_ERR_FIX( CONST_EXPECTED, ';' );
	    if( t_val < 1 )
		_ERR_FIX( BAD_ARRAY, ';' );
	    NUMVAR += t_val;

	    /* Pick up the closing bracket */
	    lexer();
	    if( t_type != ']' )
		_ERR_FIX( BRACKET_EXPECTED, ';' );

	    /* Pick up the comma */
	    lexer();
	}
	else
	    NUMVAR += 1;
    } while( t_type == ',' );
    if( t_type != ';' )
	_ERR_FIX( SEMI_EXPECTED, ';' );
}


	/***************************************************************\
	*								*
	*	getlist( t ) - Get a declaration list, such as		*
	*		ADJEC red, green, blue;				*
	*	and insert the tokens where they belong.		*
	*								*
	\***************************************************************/
  
getlist( t )
int16
    t;			/* The type of the declaration */
{
    int16
	*addr;		/* The address of the counter to be incremented */

    /* Get the address and the type of the tokens being declared. */
    switch( t ) {
	case VERB_D	: addr = &NUMVERB;	t = VERB;	break;
	case ROUT_D	: addr = &NUMROUT;	t = ROUTINE;	break;
	case ADJEC_D	: addr = &NUMADJ;	t = ADJEC;	break;
	case ART_D	: addr = 0;		t = ARTICLE;	break;
	case PREP_D	: addr = &numprep;	t = PREP;	break;
    }

    /* Actually declare the token list */
    do {
	lexer();
	if( t_type != UNDECLARED )
	    /* Only tokens not previously declared are allowed */
	    _ERR_FIX( ILLEGAL_SYMBOL, ';' );

	if( addr ) {
	    /* Declare the token and increment the counter */
	    if( t == ROUTINE )
		insert_sys( token, t, *addr );
	    else
		insertkey( token, t, *addr, 1 );
	    (*addr) += 1;
	}
	else {
	    /* Articles are just noise, no values are needed. */
	    insertkey( token, t, 0, 1 );
	}
	lexer();
    } while( t_type == ',' );
    if( t_type != ';' )
	_ERR_FIX( SEMI_EXPECTED, ';' );
}


	/***************************************************************\
	*								*
	*	getassign() - get an assignment statement like		*
	*		toolbox = tool box;				*
	*	or							*
	*		SEEN = 3;					*
	*								*
	\***************************************************************/

int16
getassign( do_insert )
int
    do_insert;
{
    char
	s[ 512 ];		/* Save area for the current token string */
    int16
	t_save,			/* Save area for the current token value */
	retval;

    if( do_insert ) {
	/* Save the current token (used to print an error message later) */
	retval = 0;
	strcpy( s, token );
    }

    /* Read the next token - it should be '=' */
    lexer();
    if( t_type != '=' )
	_ERR_FIX( EQUAL_EXPECTED, ';' );

    /* Read the value of the assignment statement */
    lexer();
    if( (t_type == ADJEC) || (t_type == VERB) ) {
	/* We may be reading a noun phrase */
	if( t_type == ADJEC )
	    t_save = t_val;
	else
	    t_save = -t_val;
	lexer();
	if( t_type == NOUN ) {
	    if( (t_save = noun_exists( t_save, t_val )) < 0 )
		_ERR_FIX( "Undeclared object.", ';' );
	    if( do_insert )
		insertkey( s, NOUN_SYN, t_save, 1 );
	    else
		retval = t_save;
	    lexer();
	}
	else if( t_type == ';' ) {
	    /* This was just foo = ADJEC or foo = VERB */
	    if( do_insert ) {
		if( t_save < 0 )
		    insertkey( s, VERB, -t_save, 0 );
		else
		    insertkey( s, ADJEC, t_save, 0 );
	    }
	    else {
		if( t_save < 0 )
		    retval = -t_save;
		else
		    retval = t_save;
	    }
	}
	else
	    _ERR_FIX( SEMI_EXPECTED, ';' );
    }
    else if( t_type == NOUN ) {
	/* Unmodified object */
	if( (t_save = noun_exists( 0, t_val )) < 0 )
	    _ERR_FIX( ATTEMPT, ';' );
	if( do_insert )
	    insertkey( s, NOUN_SYN, t_save, 1 );
	else
	    retval = t_save;
	lexer();
    }
    else if( (t_type >= MIN_LEGAL) && (t_type <= MAX_LEGAL) ) {
        /* We're reading a simple synonym */
	if( do_insert ) {
	    if( (t_type >= MIN_RT) && (t_type <= MAX_RT) && (t_type != STRING) )
		insertkey( s, t_type, t_val, 0 );
	    else
		insert_sys( s, t_type, t_val );
	}
	else
	    retval = t_val;
	lexer();
    }
    else if( (t_type == '(') || (t_type == LOCAL_D) ) {
	/* We're creating a routine */
	if( do_insert )
	    insert_sys( s, ROUTINE, NUMROUT );
	else
	    retval = NUMROUT;
	routspace[ NUMROUT++ ] = currcode();
	get_local();
	getroutine( 1 );
	newcode( RET, 0L );
	unget_local();
    }
    if( t_type != ';' )
	_ERR_FIX( SEMI_EXPECTED, ';' );
    return retval;
}

	/***************************************************************\
	*								*
	*	adlrout( t_read ) - read in an ADL routine and return	*
	*	its starting address.  If t_read is false, the initial	*
	*	'=' [ locals ] '(' sequence is read from the input	*
	*	 stream.						*
	*								*
	\***************************************************************/

address
adlrout( t_read )
int16
    t_read;
{
    address
	t;

    if( !t_read ) {
	lexer();
	if( t_type != '=' )
	    _ERR_FX0( EQUAL_EXPECTED, ';' );
	lexer();
	if( (t_type != '(') && (t_type != LOCAL_D) )
	    _ERR_FX0( LEFT_EXPECTED, ';' );
    }
    t = currcode();
    get_local();
    getroutine( 1 );
    newcode( RET, 0L );
    unget_local();
    if( t_type != ';' )
	_ERR_FX0( SEMI_EXPECTED, ';' );
    return t;
}


	/***************************************************************\
	*								*
	*	getverb() - handle things like				*
	*		north(PREACT) = ($setg GOVERB TRUE);		*
	*	or							*
	*		north ball(WEIGH) = 300;			*
	*								*
	\***************************************************************/

getverb()
{
    int16
	 verb,		/* The verb which is under consideration */
	 val,		/* The property which is being assigned */
	 rval;		/* The 16-bit value of the RHS of the expression */

    verb = t_val;
    lexer();
    if( t_type == NOUN ) {
	/* This is actually a noun assignment */
	nounassign( 2, -verb );
	return;
    }
    else if( t_type == PREP ) {
	/* This is actually a VERB PREP = VERB statement */
	getverbsyn( verb );
	return;
    }

    /* This is a verb assignment.  Get the property number. */
    if( t_type != '(' )
	_ERR_FIX( LEFT_EXPECTED, ';' );
    lexer();
    if( t_type != CONST )
	_ERR_FIX( CONST_EXPECTED, ';' );
    if( (t_val != _PREACT) && (t_val != _ACT) )
	_ERR_FIX( "PREACT or ACTION expected.\n", ';' );
    val = t_val;
    lexer();
    if( t_type != ')' )
	_ERR_FIX( RIGHT_EXPECTED, ';' );

    /* Get the RHS of the expression - it must be a routine ID or a routine */
    lexer();
    if( t_type != '=' )
	_ERR_FIX( EQUAL_EXPECTED, ';' );
    lexer();
    if( t_type == ROUTINE ) {
	rval = t_val;
	lexer();
	if( t_type != ';' )
	    _ERR_FIX( SEMI_EXPECTED, ';' );
    }
    else if( (t_type == '(') || (t_type == LOCAL_D) ) {
	rval = NUMROUT;
	routspace[ NUMROUT++ ] = adlrout( 1 );
    }
    else
	_ERR_FIX( "Routine expected", ';' );

    /* Put the RHS into the proper property. */
    switch( val ) {
	case _PREACT :
	    if( verbspace[ verb ].preact )
		warning( "verb( PREACT ) already assigned.\n" );
	    verbspace[ verb ].preact = rval;
	    break;
	case _ACT :
	    if( verbspace[ verb ].postact )
		warning( "verb( ACTION ) already assigned.\n" );
	    verbspace[ verb ].postact = rval;
	    break;
    }
}


	/***************************************************************\
	*								*
	*	globassign() - Read and process a statement such as	*
	*		(MaxScore) = 400;				*
	*								*
	\***************************************************************/

globassign()
{
    int16
	v;	/* The actual variable id */

    /* Read the variable */
    lexer();
    if( t_type != VAR )
	_ERR_FIX( VAR_EXPECTED, ';' );
    v = t_val;

    lexer();
    if( t_type == '+' ) {
	/* The user wants to assign into an array.  Get the offset. */
	lexer();
	if( t_type != CONST )
	    _ERR_FIX( CONST_EXPECTED, ';' );
	v += t_val;

	/* Pick up the closing parenthesis */
	lexer();
    }

    if( t_type != ')' )
	_ERR_FIX( RIGHT_EXPECTED, ')' );

    if( varspace[ v ] )
	warning( "Re-assignment of VAR value.\n" );

    /* Get the RHS and stuff it into the correct place. */
    varspace[ v ] = getassign( 0 );
}


	/***************************************************************\
	*								*
	*	routassign() - handle the assignment of an actual	*
	*	routine to a previously declared routine ID.		*
	*								*
	\***************************************************************/

routassign()
{
    int16
	v;	/* The routine ID */

    v = t_val;
    if( routspace[ v ] )
	warning( "Re-assignment of ROUTINE value.\n" );
    routspace[ v ] = adlrout( 0 );
}


	/***************************************************************\
	*								*
	*	prepassign() - handle constructs like			*
	*		in front of = before;				*
	*								*
	\***************************************************************/

prepassign()
{
    int16
	adj;

    /* Save the value of the first preposition */
    prepspace[ NUMPP ].first = t_val;

    /* Get the middle part of the phrase */
    lexer();
    if( t_type == NOUN_SYN ) {
	prepspace[ NUMPP ].obj = t_val;
	lexer();
    }
    else if( t_type != PREP ) {
	if( t_type == VERB ) {
	    adj = -t_val;
	    lexer();
	}
	else if( t_type == ADJEC ) {
	    adj = t_val;
	    lexer();
	}
	else
	    adj = 0;
	if( t_type != NOUN )
	    _ERR_FIX( NOUN_WANTED, ';' );
	if( (prepspace[ NUMPP ].obj = noun_exists( adj, t_val )) < 0 )
	    _ERR_FIX( "Illegal object.\n", ';' );
	lexer();
    }

    /* Get the second preposition and save it */
    if( t_type != PREP )
	_ERR_FIX( PREP_EXPECTED, ';' );
    prepspace[ NUMPP ].last = t_val;

    /* Get the '=' PREP ';' */
    lexer();
    if( t_type != '=' )
	_ERR_FIX( EQUAL_EXPECTED, '=' );
    lexer();
    if( t_type != PREP )
	_ERR_FIX( PREP_EXPECTED, ';' );
    prepspace[ NUMPP ].val = t_val;
    lexer();
    if( t_type != ';' )
	_ERR_FIX( SEMI_EXPECTED, ';' );
    NUMPP++;
}


	/***************************************************************\
	*								*
	*	getverbsyn( verb ) - get a statement like		*
	*		put on = wear;					*
	*								*
	\***************************************************************/

getverbsyn( verb )
int16
    verb;
{
    /* At this point, we have VERB PREP */
    verbsyn[ NUMVS ].vrb = verb;
    verbsyn[ NUMVS ].prp = t_val;

    /* Get the equals sign */
    lexer();
    if( t_type != '=' )
	_ERR_FIX( EQUAL_EXPECTED, ';' );

    /* Get the following verb */
    lexer();
    if( t_type != VERB )
	_ERR_FIX( "VERB expected.\n", ';' );
    verbsyn[ NUMVS++ ].val = t_val;

    /* Get the closing semicolon */
    lexer();
    if( t_type != ';' )
	_ERR_FIX( SEMI_EXPECTED, ';' );
}

/*** EOF adlmisc.c ***/
