	/***************************************************************\
	*								*
	*	adlcomp.h - variable, function, and macro definitions	*
	*	for use by adlcomp.					*
	*								*
	\***************************************************************/


extern char
    token[],			/* Current token found by lexer		*/
    inname[];			/* Name of the current input file	*/

extern int16
    t_val,			/* Val of token from dict		*/
    t_type,			/* Type of token from dict		*/
    numerr,			/* Number of errors found so far	*/
    numwarn,			/* Number of warnings found so far	*/
    numline,			/* Number of lines encountered		*/
    wignore,			/* Ignore warnings?			*/
    maxerr;			/* Maximum number of errors allowed	*/

extern char
    *BAD_ARRAY,			/* Array size too small			*/
    *BRACKET_EXPECTED,		/* ']' expected				*/
    *LEFT_EXPECTED,		/* '(' expected				*/
    *RIGHT_EXPECTED,		/* ')' expected				*/
    *SEMI_EXPECTED,		/* ';' expected				*/
    *COMMA_EXPECTED,		/* ',' expected				*/
    *NOUN_WANTED,		/* Noun expected in expression		*/
    *CONST_EXPECTED,		/* Constant expected in expression	*/
    *VAR_EXPECTED,		/* VAR expected in expression		*/
    *EQUAL_EXPECTED,		/* '=' expected				*/
    *PREP_EXPECTED,		/* Prep expected in expression		*/
    *ATTEMPT,			/* Attempt to redefine a noun		*/
    *ILLEGAL_SYMBOL;		/* Illegal symbol (syntax error)	*/

extern int16
    bitpat[],			/* Bit masks for bit properties		*/
    ibitpat[];			/* Same as above, but inverted.		*/

extern int
    CODE_F;			/* Temporary file for paging code	*/

extern int
    breaker();			/* Exit from ADLCOMP after fixing files	*/

/* From codegen.c */
extern address
    newcode(),			/* Add a new instruction to CODE_F	*/
    oldcode(),			/* Modify an old instruction		*/
    currcode();			/* Return the current end of CODE_F	*/

/* From adlobj.c */
extern int16
    getnew(),			/* Get an undeclared object from input	*/
    getold(),			/* Get previously declared obj from input */
    noun_exists(),		/* Does an object exist with the specified
				   modifier and noun? */
/* From dict.c */
    lookup(),			/* Find the type and value of a token	*/
    insertkey();		/* Make a new token, given type and val	*/

/* Error macros */
#define _ERR_FIX( str, ch )	{ error( str ); eatuntil( ch ); return; }
#define _ERR_FX0( str, ch )	{ error( str ); eatuntil( ch ); return 0; }

/*** EOF adlcomp.h ***/
