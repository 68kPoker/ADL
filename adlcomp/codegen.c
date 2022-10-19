	/***************************************************************\
	*								*
	*	codegen.c - routines to generate appropriate ADL	*
	*	stack machine instructions.				*
	*	Copyright 1987 by Ross Cunniff.				*
	*								*
	\***************************************************************/

#include <stdio.h>

#include "adltypes.h"
#include "virtmem.h"
#include "adldef.h"

address
    numcode = 1;		/* Number of bytes of code written	*/

extern struct pagetab
    codetab;			/* Code paging table.			*/

	/***************************************************************\
	*								*
	*	oldcode( addr, opcode, opnd ) - Emits instruction	*
	*	opcode opnd at address addr.  Compresses the instr.	*
	*	if possible.						*
	*								*
	\***************************************************************/

address
oldcode( addr, opcode, opnd )
address
    addr;
int16
    opnd;
char
    opcode;
{
    int16
	num;

    switch( opcode ) {
	case PUSH :
	    if( (opnd >= -127) && (opnd <= 0) ) {
		opcode = PUSHN | (char)(opnd & 0x07F);
		num = 1;
	    }
	    else if( (opnd >= -1024) && (opnd <= 1023) ) {
		opcode = PUSHS | (char)((opnd >> 8) & 0x03F);
		opnd &= 0x0FF;
		num = 2;
	    }
	    else
		num = 3;
	   break;
	case PUSHARG :
	case PUSHLOCL :
	case CALL :
	    opcode |= (char)(opnd & 0x01F);
	case NOP :
	case POP :
	case PUSHME :
	case RET :
	    num = 1;
	    break;
	case JMP :
	case JMPZ : 
	case FILEN :
	case LINEN :
	    num = 3;
	    break;
    }
    vm_put8( opcode, (int32)(addr++), &codetab );
    switch( num ) {
	case 1 :
	    break;
	case 2 :
	    vm_put8( (char)(opnd & 0x0ff), (int32)addr, &codetab );
	    break;
	case 3 :
	    vm_put16( (int16)(opnd & 0x0ffff), (int32)addr, &codetab );
	    break;
    }
    return (address)num;
}


	/***************************************************************\
	*								*
	*	newcode( opcode, opnd ) - emit instruction opcode opnd	*
	*	at the current address in the code file.		*
	*								*
	\***************************************************************/

address
newcode( opcode, opnd )
char
    opcode;
int16
    opnd;
{
    address
	nc_save;

    nc_save = numcode;
    numcode += oldcode( numcode, opcode, opnd );
    return nc_save;
}


	/***************************************************************\
	*								*
	*	currcode() - returns the current code address.		*
	*								*
	\***************************************************************/

address
currcode()
{
    return numcode;
}

/*** EOF codegen.c ***/
