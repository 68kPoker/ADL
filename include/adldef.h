/* Token types */


#define MIN_LEGAL	400	/* Lower bound of types that can appear
				   in an ADL expression */
#define ROUTINE		400
#define CONST		401
#define VAR		402
#define ARGUMENT	403
#define LOCAL		404

#define MIN_RT		405	/* Lower bound of types that can appear
				   in an ADL sentence */
#define PREP		405
#define VERB		406
#define STRING		407
#define ADJEC		408
#define NOUN		409
#define NOUN_SYN	410

#define MAX_LEGAL	410	/* Upper bound of types that can appear
				   in an ADL expression */
#define ARTICLE		411
#define ART		411
#define	CONJ		412
#define COMMA		413
#define	SEP		414

#define MAX_RT		413	/* Upper bound of types that can appear
				   in an ADL sentence */
#define MYVAL		414	/* .ME */
#define UNDECLARED	415


/* Control statements */

#define IF		500
#define THEN		501
#define ELSEIF		502
#define ELSE		503
#define WHILE		504
#define DO		505


/* Declaration statements */

#define	MIN_D		600

#define ROUT_D		600
#define VERB_D		601
#define ADJEC_D		602
#define ART_D		603
#define PREP_D		604

#define MAX_D		604

#define VAR_D		605
#define LOCAL_D		606
#define NOUN_D		607
#define INCLUDE		608
#define MESSAGE		609


/* stack machine opcodes */

#define NOP	0x00		/* 00000000			*/
#define PUSHME	0x01		/* 00000001			*/
#define POP	0x02		/* 00000010			*/
#define RET	0x03		/* 00000011			*/
#define FILEN	0x04		/* 00000100 ........ ........	*/
#define LINEN	0x05		/* 00000101 ........ ........	*/
#define PUSH	0x06		/* 00000110 ........ ........	*/
#define ILLEGAL	0x07		/* 00000111			*/
#define PUSHS	0x08		/* 00001... ........		*/
#define JMP	0x10		/* 00010... ........ ........	*/
#define JMPZ	0x18		/* 00011... ........ ........	*/
#define PUSHARG	0x20		/* 001.....			*/
#define CALL	0x40		/* 001.....			*/
#define PUSHLOCL 0x60		/* 001.....			*/
#define PUSHN	0x80		/* 1.......			*/

/*** EOF adldef.h ***/
