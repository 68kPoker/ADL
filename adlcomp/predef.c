/* predef.c - predeclared symbols for adlcomp */
#include <stdio.h>		/* To get EOF */
#include "adltypes.h"
#include "adlprog.h"
#include "adldef.h"
#include "builtins.h"

init_predefs()
{
    insert_sys(  ".ME",		MYVAL, 0 );

    insert_sys( "INCLUDE",	INCLUDE,	0 );
    insert_sys( "MESSAGE",	MESSAGE,	0 );
    insert_sys( "VAR",		VAR_D,		0 );
    insert_sys( "ROUTINE",	ROUT_D,		0 );
    insert_sys( "LOCAL",	LOCAL_D,	0 );

    insert_sys( "ARTICLE",	ART_D,		0 );
    insert_sys( "PREP",		PREP_D,		0 );
    insert_sys( "ADJEC",	ADJEC_D,	0 );
    insert_sys( "NOUN",		NOUN_D,		0 );
    insert_sys( "VERB",		VERB_D,		0 );

    insert_sys( "DWIMD",	ROUTINE,	_DWIMD );
    insert_sys( "DWIMI",	ROUTINE,	_DWIMI );
    insert_sys( "START",	ROUTINE,	_START );

    insert_sys( "NOVERB",	VERB,		_NOVERB );
    insert_sys( "TELLER",	VERB,		_TELLER );

    insert_sys( "LDESC",	CONST,		_LD );
    insert_sys( "SDESC",	CONST,		_SD );
    insert_sys( "ACTION",	CONST,		_ACT );
    insert_sys( "PREACT",	CONST,		_PREACT );

    insert_sys( "Verb",		VAR,		_VERB_G );
    insert_sys( "Dobj",		VAR,		_DOBJ_G );
    insert_sys( "Iobj",		VAR,		_IOBJ_G );
    insert_sys( "Prep",		VAR,		_PREP_G );
    insert_sys( "Conj",		VAR,		_CONJ_G );
    insert_sys( "Numd",		VAR,		_NUMD_G );

    insert_sys( ".ALL",		NOUN_SYN,	_ALL );
    insert_sys( "STRING",	NOUN_SYN,	_STRING );

    insert_sys( "WHILE",	WHILE,		0 );
    insert_sys( "DO",		DO,		0 );
    insert_sys( "IF",		IF,		0 );
    insert_sys( "THEN",		THEN,		0 );
    insert_sys( "ELSEIF",	ELSEIF,		0 );
    insert_sys( "ELSE",		ELSE,		0 );

    insert_sys( "$loc",		ROUTINE,	_LOC );
    insert_sys( "$cont",	ROUTINE,	_CONT );
    insert_sys( "$link",	ROUTINE,	_LINK );
    insert_sys( "$ldesc",	ROUTINE,	_LDESC );
    insert_sys( "$sdesc",	ROUTINE,	_SDESC );
    insert_sys( "$action",	ROUTINE,	_ACTION );
    insert_sys( "$prop",	ROUTINE,	_PROP );
    insert_sys( "$setp",	ROUTINE,	_SETP );
    insert_sys( "$move",	ROUTINE,	_MOVE );
    insert_sys( "$modif",	ROUTINE,	_MODIF );

    insert_sys( "$vset",	ROUTINE,	_VSET );
    insert_sys( "$vprop",	ROUTINE,	_VPROP );

    insert_sys( "$plus",	ROUTINE,	_PLUS );
    insert_sys( "$minus",	ROUTINE,	_MINUS );
    insert_sys( "$times",	ROUTINE,	_TIMES );
    insert_sys( "$div",		ROUTINE,	_DIV );
    insert_sys( "$mod",		ROUTINE,	_MOD );
    insert_sys( "$rand",	ROUTINE,	_RAND );

    insert_sys( "$and",		ROUTINE,	_AND );
    insert_sys( "$or",		ROUTINE,	_OR );
    insert_sys( "$not",		ROUTINE,	_NOT );
    insert_sys( "$yorn",	ROUTINE,	_YORN );
    insert_sys( "$pct",		ROUTINE,	_PCT );
    insert_sys( "$eq",		ROUTINE,	_EQ );
    insert_sys( "$ne",		ROUTINE,	_NE );
    insert_sys( "$lt",		ROUTINE,	_LT );
    insert_sys( "$gt",		ROUTINE,	_GT );
    insert_sys( "$le",		ROUTINE,	_LE );
    insert_sys( "$ge",		ROUTINE,	_GE );

    insert_sys( "$say",		ROUTINE,	_SAY );
    insert_sys( "$arg",		ROUTINE,	_ARG );
    insert_sys( "$exit",	ROUTINE,	_EXIT );
    insert_sys( "$return",	ROUTINE,	_RETURN );
    insert_sys( "$val",		ROUTINE,	_VAL );
    insert_sys( "$phase",	ROUTINE,	_PHASE );
    insert_sys( "$spec",	ROUTINE,	_SPEC );

    insert_sys( "$setg",	ROUTINE,	_SETG );
    insert_sys( "$global",	ROUTINE,	_GLOBAL );
    insert_sys( "$verb",	ROUTINE,	_VERB );
    insert_sys( "$dobj",	ROUTINE,	_DOBJ );
    insert_sys( "$iobj",	ROUTINE,	_IOBJ );
    insert_sys( "$prep",	ROUTINE,	_PREP );
    insert_sys( "$conj",	ROUTINE,	_CONJ );
    insert_sys( "$numd",	ROUTINE,	_NUMD );

    insert_sys( "$setv",	ROUTINE,	_SETV );
    insert_sys( "$hit",		ROUTINE,	_HIT );
    insert_sys( "$miss",	ROUTINE,	_MISS );

    insert_sys( "$eqst",	ROUTINE,	_EQST );
    insert_sys( "$subs",	ROUTINE,	_SUBS );
    insert_sys( "$leng",	ROUTINE,	_LENG );
    insert_sys( "$cat",		ROUTINE,	_CAT );
    insert_sys( "$pos",		ROUTINE,	_POS );
    insert_sys( "$chr",		ROUTINE,	_CHR );
    insert_sys( "$ord",		ROUTINE,	_ORD );
    insert_sys( "$read",	ROUTINE,	_READ );
    insert_sys( "$savestr",	ROUTINE,	_SAVESTR );
    insert_sys( "$name",	ROUTINE,	_NAME );
    insert_sys( "$vname",	ROUTINE,	_VNAME );
    insert_sys( "$mname",	ROUTINE,	_MNAME );
    insert_sys( "$pname",	ROUTINE,	_PNAME );
    insert_sys( "$define",	ROUTINE,	_DEFINE );
    insert_sys( "$undef",	ROUTINE,	_UNDEF );
    insert_sys( "$str",		ROUTINE,	_STR );
    insert_sys( "$num",		ROUTINE,	_NUM );

    insert_sys( "$sdem",	ROUTINE,	_SDEM );
    insert_sys( "$ddem",	ROUTINE,	_DDEM );
    insert_sys( "$sfus",	ROUTINE,	_SFUS );
    insert_sys( "$dfus",	ROUTINE,	_DFUS );
    insert_sys( "$incturn",	ROUTINE,	_INCTURN );
    insert_sys( "$turns",	ROUTINE,	_TURNS );
    insert_sys( "$prompt",	ROUTINE,	_PROMPT );
    insert_sys( "$actor",	ROUTINE,	_ACTOR );
    insert_sys( "$delact",	ROUTINE,	_DELACT );
}

/*** EOF predef.c ***/
