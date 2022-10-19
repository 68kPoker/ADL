/* Undefine any predefined things */
#ifdef HPUX
#	undef HPUX
#endif

#ifdef MSDOS
#	undef MSDOS
#endif

#ifdef BSD
#	undef BSD
#endif

#ifdef AMIGA
#	undef AMIGA
#endif

#ifdef PCC
#	undef PCC
#endif

#ifdef LATTICE
#	undef LATTICE
#endif

#ifdef DEBUG
#	undef DEBUG
#endif

#ifdef HPTERM
#	undef HPTERM
#endif

#ifdef ANSI
#	undef ANSI
#endif

#ifdef TERMCAP
#	undef TERMCAP
#endif

#ifdef UNIX
#	undef UNIX
#endif

#ifdef MULTIPLEX
#	undef MULTIPLEX
#endif


	/***************************************************************\
	*								*
	*	The following #defines configure the ADL system for	*
	*	local machine architectures and operating systems.	*
	*	Read them carefully, then appropriately define the	*
	*	constants.  If your system doesn't exist here, make	*
	*	up a new name and create a new set of typedefs		*
	*	and system-dependant #defines for that system.		*
	*								*
	\***************************************************************/

#define HPUX	1		/* HP-UX or SysIII or SysV UN*X	*/
#define MSDOS	0		/* MS-DOS 2.1 or greater	*/
#define BSD	0		/* BSD 4.2 or greater		*/
#define AMIGA	0		/* Amiga computer		*/

/* Language defines */
#define PCC	1		/* UN*X-style C compiler	*/
#define LATTICE 0		/* Lattice C			*/

/* Utility defines */
#define DEBUG	1		/* Debugging in ADLRUN		*/

/* Terminal defines */
#define HPTERM	0		/* HP terminal I/O		*/
#define ANSI	0		/* ANSI terminal I/O		*/
#define TERMCAP	1		/* Termcap I/O			*/

/* The following is UNTESTED - do not use it! */
#define MULTIPLEX 0		/* Allow multiplexing of terminals? */


/* The following is for BSD-like systems - it allows for the
   execution of an ADL binary directly.  Change it to reflect
   where adlrun lives on your system.
*/

#ifndef ADL_NAME
#	define ADL_NAME "/users/cunniff/bin/adlrun"
#endif


/* The following is the character which is used to obfuscate strings */
#define CODE_CHAR 0xff

#if HPUX
#  define	UNIX 1
   typedef	short	int16;
   typedef	long	int32;
   typedef	unsigned short address;
#  define	SRAND	srand
#  define	RAND	rand()
#  define	RB	O_RDWR
#  define	WB	(O_RDWR | O_CREAT | O_TRUNC)
#endif

#if BSD
#  define	UNIX	1
   typedef	short			int16;
   typedef	long			int32;
   typedef	unsigned short		address;
#  define	SRAND	srandom
#  define	RAND	random()
#  define	RB	O_RDWR
#  define	WB	(O_RDWR | O_CREAT | O_TRUNC)
#endif

#if AMIGA
#  define	UNIX	0
   typedef	short			int16;
   typedef	long			int32;
   typedef	unsigned short		address;
#  define	SRAND	srand
#  define	RAND	rand()
#  define	time	mytime
#  define	RB	O_RDWR
#  define	WB	(O_RDWR | O_CREAT | O_TRUNC)
#endif

#if MSDOS
#  define	UNIX			0
#  define	int16			int
#  define	int32			long
#  define	address			unsigned
#  define	SRAND			srand
#  define	RAND			rand()
#  define	time			mytime
#  define	RB			(O_RDWR | O_RAW)
#  define	WB			(O_RDWR | O_CREAT | O_TRUNC | O_RAW)
#  define	void			int
#endif

/*** EOF adltypes.h ***/
