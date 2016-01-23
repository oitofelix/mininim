ENGINE_MODULES = prince anim pos physics level consistency stars			\
  floor broken-floor skeleton-floor loose-floor opener-floor					\
  closer-floor spikes-floor door level-door pillar big-pillar chopper	\
  bricks torch window balcony arch carpet mirror room fire potion			\
  sword clock princess jaffar mouse fight samples cutscenes
WALL_MODULES = wall wall-depedv wall-pv wall-dcpc
KID_MODULES = kid kid-normal kid-walk kid-start-run kid-run						\
  kid-stop-run kid-turn-run kid-couch kid-turn kid-stabilize kid-jump	\
  kid-vjump kid-run-jump kid-misstep kid-hang kid-hang-wall						\
  kid-hang-free kid-climb kid-unclimb kid-fall kid-drink							\
  kid-raise-sword kid-keep-sword kid-take-sword kid-sword-normal			\
  kid-sword-walkf kid-sword-walkb kid-sword-defense kid-sword-attack	\
  kid-sword-hit kid-die kid-stairs
GUARD_MODULES = guard guard-normal guard-vigilant guard-hit guard-die	\
  guard-fall guard-walkf guard-walkb guard-attack guard-defense
LEVEL_MODULES = level-1 consistency-level legacy-level
KERNEL_MODULES = event timer video audio memory keyboard random array	\
  xerror

MODULES = ${KERNEL_MODULES:%=src/kernel/%} ${ENGINE_MODULES:%=src/%}	\
	${WALL_MODULES:%=src/wall/%} ${KID_MODULES:%=src/kid/%}							\
	${GUARD_MODULES:%=src/guard/%} ${LEVEL_MODULES:%=src/levels/%}

SRCS = ${MODULES:=.c}
OBJECTS = ${MODULES:=.o}
COV_FILES = ${MODULES:=.gcda} ${MODULES:=.gcno} gmon.out
LDFLAGS = -lm -lallegro -lallegro_image -lallegro_audio	\
  -lallegro_acodec -lallegro_font -lallegro_primitives
CFLAGS_DEV = -Wall -ggdb3 -Werror -Wno-error=unused-function \
  -Wno-error=unused-variable -Wno-error=unused-but-set-variable
CFLAGS_REL = -O3 -march=native
CFLAGS_COV = ${CFLAGS_DEV} --coverage -pg
CFLAGS = ${CFLAGS_DEV}
CPPFLAGS = -Isrc -Isrc/kernel -Isrc/guard -Isrc/kid -Isrc/levels	\
  -Isrc/wall

prince : ${OBJECTS}
	${CC} ${OBJECTS} -o $@ ${CPPFLAGS} ${CFLAGS} ${LDFLAGS}

# .depend: ${SRCS}
# 	rm -f .depend
# 	$(CC) ${CPPFLAGS} $(CFLAGS) -MM $^ > .depend

# include .depend

.PHONY: clean
clean:
	rm -f prince ${OBJECTS} .depend
