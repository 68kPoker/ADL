extern int16
    newstr(	 /* str */ ),	/* Sets up str as a new permanent string */
    newtstr( /* str */ ),	/* Sets up str as a new temporary string */
    vs_save( /* int16 */ ),	/* Sets up virtstr( int16 ) as a new "CMOS"
				   string */
    numstr();			/* Returns the number of permanent strings
				   defined */

extern int32
    numchar();			/* Returns the number of permanent chars
				   allocated */
extern char
    *virtstr( /* index */ );	/* Returns the virtstr at index */

/*** EOF vstring.h ***/
