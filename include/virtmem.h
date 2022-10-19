/* Include <stdio.h> and "adltypes.h" before including this file */

#define BSIZE	512
#define	NUMP	16


struct	pt_entry {
	struct	pt_entry	*prev,		/* Previous page in lru list */
				*next;		/* Next page in lru list */
	int16			pnum;		/* Page number */
	char			buffer[BSIZE];	/* Actual data buffer */
};


struct	pagetab {
	struct	pt_entry	*lru,
				*mru,
				ptabs[NUMP];
	int			pfile;
	int32			offset,
				nhits,
				nmiss,
				nswap;
	int16			numpages,
				maxpage,
				dirty;
};


extern
    vm_init(  /*int which; int32 offs; struct pagetab *handle; int16 dirty*/ ),
    vm_flush( /* struct pagetab *handle */ ),
    vm_put8(  /* char data; int32 where; struct pagetab *handle */ ),
    vm_put16( /* int16 data; int32 where; struct pagetab *handle */ );

extern char
    vm_get8( /* int32 where; struct pagetab *handle */ );

extern int16
    vm_get16( /* int32 where; struct pagetab *handle */ );

/*** EOF virtmem.h ***/
