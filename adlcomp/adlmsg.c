	/***************************************************************\
	*								*
	*	adlmsg.c - routines to print error messages during	*
	*	compilation.						*
	*	Copyright 1987 by Ross Cunniff.				*
	*								*
	\***************************************************************/

#include <stdio.h>

#include "adltypes.h"
#include "adlprog.h"
#include "adlcomp.h"


/* Predefined error messages */
char
    *BRACKET_EXPECTED	= "']' expected.\n",
    *BAD_ARRAY		= "Array size must be greater than 0.\n",
    *LEFT_EXPECTED	= "'(' expected.\n",
    *RIGHT_EXPECTED	= "')' expected.\n",
    *SEMI_EXPECTED	= "';' expected.\n",
    *COMMA_EXPECTED	= "',' expected.\n",
    *NOUN_WANTED	= "Noun expected.\n",
    *CONST_EXPECTED	= "Constant expected.\n",
    *VAR_EXPECTED	= "Global expected.\n",
    *EQUAL_EXPECTED	= "'=' expected.\n",
    *PREP_EXPECTED	= "Preposition expected.\n",
    *ATTEMPT	= "Attempt to use a modified noun without a modifier.\n",
    *ILLEGAL_SYMBOL	= "Illegal symbol.\n";

int16
    wignore = 0,	/* Ignore warning messages?			*/
    maxerr  = -1;	/* Maximum number of errors before crash	*/


	/***************************************************************\
	*								*
	*	eatuntil( c ) - eat tokens until a token with type c	*
	*	is found, and EOF is found, or a semicolon is found,	*
	*	whichever comes first.					*
	*								*
	\***************************************************************/

eatuntil( c )
int16
    c;
{
    while( (t_type != c) && (t_type != EOF) ) {
	if( (t_type == '(') && (c == ')') ) {
	    lexer();
	    eatuntil( ')' );
	}
	else if( t_type == ';' )
	    return;	/* Don't eat past a ';' */
	lexer();
   }
}



	/***************************************************************\
	*								*
	*	error( msg ) - Print ERROR: msg, increment the error	*
	*	counter, and exit if too many errors have occurred.	*
	*								*
	\***************************************************************/

error( s )
char
    *s;
{
    fprintf( stderr, "ERROR: %s", s );
    if( t_type != EOF ) {
	fprintf( stderr, "File \"%s\", line %d, token \"%s\"\n",
		     inname, numline, token );
	fflush( stderr );
    }
    numerr++;
    if( (maxerr >= 0) && (numerr > maxerr) )
	fatal( "Maximum number of errors exceeded.\n" );
}


	/***************************************************************\
	*								*
	*	warning( msg ) - If warning messages are enabled, print	*
	*	the string WARNING: msg and increment the warning count.*
	*								*
	\***************************************************************/

warning( s )
char
    *s;
{
    if( !wignore ) {
	fprintf( stderr, "WARNING: %s", s );
	if( t_type != EOF )
	    fprintf( stderr, "File \"%s\", line %d, token \"%s\"\n",
		       inname, numline, token );
	fflush( stderr );
    }
    numwarn++;
}


	/***************************************************************\
	*								*
	*	fatal( s ) - print the message FATAL ERROR: s and	*
	*	exit the program.					*
	*								*
	\***************************************************************/

fatal( s )
char
    *s;
{
    fprintf( stderr, "FATAL ERROR: %s", s );
    fputs( "ADL aborting.\n", stderr );
    breaker();
}

/*** EOF adlmsg.c ***/
