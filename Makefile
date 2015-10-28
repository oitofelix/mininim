KERNEL_MODULES = event timer video audio memory keyboard random
ENGINE_MODULES = anim physics level stars room fire clock kid princess jaffar
LEVEL_MODULES = title level-1
MAIN_MODULES = prince

MODULES = ${KERNEL_MODULES:%=kernel/%} ${ENGINE_MODULES:%=engine/%}	\
	${LEVEL_MODULES:%=levels/%} ${MAIN_MODULES}

SRCS = ${MODULES:=.c}
OBJECTS = ${MODULES:=.o}
LDFLAGS = -lallegro -lallegro_image -lallegro_audio -lallegro_acodec -lallegro_font
CFLAGS = -Wall -ggdb3 -Werror
CPPFLAGS = -I$(shell pwd)

prince : ${OBJECTS} .depend
	${CC} ${CPPFLAGS} ${CFLAGS} ${LDFLAGS} ${OBJECTS} -o $@

.depend: ${SRCS}
	rm -f .depend
	$(CC) ${CPPFLAGS} $(CFLAGS) -MM $^ > .depend

include .depend

.PHONY: clean
clean:
	rm -f prince ${OBJECTS} .depend
