# AR = shar -c -v
AR1 = post -c -v -p 13 17
AR2 = post -c -v -p 14 17
B = ../..
D = samples/mpu/

all		: mpu

mpu		: *.adl ../../adlcomp/adlcomp ../standard.adl
	adlcomp mpu.adl -i.. -o mpu

shar		: mpu1.shar	mpu2.shar

clean		:
	rm -f mpu core mpu1.shar mpu2.shar

mpu1.shar	: README	COPYRIGHT	Makefile	locales.adl \
		  mpu.adl	routines.adl	verbs.adl
	( cd ${B} ; ${AR1}	\
		${D}README	${D}COPYRIGHT	${D}Makefile	\
		${D}locales.adl	${D}mpu.adl	${D}routines.adl \
		${D}verbs.adl	\
	> ${D}mpu1.shar )

mpu2.shar	: constant.adl	locnames.adl	objects.adl \
		  transit.adl
	( cd ${B} ; ${AR2}	\
		${D}constant.adl	${D}locnames.adl	\
		${D}objects.adl		${D}transit.adl		\
	> ${D}mpu2.shar )
