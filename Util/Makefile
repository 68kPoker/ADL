INC = ../include
# AR = shar -c -v
AR = post -c -v -p 9 17
B = ..
D = util/

DFLAGS = -O
CFLAGS = -I${INC} ${DFLAGS}

OBJS = vstring.o	virtmem.o 	  mytime.o

all		: ${OBJS}

shar		: util.shar

util.shar	: README Makefile *.c
	( cd ${B} ; ${AR}	\
		${D}README	${D}COPYRIGHT	${D}Makefile	\
		${D}mytime.c	${D}virtmem.c	${D}vstring.c	\
	> ${D}util.shar )

mytime.o	: mytime.c

virtmem.o	: virtmem.c	${INC}/adltypes.h	${INC}/virtmem.h

vstring.o	: vstring.c	${INC}/adltypes.h	${INC}/virtmem.h

clean		:
	rm -f ${OBJS} core util.shar
