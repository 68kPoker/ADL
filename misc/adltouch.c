/* Adltouch - touch an ADL save file */

#include <stdio.h>

#if MSDOS
#  define RB "rb+"
#else
#  define RB "r+"
#endif

main( argc, argv )
int
    argc;
char
    *argv[];
{
    FILE
	*f;
    long
	l,
	atol();

    if( argc != 3 ) {
	printf( "Usage: adltouch <file> <number>\n" );
	exit( 1 );
    }

    if( !(f = fopen( argv[ 1 ], RB )) ) {
	printf( "Error opening file %s\n", argv[ 1 ] );
	exit( 2 );
    }

    l = atol( argv[ 2 ] );
    fseek( f, 0L, 0 );
    fwrite( &l, sizeof( long ), 1, f );
    fclose( f );
}

/*** EOF adltouch.c ***/
