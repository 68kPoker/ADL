#include <stdio.h>
#include "adltypes.h"
#include "adlprog.h"

struct data {			/* Structure of the data in the dictionary */
    int16
	type, val, first;
};

struct letter {
    char
	let_name;		/* Name of this letter	*/
    struct data
	*let_val;		/* Value of this token	*/
    struct letter
	*next,			/* Next letter on this level */
	*child;			/* Next level */
};


static struct letter
    *trie;			/* The dictionary */

char
    *malloc();

	/***************************************************************\
	*								*
	*	new_letter( let ) - allocate a new letter for the	*
	*	trie, initializing the letter to let.			*
	*								*
	\***************************************************************/

static struct letter *
new_letter( let )
char
    let;
{
    struct letter
	*temp;

    temp = (struct letter *)malloc( sizeof( struct letter ) );
    if( temp == (struct letter *)0 )
	error( 27 );			/* Out of memory */
    temp->let_name = let;
    temp->let_val = (struct data *)0;
    temp->next = temp->child = (struct letter *)0;
    return temp;
}


insertkey( name, type, val, first )
char
    *name;
int16
    type, val, first;
{
    struct letter
	**t,
	*temp;
    int16
	done = 0,
	retval;

    t = &trie;
    while( !done ) {
	temp = *t;

	/* Find the first letter in the name */
	while( (temp != (struct letter *)0) && (temp->let_name != *name) )
	    temp = temp->next;

	if( temp == (struct letter *)0 ) {
	    /* This letter wasn't in the trie, so put it there. */
	    temp = new_letter( *name );
	    temp->next = *t;
	    *t = temp;
	}

	if( name[ 1 ] == '\0' ) {
	    /* We are at the end of the string.  Insert it */
	    if( temp->let_val != (struct data *)0 )
		/* This name already defined. */
		retval =  0;
	    else {
		temp->let_val = (struct data *)malloc( sizeof( struct data ) );
		if( temp->let_val == (struct data *)0 )
		    error( 27 );		/* Out of memory */
		temp->let_val->type = type;
		temp->let_val->val = val;
		temp->let_val->first = first;
		retval = 1;
	    }
	    done = 1;
	}
	else {
	    /* We have more letters to go.  Recursively insert them. */
	    name++;
	    t = &temp->child;
	}
    }
    return retval;
}


int16
lookup( name, val )
char
    *name;
int16
    *val;
{
    int16
	retval,
	done = 0;
    struct letter
	*t;

    name[ LENGTH ] = '\0';		/* Trim the name */
    t = trie;
    while( !done ) {
	/* Find the first letter in the name */
	while( (t != (struct letter *)0) && (t->let_name != *name) )
	    t = t->next;

	if( t == (struct letter *)0 ) {
	    /* This letter wasn't in the trie.  Return failure. */
	    retval =  -1;
	    done = 1;
	}
	else if( name[ 1 ] == '\0' ) {
	    /* We are at the end of the string.  See if we have data. */
	    if( t->let_val != (struct data *)0 ) {
		*val = t->let_val->val;
		retval =  t->let_val->type;
		done = 1;
	    }
	    else {
		/* No data. Check for abbreviation. */
		for( t = t->child; !done; t = t->child ) {
		    if( t == (struct letter *)0 ) {
			/* We should never get here, but... */
			retval = -2;
			done = 1;
		    }
		    else if( t->next != (struct letter *)0 ) {
			/* There are other strings beginning with s */
			retval = -2;
			done = 1;
		    }
		    else if( t->let_val != (struct data *)0 ) {
			/* There is data.  See if there is more string. */
			if( t->child != (struct letter *)0 ) {
			    /* Whoops - this is a substring. */
			    retval = -2;
			    done = 1;
			}
			else {
			    /* Aha! An abbreviation! */
			    *val = t->let_val->val;
			    retval = t->let_val->type;
			    done = 1;
			}
		    }
		}
	    }
	}
	else {
	    /* We have more letters to go.  Recursively check them */
	    name++;
	    t = t->child;
	}
    }
    return retval;
}


static
search_dict( t, s, proc )
struct letter
    *t;
char
    *s;
int
    (*proc)();
{
    int16
	len;

    len = strlen( s );
    while( t != (struct letter *)0 ) {
	s[ len ] = t->let_name;
	s[ len + 1 ] = '\0';
	if( t->let_val != (struct data *)0 )
	    if( (*proc)( s, t->let_val ) != 0 )
		return 1;
	if( search_dict( t->child, s, proc ) != 0 )
	    return 1;
	t = t->next;
    }
    return 0;
}


static int16
    find_type,
    find_val;

static int
finder( name, dat )
char
    *name;
struct data
    *dat;
{
    if( (dat->type == find_type) && (dat->val == find_val) && dat->first )
	return 1;
    else
	return 0;
}


char *
findone( type, val )
int16
    type,
    val;
{
    static char
	buff[ 80 ];

    find_type = type;	find_val = val;
    buff[ 0 ] = 0;
    if( !search_dict( trie, buff, finder ) )
	buff[ 0 ] = 0;
    return buff;
}


read_symtab( fd, dir )
int
    fd;
struct adldir
    *dir;
{
    int
	i, j;
    struct symbol
	symb;

    lseek( fd, dir->ptr, 0 );
    for( i = 0; i < NUMSYM; i++ ) {
	read( fd, &symb, sizeof( struct symbol ) );
	for( j = 0; j < strlen( symb.name ); j++ )
	    symb.name[ j ] ^= CODE_CHAR;
	insertkey( symb.name, symb.type, symb.val, symb.first );
    }
}

/*** EOF rtdict.c ***/
