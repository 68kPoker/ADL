	/***************************************************************\
	*								*
	*	adlscrn.c - Screen I/O for adlrun.  Add new def's	*
	*	for a new terminal here (unless using termcap).		*
	*								*
	\***************************************************************/

#include <stdio.h>

#include "adltypes.h"
#include "adlprog.h"
#include "adldef.h"
#include "adlrun.h"
#include "vstring.h"


#if HPTERM
static char
    TGOTO[]	= "\033&a%dy%dC",
    CLEAR[]	= "\033J",
    STANDOUT[]	= "\033&dJ",
    STANDEND[]	= "",
    LOCK[]	= "\033l",
    NOLOCK[]	= "\033m";
#endif

#if ANSI
static char
    TGOTO[]	= "\033[%02d;%02dH",
    CLEAR[]	= "\014",
    STANDOUT[]	= "\033[0;7m",
    STANDEND[]	= "\033[0m",
    LOCK[]	= "",
    NOLOCK[]	= "";
#endif

#if TERMCAP
char
    *BC, *UP;		/* Just to satisfy the linker */

static char
    BUFF[1024],		/* Buffer for tgetent */
    TGOTO[20],		/* cm		*/
    CLEAR[10],		/* cd		*/
    STANDOUT[10],	/* so		*/
    STANDEND[10],	/* se		*/
    LOCK[10],		/* ml		*/
    NOLOCK[10];		/* mu		*/
#endif

static int
    END = -1;		/* Last line on the screen */

#if AMIGA == 0
char
    *getenv();		/* Find an environment variable */
#endif

head_setup()
{
    char
	*value;		/* Value of an environment variable */
    FILE
	*outfile;

    if( !header )
	return;

#if MULTIPLEX
    outfile = CURRTTY;
#else
    outfile = stdout;
#endif

#if TERMCAP
    /* Initialize termcap */
    if( (value = getenv( "TERM" )) == (char *)0 )
	error( 29 );		/* Bad termcap */
    if( tgetent( bp, value ) <= 0 )
	error( 29 );		/* Bad termcap */

    /* Get the number of lines on the screen */
    END = tgetnum( "li" );

    /* Get the command strings */
    (void)mygetstr( "cm", TGOTO, 1 );
    (void)mygetstr( "so", STANDOUT, 0 );
    (void)mygetstr( "se", STANDEND, 0 );
    if( mygetstr( "cd", CLEAR, 0 ) == 0 )
	(void)mygetstr( "cl", CLEAR, 1 );
    (void)mygetstr( "ml", LOCK );
    (void)mygetstr( "mu", NOLOCK );
#endif

#if AMIGA
    /* Set the screen dimensions for a standard console window */
    END = 22;
#else
    /* See if the END is in the environment */
    if( (value = getenv( "LINES" )) != (char *)0 )
	END = atoi( value );

    if( END < 0 )
	/* Assume a standard size terminal */
	END = 23;
    else
	/* Last line is number of lines minus one. */
	END--;
#endif

    /* Go to the top of the screen */
#if TERMCAP
    fprintf( outfile, tgoto( TGOTO, 0, 0 ) );
#endif
#if ANSI
    fprintf( outfile, TGOTO, 1, 1 );
#endif
#if HPTERM
    fprintf( outfile, TGOTO, 0, 0 );
#endif

    fputs( CLEAR, outfile );		/* Clear the screen */
    fputs( STANDOUT, outfile );		/* First line inverse video */
    fprintf( outfile, H_STR, "", 0, 0 );		/* Header line */
    fputs( STANDEND, outfile );		/* End inverse video */
    fputs( LOCK, outfile );		/* lock first line */

    /* Go to the end of the screen */
#if TERMCAP
    fprintf( outfile, tgoto( TGOTO, 0, END ) );
#endif
#if ANSI
    fprintf( outfile, TGOTO, END+1, 1 );
#endif
#if HPTERM
    fprintf( outfile, TGOTO, END, 0 );
#endif

    fflush( outfile );
}


#if TERMCAP
mygetstr( which, where, need )
char
    *which,
    *where;
int
    need;
{
    int
	retval;
    char
	*temp,
	*save,
	*tgetstr();

    save = where;
    temp = tgetstr( which, &where );
    if( temp == (char *)0 ) {
	if( need ) {
	    error( 29 );			/* Bad termcap */
	    /*NOTREACHED*/
	}
	else {
	    *save = '\0';
	    retval = 0;
	}
    }
    else {
	*temp = '\0';
	retval = 1;
    }
    return retval;
}
#endif


write_head()
{
    FILE
	*outfile;

#if MULTIPLEX
    outfile = CURRTTY;
#else
    outfile = stdout;
#endif

    if( !header )
	return;

    assertargs( "$spec 9", 4 );		/* ($spec 9 str score moves)	*/

    fputs( NOLOCK, stdout );		/* Turn off memory lock		*/

    /* Go to the top of the screen */
#if TERMCAP
    fprintf( outfile, tgoto( TGOTO, 0, 0 ) );
#endif
#if ANSI
    fprintf( outfile, TGOTO, 1, 1 );
#endif
#if HPTERM
    fprintf( outfile, TGOTO, 0, 0 );
#endif

    fputs( STANDOUT, outfile );		/* Inverse video		*/
    fprintf( outfile, H_STR, virtstr( ARG( 2 ) ), ARG( 3 ), ARG( 4 ) );
    fputs( STANDEND, outfile );		/* Normal video			*/
    fputs( LOCK, outfile );		/* Lock the first line		*/

    /* Go to the end of the screen */
#if TERMCAP
    fprintf( outfile, tgoto( TGOTO, 0, END ) );
#endif
#if ANSI
    fprintf( outfile, TGOTO, END+1, 1 );
#endif
#if HPTERM
    fprintf( outfile, TGOTO, END, 0 );
#endif

    fflush( outfile );
}


head_term()
{
#if MULTIPLEX
    fputs( NOLOCK, CURRTTY );
    fflush( CURRTTY );
#else
    fputs( NOLOCK, stdout );		/* Turn off memory lock		*/
    fflush( stdout );
#endif
}

/*** EOF adlscrn.c ***/
