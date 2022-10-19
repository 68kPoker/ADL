#include <stdio.h>

#include "adltypes.h"
#include "adlprog.h"
#include "adlrun.h"
#include "adldef.h"
#include "virtmem.h"


extern
struct	pagetab	codetab;

extern
int16	filenum,	/* From adlerr.c */
	linenum,	/* From adlerr.c */
	foundfile;	/* From adlerr.c */


callrouts()
{
    int16
	i, dirobj;

    if( dwimi() < 0 )					/* Iobj DWIMMING */
	return;

    if( dwimd() < 0 )				/* Dobj DWIMMING */
	return;

    for( (dirobj = NumDobj ? NumDobj : 1), i = 0; i < dirobj; i++ ) {
	if( SET_EXIT( 2 ) != 0 )
	    /* $exit 2 */
	    continue;

	*Conj = Conj[ i ];
	*Dobj = Dobj[ i ];

	Phase = 2;
	if( SET_EXIT( 0 ) == 0 )
	    /* Actor ACTION */
	    callone( objspace[ CURRACT ].props[ _ACT - 17 ] );

	Phase = 3;
	if( SET_EXIT( 0 ) == 0 )
	    /* Verb PREACT */
	    callone( verbspace[ Verb ].preact );

	Phase = 4;
	if( SET_EXIT( 0 ) == 0 )
	    /* Indirect object ACTION */
	    callone( objspace[ ((Iobj < 0)?_STRING:Iobj) ].props[ _ACT - 17 ] );

	Phase = 5;
	if( SET_EXIT( 0 ) == 0 )
	    /* Dobj ACTION */
	    callone( objspace[((*Dobj < 0)?_STRING:*Dobj)].props[ _ACT - 17 ] );

	Phase = 6;
	if( SET_EXIT( 0 ) == 0 )
	    /* Verb ACTION */
	    callone( verbspace[ Verb ].postact );
    }
    CLR_EXIT( 2 );

    Phase = 7;
    if( SET_EXIT( 0 ) == 0 )
	/* ROOM ACTION */
	callone( objspace[ objspace[ CURRACT ].loc ].props[ _ACT - 17 ] );
    CLR_EXIT( 0 );

    CLR_EXIT( 1 );
}


execdems()
{
    int16
	i;

    if( !numact ) {
	fputs( "No actors active.  ADL aborting.\n", stderr );
	head_term();
	exit( -1 );
    }
    for( i = 0; i < numd; i++ )
	callone( demons[ i ] );
}


execfuses()
{
    int16
	i;

    for( i = numf - 1; i >= 0; i-- )
	if( (ftimes[ i ] <= currturn) && (f_actors[ i ] == CURRACT) ) {
	    ip = routspace[ fuses[ i ] ];
	    push( 1 );		/* stackdepth */
	    push( bp );
	    push( 0 );			/* ip (make it 0 so runprog returns
					   after executing just this fuse */
	    bp = sp - 3;		/* new bp */
	    runprog();
	    if( sp <= NUMVAR )		/* We did a $exit */
		return;
	    pop();			/* retval */
            delfuse( CURRACT, fuses[ i ] );
	}
}


runprog()
{
#if DEBUG
    address
	tip;
#endif
    int16
	instr,
	t,
	t1;
#if DEBUG
    int16
	tbp,
	tsp;
#endif

    while( ip ) {
#if DEBUG
	tip = ip;
	tbp = bp;
	tsp = sp;
	t1 = 0;
#endif
	instr = vm_get8( (int32)(ip++), &codetab );
	if( instr & PUSHN ) {
	    if( instr & 0x07F )
		t = 0xFF00 | instr;	/* Sign extend the thing */
	    else
		t = 0;
	    instr = PUSHN;
	    push( t );
#if DEBUG
	    t1 = t;
#endif
	}
	else if( instr & (PUSHARG | PUSHLOCL | CALL) ) {
	    t = instr & 0x01F;
	    instr &= (PUSHARG | PUSHLOCL | CALL);
	    switch( instr ) {
		case PUSHARG  :
		    if( t )
			push( stack[ bp + t ] );
		    else
			push( stack[ bp ] - 1 );
		    break;
		case PUSHLOCL :
		    push( bp + stack[ bp ] + 2 + t );
		    break;
		case CALL :
		    docall( t );
		    break;
	    }
#if DEBUG
	    t1 = t;
#endif
	}
	else if( instr & (PUSHS | JMP | JMPZ) ) {
	    switch( instr & (PUSHS | JMP | JMPZ) ) {
		case PUSHS :
		    t = (instr & 0x03) << 8;
		    t |= (vm_get8( (int32)(ip++), &codetab ) & 0x0FF);
		    push( t );
#if DEBUG
		    t1 = t;
#endif
		    break;
		case JMP :
		    t1 = vm_get16( (int32)ip, &codetab ) & 0x0FFFF;
		    ip = t1;
		    break;
		case JMPZ  :
		    t1 = vm_get16( (int32)ip, &codetab ) & 0x0FFFF;
		    if( stack[ sp - 1 ] == 0 )
			ip = t1;		/* Jump to the target */
		    else
			ip += 2;		/* Skip the operand */
		    break;
	    }
	    instr &= (PUSHS | JMP | JMPZ);
	}
	else switch( instr ) {
	    case POP :
		pop();
		break;
	    case PUSHME :
		push( CURRACT );
		break;
	    case RET :
		doret();
		break;
	    case PUSH :
		t = vm_get16( (int32)ip, &codetab );
		ip += 2;		/* Skip over operand */
		push( t );
#if DEBUG
		t1 = t;
#endif
		break;
#if DEBUG
	    case FILEN :
		foundfile = 1;
		t = vm_get16( (int32)ip, &codetab );
		ip += 2;
		filenum = t;
		t1 = t;
		break;
	    case LINEN :
		foundfile = 1;
		t = vm_get16( (int32)ip, &codetab );
		ip += 2;
		linenum = t;
		t1 = t;
		break;
#endif
	    default :
		error( 11 );	/* Illegal instruction */
	}
#if DEBUG
	if( debug ) {
	    fprintf( stderr, "ip = %d, bp = %d, sp = %d, instr = 0x%02x %d\n",
		        tip, tbp, tsp, instr, t1 );
	    printstack();
	}
#endif
    }
}


#if DEBUG
push( x )
int16
    x;
{
    stack[ sp++ ] = x;
    if( sp >= STACKSIZE )
	error( 8 );		/* Stack overflow */
}


int16
pop()
{
    if( sp <= NUMVAR )
	error( 9 );		/* Stack underflow */
    return stack[ --sp ];
}
#endif


doret()
{
    int16
	retval, tbp;

    retval = pop();
    sp = bp + stack[ bp ] + 2;	/* Remove locals from stack */
    popip();
    tbp = pop();
    sp = bp;
    bp = tbp;
    push( retval );
}


breaker()
{
    printf( "***BREAK***\n" );
    exit( 1 );
}


docall( stackdepth )
int16
    stackdepth;
{
    int16
	which;
	
    checkbreak( breaker );		/* Check for ^C */
    push( bp );
    puship();
    bp = sp - stackdepth - 2;
    which = stack[ bp ];
#if DEBUG
    if( debug )
	fprintf( stderr, "Calling routine %d\n", which );
#endif
    stack[ bp ] = stackdepth;
    if( which < 0 ) {
	dosysfunc( which );
    }
#if DEBUG
    else if( which > NUMROUT )
	error( 10 );		/* Illegal routine call */
#endif
    else {
	ip = routspace[ which ];
	if( !ip )	/* null routine */ {
	    push( 0 );
	    doret();
	}
    }
}


#if DEBUG
printstack()
{
    int16
	i;

    fputs( "Stack = ", stderr );
    for( i = NUMVAR; i < sp; i++ )
	fprintf( stderr, "%04x ", stack[ i ] );
    fputs( "\n", stderr );
}
#endif


callone( rp )
int16
    rp;
{
#if DEBUG
    if( debug )
	fprintf( stderr, "Calling routine %d\n", rp );
#endif

    if( !rp )
	return;
#if DEBUG
    else if( (rp < 0) || (rp > NUMROUT) )
	error( 12 );		/* Illegal routine call */
#endif
    bp = sp = NUMVAR;
    ip = routspace[ rp ];
    push( 1 );		/* stackdepth */
    push( NUMVAR );	/* bp */
    push( 0 );		/* ip */
    runprog();
}


u_prompt()
{
    callone( prompter );
}

/*** EOF adlmach.c ***/
