#include <stdio.h>

#include "adltypes.h"
#include "adlprog.h"
#include "adlrun.h"

struct	macro
    *mactab;

extern char
    *calloc();

define( str1, str2 )
char
    *str1,
    *str2;
{
    struct macro
	*temp;

    temp = (struct macro *)calloc( sizeof( struct macro ), 1 );
    if( temp == (struct macro *)0 )
	error( 27 );			/* Out of memory */

    temp->next = mactab;
    mactab = temp;
    strcpy( mactab->name, str1 );
    strcpy( mactab->val, str2 );
}


int16
nummacro()
{
    int16
	count;
    struct macro
	*m;

    count = 0;
    for( m = mactab; m; m = m->next )
	count++;
    return count;
}


undef( str )
char
    *str;
{
    struct macro
	*which,
	*last;

    if( !mactab )		/* No macros defined */
	return;
    if( !strcmp( mactab->name, str ) ) {	/* First entry is the one we want */
	which = mactab;
	mactab = mactab->next;
	free( mactab );
	return;
    }
    which = mactab->next;
    last = mactab;
    while( which ) {
	if( !strcmp( which->name, str ) ) {
	    last->next = which->next;
	    free( which );
	    return;
	}
	last = which;
	which = which->next;
    }
}


char	*
expand( str )
char
    *str;
{
  struct macro
	*which;

    which = mactab;
    while( which ) {
	if( !strcmp( which->name, str ) )
	    return which->val;
	which = which->next;
    }
    return str;
}


clearmacro()
{
    struct macro
	*which, *temp;

    which = mactab;
    while( which ) {
	temp = which->next;
	free( which );
	which = temp;
    }
    mactab = (struct macro *)0;
}

/*** EOF adlmacro.c ***/
