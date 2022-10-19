#include <stdio.h>

#include "adltypes.h"
#include "vstring.h"

int16
    foundfile,
    filenum,
    linenum;
extern int16
    ip;


error( num )
int
    num;
{
#if DEBUG
    char
	*msg;
    switch( num ) {
	case 1 : msg = "Divide by zero";			break;
	case 2 : msg = "Too few arguments";			break;
	case 3 : msg = "Illegal builtin routine";		break;
	case 4 : msg = "Illegal routine for $sdem";		break;
	case 5 : msg = "Illegal routine for $sfus";		break;
	case 6 : msg = "Illegal routine for $prompt";		break;
	case 7 : msg = "Illegal object for $actor";		break;
	case 8 : msg = "Stack overflow";			break;
	case 9 : msg = "Stack underflow";			break;
	case 10: msg = "Illegal routine call";			break;
	case 11: msg = "Illegal instruction";			break;
	case 12: msg = "Illegal routine call";			break;
	case 13: msg = "Illegal global for $glob";		break;
	case 14: msg = "Illegal global for $setg";		break;
	case 15: msg = "Illegal object for $setp";		break;
	case 16: msg = "Illegal propnum for $setp";		break;
	case 17: msg = "Illegal object for $move";		break;
	case 18: msg = "Illegal object for $prop";		break;
	case 19: msg = "Illegal promnum for $prop";		break;
	case 20: msg = "Illegal verb for $vset";		break;
	case 21: msg = "Illegal propnum for $vset";		break;
	case 22: msg = "Illegal verb for $vprop";		break;
	case 23: msg = "Illegal propnum for $vprop";		break;
	case 24: msg = "Illegal parameter for $spec";		break;
	case 25: msg = "Illegal object for $name";		break;
	case 26: msg = "Illegal rout for $miss";		break;
	case 27: msg = "Out of memory";				break;
	case 28: msg = "Illegal exitcode";			break;
	case 29: msg = "Insufficient terminal definition";	break;
	case 30: msg = "Illegal object for $sfus";		break;
	case 31: msg = "Illegal actor for $spec 11";		break;
	case 32: msg = "Unable to open tty";			break;
	case 33: msg = "Illegal actor for $spec 12";		break;
	default: msg = "Unknown error number";
    }
    fputs( msg, stderr );
#else
    fprintf( stderr, "Error #%d", num );
#endif
    if( foundfile )
	fprintf( stderr, ", file \"%s\", line %d",
		virtstr( filenum ), linenum );
    fprintf( stderr, ", ip = %ld\n", (long)ip );
    head_term();
    exit( num );
}

/*** EOF adlerr.c ***/
