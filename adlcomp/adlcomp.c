	/***************************************************************\
	*								*
	*	adlcomp.c - ADL compiler.  See the ADL documentation	*
	*	for information about ADL.				*
	*		Copyright 1987 by Ross Cunniff			*
	*								*
	\***************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>

#include "adltypes.h"

#if UNIX
#  include <signal.h>
#endif

#include "adlprog.h"
#include "builtins.h"
#include "adldef.h"
#include "vstring.h"
#include "virtmem.h"
#include "adlcomp.h"

#define MAXDIR 4			/* Maximum number of -i dirs	*/

#if UNIX
char	*S_VAL = "adlsXXXXXX",		/* See below - patched		*/
	*CODE  = "adlcXXXXXX",		/* to use mktmp( 3 )		*/
	*tmpdir= "/tmp",		/* Default to /tmp		*/
	*PATH_SEPS = "/",		/* Set of path separators	*/
	*LAST_SEP = "/";		/* Final separator		*/
#endif

#if MSDOS
char	*S_VAL = "adlsval.tmp",		/* Temporary string file	*/
	*CODE  = "adlcode.tmp",		/* Temporary code file		*/
	*tmpdir= "",			/* Default to current dir	*/
	*PATH_SEPS = ":/\\",		/* Set of path separators	*/
	*LAST_SEP = "\\";		/* Final separator		*/
#endif

#if AMIGA
char
	*S_VAL = "adlsval.tmp",		/* Ditto above			*/
	*CODE  = "adlcode.tmp",		/* Ditto			*/
	*tmpdir= "RAM:",		/* Default to RAM:		*/
	*PATH_SEPS = ":/",		/* Set of path separators	*/
	*LAST_SEP = "/";		/* Final separator		*/
#endif

char	tmp_s[ 256 ],			/* Combined path and base	*/
	tmp_c[ 256 ],			/*   of CODE and S_VAL		*/
	*outname = "adlcomp.out",	/* Default output file name	*/
	*dirnames[MAXDIR];		/* -i names			*/

extern	char	*calloc();		/* Memory allocator		*/

struct	pagetab	codetab;		/* Structure for virtual code	*/

int16	NUM_VARS = 64,			/* Default # of ADL variables	*/
	NUM_ROUTS = 512,		/* Default # of ADL routines	*/
	NUM_OBJS = 256,			/* Default # of ADL objects	*/
	NUM_VERBS = 128,		/* Default # of ADL verbs	*/
	NUM_STR = 1024,			/* Default # of ADL strings	*/
	NUM_PREP = 8,			/* Default # of prep synonyms	*/
	NUM_VS = 8,			/* Default # of verb synonyms	*/

	numprep,			/* Actual # of prep synonyms	*/
	header,				/* Produce a status line?	*/
	debugging,			/* Retain symbols?		*/
	filenum,			/* String index of file name	*/
	numdir;				/* Number of -i dirs specified	*/

int	S_VAL_F,			/* String paging file		*/
	CODE_F;				/* Code paging file		*/

#if MSDOS
unsigned
    _stack = 10000;			/* Large default stack size	*/
#endif

	/***************************************************************\
	*								*
	*	main() - The main routine.  Opens files, initializes	*
	*	structures, compiles the code, writes the result.	*
	*	Pretty simple, no?					*
	*								*
	\***************************************************************/

main( argc, argv )
int	argc;
char	*argv[];
{
    getadlargs( argc, argv );		/* Get command line args	*/
    init();				/* Initialize structures	*/

    /* Print the copyright message */
    fputs( "ADL compiler - Version 3.2 - June 7, 1987\n", stderr );
    fputs( "Copyright 1985, 1986, 1987 by Ross Cunniff\n", stderr );
    fputs( "All rights reserved\n", stderr );
    fflush( stderr );

    adlcomp();				/* Compile the source		*/
    strcpy( token, "EOF" );		/* Indicate we're at EOF	*/
    write_code();			/* Write the code file		*/
    wrapup();				/* Close files			*/
    exit( numerr );			/* Return the number of errors	*/
}


	/***************************************************************\
	*								*
	*	getadlargs( argc, argv ) - extract meaning from		*
	*	the command line.  See the adlcomp man page for		*
	*	more information.					*
	*								*
	\***************************************************************/

getadlargs( argc, argv )
int	argc;
char	*argv[];
{
    int	i;
    char ch;
    char *getnext();

#if MSDOS
    argv[ 0 ] = "adlcomp";		/* MSDOS has no notion of argv[ 0 ] */
#endif

    if( argc < 2 )
	/* We must at least specify an input file */
	print_usage( argv[ 0 ] );

    /* Check each argument */
    for( i = 1; i < argc; i++ ) {
	if( *argv[ i ] == '-' ) {
	    ch = *++argv[ i ];
	    if( isupper( ch ) )
		ch = tolower( ch );
	    switch( ch ) {
		case 'e' : maxerr	= atoi( getnext( argv, &i ) );	break;
		case 'g' : NUM_VARS	= atoi( getnext( argv, &i ) );	break;
		case 'm' : NUM_STR	= atoi( getnext( argv, &i ) );	break;
		case 'n' : NUM_OBJS	= atoi( getnext( argv, &i ) );	break;
		case 'p' : NUM_PREP	= atoi( getnext( argv, &i ) );	break;
		case 'r' : NUM_ROUTS	= atoi( getnext( argv, &i ) );	break;
		case 's' : NUM_VS	= atoi( getnext( argv, &i ) );	break;
		case 'v' : NUM_VERBS	= atoi( getnext( argv, &i ) );	break;

		case 'i' : if( numdir >= MAXDIR )
			       fatal( "Too many -i options\n" );
			   dirnames[ numdir++ ] = getnext( argv, &i );
			   break;
		case 'o' : outname	=       getnext( argv, &i );	break;
		case 't' : tmpdir	=	getnext( argv, &i );	break;

		case 'w' : wignore	= 1;				break;
		case 'd' : debugging	= 1;				break;
		case 'h' : header	= 1;				break;

		default  : print_usage( argv[ 0 ] );
	    }
	}
	else if( *inname )
	    print_usage( argv[ 0 ] );
	else
	    strcpy( inname, argv[ i ] );
    }
    if( !*inname )
	print_usage( argv[ 0 ] );
}


	/***************************************************************\
	*								*
	*	print_usage( cname ) - print the USAGE error message	*
	*								*
	\***************************************************************/

print_usage( cname )
char
    *cname;
{
    fprintf( stderr, "Usage: %s Infile (with any of the following)\n", cname );
    fprintf( stderr, "	-o Outfile		File for output\n" );
    fprintf( stderr, "	-i Dir			Directory for INCLUDEs\n" );
    fprintf( stderr, "	-t Dir			Directory for temp files\n" );
    fprintf( stderr, "	-d			Output debugging info\n" );
    fprintf( stderr, "	-w			Suppress warnings\n" );
    fprintf( stderr, "	-e N			Max # of errors\n" );
    fprintf( stderr, "	-g N			Max # of globals\n" );
    fprintf( stderr, "	-m N			Max # of strings\n" );
    fprintf( stderr, "	-n N			Max # of objects\n" );
    fprintf( stderr, "	-p N			Max # of prep phrases\n" );
    fprintf( stderr, "	-r N			Max # of routines\n" );
    fprintf( stderr, "	-s N			Max # of verb phrases\n" );
    fprintf( stderr, "	-v N			Max # of verbs\n" );
    exit( 1 );
}


	/***************************************************************\
	*								*
	*	getnext( argv, indx ) - get the next string from	*
	*	argv, whether it be the rest of the current arg		*
	*	or the next arg.  This is to allow arguments like	*
	*	"-ifoo" and "-i foo" to be equivalent.			*
	*								*
	\***************************************************************/

char *
getnext( argv, indx )
char	*argv[];
int	*indx;
{
    if( *++argv[ *indx ] )
	return argv[ *indx ];
    else
	return argv[ ++*indx ];
}


	/***************************************************************\
	*								*
	*	init() - initialize the structures and files.		*
	*								*
	\***************************************************************/

init()
{
    /* Initialize the object sizes */
    hdr.nounindex.objsize	= sizeof( int16 );
    hdr.symindex.objsize	= sizeof( struct symbol );
    hdr.verbindex.objsize	= sizeof( struct verbrec );
    hdr.objindex.objsize	= sizeof( struct objrec );
    hdr.prepindex.objsize	= sizeof( struct preprec );
    hdr.vsindex.objsize		= sizeof( struct vp_syn );
    hdr.routindex.objsize	= sizeof( address );
    hdr.varindex.objsize	= sizeof( int16 );
    hdr.codeindex.objsize	= 512;
    hdr.strtabindex.objsize	= sizeof( int32 );
    hdr.strindex.objsize	= 512;
  
    /* Allocate space for the arrays */
    varspace	= (int16 *)	       calloc(hdr.varindex.objsize, NUM_VARS);
    routspace	= (address *)	       calloc(hdr.routindex.objsize,NUM_ROUTS);
    verbspace	= (struct verbrec *)   calloc(hdr.verbindex.objsize,NUM_VERBS);
    nounspace	= (int16 *)	       calloc(hdr.nounindex.objsize,NUM_OBJS);
    objspace	= (struct objrec *)    calloc(hdr.objindex.objsize, NUM_OBJS);
    prepspace	= (struct preprec *)   calloc(hdr.prepindex.objsize,NUM_PREP);
    verbsyn	= (struct vp_syn *)    calloc(hdr.vsindex.objsize,  NUM_VS);
    str_tab	= (int32 *)	       calloc(hdr.strtabindex.objsize,NUM_STR);

    /* Check the memory allocation */
    if(		(varspace	== (int16 *)		0)		||
		(routspace	== (address *)		0)		||
		(verbspace	== (struct verbrec *)	0)		||
		(objspace	== (struct objrec *)	0)		||
		(nounspace	== (int16 *)		0)		||
		(prepspace	== (struct preprec *)	0)		||
		(verbsyn	== (struct vp_syn *)	0)		||
		(str_tab	== (int32 *)		0)		)
	fatal( "Out of memory.\n" );

    /* Make the temporary names */
    mkpath( tmp_s, tmpdir, S_VAL );
    mkpath( tmp_c, tmpdir, CODE );

    /* Initialize the virtual code & string routines */
#if UNIX
    mktemp( tmp_s );
    mktemp( tmp_c );
#endif
    S_VAL_F = open( tmp_s, WB );
    CODE_F = open( tmp_c, WB );
    if( (S_VAL_F < 0) || (CODE_F < 0) )
	fatal( "Unable to open temporary files.\n" );

    vsinit( S_VAL_F, 0L, 0,(char *)NULL,(char *)NULL,(int16 *)NULL, str_tab );
    vm_init( CODE_F, 0L, &codetab, 1 );

    /* Set up some initial values */
    NUMOBJ = 2;				/* .ALL and STRING */
    NUMNOUN = 1;			/* Skip the null noun */
    NUMVERB = 2;			/* TELLER and NOVERB */
    NUMROUT = 4;			/* NULL, DWIMD, DWIMI, and START */
    numprep = 1;			/* Skip the null preposition */
    objspace[ _ALL ].cont = _STRING;

    /* Set up the input file */
    if( open_input( inname ) == 0 ) {
	fprintf( stderr, "Error opening input file %s\n", inname );
	exit( -1 );
    }
    filenum = newstr( inname );

    /* Initialize the dictionary */
    init_predefs();
}


	/***************************************************************\
	*								*
	*	breaker() - handle a signal				*
	*								*
	\***************************************************************/

breaker()
{
    printf( "***BREAK***\n" );
    close( S_VAL_F );	close( CODE_F );
    unlink( tmp_s );	unlink( tmp_c );
    exit( -1 );
}


	/***************************************************************\
	*								*
	*	adlcomp() - the heart of the compiler.  Reads tokens	*
	*	from the current input file, processing them until	*
	*	EOF or until the maximum number of errors has been	*
	*	exceeded.  Parsing is done with recursive descent.	*
	*								*
	\***************************************************************/

adlcomp()
{
    printf( "Reading from file %s\n", inname );
    fflush( stdout );
    while( 1 ) {
	lexer();			/* Read a token			*/
	if( t_type == EOF ) {
	    printf( "Done reading file %s\n", inname );
	    fflush( stdout );
	    return;
	}

	if( (t_type >= MIN_D) && (t_type <= MAX_D) )
	    /* This is one of PREP, ADJEC, etc.  Handle them generically */
	    getlist( t_type );
	else {
	    /* We need to special case the declaration. */
	    switch( t_type ) {
		case VAR_D	: getvars();		break;
		case NOUN_D	: getnouns();		break;
		case UNDECLARED	: getassign( 1 );	break;
		case INCLUDE	: getfile();		break;
		case MESSAGE	: printmsg();		break;
		case VERB	: getverb();		break;
		case ADJEC	:
		case NOUN	:
		case NOUN_SYN	: nounassign( 1, 0 );	break;
		case '('	: globassign();		break;
		case ROUTINE	: routassign();		break;
		case PREP	: prepassign();		break;
		default		: error( ILLEGAL_SYMBOL );
				  eatuntil( ';' );
	    }	/* switch */
	}	/* else */
    }		/* while */
}		/* adlcomp */


	/***************************************************************\
	*								*
	*	printmsg() - executes the MESSAGE directive, and	*
	*	prints a message on the user's terminal.		*
	*								*
	\***************************************************************/

printmsg()
{
    lexer();				/* Get a token.			*/
    if( t_type != STRING )
	_ERR_FIX( "Illegal compile time message.\n", ';' );
    fputs( token + 1, stderr );		/* Print the token.		*/
    fflush( stderr );
    lexer();
    if( t_type != ';' )			/* Expect a following semicolon	*/
	_ERR_FIX( SEMI_EXPECTED, ';' );
}


	/***************************************************************\
	*								*
	*	getfile() - handle the INCLUDE directive, reading	*
	*	from the appropriate file.				*
	*								*
	\***************************************************************/

getfile()
{
    char *fsave;
    char msg[ 80 ], nsave[ 512 ], t_in[ 512 ], *sprintf();
    int16 lsave, numsave, i, found;

    lexer();				/* Get a token.			*/
    if( t_type != STRING )
	_ERR_FIX( "File name expected for INCLUDE.\n", ';' );
    save_input( &fsave );		/* Save the current input file	*/
    strcpy( nsave, inname );		/* Save the current input name	*/
    lsave = numline;			/* Save the current line number	*/
    numsave = filenum;			/* Save the current file number	*/
    strcpy( inname, virtstr( t_val ) );	/* Get the rep. of the new name	*/
    filenum = t_val;			/* Get the new file number	*/
    numline = 1;			/* Set the new line number	*/

    /* Try to find the file in the current directory. */
    found = 0;
    if( open_input( inname ) == 0 ) {
	/* Couldn't find it.  Try to find it in the -i directories */
	for( i = 0; i < numdir; i++ ) {
	    mkpath( t_in, dirnames[ i ], inname );
	    if( open_input( t_in ) != 0 ) {
		found = 1;
		break;
	    }
	}
	if( !found ) {
	    sprintf( msg, "Error opening file \"%s\".\n", inname );
	    fatal( msg );
	}
	strcpy( inname, t_in );
    }
    adlcomp();			/* Recursively compile the new file.	*/
    close_input();		/* Close the file			*/
    restore_input( fsave );	/* Restore the old input file		*/
    numline = lsave;		/* Restore the old line number		*/
    strcpy( inname, nsave );	/* Restore the old file name		*/
    filenum = numsave;		/* Restore the old file number		*/
    lexer();			/* Get a token.				*/
    if( t_type != ';' )		/* Expect a ';' after the INCLUDE	*/
	_ERR_FIX( SEMI_EXPECTED, ';' );
}


mkpath( s, dir, base )
char
    *s,
    *dir,
    *base;
{
    int
	len,
	sep_found;
    char
	*check;

    /* Copy the directory name into the string */
    strcpy( s, dir );
    len = strlen( s ) - 1;

    /* Check to see if the name has a path separator at the end */
    check = PATH_SEPS;
    sep_found = 0;
    while( *check ) {
	if( s[ len ] == *check ) {
	    sep_found = 1;
	    break;
	}
	check++;
    }
    if( !sep_found )
	/* Add the separator */
	strcat( s, LAST_SEP );

    /* Concatenate the base name onto the string */
    strcat( s, base );
}


	/***************************************************************\
	*								*
	*	check_predefs() - check to see whether DWIMI, DWIMD,	*
	*	START, STRING, and TELLER have been defined.  Also,	*
	*	check the current number of various things against	*
	*	their maximum.						*
	*								*
	\***************************************************************/

check_predefs()
{
    if( !routspace[ _START ] )
	error( "START undefined.\n" );
    if( !routspace[ _DWIMI ] )
	warning( "DWIMI undefined.\n" );
    if( !routspace[ _DWIMD ] )
	warning( "DWIMD undefined.\n" );
    if( !objspace[ _STRING ].props[ _ACTION - 17 ] )
	warning( "STRING undefined.\n" );
    if( !(verbspace[ _TELLER ].preact||verbspace[ _TELLER ].postact) )
	warning( "TELLER undefined.\n" );
    if( !(verbspace[ _NOVERB ].preact||verbspace[ _NOVERB ].postact) )
	warning( "NOVERB undefined.\n" );
    checkmax( NUMVAR, NUM_VARS, "VARs" );
    checkmax( NUMROUT, NUM_ROUTS, "routines" );
    checkmax( NUMOBJ, NUM_OBJS, "objects" );
    checkmax( NUMVERB, NUM_VERBS, "verbs" );
    checkmax( NUMSTR, NUM_STR, "strings" );
    checkmax( NUMPP, NUM_PREP, "prep synonyms" );
    checkmax( NUMVS, NUM_VS, "[verb prep] synonyms" );
}

	/***************************************************************\
	*								*
	*	checkmax( n, max_n, name ) - checks n against max_n,	*
	*	and prints an appropriate message if n > max_n.		*
	*								*
	\***************************************************************/

checkmax( n, max_n, name )
int
    n, max_n;
char
    *name;
{
    printf( "%d out of %d %s used\n", n, max_n, name );
    fflush( stdout );
    if( n > max_n )
	error( "Number of %s is greater than %d!\n", name );
}


	/***************************************************************\
	*								*
	*	write_code() - write out the ADL executable.		*
	*								*
	\***************************************************************/

write_code()
{
    int outf;		/* The actual file being written		*/
    int32 temp;		/* Temporary area for long int calculations	*/
    int cmask;		/* Value of the umask call			*/
    int32 time();	/* Current time stamp.				*/

    /* Flush out all dirty pages to disk. */
    vsflush();
    vm_flush( &codetab );

    /* Tie up a few loose ends */
    hdr.strtabindex.numobjs = numstr();
    temp = currcode();
    hdr.codeindex.numobjs = (temp / 512L) + 1;
    temp = numchar();
    hdr.strindex.numobjs = (temp / 512L) + 1;

    /* Make sure all is OK */
    check_predefs();

    /* Don't write any code if errors were encountered. */
    if( numerr )
	return;

    /* Set up the file pointers */
    hdr.symindex.ptr	= sizeof( struct header );
    count_symtab( debugging );
    hdr.verbindex.ptr	= hdr.symindex.ptr    +
   				hdr.symindex.objsize    * NUMSYM;
    hdr.objindex.ptr	= hdr.verbindex.ptr   +
				hdr.verbindex.objsize   * NUMVERB;
    hdr.nounindex.ptr	= hdr.objindex.ptr +
				hdr.objindex.objsize	* NUMOBJ;
    hdr.varindex.ptr	= hdr.nounindex.ptr   +
				hdr.nounindex.objsize    * NUMNOUN;
    hdr.prepindex.ptr	= hdr.varindex.ptr    +
				hdr.varindex.objsize    * NUMVAR;
    hdr.vsindex.ptr	= hdr.prepindex.ptr   +
				hdr.prepindex.objsize	* NUMPP;
    hdr.routindex.ptr	= hdr.vsindex.ptr   +
				hdr.vsindex.objsize	* NUMVS;
    hdr.codeindex.ptr	= hdr.routindex.ptr   +
				hdr.routindex.objsize   * NUMROUT;
    hdr.strtabindex.ptr	= hdr.codeindex.ptr   +
				hdr.codeindex.numobjs   * 512;
    hdr.strindex.ptr	= hdr.strtabindex.ptr +
				hdr.strtabindex.objsize * NUMSTR;
  
    /* Set the timestamp and the magic number */
    hdr.adlid = time( 0 );
    hdr.magic = M_ADL;
    sprintf( hdr.adlname, "#! %s%s%s\n", ADL_NAME,
		   (header?" -h":"\0"),
		   (debugging?" -d":"\0") );

    /* Write the file */
    if( (outf = open( outname, WB )) < 0 )
	fatal( "Error writing to output file.\n" );
    adlstats();
    lseek( outf, 0L, 0 );
    write( outf, &hdr, sizeof( struct header ) );
    write_symtab( outf, debugging );
    writebuf( outf, verbspace,	&hdr.verbindex );
    writebuf( outf, objspace,	&hdr.objindex );
    writebuf( outf, nounspace,	&hdr.nounindex );
    writebuf( outf, varspace,	&hdr.varindex );
    writebuf( outf, prepspace,	&hdr.prepindex );
    writebuf( outf, verbsyn,	&hdr.vsindex );
    writebuf( outf, routspace,	&hdr.routindex );
    writestuff( CODE_F, outf,	&hdr.codeindex );
    writebuf( outf, str_tab, &hdr.strtabindex );
    writestuff( S_VAL_F, outf,	&hdr.strindex );

    /* Close the file */
    close( outf );

#if UNIX
    /* Make it executable */
    cmask = umask( 0 );
    (void)umask( cmask );
    chmod( outname, (0777 & (~cmask)) );
#endif
}


	/***************************************************************\
	*								*
	*	adlstats() - print some statistics about the		*
	*	compilation						*
	*								*
	\***************************************************************/

adlstats()
{
    int i;
    printf( "adlcomp %s -o %s ", inname, outname );
    printf( "-g %d -r %d ", NUMVAR+1, NUMROUT+1 );
    printf( "-v %d -n %d -m %d ", NUMVERB+1, NUMOBJ+1, NUMSTR+1 );
    printf( "-p %d -s %d%s%s", NUMPP, NUMVS,
		(wignore?" -w":""),(debugging?" -d":"") );

    for( i = 0; i < numdir; i++ )
	printf( " -i %s", dirnames[ i ] );
    printf( "\nWriting %ld bytes\n",
		hdr.strindex.ptr + 512*hdr.strindex.numobjs );
    fflush( stdout );
}


	/***************************************************************\
	*								*
	*	writebuf( f, b, d ) - write the buffer b to the file	*
	*	f, given the information in the header structure d.	*
	*								*
	\***************************************************************/

/*VARARGS 1*/
writebuf( f, b, d )
int	f;
char	*b;
struct	adldir	*d;
{
    lseek( f, 0L, 2 );				/* Seek to EOF		*/
    write( f, b, d->numobjs * d->objsize );	/* Write the data	*/
}


	/***************************************************************\
	*								*
	*	writestuff( ifile, ofile, dir ) - Copy data from	*
	*	ifile to ofile, according to the information in dir.	*
	*								*
	\***************************************************************/

writestuff( ifile, ofile, dir )
int	ifile, ofile;
struct	adldir	*dir;
{
    int32 t, i, n, r;
    char buf[ 512 ];

    t = dir->numobjs * dir->objsize;
    n = t / 512;			/* Write in 512 byte chunks	*/
    r = t % 512;			/* Number of remaining bytes	*/
    for( i = 0; i < n; i++ ) {
	lseek( ifile, (long)(i*512), 0 );
	read( ifile, buf, 512 );
	lseek( ofile, 0L, 2 );		/* Seek to EOF 			*/
	write( ofile, buf, 512 );
    }
    if( r ) {
	/* Write the remaining bytes */
	lseek( ifile, (long)(n*512), 0 );
	read( ifile, buf, 512 );
	lseek( ofile, 0L, 2 );		/* Seek to EOF			*/
	write( ofile, buf, 512 );
    }
    close( ifile );			/* Close the input file.	*/
}


	/***************************************************************\
	*								*
	*	wrapup() - clean up our mess and go home.		*
	*								*
	\***************************************************************/

wrapup()
{
    char *ep, *wp;

    /* Remove the temporary files. */
    unlink( tmp_s ); unlink( tmp_c );

    /* Print how many errors and warnings we've encountered. */
    ep = ( (numerr == 1) ? "" : "s" );
    wp = ( (numwarn == 1) ? "" : "s" );
    printf( "adlcomp complete. %d error%s, %d warning%s.\n", numerr, ep,
		numwarn, wp );
}

/*** EOF adlcomp.c ***/
