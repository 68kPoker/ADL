#include "adltypes.h"
#include "adlprog.h"


struct	header		hdr;		/* Actual header		*/
struct	verbrec		*verbspace;	/* Verb contents		*/
struct	objrec		*objspace;	/* Object contents		*/
struct	preprec		*prepspace;	/* Preposition contents		*/
struct	vp_syn		*verbsyn;	/* Verb synonyms		*/
int16			*varspace;	/* Stack & variables		*/
int16			*nounspace;	/* Noun indices			*/
address			*routspace;	/* Routine indexes		*/
int32			*str_tab;	/* String table			*/

int16	bitpat[16] =  {	0x0001, 0x0002, 0x0004, 0x0008,
			0x0010, 0x0020, 0x0040, 0x0080,
			0x0100, 0x0200, 0x0400, 0x0800,
			0x1000, 0x2000, 0x4000, 0x8000  },

	ibitpat[16] = {	0xFFFE, 0xFFFD, 0xFFFB, 0xFFF7,
			0xFFEF, 0xFFDF, 0xFFBF, 0xFF7F,
			0xFEFF, 0xFDFF, 0xFBFF, 0xF7FF,
			0xEFFF, 0xDFFF, 0xBFFF, 0x7FFF  };

/*** EOF global.c ***/
