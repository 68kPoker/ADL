	/***************************************************************\
	*								*
	*	adllex.c - Lexical anylizer for adlcomp.		*
	*	Copyright 1987 by Ross Cunniff.				*
	*								*
	\***************************************************************/

#include <ctype.h>
#include <stdio.h>

#include "adltypes.h"
#include "adldef.h"
#include "vstring.h"
#include "adlprog.h"
#include "adlcomp.h"

/* adlchr( c ) is TRUE iff c is a valid character in a token */

#define adlchr( c ) (				\
			isalnum( c ) ||		\
			( c == '_' ) ||		\
			( c == '%' ) ||		\
			( c == '$' ) ||		\
			( c == '.' ) ||		\
			( c == '-' )		\
		)

/*  */

/***************************************************************\
*								*
*	Global variables					*
*								*
\***************************************************************/

char	token[ 512 ],		/* Last token read			*/
	*EOFMSG = "Unexpected EOF.\n";	/* Message for EOF		*/

int16	t_val,			/* Value of last token read		*/
	t_type,			/* Type of last token read		*/
	numerr	= 0,		/* Number of errors encountered		*/
	numwarn = 0,		/* Number of warnings encountered	*/
	numline = 1;		/* Number of lines read			*/

extern
int16	debugging,		/* Are we generating FILE and LINE code? */
	inrout,			/* Are we inside a routine definition?	*/
	filenum;		/* Current file number.			*/

FILE	*infile;		/* Current input file			*/
char	inname[ 512 ];		/* Name of current input file		*/

/*  */

	/***************************************************************\
	*								*
	*	lexer() - return the next input token from the input	*
	*	stream in the form of a value and a type.		*
	*								*
	\***************************************************************/

lexer()
{
    int16 t;

    if( gettoken( token ) == EOF ) {
	/* We reached the end of file. */
	t_type = EOF;
	return;
    }

    if( isnumber( token ) ) {
	/* This token is a constant number. */
	t_val = atoi( token );
	t_type = CONST;
	return;
    }

    else if( adlchr( *token ) ) {
	if( *token == '%' ) {
	    /* This token should be an argument number */
	    if( !isnumber( token + 1 ) )
		error( "Illegal argument number.\n" );
	    t_val = atoi( token + 1 );
	    t_type = ARGUMENT;
	    return;
	}

	/* This token should be an identifier. */
	if( !adlident( token ) )
	    error( "Illegal token.\n" );

	t = lookup( token, &t_val, 0 );
	if( t >= 0 ) {
	    /* This token has already been declared. */
	    t_type = t;
	    return;
	}
	else {
	    /* This token has not previously been declared */
	    t_type = UNDECLARED;
	    return;
	}
    }

    else if( *token == '"' ) {
	/* This token is a compile time string */
	t_val = newstr( token + 1 );
	t_type = STRING;
	return;
    }

    else {
	/* This token is punctuation */
	t_type = *token;
	return;
    }
}

/*  */

	/***************************************************************\
	*								*
	*	gettoken( s ) - returns the next token from infile in	*
	*	s.  A token is a number, an identifier, a string,	*
	*	or punctuation.						*
	*								*
	\***************************************************************/

gettoken( s )
char	*s;
{
    int ch;
    int count = 0;

    ch = eatwhite();		/* Get rid of unneeded white space	*/

    if( ch == '"' )
	/* Get a string */
	return getstring( s );

    else {
	/* Get an identifier, number, or argument. */
	while( adlchr( ch ) ) {
	    count++;
	    *s++ = ch;
	    ch = mygetc();
	}
	if( count ) {
	    /* We read more than one character. */
	    if( ch != EOF )
		/* We read a character which should be read later */
		ch = myunget( ch );
	}
	else
	    *s++ = ch;
	*s = '\0';
	return ch;
    }	/* else */
}	/* gettoken */

/*  */

	/***************************************************************\
	*								*
	*	eatwhite() - eats up white space and comments from	*
	*	the infile.						*
	*								*
	\***************************************************************/

eatwhite()
{
    int ch;
    char s[ 512 ];

    for(	ch = mygetc();
		(ch == ' ')||(ch == '\t')||(ch == '{')||(ch == '\n');
		ch = mygetc() ) {
	if( ch == '{' ) {
	    for( ch = mygetc(); (ch != '}'); ch = mygetc() ) {
		/* Eat up the comment */
		if( ch == EOF )
		    fatal( EOFMSG );
		else if( ch == '"' )
		    /* Don't allow quoted comments to confuse us */
		    ch = getstring( s );
	    }	/* for */
	}	/* if */
    } /* for */
    return ch;
}

/*  */

	/***************************************************************\
	*								*
	*	getstring( s ) - reads a quoted string from the infile,	*
	*	approprately transforming escape sequences, and returns	*
	*	the string in s.					*
	*								*
	\***************************************************************/

getstring( s )
char	*s;
{
    int	ch, n;

    n = 0;
    *s++ = '"';
    for( ch = mygetc(); (ch != '"'); ch = mygetc() ) {
	if( ++n == 511 )
	    error( "String too long.\n" );
	if( ch == '\\' ) {
	    if( (ch = getescape()) == EOF )
		fatal( EOFMSG );
	    if( n < 511 )
		*s++ = ch;
	}
	else if( ch == EOF )
	    fatal( EOFMSG );
	else {
	    if( ch == '\n' )
		ch = ' ';
	    if( n < 511 )
		*s++ = ch;
	}
    }
    *s = '\0';
    if( ch == EOF )
	return EOF;
    else
	return ' ';
}		/* getstring */


	/***************************************************************\
	*								*
	*	getescape() - reads an escape sequence such as \n or	*
	*	\b or \033 from the infile and returns the translated	*
	*	character.
	*								*
	\***************************************************************/

getescape()
{
    int t, ch;
    int count;

    ch = mygetc();
    if( isdigit( ch ) ) {
	count = 1;
	t = ch - '0';
	while( isdigit( ch = mygetc() ) && (count++ <= 3) )
	    t = t * '\010' + ch - '0';
	if( ch != EOF )
	    ch = myunget( ch );
    }
    else
	switch( ch ) {
	    case 'n'  : t = '\n'; break;
	    case 't'  : t = '\t'; break;
	    case 'b'  : t = '\b'; break;
	    case 'r'  : t = '\r'; break;
	    case 'f'  : t = '\f'; break;
	    case '\\' : t = '\\'; break;
	    default   : t = ch;
	} /* switch */
    return t;
}

/**/

	/***************************************************************\
	*								*
	*	Token type query routines.  These two routines verify	*
	*	whether a token is of the appropriate type.  They are:	*
	*								*
	*		isnumber( s ) - TRUE iff s is a decimal number	*
	*		adlident( s ) - TRUE iff s is a legal ADL ident	*
	*								*
	\***************************************************************/

isnumber( s )
char	*s;
{
    if( *s == '-' )	/* Skip initial '-' */
	s++;
    while( *s )
	if( !isdigit( *s ) )
	    return 0;
	else
	    s++;
    return 1;
}


adlident( s )
char	*s;
{
    if( (*s == '$') || (*s == '.') || (*s == '_') || (*s == '-') )
	s++;
    if( !isalpha( *s ) )
	return 0;
    s++;
    while( *s )
	if( !(isalnum( *s ) || (*s == '_') || (*s == '-')) )
	    return 0;
	else
	    s++;
    return 1;
}

/*  */

	/***************************************************************\
	*								*
	*	These routines handle the actual i/o with the infile.	*
	*	They keep track of the current line number.  They are:	*
	*								*
	*		mygetc() - return the next char from infile	*
	*		myunget( c ) - push c back into the infile	*
	*								*
	\***************************************************************/

mygetc()
{
    int	result;
    int breaker();

    result = getc( infile );
    if( result == '\n' ) {
	checkbreak( breaker );			/* Check for ^C */
	numline++;
	emit_file();
    }
    return result;
}


myunget( c )
int	c;
{
    if( c == '\n' )
	numline--;
    return ungetc( c, infile );
}


	/***************************************************************\
	*								*
	*	emit_file() - if debugging mode is set, and we are	*
	*	compiling a routine, emit the file number and line	*
	*	number into the code space, for better error tracking.	*
	*								*
	\***************************************************************/

emit_file()
{
    if( debugging && inrout ) {
	newcode( FILEN, filenum );
	newcode( LINEN, numline );
    }
}

/*  */

	/***************************************************************\
	*								*
	*	The following routines are here to hide the details	*
	*	implementation of the input files from the routines	*
	*	 which use the lexer.  The routines are:		*
	*								*
	*		open_input( name )	- open the infile	*
	*		close_input()		- close the infile	*
	*		save_input( &buf )	- save the infile	*
	*		restore_input( buf )	- restore the infile	*
	*								*
	\***************************************************************/

open_input( name )
char 	*name;
{
    infile = fopen( name, "r" );
    if( infile == (FILE *)NULL )
	return 0;
    else
	return 1;
}


close_input()
{
    fclose( infile );
}


save_input( buffer )
char	**buffer;
{
    *buffer = (char *)infile;
}


restore_input( buffer )
char	*buffer;
{
    infile = (FILE *)buffer;
}

/*** EOF adllex.c ***/
