#include <stdio.h>

#include "adltypes.h"
#include "virtmem.h"

/* Macros to find the page number and offset of an address */
#define SEGMENT( where )	((where >> 9) & 0x7FFF)
#define OFFSET( where )		(where & 0x01FF)

vm_init( which, offs, handle, d )
int
    which;		/* Paging file */
int32
    offs;		/* Starting offset into the paging file */
struct pagetab
    *handle;		/* LRU queue */
int16
    d;			/* Dirty bit */
{
    handle->pfile = which;
    handle->offset = offs;
    handle->dirty = d;
}


vm_flush( handle )
struct pagetab
    *handle;
{
    int
	i;

    if( handle->dirty ) {
	for( i = 0; i < handle->numpages; i++ ) {
	    lseek(	handle->pfile,
			handle->offset + (int32)(handle->ptabs[i].pnum * BSIZE),
			0 );
	    write( handle->pfile, handle->ptabs[ i ].buffer, BSIZE );
	}
    }
}


char	*
vm_getpg( num, handle )
int
    num;
struct	pagetab
    *handle;
{
    struct pt_entry
	*ptr;

    /* Search for the page number in the LRU queue */
    for( ptr = handle->mru; ptr; ptr = ptr->next ) {
	if( ptr->pnum == num ) {
	    /* Page is in memory */
	    if( ptr != handle->mru ) {
		/* Put the entry at the head of the LRU queue */
		(ptr->prev)->next = ptr->next;
		if( ptr->next )
		    (ptr->next)->prev = ptr->prev;
		else
		    handle->lru = ptr->prev;

		ptr->next = handle->mru;
		(handle->mru)->prev = ptr;

		ptr->prev = (struct pt_entry *)0;
		handle->mru = ptr;
	    }
	    return ptr->buffer;
	}
    }

    /* If we get here, the page is not in memory.  Read it in. */
    if( handle->numpages < NUMP ) {
	/* We have available pages */
	ptr = handle->ptabs + handle->numpages++;
	if( handle->numpages == NUMP ) {
	    /* We just allocated the last available page.  Find the LRU. */
	    for(	handle->lru = handle->mru;
			(handle->lru)->next;
			handle->lru = (handle->lru)->next )
		/* NOTHING */;
	}
    }
    else {			/* We need to swap an old page */
	/* Take the LRU off the end of the list. */
	ptr = handle->lru;
	handle->lru = ptr->prev;
	(ptr->prev)->next = (struct pt_entry *)0;

	if( handle->dirty ) {
	    /* We need to write the old contents */
	    lseek(	handle->pfile,
			handle->offset + (int32)(ptr->pnum * BSIZE),
			0 );
	    write( handle->pfile, ptr->buffer, BSIZE );
	}
    }

    /* Put the old LRU at the head of the list, as the MRU */
    ptr->prev = (struct pt_entry *)0;
    ptr->next = handle->mru;
    if( handle->mru )
	(handle->mru)->prev = ptr;

    /* Change the data, then read in the page */
    ptr->pnum = num;
    handle->mru = ptr;
    lseek( handle->pfile, handle->offset + (int32)(num * BSIZE), 0 );
    read( handle->pfile, ptr->buffer, BSIZE );
    return ptr->buffer;
}


vm_put8( data, where, handle )
char
    data;
int32
    where;
struct pagetab
    *handle;
{
    char
	*buff;
    int
	seg,
	offs;

    seg = SEGMENT( where );
    offs = OFFSET( where );
    if( seg >= handle->maxpage ) {		/* Grow the file */
	lseek( handle->pfile, 0L, 2 );	/* EOF */
	write( handle->pfile, handle->ptabs[0].buffer, BSIZE );
	handle->maxpage++;
    }
    buff = vm_getpg( seg, handle );
    buff[ offs ] = data;
}


vm_put16( data, where, handle )
int16
    data;
int32
    where;
struct pagetab
    *handle;
{
    vm_put8( data & 0x0ff, where, handle );
    vm_put8( (data >> 8) & 0x0ff, where + 1, handle );
}


char
vm_get8( where, handle )
int32
    where;
struct pagetab
    *handle;
{
    int
	seg, offs;
    char
	*buff;

    seg = SEGMENT( where );
    offs = OFFSET( where );
    buff = vm_getpg( seg, handle );
    return buff[ offs ];
}


int16
vm_get16( where, handle )
int32
    where;
struct pagetab
    *handle;
{
    int16
	t;

    t = vm_get8( where, handle ) & 0x0ff;
    t |= vm_get8( where + 1, handle ) << 8;
    return t;
}

/*** EOF virtmem.c ***/
