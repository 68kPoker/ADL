	/***************************************************************\
	*								*
	*	routcomp.c - routines to compile ADL routines.		*
	*	Copyright 1987 by Ross Cunniff.				*
	*								*
	\***************************************************************/

#include <stdio.h>

#include "adltypes.h"
#include "adlprog.h"
#include "builtins.h"
#include "adldef.h"
#include "adlcomp.h"


int16
    inrout;			/* Are we inside a routine?	*/
extern int16
    filenum;			/* Current file number		*/

int16	getargs();	/* Forward declaration for daisy chaining	*/


char	*UNDEC_ID = "Undeclared identifier\n";


	/***************************************************************\
	*								*
	*	getglob() - generate the instruction for the '@glob'	*
	*	construct.						*
	*								*
	\***************************************************************/

getglob()
{
    lexer();
    newcode( PUSH, _GLOBAL );
    if( t_type == VAR )
	newcode( PUSH, t_val );
    else if( t_type == LOCAL )
	newcode( PUSHLOCL, t_val );
    else if( t_type == ARGUMENT )
	newcode( PUSHARG, t_val );
    else if( t_type == UNDECLARED )
	error( UNDEC_ID );
    else
	error( "Illegal '@'.\n" );
    newcode( CALL, 2L );
}


	/***************************************************************\
	*								*
	*	getpair() - generate code for the '[ mod noun ]'	*
	*	construct.						*
	*								*
	\***************************************************************/

getpair()
{
    int16
	getold();

    newcode( PUSH, getold( 0, 0 ) );
    lexer();
    if( t_type != ']' )
	_ERR_FIX( BRACKET_EXPECTED, ']' );
}


	/***************************************************************\
	*								*
	*	getexpr( t_read ) - generates code for an ADL routine	*
	*	expression.  t_read is 0 if a token has not already	*
	*	been read.						*
	*								*
	\***************************************************************/

getexpr( t_read )
int16
    t_read;
{
    if( !t_read )
	lexer();
    if( t_type == '(' )
	getform();
    else if( t_type == '@' )
	getglob();
    else if( t_type == '[' )
	getpair();
    else if( t_type == ARGUMENT )
	newcode( PUSHARG, t_val );
    else if( t_type == LOCAL )
	newcode( PUSHLOCL, t_val );
    else if( t_type == MYVAL )
	newcode( PUSHME, 0 );
    else if( t_type == NOUN) {
	if( (t_val = noun_exists( 0, t_val )) < 0 )
	    error( ATTEMPT );
	else
	    newcode( PUSH, t_val );
    }
    else if( (t_type >= MIN_LEGAL) && (t_type <= MAX_LEGAL) )
	newcode( PUSH, t_val );
    else if( t_type == UNDECLARED )
	error( UNDEC_ID );
    else
	error( ILLEGAL_SYMBOL );
}


	/***************************************************************\
	*								*
	*	getform() - get a routine form such as			*
	*		(name arg arg...)				*
	*	or							*
	*		(IF expr THEN arg arg ... ELSEIF ......)	*
	*	or							*
	*		(WHILE expr DO arg arg ...)			*
	*								*
	\***************************************************************/

getform()
{
    int16
	t_save,
	getargs();

    lexer();
    if( t_type == IF )
	getif();
    else if( t_type == WHILE )
	getwhile();
    else {
	t_save = t_type;
	if( t_type == UNDECLARED )
	    _ERR_FIX( UNDEC_ID, ')' )	/* Note - no semicolon! */
	else if( t_type == '(' )
	    getform();
	else if( t_type == ARGUMENT )
	    newcode( PUSHARG, t_val );
	else if( t_type == '@' )
	    getglob();
	else
	    newcode( PUSH, t_val );
	lexer();
	if(	(t_save == '@') || (t_save == ROUTINE) ||
		(t_save == '(') || (t_save == ARGUMENT) )
	    newcode( CALL, getargs() + 1 );
	else
	    error( "Illegal function call.\n" );
    }
}


	/***************************************************************\
	*								*
	*	getwhile() - generate code for the WHILE form.		*
	*								*
	\***************************************************************/

getwhile()
{
    address
	topaddr,
	breakaddr;

    topaddr = currcode();	/* Top of loop */
    getexpr( 0 );		/* Conditional */

    breakaddr = newcode( JMPZ, 0 );	/* If 0 then exit loop */
    newcode( POP, 0 );		/* Pop the condition code */

    lexer();
    if( t_type != DO )
	error( "'DO' expected in WHILE loop.\n" );

    getroutine( 0 );
    if( t_type != ')' )
	_ERR_FIX( RIGHT_EXPECTED, ')' );
    newcode( POP, 0 );
    newcode( JMP, topaddr );

    oldcode( breakaddr, JMPZ, currcode() );	/* Fix up the breakaddr */
}


	/***************************************************************\
	*								*
	*	getif() - generate code for the IF...ELSEIF...ELSE form	*
	*								*
	\***************************************************************/

getif()
{
    address
	oldaddr,
	breakaddr;

    getexpr( 0 );				/* Get the conditional */
    oldaddr = newcode( JMPZ, 0 );		/* Save the cond. br. addr */

    lexer();					/* Read the THEN */
    if( t_type != THEN )
	error( "'THEN' expected.\n" );

    newcode( POP, 0 );				/* Pop the condition */
    getroutine( 0 );				/* Get the body of the IF */

    if( t_type == ')' ) {
	/* We're done reading the IF statement */
	oldcode( oldaddr, JMPZ, currcode() );	/* Fix up the IF jump */
    }

    else {
	/* There was an ELSE or ELSEIF somewhere */
	breakaddr = newcode( JMP, 0 );		/* Skip the ELSE or ELSEIF */
	oldcode( oldaddr, JMPZ, currcode() );	/* Fix up the IF jump */
	newcode( POP, 0 );			/* Pop the condition code */

	if( t_type == ELSEIF ) {
	    /* This should be almost the same as an IF statement */
	    getif();		/* Recursively read the ELSEIF...ELSE */
	    oldcode( breakaddr, JMP, currcode() ); /* Fixup */
	}

	else if( t_type == ELSE ) {
	    /* This is slightly different */
	    getroutine( 0 );			/* Get the ELSE body */
	    if( t_type != ')' )
		_ERR_FIX( RIGHT_EXPECTED, ')' );
	    oldcode( breakaddr, JMP, currcode() );	/* Fixup */
	}

	else
	    _ERR_FIX( ILLEGAL_SYMBOL, ')' );
    }
}


	/***************************************************************\
	*								*
	*	getargs() - generate code for a list of arguments to	*
	*	a routine call.						*
	*								*
	\***************************************************************/

int16
getargs()
{
    int16
	temp = 0;	/* Number of arguments found */

    while( 1 ) {
	if( t_type == ')' )
	    /* We're done reading arguments */
	    return temp;
	getexpr( 1 );	/* Get an argument */
	lexer();	/* Get the next token */
	temp++;		/* Increment the number of args found */
    }
}

	/***************************************************************\
	*								*
	*	getroutine( t_read ) - parse and generate code for	*
	*	an ADL routine.						*
	*								*
	\***************************************************************/

getroutine( t_read )
int16
    t_read;
{
    int16
	irsave;

    irsave = inrout;
    if( !inrout ) {
	inrout = 1;
	emit_file();
    }
    if( !t_read )
	lexer();
    while( t_type == '(' ) {
	getform();
	lexer();
	if( t_type == '(' )
	    newcode( POP, 0 );
    }
    inrout = irsave;
}

/*** EOF routcomp.c ***/
