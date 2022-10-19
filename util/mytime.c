#include "adltypes.h"

#if AMIGA
#include <libraries/dos.h>

#define MINUTES_PER_DAY	(60 * 24)
#define TICKS_PER_MINUTE (60 * TICKS_PER_SECOND)

int32
mytime()
{
    struct DateStamp
	ds;
    long
	retval;

    DateStamp( &ds );
    retval = MINUTES_PER_DAY * ds.ds_Days + ds.ds_Minute;
    retval = TICKS_PER_MINUTE * retval + ds.ds_Tick;
    return retval;
}

checkbreak( rout )
int
    (*rout)();
{
    if( SetSignal( 0L, 0L ) & SIGBREAKF_CTRL_C ) {
	/* We were signalled - call the routine */
	(*rout)();
	/* Reset the signal */
	(void)SetSignal( 0L, SIGBREAKF_CTRL_C );
    }
}

#endif


#if MSDOS
#include <dos.h>
#undef time

int32
mytime( )
{
    int32 temp[ 3 ];		/* Buffer */

    time( (char *)temp, 0 );	/* Get a bcd version of time of day */
    return temp[ 0 ];		/* And pretend that it fits in one long */
}

checkbreak( rout )
int
    (*rout)();
{
    /* No convenient way of doing this from C under MS-DOS.  Sigh. */
    union REGS
	reglist;

    reglist.h.ah = 0x0b;		/* Check keyboard status */
    int86( 0x21, &reglist, &reglist );
    /* If the keyboard buffer contained CTRL-C, the program MIGHT exit */
}
#endif


#if UNIX
#include <signal.h>
static int
    (*_break_rout)();

static
_do_break()
{
    /* This routine is the one which actually gets called when signalled */
    signal( SIGINT, SIG_IGN );		/* Ignore this signal until ready */
    (*_break_rout)();			/* Call the routine */
    signal( SIGINT, _do_break );	/* Ready */
}


checkbreak( rout )
int
    (*rout)();
{
    if( _break_rout == (int (*)())0 )
	signal( SIGINT, _do_break );
    _break_rout = rout;
}

#endif

/*** EOF mytime.c ***/
