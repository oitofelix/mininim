KERNEL_MODULES = event timer video audio memory keyboard random array	\
xerror
ENGINE_MODULES = anim pos physics level consistency stars floor				\
  broken-floor skeleton-floor loose-floor opener-floor closer-floor		\
  spikes-floor door level-door pillar big-pillar chopper bricks torch	\
  window balcony arch carpet mirror room fire potion sword clock			\
  princess jaffar mouse
WALL_MODULES = wall wall-depedv wall-pv wall-dcpc
KID_MODULES = kid kid-normal kid-walk kid-start-run kid-run						\
  kid-stop-run kid-turn-run kid-couch kid-turn kid-stabilize kid-jump	\
  kid-vjump kid-run-jump kid-misstep kid-hang kid-hang-wall						\
  kid-hang-free kid-climb kid-unclimb kid-fall kid-drink							\
  kid-raise-sword kid-keep-sword kid-take-sword kid-sword-normal			\
  kid-sword-walkf kid-sword-walkb kid-sword-defense kid-sword-attack	\
  kid-die kid-stairs
LEVEL_MODULES = title level-1 consistency-level legacy-level
MAIN_MODULES = prince

MODULES = ${KERNEL_MODULES:%=kernel/%} ${ENGINE_MODULES:%=engine/%}	\
	${WALL_MODULES:%=engine/wall/%} ${KID_MODULES:%=engine/kid/%}			\
	${LEVEL_MODULES:%=levels/%} ${MAIN_MODULES}

SRCS = ${MODULES:=.c}
OBJECTS = ${MODULES:=.o}
COV_FILES = ${MODULES:=.gcda} ${MODULES:=.gcno}
LDFLAGS = -lm -lallegro -lallegro_image -lallegro_audio	\
  -lallegro_acodec -lallegro_font -lallegro_primitives
CFLAGS_DEV = -Wall -Og -ggdb3 -Werror -Wno-error=unused-function \
  -Wno-error=unused-variable -Wno-error=unused-but-set-variable
CFLAGS_REL = -O3 -march=native
CFLAGS_COV = ${CFLAGS_DEV} --coverage -pg
CFLAGS = ${CFLAGS_DEV}
CPPFLAGS = -I$(shell pwd)

prince : ${OBJECTS}
	${CC} ${OBJECTS} -o $@ ${CPPFLAGS} ${CFLAGS} ${LDFLAGS}

# .depend: ${SRCS}
# 	rm -f .depend
# 	$(CC) ${CPPFLAGS} $(CFLAGS) -MM $^ > .depend

# include .depend

.PHONY: clean
clean:
	rm -f prince ${OBJECTS} ${COV_FILES} gmon.out .depend
