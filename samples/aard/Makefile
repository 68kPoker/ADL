# AR = shar -c -v
AR1 = post -c -v -p 10 17
AR2 = post -c -v -p 11 17
B = ../..
D = samples/aard/

all		: aard

aard		: *.adl ../../adlcomp/adlcomp ../standard.adl
	adlcomp aard.adl -w -o aard -i..
shar		: aard1.shar	aard2.shar

clean		:
	rm -f aard core aard1.shar aard2.shar

aard1.shar	: README COPYRIGHT Makefile locales.adl objects.adl verbs.adl
	( cd ${B} ; ${AR1}	\
		${D}README	${D}COPYRIGHT	${D}Makefile	\
		${D}locales.adl	${D}objects.adl	${D}verbs.adl	\
	> ${D}aard1.shar )

aard2.shar	: aard.adl	routines.adl	constant.adl \
		  locnames.adl	objrouts.adl	transit.adl
	( cd ${B} ; ${AR2}	\
		${D}aard.adl	${D}routines.adl	${D}constant.adl \
		${D}locnames.adl ${D}objrouts.adl	${D}transit.adl	\
	> ${D}aard2.shar )
