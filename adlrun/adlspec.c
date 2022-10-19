#include <stdio.h>
#include <fcntl.h>

#include "adltypes.h"
#include "adlprog.h"
#include "builtins.h"
#include "vstring.h"
#include "adlrun.h"


extern
char	*H_STR;


special()
{
    int16
	t;

    assertargs( "$spec", 1 );	/* Have to have at least ($spec F) */
    switch( ARG( 1 ) ) {
	case 1 :
	    debug = !debug;
	    break;
	case 2 :
	    restart();
	    /* NOTREACHED */
	case 3 :
	    head_term();
	    exit( 0 );		/* terminate game */
	case 4 :
	    assertargs( "$spec 4", 2 );
	    savegame( virtstr( ARG( 2 ) ) );
	    break;
	case 5 :
	    assertargs( "$spec 5", 2 );
	    if( restoregame( virtstr( ARG( 2 ) ) ) )
		return;	/* Skip the fixup stuff */
	    break;
	case 6 :
	    callextern();
	    break;
	case 7 :
	    do_wordfile();
	    break;
	case 8 :
	    scripter();
	    break;
	case 9 :
	    write_head();
	    break;
	case 10 :
	    assertargs( "$spec 10", 2 );
	    scrwidth = ARG( 2 );
	    break;
	case 11 :
	    set_tty();
	    break;
	case 12 :
	    change_tty();
	    break;
	default :
	    error( 24 );	/* Illegal parameter for $spec */
    }
    popip();
    t = pop();
    sp = bp + 1;
    bp = t;
}

restart()
{
    int
	i;

#if MULTIPLEX
    for( i = 0; i < numact; i++ ) {
	if( actlist[ i ].ttyfile != (FILE *)0 ) {
	    /* Close the tty */
	    fclose( actlist[ i ].ttyfile );
	    *actlist[ i ].ttyname = '\0';
	}
    }
#endif
    restarted = 1;
    DO_EXIT( 4 );
}


set_tty()
{
#if MULTIPLEX
    int
	i;

    /* Set the actor's TTY */
    assertargs( "$spec 11", 3 );
    if( (ARG( 1 ) < 0) || (ARG( 1 ) > NUMOBJ) )
	error( 31 );		/* Illegal actor for $spec 11 */
    for( i = 0; i < numact; i++ ) {
	if( actlist[ i ].actor == ARG( 1 ) ) {
	    /* We found the actor */
	    if( actlist[ i ].ttyfile != (FILE *)0 )
		/* Close the old tty */
		fclose( actlist[ i ].ttyfile );
	    actlist[ i ].ttyfile = fopen( virstr( ARG( 2 ) ), "r+" );
	    if( actlist[ i ].ttyfile == (FILE *)0 )
		error( 32 );	/* Bad tty name */
	    strcpy( actlist[ i ].ttyname, virtstr( ARG( 2 ) ) );
	    return;
	}
    }
    error( 31 );		/* Illegal actor for $spec 11 */
#endif
}


change_tty()
{
#if MULTIPLEX
    int
	i;

    /* Set the current TTY */
    assertargs( "$spec 12", 2 );
    if( (ARG( 1 ) < 0) || (ARG( 1 ) > NUMOBJ) )
	error( 33 );		/* Illegal actor for $spec 11 */
    for( i = 0; i < numact; i++ ) {
	if( actlist[ i ].actor == ARG( 1 ) ) {
	    /* We found the actor */
	    if( actlist[ i ].ttyfile == (FILE *)0 )
		CURRTTY = SYSTTY;
	    else
		CURRTTY = actlist[ i ].ttyfile;
	    return;
	}
    }
    error( 33 );		/* Illegal actor for $spec 12 */
#endif
}


invert( s, n )
char
    *s;
int16
    n;
{
    int16
	i;

    for( i = 0; i < n; i++ )
	s[ i ] ^= CODE_CHAR;
}


savegame( savename )
char
    *savename;
{
    int
	savefile,
	cmask;
    char
	tempstr[ 80 ];
    int16
	yesno(),
	temp,
	i;
    struct macro
	*m;

    RETVAL = 0;
    savefile = open( savename, RB );
    if( savefile >= 0 ) {
	close( savefile );
	sayer( "File " );
	sayer( savename );
	sayer( " already exists. Destroy it? " );
	if( !yesno() )
	    return;
    }
    savefile = open( savename, WB );
    if( savefile < 0 ) {
	sayer( "Error opening file " );
	sayer( savename );
	sayer( "\n" );
	return;
    }
    invert( savec, numsave );
    for( i = 0; i < numact; i++ ) {
	strcpy( tempstr, actlist[ i ].linebuf );
	strcpy( actlist[ i ].savebuf, tempstr );
	actlist[ i ].linebuf = actlist[ i ].savebuf;
    }
    temp = nummacro();
    write( savefile, &hdr.adlid,	  sizeof( int32 ) );
    write( savefile, &numsave,		  sizeof( int16 ) );
    write( savefile, savec,		  numsave );
    write( savefile, &numact,		  sizeof( int16 ) );
    write( savefile, actlist,	numact *  sizeof( struct actrec ) );
    write( savefile, vecverb,	10 *	  sizeof( int16 ) );
    write( savefile, &currturn,		  sizeof( int16 ) );
    write( savefile, &prompter,		  sizeof( int16 ) );
    write( savefile, &numd,		  sizeof( int16 ) );
    write( savefile, demons,	10 *	  sizeof( int16 ) );
    write( savefile, &numf,		  sizeof( int16 ) );
    write( savefile, fuses,	10 *	  sizeof( int16 ) );
    write( savefile, ftimes,	10 *	  sizeof( int16 ) );
    write( savefile, f_actors,	10 *	  sizeof( int16 ) );
    write( savefile, objspace,	NUMOBJ  * sizeof( struct objrec ) );
    write( savefile, verbspace,	NUMVERB * sizeof( struct verbrec ) );
    write( savefile, stack,	NUMVAR  *  sizeof( int16 ) );
    write( savefile, &temp,		  sizeof( int16 ) );
    for( m = mactab; m; m = m->next ) {
	invert( m->name, 10 );
	invert( m->val, 80 );
	write( savefile, m->name, 10 );
	write( savefile, m->val,  80 );
	invert( m->name, 10 );
	invert( m->val, 80 );
    }
    close( savefile );
#if UNIX
    cmask = umask( 0 );
    (void)umask( cmask );
    chmod( savename, 0666 & ~cmask );
#endif
    invert( savec, numsave );
    RETVAL = 1;
}


restoregame( savename )
char
    *savename;
{
    int
	savefile;
    char
	mname[ 10 ],
	mval[ 80 ];
    int32
	tempid;
    int16
	num,
	i;

    savefile = open( savename, RB );
    if( savefile < 0 ) {
	sayer( "Error opening file " );
	sayer( savename );
	sayer( "\n" );
	return 0;
    }
    read( savefile, &tempid,	sizeof( int32 ) );
    if( tempid != hdr.adlid ) {
	sayer( "Error: \"" );
	sayer( savename );
	sayer( "\" is not a save file for this game.\n" );
	return 0;
    }
#if MULTIPLEX
    for( i = 0; i < numact; i++ )
	if( actlist[ i ].ttyfile != (FILE *)0 )
	    /* Close the tty */
	    fclose( actlist[ i ].ttyfile );
#endif
    read( savefile, &numsave,		 sizeof( int16 ) );
    read( savefile, savec,		 numsave );
    read( savefile, &numact,		 sizeof( int16 ) );
    read( savefile, actlist,	numact * sizeof( struct actrec ) );
    read( savefile, vecverb,	10 *	 sizeof( int16 ) );
    read( savefile, &currturn,		 sizeof( int16 ) );
    read( savefile, &prompter,		 sizeof( int16 ) );
    read( savefile, &numd,		 sizeof( int16 ) );
    read( savefile, demons,	10 *	 sizeof( int16 ) );
    read( savefile, &numf,		 sizeof( int16 ) );
    read( savefile, fuses,	10 *	 sizeof( int16 ) );
    read( savefile, ftimes,	10 *	 sizeof( int16 ) );
    read( savefile, f_actors,	10 *	 sizeof( int16 ) );
    read( savefile, objspace,	NUMOBJ * sizeof( struct objrec ) );
    read( savefile, verbspace,	NUMVERB * sizeof( struct verbrec ) );
    read( savefile, stack,	NUMVAR * sizeof( int16 ) );
    read( savefile, &num,		 sizeof( int16 ) );
    clearmacro();
    for( i = 0; i < num; i++ ) {
	read( savefile, mname,	10 );
	read( savefile, mval,	80 );
	invert( mname, 10 );
	invert( mval, 80 );
	define( mname, mval );
    }
    close( savefile );

    /* Fix up the actlist string pointers, which may have changed since
       the file was saved. */
    for( i = 0; i < numact; i++ )
	actlist[ i ].linebuf = actlist[ i ].savebuf;

    /* Decode the saved character buffer */
    invert( savec, numsave );

#if MULTIPLEX
    /* Reopen the actor tty's */
    for( i = 0; i < numact; i++ )
	if( actlist[ i ].ttyfile != (FILE *)0 )
	    actlist[ i ].ttyfile = fopen( actlist[ i ].ttyname, "r+" );
#endif

    /* It was a successful restore, so stop running */
    ip = 0L;
    bp = sp = NUMVAR;
    return 1;
}


do_wordfile()
{
    assertargs( "$spec 7", 2 );
    if( wordwrite )
	fclose( wordfile );
    wordwrite = 0;
    if( ARG( 2 ) ) {
	wordwrite = 1;
	wordfile = fopen( virtstr( ARG( 2 ) ), "a" );
    }
}


callextern()
{
#if UNIX
    int
	i;
    char
	**argv,
	*t;

    /* Have to have at least ($spec 6 "name" 0) */
    assertargs( "$spec 6", 3 );
    if( fork() )
	/* We're the parent - wait for the child to die */
	RETVAL = wait( 0 );
    else {
	/* We're the child. Get memory for the argument vector */
	argv = (char **)malloc( (RETVAL - 1) * sizeof( char * ) );
	if( argv == (char **)0 )
	    error( 27 );			/* Out of memory */

	/* Fill the argument vectors */
	for( i = 2; i < RETVAL && ARG( i ); i++ ) {
	    t = virtstr( ARG( i ) );
	    argv[ i - 2 ] = malloc( strlen( t ) + 1 );
	    if( argv[ i - 2 ] == (char *)0 )
		error( 27 );			/* Out of memory */

	    strcpy( argv[ i - 2 ], t );
	}

	/* Set the last argument to be zero */
	argv[ RETVAL - 2 ] = (char *)0;

	/* Execute the program */
	execv( argv[ 0 ], argv );

	/* In case the exec failed, exit. */
	head_term();
	exit( -1 );
    }
#endif
}



scripter()
{
    assertargs( "$spec 8", 2 );
    if( scriptfile != (FILE *)0 )
	fclose( scriptfile );
    scriptfile = (FILE *)0;
    if( ARG( 2 ) )
	scriptfile = fopen( virtstr( ARG( 2 ) ), "w" );
}

/*** EOF adlspec.c ***/
