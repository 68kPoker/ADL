#define NUMPROP 32			/* Max # of object properties	*/

/* Structure of an object */
struct objrec {
	int16	loc,			/* obj index			*/
		cont,			/* obj index			*/
		link,			/* obj index			*/
		adj,			/* adj index			*/
		others,			/* obj index			*/
		noun,			/* noun index			*/
		props1to16,		/* 0 or 1			*/
		props[ NUMPROP - 16 ];	/* General props		*/
}; /* 44 bytes */

/* Structure of a verb */
struct verbrec {
	int16	preact,			/* rout index			*/
		postact;		/* rout index			*/
}; /* 4 bytes */

/* Structure of a verb-prep synonym */
struct vp_syn {
	int16	vrb,			/* Verb index			*/
		prp,			/* Prep index			*/
		val;			/* Verb index			*/
}; /* 6 bytes */

/* Structure of a multi-word preposition ("in front of") */
struct preprec {
	int16	first,			/* Prep #			*/
		obj,			/* Object #			*/
		last,			/* Prep #			*/
		val;			/* Prep #			*/
}; /* 8 bytes */


/* "Directory" of compiled dungeon */
struct adldir {
	int16	numobjs,		/* Number of objects		*/
		objsize;		/* Size of an object		*/
	int32	ptr;			/* Lseek index into the file	*/
}; /* 8 bytes */


/* Structure of a symbol */
#define LENGTH 8			/* Maximum symbol name length	*/
struct symbol {
    char
	first,				/* Was this the first symbol?	*/
	name[ LENGTH + 1 ];		/* Name of the symbol		*/
    int16
	type,				/* Type (NOUN, ADJEC, etc.)	*/
	val;				/* Value			*/
}; /* 14 bytes */

/* Header of an ADL executable file */
struct header {
	char		adlname[ 80 ];	/* Hack for BSD systems		*/
	struct	adldir	codeindex,	/* Index for the code		*/
			strtabindex,	/* Index for the string tab	*/
			strindex,	/* Index for the strings	*/
			symindex,	/* Index for the sym table	*/
			verbindex,	/* Index for the verbs		*/
			objindex,	/* Index for the objects	*/
			nounindex,	/* Index for the nouns		*/
			varindex,	/* Index for the vars		*/
			prepindex,	/* Index for the preps		*/
			vsindex,	/* Index for the verb syns	*/
			routindex;	/* Index for the routines	*/
	int32		adlid,		/* Timestamp			*/
			magic;		/* Magic number			*/
};

extern	struct	verbrec		*verbspace;	/* Verb contents	*/
extern	struct	objrec		*objspace;	/* Object contents	*/
extern	struct	preprec		*prepspace;	/* Preposition contents	*/
extern	struct	vp_syn		*verbsyn;	/* Verb synonyms	*/
extern	int16			*varspace;	/* Stack & variables	*/
extern	int16			*nounspace;	/* Noun indices		*/
extern	address			*routspace;	/* Routine indices	*/
extern	int32			*str_tab;	/* String table		*/

extern	struct	header		hdr;		/* Global header struct	*/

/* Some #defines to make life easier */

#define NUMNOUN		hdr.nounindex.numobjs
#define NUMSYM		hdr.symindex.numobjs
#define NUMVERB		hdr.verbindex.numobjs
#define NUMOBJ		hdr.objindex.numobjs
#define NUMVAR		hdr.varindex.numobjs
#define NUMROUT		hdr.routindex.numobjs
#define NUMSTR		hdr.strtabindex.numobjs
#define NUMPP		hdr.prepindex.numobjs
#define NUMVS		hdr.vsindex.numobjs


/* Predefined stuff */

#define M_ADL	0x0ad10ad1L			/* Adl magic number	*/

#define _START	1				/* Rout # for START	*/
#define _DWIMI	2				/* Rout # for DWIMI	*/
#define _DWIMD	3				/* Rout # for DWIMD	*/

#define _NOVERB 0				/* Verb # for NOVERB	*/
#define _TELLER	1				/* Verb # for TELLER	*/

#define _ALL	0				/* Obj # for .ALL	*/
#define _STRING	1				/* Obj # for STRING	*/

#define _ACT	NUMPROP				/* Prop # for ACTION	*/
#define _LD	(_ACT - 2)			/* Prop # for LDESC	*/
#define _SD	(_ACT - 1)			/* Prop # for SDESC	*/
#define _PREACT	(_ACT + 1)			/* Prop # for PREACT	*/

#define _VERB_G -1				/* Glob # for Verb	*/
#define _DOBJ_G -2				/* Glob # for Dobj	*/
#define _IOBJ_G -3				/* Glob # for Iobj	*/
#define _PREP_G -4				/* Glob # for Prep	*/
#define _CONJ_G -5				/* Glob # for Conj	*/
#define _NUMD_G -6				/* Glob # for Numd	*/

/*** EOF adlprog.h ***/
