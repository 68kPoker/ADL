/*	builtins.h -- values for built in functions */

/* Object functions */

#define _LOC		 -1	/* ($loc OBJ) container of OBJ */
#define	_CONT		 -2	/* ($cont OBJ) First item contained in OBJ */
#define	_LINK		 -3	/* ($link OBJ) Next obj in same node as OBJ */
#define	_LDESC		 -4	/* ($ldesc OBJ) LDesc of OBJ */
#define _SDESC		 -5	/* ($sdesc OBJ) SDesc of OBJ */
#define _ACTION		 -6	/* ($action OBJ) Action routine for OBJ */
#define	_PROP		 -7	/* ($prop OBJ NUM) NUM'th prop of OBJ */
#define _SETP		 -8	/* ($setp OBJ NUM VAL) OBJ(NUM) = VAL */
#define _MOVE		 -9	/* ($move OBJ1 OBJ2)  move OBJ1 to OBJ2 */
#define _MODIF		-10	/* ($modif OBJ) modifier of OBJ -
				   if < 0, it is a verb; if > 0 it
				   is an adjec, otherwise, it's null */

/* Verb functions */

#define	_VSET		-11	/* ($vset VERB PROP VAL) VERB(PROP) = VAL */
#define _VPROP		-12	/* ($vprop VERB PROP)	 returns VERB(PROP) */

/* Arithmetic functions */

#define _PLUS		-13	/* ($plus A B) A + B */
#define _MINUS		-14	/* ($minus A B) A - B */
#define _TIMES		-15	/* ($times A B) A * B */
#define _DIV		-16	/* ($div A B) A / B */
#define _MOD		-17	/* ($mod A B) A % B */
#define _RAND		-18	/* ($rand N) Random # from 1 to N, inclusive */

/* Boolean functions */

#define _AND		-19	/* ($and A B) A & B */
#define _OR		-20	/* ($or A B) A | B */
#define _NOT		-21	/* ($not N) 1 if N==0, 0 otherwise */
#define _YORN		-22	/* ($yorn) (user input)[0] in [ 'y', 'Y' ] */
#define _PCT		-23	/* ($pct N) 1 N% of the time */
#define _EQ		-24	/* ($eq A B) A == B */
#define _NE		-25	/* ($ne A B) A != B */
#define _LT		-26	/* ($lt A B) A < B */
#define _GT		-27	/* ($gt A B) A > B */
#define _LE		-28	/* ($le A B) A <= B */
#define _GE		-29	/* ($ge A B) A >= B */

/* Miscellaneous Routines */

#define _SAY		-30	/* ($say A B ...) printf( "%s%s...", A,B,...)*/
#define _ARG		-31	/* ($arg N) Nth arg to this routine */
#define _EXIT		-32	/* ($exit N) pop stack; if N !=0 next turn */
#define _RETURN		-33	/* ($return V) pop stack, retval = V */
#define _VAL		-34	/* ($val E) returns E */
#define _PHASE		-35	/* ($phase) returns current phase # */
#define _SPEC		-36	/* ($spec CODE A B C ...) perform one of:
				   CODE = 1, Toggle debugging mode
				   CODE = 2, Restart this run of ADL
				   CODE = 3, Terminate this run of ADL
				   CODE = 4, Save the game
				   CODE = 5, Restore a game
				   CODE = 6, Execute a program A with args B...
				   CODE = 7, Set the unknown words file
				   CODE = 8, Set script file
				   CODE = 9, Write a header
				   CODE = 10, Set left & right margins
				*/

/* Global-value functions */

#define _SETG		-37	/* ($setg VAR VAL) (VAR) = VAL */
#define _GLOBAL		-38	/* ($global VAR) @VAR */
#define _VERB		-39	/* ($verb) @Verb */
#define _DOBJ		-40	/* ($dobj) @Dobj */
#define _IOBJ		-41	/* ($iboj) @Iobj */
#define _PREP		-42	/* ($prep) @Prep */
#define _CONJ		-43	/* ($conj) @Conj */
#define _NUMD		-44	/* ($numd) @Numd */

/* Transition procedures */

#define _SETV		-45	/* ($setv V1 .. V10) VECVERB = V1 .. V10 */
#define _HIT		-46	/* ($hit OBJ D1 .. D10)
					($move OBJ D[ pos( @Verb, VECVERB )) */
#define _MISS		-47	/* ($miss R1 .. R10)
					eval( R[ pos( @Verb, VECVERB ) ) */

/* String functions */

#define _EQST		-48	/* ($eqst A B) returns strcmp( A, B ) == 0 */
#define _SUBS		-49	/* ($subs S P N) returns copy( S, P, N ) */
#define _LENG		-50	/* ($leng S) returns length( S ) */
#define _CAT		-51	/* ($cat S1 S2) returns strcat( S1, S2 ) */
#define _POS		-52	/* ($pos S1 S2) returns strpos( S1, S2 ) */
#define _CHR		-53	/* ($chr N) returns '\NNN' */
#define _ORD		-54	/* ($ord S) returns (int16) S[ 0 ] */
#define _READ		-55	/* ($read) returns user input string */
#define _SAVESTR	-56	/* ($savestr S) saves S in perm. area */
#define _NAME		-57	/* ($name OBJ) returns (2 word) name of OBJ */
#define _VNAME		-58	/* ($vname VERB) returns name of VERB */
#define _MNAME		-59	/* ($mname MODIF) returns name of MODIF */
#define _PNAME		-60	/* ($pname PREP) returns name of PREP */
#define _DEFINE		-61	/* ($define a b) expands a to b at runtime */
#define _UNDEF		-62	/* ($undef S) undefines S */
#define _STR		-63	/* ($str N) returns the ascii value of N */
#define _NUM		-64	/* ($num S) returns numeric value of S */

/* Demons, fuses, and actors */

#define _SDEM		-65	/* ($sdem R) activates R as a demon */
#define _DDEM		-66	/* ($ddem R) deactivates R as a demon */
#define _SFUS		-67	/* ($sfus R N) Activates R as a fuse,
					burning down in N turns */
#define _DFUS		-68	/* ($dfus R) Quenches R as a fuse */
#define _INCTURN	-69	/* ($incturn) Increment the turn counter */
#define _TURNS		-70	/* ($turns) Current val. of turn counter */
#define _PROMPT		-71	/* ($prompt R) Sets R as the prompt routine */
#define _ACTOR		-72	/* ($actor OBJ STR FLAG) new actor */
#define _DELACT		-73	/* ($delact OBJ) Deletes actor OBJ */

/*** EOF builtins.h ***/
