/*LINTLIBRARY*/
#include <stdio.h>

#include "adltypes.h"
#include "virtmem.h"

#define MINTEMP (-10000) /* Minimum temp value */

static char
   *temptab,		/* Storage for temporary strings */
   *savetab;		/* Storage for saved strings */

static int32
    *ndxtab,		/* Index page area */
    numc = 1L;		/* Number of characters in strings */

int16
    strdirty = 0,	/* Do we need to flush the strings? */
    numtemp  = 1,	/* Number of temporary chars */
    *num_save,		/* Number of saved chars */
    nums     = 1;	/* Number of string table entries */


struct pagetab
    strpages;		/* Page table for virtmem */


int16
numstr()
{
    return nums;
}


int32
numchar()
{
    return numc;
}


int16
newtstr( s )
char
    *s;
{
    int16
	t;

    strcpy( &temptab[ numtemp ], s );
    t = numtemp;
    numtemp += strlen( s ) + 1;
    return -t;
}


int16
vs_save( n )
int16
    n;
{
    int16 t;
    char *virtstr();

    if( n >= 0 )
	return n;
    strcpy( &savetab[ *num_save ], virtstr( n ) );
    t = *num_save;
    *num_save += strlen( &savetab[ *num_save ] ) + 1;
    return MINTEMP - t;
}


vsflush()
{
    if( strdirty )
	vm_flush( &strpages );
    numtemp = 1;
}


encode( s )
char
    *s;
{
    while( *s )
	*s++ ^= CODE_CHAR;
}


decode( s )
char
    *s;
{
    while( *s )
	*s++ ^= CODE_CHAR;
}


int16
newstr( s )
char
    *s;
{
    char
	t[ 512 ];

    ndxtab[ nums ] = numc;	/* Put current char index into str tab */
    strcpy( t, s );		/* Get a working copy of s */
    encode( t );		/* Make it unreadable by human eyes */
    s = t;
    while( *s )			/* Write the string out */
	vm_put8( *s++, numc++, &strpages );
    vm_put8( (char)0, numc++, &strpages );
    return nums++;
}


static char *
tvirt( id )
int16
    id;
{
    if( id <= MINTEMP )
	return &savetab[ MINTEMP - id ];
    else
	return &temptab[ -id ];
}


char *
virtstr( id )
int16
    id;
{
    int32
	adr;			/* Address of the string in the file */
    char
	ch,			/* Current char of the string */
	*s;			/* Pointer to current char */
    static char
	tvs[ 512 ];		/* Save area */

    if( id < 0 )
	/* This was a temp string - return one. */
	return tvirt( id );

    adr = ndxtab[ id ];
    s = tvs;

    /* Read in the string */
    for( ch=vm_get8( adr++, &strpages ); ch; ch=vm_get8( adr++, &strpages ) )
	*s++ = ch;
    *s = '\0';

    decode( tvs );
    return tvs;
}


vsinit( strf, stri, init, temp, save, ns, ndx )
int
    strf;				/* The swap file		*/
int32
    stri;				/* Offset into the swap file	*/
int16
    init;				/* Read the first page in?	*/
char
    *temp;				/* Temp string area		*/
char
    *save;				/* Save string area		*/
int16
    *ns;				/* Area for # of save strings	*/
int32
    *ndx;				/* Index table			*/
{
    strdirty = !init;
    vm_init( strf, stri, &strpages, strdirty );
    num_save = ns;
    temptab = temp;
    savetab = save;
    ndxtab = ndx;
}

/*** EOF vstring.h ***/
