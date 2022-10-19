#include <ctype.h>
#include <stdio.h>

#include "adltypes.h"
#include "adlprog.h"
#include "vstring.h"
#include "adldef.h"
#include "adlrun.h"

#define MAXDEPTH	20	/* Maximum nesting of macros */

int16
    t_type,		/* Current token type */
    t_val,		/* Current token value */
    read_t;		/* Instruction to lexer to read another token */
char
    *s,			/* Save area for current token */
    *xp;		/* Expansion of s */


	/***************************************************************\
	*								*
	*	gettoken() - Read the first token from PSTRING, macro	*
	*	expand it, and place the type and value of the token	*
	*	in t_type and t_val, respectively.  The actual token	*
	*	is located in the external string s.  On an error,	*
	*	t_type will contain one of the following values:	*
	*		-1	:	unknown word			*
	*		-2	:	ambiguous abbreviation		*
	*		-3	:	invalid macro expansion		*
	*								*
	\***************************************************************/

gettoken()
{
    int
	depth,		/* Depth of the macro expansion */
	success;	/* Did the token expand? */


    /* Check to see whether we want to read a new token */
    if( !read_t ) {
	read_t = 1;
	return;
    }

    for( depth = 0; depth < MAXDEPTH; depth++ ) {
	/* Get the first token from the string */
	if( lexer() )
	    return;		/* The lexer fully handled the token */

	/* Process the token. */
	if( !*s ) {
	    /* The string was empty, so return SEP (separator) */
	    t_type = SEP;
	    t_val = 0;
	    return;
	}
	else {
	    success = try_expand();
	    if( success < 0 ) {
		/* An error occured during expansion */
		t_type = -3;	/* Invalid macro expansion */
		t_val = 0;
		return;
	    }
	    else if( success == 0 ) {
		/* The token WASN'T a macro, so process it and return. */
		if( numberp( s ) ) {
		    /* The token was a number. Coerce it into a string */
		    t_type = STRING;
		    t_val = newtstr( s );
		}
		else {
		    /* The token was an identifier.  Look it up. */
		    t_type = lookup( s, &t_val, 1 );
		    if( t_type < 0 && wordwrite )
			/* Write the token to the unknown words file */
			fprintf( wordfile, "%s\n", s );
		}
		return;	/* Don't try to further expand this token */
            }
	}
    }
    /* If we get here, we expanded the thing MAXDEPTH times. */
    t_type = -3;	/* Invalid macro expansion */
}

/**/

	/***************************************************************\
	*								*
	*	lexer() - read the first token from PSTRING.  Returns	*
	*	1 if the token is a quoted string, 0 otherwise.		*
	*								*
	\***************************************************************/

lexer()
{
    int
	count = 0;	/* Number of chars in the token */
    char
	c,		/* Current char in the token */
	*t;		/* Holding place for the beginning of the token */

    eatwhite();		/* Delete leading whitespace */
    t = s;		/* Save the starting address. */

    if( *PSTRING ) {
	/* There are indeed non-blank characters in the string */

	if( !adlchr( *PSTRING ) ) {
	    /* The current character can't appear in an identifier */

	    if( (*PSTRING == '"') || (*PSTRING == '\'') ) {
		/* We need to (recursively) find a quoted string */
		getquotes( *PSTRING++ );
		*(--s) = '\0';		/* Destroy closing quote */
		s = t;
		t_val = newtstr( s );
		t_type = STRING;

		/* Return the fact that we handled everything */
		return 1;
	    }
	    else
		/* The current character is simply punctuation */
		*s++ = *PSTRING++;
	}

	else {
	    /* The current character is the first of an identifier */

	    while( adlchr( c = *PSTRING++ ) ) {
		/* Read the characters into s (iff there is room in s) */
		if( SLEN > ++count ) {
		    if( isupper( c ) )
			*s++ = tolower( c );
		    else
			*s++ = c;
        	}
	    }
	    PSTRING--;		/* Put back the last character. */
	}
    }
    *s = '\0';			/* Null terminate the string */
    s = t;			/* Point to the beginning of the token */
    return 0;			/* Further processing is required. */
}

/**/

	/***************************************************************\
	*								*
	*	try_expand() - attempt to expand the current token.	*
	*	Returns 1 if the token expands, 0 if the token doesn't	*
	*	expand, and -1 if expanding the token would overflow	*
	*	the string.						*
	*								*
	\***************************************************************/

try_expand()
{
    char
	tsave[ SLEN ];		/* Save area for concatenation */

    xp = expand( s );
    if( strcmp( xp, s ) != 0 ) {
	/* The token was a macro - put it in the right place */

	if( (strlen( xp ) + strlen( PSTRING )) > (SLEN - 1) ) {
	    /* Macro expansion overflow - not enough room */
	    return -1;		/* error */
	}
	else {
	    /* PSTRING := concat( xp, PSTRING ) */

	    strcpy( tsave, PSTRING );
	    PSTRING = actlist[ curract ].savebuf;
	    strcpy( PSTRING, xp );
	    strcat( PSTRING, tsave );
	    return 1;		/* successful expansion */
	}
    }
    else
	return 0;		/* It didn't expand */
}

/**/

	/***************************************************************\
	*								*
	*	numberp( s ) - returns 1 if s is a valid signed		*
	*	integer, 0 otherwise.					*
	*								*
	\***************************************************************/

numberp( s )
char
    *s;
{
    if( *s == '-' ) {
	/* Preceding minus sign */
	s++;
	if( !*s ) {
	    /* Just a minus sign is NOT a number */
	    return 0;
	}
    }
    while( *s ) {
	if( !isdigit( *s ) )
	    return 0;
	else
	    s++;
    }
    return 1;
}


	/***************************************************************\
	*								*
	*	getquotes( ch ) - Get a quoted string delimited by ch	*
	*	from PSTRING.  Nesting of strings is allowed, e.g.:	*
	*								*
	*		'The string "foobar 'bletch'" is legal.'	*
	*								*
	*	Note that the trailing quote is optional.  If the	*
	*	trailing quote is absent, the end of the string is	*
	*	delimited by the end of the line.			*
	*								*
	\***************************************************************/

getquotes( ch )
int16
    ch;
{
    while( (*PSTRING) && (*PSTRING != ch) ) {
	*s++ = *PSTRING;
	if( (*PSTRING == '\"') || (*PSTRING == '\'') )
	    getquotes( *PSTRING++ );
	else
	    PSTRING++;
    }
    *s++ = ch;
    if( *PSTRING )
	PSTRING++;		/* Eat up closing quote */
}


	/***************************************************************\
	*								*
	*	eatwhite() - Removes leading blanks from PSTRING.	*
	*								*
	\***************************************************************/

eatwhite()
{
    while( (*PSTRING == ' ') || (*PSTRING == '\t') )
	PSTRING++;
}



	/***************************************************************\
	*								*
	*	adlchr( c ) - returns 1 if c is a valid character in an	*
	*	ADL identifier, 0 otherwise.				*
	*								*
	\***************************************************************/

adlchr( c )
int	c;
{
    return isalnum(c) || (c == '-');
}

/*** EOF rtlex.c ***/
