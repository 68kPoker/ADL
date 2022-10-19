#include <stdio.h>
#include <fcntl.h>

#include "adltypes.h"
#include "adlprog.h"
#include "adlrun.h"
#include "adldef.h"
#include "virtmem.h"
#include "vstring.h"

struct	pagetab
    codetab;
struct	actrec
    actlist[ 10 ];
address
    ip;
int16
    stack[ STACKSIZE ],
    sp,
    bp,
    numact,
    curract,
    Verb,
    Prep,
    Iobj,
    Inoun,
    Imod,
    NumDobj,
    Conj[ NUMDO ],
    Dnoun[ NUMDO ],
    Dobj[ NUMDO ],
    Dmod[ NUMDO ],
    vecverb[ 10 ],
    demons[ 10 ],
    fuses[ 10 ],
    ftimes[ 10 ],
    f_actors[ 10 ],
    numd,
    numf,
    currturn,
    prompter,
    wordwrite,
    numsave,
    restarted,
    debug,
    header,
#if AMIGA
    scrwidth = 70,
#else
    scrwidth = 79,
#endif
    numcol,
    Phase,
    /* The following 6 vars are not actually used in adlrun,
       but we need to define them for the loader. */
    NUM_VARS,
    NUM_ROUTS,
    NUM_OBJS,
    NUM_VERBS,
    NUM_PREP;
char
    *inname = "adlcomp.out",
    *r_name = "\0",
    *USAGE = "Usage: adlrun [filename] [-d] [-h] [-r savefile]\n",
    *H_STR = "%-48sScore: %-4d  Moves: %-4d\n",
    tempc[ 4096 ],		/* Temporary strings */
    savec[ 1024 ];		/* "Permanent" strings */
int
    infile = -1;
FILE
#if MULTIPLEX
    *CURRTTY,			/* Current output file */
    *SYSTTY,			/* Default output file */
#endif
    *wordfile = (FILE *)0,
    *scriptfile = (FILE *)0;

struct exit_place
    exits[ 5 ];

#if MSDOS
	extern char
	    *getml();
#	define calloc( size, num )	getml( (long)(size * num) )
#else
	extern char
	    *calloc();
#endif

#define	islegal(c)	(c&&(c!=' ')&&(c!='\t')&&(c!='\n')&&(c!='-'))


main( argc, argv )
int	argc;
char	*argv[];
{
    getadlargs( argc, argv );
    while( 1 )
	if( SET_EXIT( 4 ) == 0 )
	    break;
    if( restarted )
	init( 0 );
    else
	init( 1 );
    driver();
}


getadlargs( argc, argv )
int
    argc;
char
    *argv[];
{  
    int
	i;
    char
	*getnext();

    for( i = 1; i < argc; i++ ) {
	if( *argv[ i ] == '-' )
	    switch( *++argv[ i ] ) {
		case 'd' :
		    debug = 1;
		    break;
		case 'h' :
		    header = 1;
		    break;
		case 'r' :
		    r_name = getnext( &i, argv );
		    break;
		default  :
		    fputs( USAGE, stderr );
		    exit( -1 );
	    }
	else if( !strcmp( inname, "adlcomp.out" ) )
	    inname = argv[ i ];
	else {
	    fputs( USAGE, stderr );
	    exit( -1 );
	}
    }
}


init( first )
int16
    first;
{
    int16
	me,
	*temp;

    if( first ) {
	insertkey( ",",		COMMA,	0, 0 );
	insertkey( "and",	CONJ,	0, 0 );
	insertkey( "but",	CONJ,	1, 0 );
	insertkey( ".",		SEP,	0, 0 );
	insertkey( "!",		SEP,	0, 0 );
	insertkey( "?",		SEP,	0, 0 );
	insertkey( "then",	SEP,	0, 0 );

	SRAND( rand_seed() );
	if( (infile = open( inname, RB )) < 0 ) {
	    fprintf( stderr, "Error opening file %s\n", inname );
	    exit( -1 );
	}

#if MULTIPLEX
	SYSTTY = fopen( "/dev/tty", "r+" );
	if( SYSTTY == (FILE *)0 ) {
	    fprintf( stderr, "Unable to open output device.\n" );
	    exit( -1 );
	}
	CURRTTY = SYSTTY;
#endif

	head_setup();
	sayer( "ADL interpreter - Version 3.2 - June 7, 1987\n" );
	sayer( "Copyright 1985, 1986, 1987 by Ross Cunniff\n" );
	sayer( "All rights reserved.\n" );

	/* Read in the input file */
	lseek( infile, 0L, 0 );
	read( infile, &hdr, sizeof( struct header ) );
	if( hdr.magic != M_ADL ) {
	    fprintf( stderr, "%s: Not an ADL datafile.\n", inname );
	    head_term();
	    exit( -1 );
	}

	/* Read the arrays */

	read_symtab( infile, &hdr.symindex );
	loadarray( (char **)&routspace,	&hdr.routindex, 1 );
	loadarray( (char **)&str_tab,	&hdr.strtabindex, 1 );
	loadarray( (char **)&prepspace,	&hdr.prepindex, 1 );
	loadarray( (char **)&verbsyn,	&hdr.vsindex, 1 );
	loadarray( (char **)&nounspace,	&hdr.nounindex, 1 );

	/* Initialize virtual code and string stuff */
	vsinit( infile, hdr.strindex.ptr, 1, tempc, savec, &numsave, str_tab );
	vm_init( infile, hdr.codeindex.ptr, &codetab );

    }	/* if( first ) */

    temp = stack;
    loadarray( (char **)&temp,		&hdr.varindex, 0 );
    loadarray( (char **)&verbspace,	&hdr.verbindex, first );
    loadarray( (char **)&objspace,	&hdr.objindex, first );

    if( scriptfile != (FILE *)0 ) {
	fclose( scriptfile );
	scriptfile = (FILE *)0;
    }

    if( wordwrite )
	fclose( wordfile );

    /* Initialize some variables (This is for ($spec 2) -> restart) */
    sp = bp = NUMVAR;
    ip = curract = numd = numf = 0;
    wordwrite = numsave = prompter = currturn = 0;

    actlist[ 0 ].linebuf = actlist[ 0 ].savebuf;
    *actlist[ 0 ].linebuf = '\0';
    actlist[ 0 ].interact = 1;
    numact = 0;

    clearmacro();

    if( *r_name ) {
	restoregame( r_name );
	r_name = "\0";
    }
    else
	callone( _START );

    restarted = 0;
}


driver()
{
    int
	i;

    /* Outermost adlrun loop (never exited except to quit adlrun) */
    while( 1 ) {

	vsflush();	/* Clear out the temporary strings */
	curract = 0;

	/* Daemon loop */
	for( curract = numact - 1; curract >= 0; curract-- ) {
#if MULTIPLEX
	    if( actrec[ curract ].ttyfile == (FILE *)0 )
		CURRTTY = SYSTTY;
	    else
		CURRTTY = actrec[ curract ].ttyfile;
#endif
	    Phase = 0;
	    if( SET_EXIT( 1 ) == 0 ) {
		if( SET_EXIT( 0 ) == 0 )
		    execdems();
	    }
	}
	CLR_EXIT( 0 );

	/* Main actor loop */
	for( curract = numact - 1; curract >= 0; curract-- ) {
#if MULTIPLEX
	    if( actlist[ curract ].ttyfile == (FILE *)0 )
		CURRTTY = SYSTTY;
	    else
		CURRTTY = actlist[ curract ].ttyfile;
#endif

	    if( SET_EXIT( 1 ) != 0 )
		continue;

	    /* Delete actor if not interactive and line buffer is empty */
	    if( (*PSTRING == 0) && (INTERACT == 0) ) {
		delactor( CURRACT );
		continue;		/* with for loop */
	    }

	    /* Read and parse the user's input */
	    initvars();
	    while( 1 )
		if( SET_EXIT( 3 ) == 0 )
		    break;
	    if( get_buffer() < 0 )
		continue;

	    /* Call the appropriate routines */
	    callrouts();

	    CLR_EXIT( 3 );
	}	/* for( curract ... ) */
    }		/* outer while( 1 ) */
}


char *
getnext( which, argv )
int
    *which;
char
    *argv[];
{
    if( *++argv[ *which ] )
	return argv[ *which ];
    else
	return argv[ ++*which ];
}


rand_seed()
{
    long
	time();

    SRAND( (int)time( (long *)0 ) );
}

sayer( s )
char
    *s;
{
    char
	word[ 80 ];

    while( *s ) {
	getchunk( &s, word );
	if( *word == '\n' ) {
	    crlf();
	    continue;
	}
	else if( (numcol + strlen( word )) > scrwidth ) {
	    while( *word == ' ' )	/* Eat up blanks */
		getchunk( &s, word );
	    if( !*word ) {
		fflush( stdout );
		return;			/* The string was blank terminated */
	    }
	    crlf();
	}
	numcol += strlen( word );
#if MULTIPLEX
	fputs( word, CURRTTY );
	fflush( CURRTTY );
#else
	fputs( word, stdout );
	fflush( stdout );
#endif
	if( scriptfile != (FILE *)0 )
	    fputs( word, scriptfile );
    }
#if MULTIPLEX
    fflush( CURRTTY );
#else
    fflush( stdout );
#endif
}


/* VARARGS */
loadarray( a, d, first )
char
    **a;
struct	adldir
    *d;
int16
    first;
{
    if( first && (d->numobjs * d->objsize) ) {
	*a = calloc( d->numobjs, d->objsize );
	if( *a == (char *)0 )
	    error( 27 );		/* Out of memory */
    }
    lseek( infile, d->ptr, 0 );
    read( infile, *a, d->numobjs * d->objsize );
}


getchunk( s, word )
char
    **s,
    *word;
{
    if( !**s ) {
	*word = '\0';
	return;
    }
    if( islegal( **s ) )
	while( islegal( **s ) )
	    *word++ = *((*s)++);
    else if( **s == '\t' ) {			/* Expand the tab */
	strcpy( word, "        " );
	word[ 8 - (numcol % 8) ] = '\0';
	(*s)++;
	return;
    }
    else
	*word++ = *((*s)++);
    *word = '\0';
}


crlf()
{
#if MULTIPLEX
    putc( '\n', CURRTTY );
#else
    putchar( '\n' );
#endif
    if( scriptfile != (FILE *)0 )
	putc( '\n', scriptfile );
    numcol = 0;
}


get_buffer()
{
    /* Loop for user input */
    while( 1 ) {

	/* Get a non-empty line */
	if( !*PSTRING ) {
	    PSTRING = &SAVEBUF[ 0 ];
	    u_prompt();
	    getstring( PSTRING );
	}

	/* Parse the string */
	if( !parse() ) {
	    *PSTRING = '\0';
	    initvars();		/* Failed parse; init the variables	*/
	    if( INTERACT )
		continue;	/* with While (interactive actor, try again) */
	    else {
		delactor( CURRACT );
	    	return -1;		/* from While (the actor needs to die) */
	    }
	}
	else
	    break;		/* The parse was successful */
    }
    return 0;
}


int16
noun_exists( adj, noun )
int16
    adj, noun;
{
    int16
	t;

    for( t = nounspace[ noun ]; t != 0; t = objspace[ t ].others )
	if( objspace[ t ].adj == adj )
	    return t;
    return -1;
}

/*** EOF adlrun.c ***/
