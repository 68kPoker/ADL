#include <setjmp.h>

#define SLEN	80		/* Maximum length of an input string	*/
#define NUMDO	10		/* Maximum # of direct objs allowed	*/
#define STACKSIZE 1024		/* Maximum stack depth			*/

/* Structure of a macro definition */
struct	macro {
    char
	name[ 10 ],		/* Name of the macro			*/
	val[ 80 ];		/* Replacement text of the macro	*/
    struct macro
	*next;			/* Next macro in the stack		*/
};


/* Structure of an actor */
struct actrec {
    int16
	actor;			/* Object id of this actor		*/
    char
	*linebuf,		/* Pointer to current input line	*/
	savebuf[ SLEN ];	/* Static save area for input line	*/
    int16
	interact;		/* Get new string if null linebuf?	*/
#if MULTIPLEX
    char
	ttyname[ SLEN ];	/* Name of the actor's tty		*/
    FILE
	*ttyfile;		/* Opened version of the same		*/
#endif
};


/* Structure of a $exit place */
struct exit_place {
    jmp_buf
	exit_goto;		/* Where to go for ($exit n)		*/
    int16
	exit_ok;		/* Is it OK to perform ($exit n)	*/
};

#if MSDOS & LATTICE
#  define SET_EXIT(n) (exits[n].exit_ok=1, setjmp(&(exits[n].exit_goto)))
#  define DO_EXIT(n)  (exits[n].exit_ok=0, longjmp(&(exits[n].exit_goto),1))
#else
#  define SET_EXIT(n) (exits[n].exit_ok=1, setjmp(exits[n].exit_goto))
#  define DO_EXIT(n)  (exits[n].exit_ok=0, longjmp(exits[n].exit_goto,1))
#endif
#define CLR_EXIT(n)   (exits[n].exit_ok=0)

extern struct actrec
    actlist[];			/* List of actors			*/

extern address
    ip;				/* Instruction pointer			*/
extern int16
    stack[],			/* Execution stack			*/
    sp,				/* Stack pointer			*/
    bp,				/* Base pointer, or Frame pointer	*/
    numact,			/* Number of Actors in Actlist		*/
    curract,			/* Current Actor			*/
    Verb,			/* Current verb				*/
    Prep,			/* Current preposition			*/
    Iobj,			/* Current indirect object		*/
    Inoun,			/*   and its noun			*/
    Imod,			/*   and its modifier			*/
    Conj[],			/* List of conjunctions			*/
    Dobj[],			/* List of direct objects		*/
    Dnoun[],			/*   and their nouns			*/
    Dmod[],			/*   and their modifiers		*/
    NumDobj,			/* Number of direct objects		*/
    vecverb[],			/* Verb list				*/
    demons[],			/* List of active demons		*/
    fuses[],			/* List of active fuses			*/
    ftimes[],			/* When to activate fuses		*/
    f_actors[],			/* Actors associated with fuses		*/
    numd,			/* Number of active demons		*/
    numf,			/* Number of active fuses		*/
    currturn,			/* Current turn counter			*/
    prompter,			/* Prompting routine			*/
    numsave,			/* Number of "saved" characters		*/
    debug,			/* Print out debugging info?		*/
    header,			/* Should we print out header bar?	*/
    wordwrite,			/* Should we write unknown words?	*/
    restarted,			/* Is the game being restarted?		*/
    scrwidth,			/* Width of the screen			*/
    numcol,			/* How far we've written across		*/
    t_type,			/* Current token type			*/
    t_val,			/* Current token value			*/
    read_t,			/* TRUE iff lexer is to read next token	*/
    Phase;			/* Current phase #			*/

extern struct exit_place
    exits[];			/* List of exit places			*/

extern char
    *H_STR,			/* Header format string			*/
    *s,				/* Current token			*/
    *xp;			/* Macro expansion of s			*/

extern FILE
    *wordfile,			/* If so, write them to this file.	*/
    *scriptfile;		/* File for "scripting" output		*/

#if MULTIPLEX
extern FILE
    *CURRTTY,			/* Current tty for output		*/
    *SYSTTY;			/* Default I/O tty			*/
#endif

extern char
    savec[];			/* The saved string array		*/

extern struct macro
    *mactab;			/* Table of macro expansions		*/

extern int16
    bitpat[],			/* Bit patterns for masking		*/
    ibitpat[];			/* Bitwise NOT of above			*/



#if DEBUG
extern int16
    pop();			/* Pop the top of the stack		*/
#else
#	define push(x)	(stack[sp++]=x)
#	define pop(x)	(stack[--sp])
#	define assertargs(s,n) 0
#endif

#define puship() push( ip )	/* Push the IP (in case of 32-bit)	*/
#define popip()	(ip = pop())	/* Pop the IP (in case of 32-bit)	*/

#define	CURRACT		actlist[ curract ].actor
#define PSTRING		actlist[ curract ].linebuf
#define SAVEBUF		actlist[ curract ].savebuf
#define INTERACT	actlist[ curract ].interact

#define	ARG( n )	stack[ bp + n ]	/* n'th argument to this rout	*/
#define	RETVAL		stack[ bp ]	/* Return val. from this rout	*/

extern int16
    noun_exists(),		/* Is [mod noun] a legal object?	*/
    yesno(),			/* Did the player type 'Y' or 'y'?	*/
    nummacro();			/* Returns number of macros defined	*/

extern char
    *malloc(),			/* Memory allocation			*/
    *findone(),			/* Find a symbol.			*/
    *expand();			/* Try to macro-expand a string		*/

extern int16
    lookup(),			/* Look up a token in the symbol table	*/
    insertkey();		/* Insert a token into the symbol table	*/

extern int
    parse();			/* Parse the player's sentence		*/

/*** EOF adlrun.h ***/
