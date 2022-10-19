#if UNIX
#  include <signal.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>

#include "adltypes.h"
#include "adlprog.h"
#include "adldef.h"
#include "vstring.h"
#include "virtmem.h"

#define NUMCODE	(hdr.codeindex.numobjs * 512)

#undef max
#undef min
#define max(a,b) (a > b ? a : b)
#define min(a,b) (a < b ? a : b)

char
    *inname;			/* Input file name			*/
int
    infile;			/* Input file				*/

struct pagetab
    codetab;			/* Virtual memory table			*/

address
    *sortedrouts,		/* Sorted list of routine entry points	*/
    bot, top;			/* Beginning and ending of action range	*/
int16
    blocked,			/* Semaphore for breaker		*/
    was_signaled,		/* Were we signaled?			*/
    my_nums;			/* Temporary for virtual strings	*/
struct symbol
    *symtab;			/* Symbol table				*/

struct	header		hdr;		/* Actual header		*/
struct	verbrec		*verbspace;	/* Verb contents		*/
struct	objrec		*objspace;	/* Object contents		*/
struct	preprec		*prepspace;	/* Preposition contents		*/
struct	vp_syn		*verbsyn;	/* Verb synonyms		*/
int16			*varspace;	/* Stack & variables		*/
int16			*nounspace;	/* Noun indices			*/
address			*routspace;	/* Routine indexes		*/
int32			*str_tab;	/* String table			*/

char
    *malloc();			/* Memory allocator			*/

main( argc, argv )
int
    argc;
char
    *argv[];
{
    if( argc != 2 ) {
	/* Too few command line arguments */
	printf( "Usage: %s dungeon.\n", argv[ 0 ] );
	exit( -1 );
    }
    inname = argv[ 1 ];		/* Save the input file name		*/
    init();			/* Initialize the structures		*/
    adldebug();			/* Do the debugging			*/
}


init()
{
    int
	breaker();		/* Signal catcher		*/

    /* Open the file */
    if( (infile = open( inname, RB )) < 0 ) {
	printf( "Error opening file %s\n", inname );
	exit( -1 );
    }

    /* Read the header structure */
    lseek( infile, 0L, 0 );
    read( infile, &hdr, sizeof( struct header ) );
    if( hdr.magic != M_ADL ) {
	printf( "%s : not an ADL datafile.\n", inname );
	exit( -1 );
    }

    /* Load the arrays of things */
    loadarray( &varspace,	&hdr.varindex );
    NUMROUT++;
    loadarray( &routspace,	&hdr.routindex );
    NUMROUT--;
    loadarray( &symtab,		&hdr.symindex );
    loadarray( &verbspace,	&hdr.verbindex );
    loadarray( &objspace,	&hdr.objindex );
    loadarray( &nounspace,	&hdr.nounindex );
    loadarray( &str_tab,	&hdr.strtabindex );

    /* Initialize the virtual memory routines */
    vsinit( infile, hdr.strindex.ptr, 1, (char *)NULL, (char *)NULL, &my_nums,
	    str_tab );
    vm_init( infile, hdr.codeindex.ptr, &codetab, 0 );

    /* Print the copyright messages */
    fputs( "ADL debugger - Version 2.1 - April 28, 1987\n", stderr );
    fputs( "Copyright 1985, 1986, 1987 by Ross Cunniff\n", stderr );
    fputs( "All rights reserved.\n", stderr );
    fflush( stderr );

    /* Sort the routine entry points, and decode the symbol table */
    sortrouts();
    sym_decode();

    /* All done.  Print the prompt and enable the break catcher. */
    fputs( "Type ? for help.\n", stderr );
    fflush( stderr );
}


loadarray( a, d )
char	**a;
struct	adldir *d;
{
    if( d->numobjs * d->objsize ) {
	lseek( infile, d->ptr, 0 );
	*a = malloc( d->numobjs * d->objsize );
	if( *a == (char *)0 ) {
	    fprintf( stderr, "Out of memory.\n" );
	    exit( -1 );
	}
	read( infile, *a, d->numobjs * d->objsize );
    }
}


sortrouts()
{
    int16
	i, j;			/* Loop counters			*/
    char
	*used;

    /* Tell the user what we're doing */
    fputs( "Sorting routines...", stderr );
    fflush( stderr );

    /* Get memory for the sorted routines and zero it out */
    sortedrouts = (address *)malloc( sizeof( int16 ) * (NUMROUT + 1) );
    used = malloc( sizeof( char ) * (NUMROUT + 1) );
    if( (sortedrouts == (address *)0) || (used == (char *)0) ) {
	fprintf( stderr, "Out of memory.\n" );
	exit( -1 );
    }

    for( i = 0; i <= NUMROUT; i++ )
	used[ i ] = sortedrouts[ i ] = 0;

    /* Use a simple selection sort. (yeah, yeah, O(N**2), but I'm lazy) */
    for( i = 1; i <= NUMROUT; i++ ) {
	for( j = 0; j < NUMROUT; j++ ) {
	    if(		!used[ j ] &&
		(routspace[ j ] > routspace[ sortedrouts[ NUMROUT - i ] ]) )
		sortedrouts[ NUMROUT - i ] = j;
	}
	used[ sortedrouts[ NUMROUT - i ] ] = 1;
    }

    routspace[ NUMROUT ] = NUMCODE + 1;

    /* All done! */
    fputs( "Done.\n", stderr );
    fflush( stderr );
    sortedrouts[ NUMROUT ] = NUMROUT;
    free( used );
}


sym_decode()
{
    int16
	i, j;

    /* Tell the user what we're doing */
    fputs( "Decoding symbol table...", stderr );
    fflush( stderr );

    /* Un-encode the symbol table, by inverting each character */
    for( i = 1; i < NUMSYM; i++ ) {
	if( !*symtab[ i ].name ) {
	    /* This name is null - make it a question mark */
	    symtab[ i ].name[ 0 ] = '?';
	    symtab[ i ].name[ 1 ] = '\0';
	}
	else {
	    for( j = 0; symtab[ i ].name[ j ]; j++ )
		symtab[ i ].name[ j ] ^= CODE_CHAR;
	}
    }

    /* All done! */
    fputs( "Done.\n", stderr );
    fflush( stderr );
}


breaker()
{
    if( !blocked ) {
	blocked = was_signaled = 1;
	puts( "\n" );
    }
}


adldebug()
{
    int
	prompt = 1;

    while( 1 ) {
	was_signaled = 0;
	blocked = 0;
	if( prompt ) {
	    fflush( stdout );
	    fputs( "> ", stderr );
	    fflush( stderr );
	}
	prompt = 1;
	switch( getchar() ) {
	    /* Execute the user's command */
	    case 'a' : eatEOL();		  dumpsymbols( ADJEC );	break;
	    case 'd' : eatEOL();		  dumpdir();		break;
	    case 'g' : getargs( 0, NUMVAR - 1 );  dumpglobs();		break;
	    case 'i' : getargs( 1, NUMCODE - 1 ); dumpinstr();		break;
	    case 'm' : getargs( 1, NUMSTR - 1 );  dumpstrings();	break;
	    case 'n' : getargs( 0, NUMNOUN - 1 ); dumpnouns();		break;
	    case 'o' : getargs( 0, NUMOBJ - 1 );  dumpobjs();		break;
	    case 'r' : getargs( 1, NUMROUT - 1 ); dumprouts();		break;
	    case 's' : getargs( 0, NUMSYM - 1 );  dumpsymbols( 0 );	break;
	    case 'v' : getargs( 0, NUMVERB - 1 ); dumpverbs();		break;
	    case '?' : eatEOL();		  dohelp();		break;
	    case EOF :
	    case 'q' : exit( 0 );
	    case '\t':
	    case ' ' : prompt = 0;
	    case '\n': break;
	    default  : eatEOL();		  puts( "\007" );
	}
    }
}


eatEOL()
{
    while( getchar() != '\n' )
	/* NOTHING */;
}


getargs( lower, upper )
address
    lower, upper;
{
    int
	ch;

    bot = top = 0;
    ch = eatwhite( ' ' );
    if( ch == '$' ) {
	bot = upper;
	ch = mygetchar();
    }
    else
	while( isdigit( ch ) ) {
	    bot = 10L * bot + (ch - '0');
	    ch = mygetchar();
	}
    bot = min( max( bot, lower), upper );
    ch = eatwhite( ch );
    if( ch == '\n' ) {
	top = bot;
	return;
    }
    else if( ch != '-' ) {
	top = bot - 1;
	while( ch != '\n' )
	    ch = mygetchar();
	return;
    }

    ch = eatwhite( ' ' );
    if( ch == '$' )
	top = upper;
    else
	while( isdigit( ch ) ) {
	    top = 10L * top + (ch - '0');
	    ch = mygetchar();
	}
    top = min( max( top, lower ), upper );
    while( ch != '\n' )
	ch = mygetchar();
}


mygetchar()
{
    int
	ch;

    if( (ch = getchar()) == EOF )
	exit( 0 );
    return ch;
}


eatwhite( ch )
int
    ch;
{
    while( (ch == ' ') || (ch == '\t') )
	ch = mygetchar();
    return ch;
}


dumpsymbols( t )
int16
    t;
{
    int
	i;
    char
	*print_type();

    if( t ) {
	bot = 1; top = NUMSYM - 1;
    }
    for( i = bot; i <= top; i++ ) {
	checkbreak( breaker );
	if( was_signaled )
	    return;
	if( t && symtab[ i ].type != t )
	    continue;
	printf( "symtab[ %4d ].type = %s .val = %4d, .name = \"%s\"\n", i,
		print_type( symtab[ i ].type ),
		symtab[ i ].val, symtab[ i ].name );
    }
}


char *
print_type( type )
int
    type;
{
    switch( type ) {
	case NOUN     : return "NOUN,    ";
	case VERB     : return "VERB,    ";
	case ADJEC    : return "ADJEC,   ";
	case PREP     : return "PREP,    ";
	case ROUTINE  : return "ROUTINE, ";
	case STRING   : return "STRING,  ";
	case CONST    : return "CONST,   ";
	case VAR      : return "GLOBAL,  ";
	case ARGUMENT : return "ARGUMENT,";
	case NOUN_SYN : return "NOUN_SYN,";
	case ARTICLE  : return "ARTICLE, ";
	default	  : return "UNKNOWN, ";
    }
}


dumpdir()
{
    printf( "File name is %s\n", inname );
    printf( "ADL id is %d\n", hdr.adlid );
    printf( "%8d bytes in instructions\n",
  	hdr.codeindex.numobjs * hdr.codeindex.objsize );
    printf( "%8ld bytes in messages\n\n",
  	(int32)((int32)hdr.strindex.numobjs * (int32)hdr.strindex.objsize ) );
    printf( "%8d bytes in %8d string table entries\n",
  	4*hdr.strtabindex.numobjs, hdr.strtabindex.numobjs );
    printf( "%8d bytes in %8d symbols",
 	NUMSYM*hdr.symindex.objsize, NUMSYM );
    printf( " (%d legal)\n", find_leg() );
    printf( "%8d bytes in %8d verbs\n",
	NUMVERB*hdr.verbindex.objsize, NUMVERB );
    printf( "%8d bytes in %8d objects\n", NUMOBJ*hdr.objindex.objsize, NUMOBJ );
    printf( "%8d bytes in %8d nouns\n",
	NUMNOUN*hdr.nounindex.objsize, NUMNOUN );
    printf( "%8d bytes in %8d routines\n",
	NUMROUT*hdr.routindex.objsize,NUMROUT);
    printf( "%8d bytes in %8d globals\n",
	NUMVAR * hdr.varindex.objsize, NUMVAR );
    printf( "%8d adjectives\n", findone( ADJEC ) );
    printf( "%8d articles\n", findone( ART ) );
}


find_leg()
{
    int
	i, num_leg;

    num_leg = 0;
    for( i = 0; i < NUMSYM; i++ )
	if( *symtab[ i ].name != '?')
	    num_leg++;
    return num_leg;
}


findone( type )
int
    type;
{
    int
	i, num;

    num = 0;
    for( i = 1; i < NUMSYM; i++ )
	if( symtab[ i ].type == type )
	    num++;
    return num;
}


dumpglobs()
{
    int
	i;

    fputs( "Globals:\n", stdout );
    for( i = bot; i <= top; i++ ) {
	checkbreak( breaker );
	if( was_signaled )
	    return;
	printf( "\tVAR( %d ) = %d\n", i, varspace[ i ] );
    }
}


dumpinstr()
{
    long
	i,		/* Loop counter				*/
	lastrout;	/* Last routine we've seen		*/
    address
	t;
    char
	printone();	/* Routine to print an instruction	*/

    /* Find which routine this instruction is in */
    lastrout = 0;
    while(	(((long)routspace[ sortedrouts[ lastrout ] ]) <= ((long)bot)) &&
		(((long)lastrout) < ((long)NUMROUT)) )
	lastrout++;
    lastrout--;

    /* Print the instructions */
    printf( "ROUTINE %d + %ld:\n", sortedrouts[ lastrout ],
	   (long)bot - (long)routspace[ sortedrouts[ lastrout ] ] );
    for( i = bot; i <= top; /* NULL */ ) {
	checkbreak( breaker );
	if( was_signaled )
	    return;
	if( ((long)i) >= ((long)routspace[ sortedrouts[ lastrout + 1 ] ]) ) {
	    /* We crossed a routine boundary */
	    if( lastrout < NUMROUT ) {
		lastrout++;
		printf( "ROUTINE %d:\n", sortedrouts[ lastrout ] );
	    }
	    else
		return;
	}
	t = i;
	(void)printone( &t );
	i = t;
    }
}


char
printone( addr )
address
    *addr;
{
    char
	opr, *s;
    address
	num;
    int16
	opnd;

    opr = vm_get8( (int32)(*addr), &codetab );
    if( opr & PUSHN ) {
	/* The high order bit is set - this is a PUSH NEG */
	if( opr & 0x7F )
	    opnd = 0xFF00 | opr;	/* Perform the sign extension */
	else
	    opnd = 0;
	opr = num = 1;
	printf( "\t\t%08d : PUSH     %d\n", *addr, opnd );
    }
    else if( opr & (PUSHARG | PUSHLOCL | CALL) ) {
	switch( opr & (PUSHARG | PUSHLOCL | CALL) ) {
	    case PUSHARG  : s = "PUSHARG  "; break;
	    case PUSHLOCL : s = "PUSHLOCL "; break;
	    case CALL     : s = "CALL     "; break;
	}
	printf( "\t\t%08d : %s%d\n", *addr, s, opr & 0x01F );
	opr = num = 1;
    }
    else if( opr & (PUSHS | JMP | JMPZ) ) {
	switch( opr & (PUSHS | JMP | JMPZ) ) {
	    case PUSHS:
		s = "PUSH     ";
		opnd = (opr & 0x07) << 8;
		opnd |= vm_get8( (int32)(*addr + 1), &codetab ) & 0xFF;
		if( opnd > 1023 )
		    /* Sign extend the thing */
		    opnd = opnd - 2048;
		num = 2;
		break;
	    case JMP  :
		s = "JMP      ";
		opnd = vm_get16( (int32)(*addr) + 1, &codetab ) & 0x0FFFF;
		num = 3;
		break;
	    case JMPZ :
		s = "JMPZ     ";
		opnd = vm_get16( (int32)(*addr) + 1, &codetab ) & 0x0FFFF;
		num = 3;
		break;
	}
	printf( "\t\t%08d : %s%d\n", *addr, s, opnd );
	opr = 1;
    }
    else {
	switch( opr ) {
	    case NOP    : s = "NOP      "; num = 1; break;
	    case PUSHME : s = "PUSHME   "; num = 1; break;
	    case POP    : s = "POP      "; num = 1; break;
	    case RET    : s = "RET      "; num = 1; break;
	    case FILEN  : s = "FILEN    "; num = 3; break;
	    case LINEN  : s = "LINEN    "; num = 3; break;
	    case PUSH   : s = "PUSH     "; num = 3; break;
	    default     : s = "ILLEGAL  "; num = 0; opr = -1;
	}
	if( num <= 1 )
	    printf( "\t\t%08ld : %s\n", (long) *addr, s );
	else
	    printf(	"\t\t%08ld : %s%d\n", (long) *addr, s,
			vm_get16( (int32)(*addr + 1), &codetab ) );
    }
    *addr += num;
    return opr;
}


dumpstrings()
{
    int16
	i;

    for( i = bot; i <= top; i++ ) {
	checkbreak( breaker );
	if( was_signaled )
	    return;
	printf( "Virtstr( %d ) = %s\n", i, virtstr( i ) );
    }
}


dumpnouns()
{
    int16
	i;

    fputs( "Nouns:\n", stdout );
    for( i = bot; i <= top; i++ ) {
	checkbreak( breaker );
	if( was_signaled )
	    return;
	printf( "\tNOUN %s\n", symtab[ find_sym( NOUN, i ) ].name );
	printf( "\t\tnounspace[ %5d ] = %d\n", i, nounspace[ i ] );
    }
}


find_sym( t, v )
int
    t;
int16
    v;
{
    int
	i;

    for( i = 0; i < NUMSYM; i++ )
	if( symtab[ i ].type == t && symtab[ i ].val == v )
	    return i;
    return 0;
}


dumpobjs()
{
    int16
	i, j, t;

    fputs( "Objects:\n", stdout );
    for( i = bot; i <= top; i++ ) {
	checkbreak( breaker );
	if( was_signaled )
	    return;

	/* Print the object name */
	t = objspace[ i ].adj;
	if( t < 0 )
	    /* This modifier is a verb */
	    printf(	"\tOBJECT %s %s", symtab[ find_sym( VERB, -t ) ].name,
			symtab[ find_sym( NOUN, objspace[ i ].noun ) ].name );
	else if( t > 0 )
	    /* This modifier is an adjective */
	    printf(	"\tOBJECT %s %s", symtab[ find_sym( ADJEC, t ) ].name,
			symtab[ find_sym( NOUN, objspace[ i ].noun ) ].name );
	else
	    /* This object has no modifier */
	    printf( "\tOBJECT %s",
			symtab[ find_sym( NOUN, objspace[ i ].noun ) ].name );
	printf( "\t{val is %d}\n", i );

	/* Print the object properties */
	printf( "\t\tLOC        = %d\n", objspace[ i ].loc );
	printf( "\t\tCONT       = %d\n", objspace[ i ].cont );
	printf( "\t\tLINK       = %d\n", objspace[ i ].link );
	printf( "\t\tOTHERS     = %d\n", objspace[ i ].others );
	printf( "\t\tPROPS 1-16 = %04x\n", objspace[ i ].props1to16 );
	for( j = 0; j < _LD - 17; j++ )
	    printf( "\t\tPROP[ %d ] = %d\n", j + 17, objspace[i].props[j]);
	printf( "\t\tLDESC      = %d\n", objspace[ i ].props[ _LD - 17] );
	printf( "\t\tSDESC      = %d\n", objspace[ i ].props[ _SD - 17 ] );
	printf( "\t\tACTION     = %d\n",objspace[i].props[ _ACT - 17 ]);
	fputs( "\n", stdout );
    }
    fputs( "\n", stdout );
}


dumprouts()
{
    address
	j;
    int16
	i;
    char
	t;

    fputs( "Routines:\n", stdout );
    for( i = bot; i <= top; i++ ) {
	printf( "\tROUTINE %d:\n", i );
	j = routspace[ i ];
	if( !j )
	    fputs( "\t\tNULL\n", stdout );
	else {
	    t = printone( &j );
	    while( (t != RET) && (t >= 0) ) {
		checkbreak( breaker );
		if( was_signaled )
		    return;
		t = printone( &j );
	    }
	}
	fputs( "\n", stdout );
    }
}


dumpverbs()
{
  int16
	i, v;

  fputs( "Verbs:\n", stdout );
    for( i = bot; i <= top; i++ ) {
	v = find_sym( VERB, i );
	checkbreak( breaker );
	if( was_signaled )
	    return;
	printf( "\tVERB %s; { val is %d }\n", symtab[ v ].name, i );
	printf( "\t\tPREACT = %d\n", verbspace[ i ].preact );
	printf( "\t\tACTION = %d\n", verbspace[ i ].postact );
	fputs( "\n", stdout );
    }
    fputs( "\n", stdout );
}


dohelp()
{
    puts( "Commands available:" );
    puts( "  a       -- print out all adjectives" );
    puts( "  d       -- print out debugging information" );
    puts( "  g RANGE -- print out globals in RANGE" );
    puts( "  i RANGE -- print out instructions in RANGE" );
    puts( "  m RANGE -- print out virtual strings in RANGE" );
    puts( "  n RANGE -- print out nouns in RANGE" );
    puts( "  o RANGE -- print out objects in RANGE" );
    puts( "  q       -- quit adldebug" );
    puts( "  r RANGE -- print out routines in RANGE" );
    puts( "  s RANGE -- print out symbols in RANGE" );
    puts( "  v RANGE -- print out verbs in RANGE" );
    puts( "  ?       -- print out this list" );
    puts( "\nRANGE is either NUMBER, or NUMBER-NUMBER, where NUMBER" );
    puts( "is either a number or the character '$' (representing the" );
    puts( "largest possible value)." );
}

/*** EOF adldebug.c ***/
