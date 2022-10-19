/*LINTLIBRARY*/

#include <stdio.h>

#include "adltypes.h"
#include "adlprog.h"

#define HTSIZE 512		/* Size of a hash table */

char
    *calloc();

/* Structure of a hash table entry */
struct hash_entry {
    struct symbol
	entry_val;
    struct hash_entry
	*next;
};

/* The three dictionaries */
static struct hash_entry
    *sys_symb[ HTSIZE ],		/* The system symbols */
    *loc_symb[ HTSIZE ],		/* The local symbols */
    *gen_symb[ HTSIZE ];		/* The general symbols */


static int16
find( s, hash, tab, val )
char
    *s;
struct hash_entry
    **tab;
int16
    hash,
    *val;
{
    struct hash_entry
	*ent;

    ent = tab[ hash ];
    while( ent != (struct hash_entry *)NULL ) {
	if( strcmp( ent->entry_val.name, s ) == 0 ) {
	    *val = ent->entry_val.val;
	    return ent->entry_val.type;
	}
	ent = ent->next;
    }
    return -1;
}


static int16
hashval( s )
char
    *s;
{
    int
	t;

    t = 0;
    while( *s )
	t = (t + *s++) % HTSIZE;
    return t;
}


static int16
insert( s, type, val, first, tab )
char
    *s;
int16
    type, val, first;
struct hash_entry
    **tab;
{
    struct hash_entry
	*ent;
    int16
	hash;
    char
	temp[ 10 ];

    /* Trim the string to the maximum appropriate length */
    strncpy( temp, s, LENGTH );
    temp[ LENGTH ] = '\0';

    hash = hashval( temp );
    ent = tab[ hash ];
    tab[ hash ] = (struct hash_entry *)calloc( 1, sizeof( struct hash_entry ) );
    tab[ hash ]->next = ent;
    strcpy( tab[ hash ]->entry_val.name, temp );
    tab[ hash ]->entry_val.type = type;
    tab[ hash ]->entry_val.val = val;
    tab[ hash ]->entry_val.first = first;
}


int16
lookup( s, val )
char
    *s;
int16
    *val;
{
    int16
	found,
	hash;
    char
	temp[ 10 ];

    /* Trim the string to the maximum appropriate length */
    strncpy( temp, s, LENGTH );
    temp[ LENGTH ] = '\0';

    hash = hashval( temp );

    /* Search the local dictionary first */
    found = find( temp, hash, loc_symb, val );
    if( found >= 0 )
	return found;

    /* Search the system dictionary next */
    found = find( temp, hash, sys_symb, val );
    if( found >= 0 )
	return found;

    /* Search the general dictionary last */
    found = find( temp, hash, gen_symb, val );
    return found;
}


int16
insertkey( s, type, val, first )
char
    *s;
int16
    type, val;
{
    insert( s, type, val, first, gen_symb );
    NUMSYM++;
}


int16
insert_sys( s, type, val )
char
    *s;
int16
    type, val;
{
    insert( s, type, val, 0, sys_symb );
}


int16
insert_local( s, type, val )
char
    *s;
int16
    type, val;
{
    insert( s, type, val, 0, loc_symb );
}


count_symtab( debug )
int
    debug;
{
    NUMSYM = write_hash( 0, gen_symb, 0 );
    if( debug )
	NUMSYM += write_hash( 0, sys_symb, 0 );
}


write_symtab( fd, debug )
int
    fd,
    debug;
{
    lseek( fd, 0L, 2 );			/* Seek to EOF */
    (void)write_hash( fd, gen_symb, 1 );
    if( debug )
	(void)write_hash( fd, sys_symb, 1 );
}


static
write_hash( fd, tab, writing )
int
    fd;
struct hash_entry
    **tab;
int
    writing;
{
    int16
	i, j, num;
    struct hash_entry
	*ent;

    num = 0;
    for( i = 0; i < HTSIZE; i++ ) {
	ent = tab[ i ];
	while( ent != (struct hash_entry *)0 ) {
	    if( writing ) {
		for( j = 0; j < strlen( ent->entry_val.name ); j++ )
		    ent->entry_val.name[ j ] ^= CODE_CHAR;
		write( fd, &ent->entry_val, sizeof( struct symbol ) );
	    }
	    ent = ent->next;
	    num++;
	}
    }
    return num;
}


del_locals()
{
    int16
	i;
    struct hash_entry
	*ent1, *ent2;

    for( i = 0; i < HTSIZE; i++ ) {
	ent1 = loc_symb[ i ];
	while( ent1 != (struct hash_entry *)0 ) {
	    ent2 = ent1;
	    ent1 = ent1->next;
	    free( ent2 );
	}
	loc_symb[ i ] = (struct hash_entry *)0;
    }
}

/*** EOF dict.c ***/
