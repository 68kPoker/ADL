#include <stdio.h>

#include "adltypes.h"
#include "adlprog.h"
#include "builtins.h"
#include "adlrun.h"

dosysfunc( rp )
int16	rp;
{
    int16
	t;

    switch( rp ) {
	/* Object routines */
	case _LOC	:
	case _CONT	:
	case _LINK	:
	case _MODIF	: objprop( rp );			break;
	case _LDESC	:
	case _SDESC	:
	case _ACTION	: t = _LDESC;
			  objprop( abs( rp - t ) + _LD );	break;
	case _PROP	: assertargs( "$prop", 2 );
			  objprop( ARG(2) );		break;
	case _SETP	: setp();				break;
	case _MOVE	: move_obj();			break;

	/* Verb routines */
	case _VSET	: vset();			break;
	case _VPROP	: vprop();			break;

	/* Arithmetic routines */
	case _PLUS	: assertargs( "$plus", 2 );
			  RETVAL = ARG(1) + ARG(2);	break;
	case _MINUS	: assertargs( "$minus", 2 );
			  RETVAL = ARG(1) - ARG(2);	break;
	case _TIMES	: assertargs( "$times", 2 );
			  RETVAL = ARG(1) * ARG(2);	break;
	case _DIV	: do_div();			break;
	case _MOD	: do_mod();			break;
	case _RAND	: assertargs( "$rand", 1);
			  RETVAL = myrand( ARG(1) );	break;

	/* Boolean routines */
	case _AND	: do_and();			break;
	case _OR	: do_or();			break;
	case _NOT	: assertargs( "$not", 1 );
			  RETVAL = !ARG(1);		break;
	case _YORN	: RETVAL = yesno();		break;
	case _PCT	: assertargs( "$pct", 1 );
			  RETVAL = ARG(1) >= myrand(100);	break;
	case _EQ	: assertargs( "$eq", 2 );
			  RETVAL = ARG(1) == ARG(2);	break;
	case _NE	: assertargs( "$ne", 2 );
			  RETVAL = ARG(1) != ARG(2);	break;
	case _LT	: assertargs( "$lt", 2 );
			  RETVAL = ARG(1) <  ARG(2);	break;
	case _GT	: assertargs( "$gt", 2 );
			  RETVAL = ARG(1) >  ARG(2);	break;
	case _LE	: assertargs( "$le", 2 );
			  RETVAL = ARG(1) <= ARG(2);	break;
	case _GE	: assertargs( "$ge", 2 );
			  RETVAL = ARG(1) >= ARG(2);	break;

	/* Miscellaneous routines */
	case _SAY	: saystr();			break;
	case _ARG	: do_args();			break;
	case _EXIT	: do_exit();			return;
	case _RETURN	: do_rtrn();			return;
	case _VAL	: do_val();			break;
	case _PHASE	: RETVAL = Phase;		break;
	case _SPEC	: special();			return;

	/* Global value routines */
	case _SETG	: setg();			break;
	case _GLOBAL	: assertargs( "$global", 1 );
			  varconts( ARG( 1 ) );		break;
	case _VERB	: varconts( _VERB_G );		break;
	case _DOBJ	: varconts( _DOBJ_G );		break;
	case _IOBJ	: varconts( _IOBJ_G );		break;
	case _PREP	: varconts( _PREP_G );		break;
	case _CONJ	: varconts( _CONJ_G );		break;
	case _NUMD	: varconts( _NUMD_G );		break;

	/* Transition routines */
	case _SETV	: setverb();			break;
	case _HIT	: hitverb();			break;
	case _MISS	: missverb();			return;

	/* String manipulation routines */
	case _EQST	: eqstring();			break;
	case _SUBS	: substring();			break;
	case _LENG	: lengstring();			break;
	case _CAT	: catstring();			break;
	case _POS	: posstring();			break;
	case _READ	: readstring();			break;
	case _SAVESTR	: savestr();			break;

	/* Name routines */
	case _NAME	: do_name();			break;
	case _VNAME	: do_vname();			break;
	case _MNAME	: do_mname();			break;
	case _PNAME	: do_pname();			break;

	/* Conversion routines */
	case _STR	: do_str();			break;
	case _NUM	: do_num();			break;
	case _ORD	: ordstring();			break;
	case _CHR	: chrstring();			break;

	/* Internal structure routines */
	case _SDEM	: setdemon();			break;
	case _DDEM	: deldemon();			break;
	case _SFUS	: setfuse();			break;
	case _DFUS	: assertargs( "$dfus", 2 );
			  delfuse( ARG(1), ARG(2) );	break;
	case _INCTURN	: incturn();			break;
	case _TURNS	: retturn();			break;
	case _PROMPT	: doprompt();			break;
	case _ACTOR	: setactor();			break;
	case _DELACT	: assertargs( "$delact", 1 );
			  delactor( ARG(1) );		break;
	case _DEFINE	: do_define();			break;
	case _UNDEF	: do_undef();			break;

	default		: error( 3 );	/* Illegal builtin */
    }

    if( sp <= NUMVAR )	/* A $exit was called by a fuse or something */
	return;
    popip();
    rp = pop();
    sp = bp + 1;
    bp = rp;
}

#if DEBUG
assertargs( s, n )
char	*s;
int16	n;
{
    if( n >= RETVAL ) {
	fprintf( stderr, "%s: ", s );
	error( 2 );		/* Too few arguments */
    }
}
#endif


do_rtrn()
{
    int16
	retval, oldbp;

    assertargs( "$return", 1 );
    popip();
    bp = pop();
    retval = pop();
    oldbp = stack[ bp + stack[ bp ] ];
    ip = stack[ bp + stack[ bp ] + 1 ];
    sp = bp;
    push( retval );
    bp = oldbp;
}


do_exit()
{
    int16
	code;

    assertargs( "$exit", 1 );
    code = ARG( 1 );
    if( (code < 0) || (code > 3) )
	error( 28 );
    if( exits[ code ].exit_ok == 0 )
	error( 28 );
    bp = sp = NUMVAR;		/* Trim the stack */
    ip = 0;			/* Clear the IP */
    DO_EXIT( code );
}


do_val()
{
    assertargs( "$val", 1 );
    RETVAL = ARG( 1 );
}


getstring( s )
char	*s;
{
#if MULTIPLEX
    fseek( CURRTTY, 0L, 0 );
    if( !fgets( s, SLEN, CURRTTY ) ) {
	/* On EOF, delete the current actor */
	delactor( CURRACT );
	DO_EXIT( 1 );
    }
#else
    if( !gets( s ) ) {		/* EOF detected! */
	head_term();
	exit( -1 );			/* Exit program */
    }
#endif

    if( scriptfile != (FILE *)0 )
	fprintf( scriptfile, "%s\n", s );
    numcol = 0;
}


int16
yesno()
{
    char
	s[ 80 ];
    int16
	i;

    getstring( s );
    for( i = 0; (s[ i ] == ' ') || (s[ i ] == '\t'); i++ )
	/* NOTHING */;
    if( (s[ 0 ] == 'y') || (s[ 0 ] == 'Y') )
	return 1;
    else
	return 0;
}


do_args()
{
    int16
	oldbp;

    assertargs( "$arg", 1 );
    oldbp = stack[ sp - 2 ];
    if( ARG( 1 ) )
	RETVAL = stack[ oldbp + ARG( 1 ) ];
    else
	RETVAL = stack[ oldbp ] - 1;
}

/*** EOF adlfuncs.c ***/
