KERNEL_MODULES = event timer video audio memory keyboard random array
ENGINE_MODULES = anim pos physics level stars floor loose-floor			\
  opener-floor closer-floor spikes-floor door pillar wall room fire	\
  potion sword clock kid princess jaffar
LEVEL_MODULES = title level-1
MAIN_MODULES = prince

MODULES = ${KERNEL_MODULES:%=kernel/%} ${ENGINE_MODULES:%=engine/%}	\
	${LEVEL_MODULES:%=levels/%} ${MAIN_MODULES}

SRCS = ${MODULES:=.c}
OBJECTS = ${MODULES:=.o}
LDFLAGS = -lm -lallegro -lallegro_image -lallegro_audio	\
  -lallegro_acodec -lallegro_font -lallegro_primitives
CFLAGS_DEV = -Wall -Og -ggdb3 -Werror -Wno-error=unused-function \
  -Wno-error=unused-variable -Wno-error=unused-but-set-variable
CFLAGS_REL = -O3 -march=native
CFLAGS = ${CFLAGS_DEV}
CPPFLAGS = -I$(shell pwd)

prince : ${OBJECTS} .depend
	${CC} ${OBJECTS} -o $@ ${CPPFLAGS} ${CFLAGS} ${LDFLAGS}

.depend: ${SRCS}
	rm -f .depend
	$(CC) ${CPPFLAGS} $(CFLAGS) -MM $^ > .depend

include .depend

.PHONY: clean
clean:
	rm -f prince ${OBJECTS} .depend
