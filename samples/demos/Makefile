AR = post -c -v -p 12 17
B = ../..
D = samples/demos/

all		: actdemo baby multi tiny

clean		:
	rm -f core actdemo tiny baby multi demos.shar

shar		: demos.shar

demos.shar	: README	COPYRIGHT	Makefile	\
		  actdemo.adl	baby.adl	multi.adl	\
		  tiny.adl
	( cd ${B} ; ${AR}	\
		${D}README	${D}COPYRIGHT	${D}Makefile	\
		${D}actdemo.adl	${D}baby.adl	${D}multi.adl	\
		${D}tiny.adl	\
	> ${D}demos.shar )

actdemo		: actdemo.adl ../standard.adl
	adlcomp actdemo.adl -o actdemo -i ..

baby		: baby.adl ../standard.adl
	adlcomp baby.adl -i.. -o baby

multi		: multi.adl
	adlcomp multi.adl -o multi

tiny		: tiny.adl ../standard.adl
	adlcomp tiny.adl -i.. -o tiny
