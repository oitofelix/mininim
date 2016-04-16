##
## Makefile.am -- Process this file with automake to produce
##                Makefile.in.
##
#
# Copyright (C) 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

include documentation.am




###########
# Program #
###########

bin_PROGRAMS = mininim
mininim_SOURCES = src/mininim.c src/mininim.h src/constants.h					\
		src/types.h src/colors.h src/anim.c src/anim.h src/pos.c					\
		src/pos.h src/physics.c src/physics.h src/level.c src/level.h			\
		src/consistency.c src/consistency.h src/stars.c src/stars.h				\
		src/floor.c src/floor.h src/broken-floor.c src/broken-floor.h			\
		src/skeleton-floor.c src/skeleton-floor.h src/loose-floor.c				\
		src/loose-floor.h src/opener-floor.c src/opener-floor.h						\
		src/closer-floor.c src/closer-floor.h src/spikes-floor.c					\
		src/spikes-floor.h src/door.c src/door.h src/level-door.c					\
		src/level-door.h src/pillar.c src/pillar.h src/big-pillar.c				\
		src/big-pillar.h src/chopper.c src/chopper.h src/bricks.c					\
		src/bricks.h src/torch.c src/torch.h src/window.c src/window.h		\
		src/balcony.c src/balcony.h src/arch.c src/arch.h src/carpet.c		\
		src/carpet.h src/mirror.c src/mirror.h src/room.c src/room.h			\
		src/fire.c src/fire.h src/potion.c src/potion.h src/sword.c				\
		src/sword.h src/clock.c src/clock.h src/princess.c src/princess.h	\
		src/jaffar.c src/jaffar.h src/mouse.c src/mouse.h src/samples.c		\
		src/samples.h src/cutscenes.c src/cutscenes.h src/fight.c					\
		src/fight.h src/dat.c src/dat.h src/wall/wall.c src/wall/wall.h		\
		src/wall/wall-depedv.c src/wall/wall-depedv.h src/wall/wall-pv.c	\
		src/wall/wall-pv.h src/wall/wall-dcpc.c src/wall/wall-dcpc.h			\
		src/kid/kid.c src/kid/kid.h src/kid/kid-normal.c									\
		src/kid/kid-normal.h src/kid/kid-walk.c src/kid/kid-walk.h				\
		src/kid/kid-start-run.c src/kid/kid-start-run.h src/kid/kid-run.c	\
		src/kid/kid-run.h src/kid/kid-stop-run.c src/kid/kid-stop-run.h		\
		src/kid/kid-turn-run.c src/kid/kid-turn-run.h src/kid/kid-couch.c	\
		src/kid/kid-couch.h src/kid/kid-turn.c src/kid/kid-turn.h					\
		src/kid/kid-stabilize.c src/kid/kid-stabilize.h										\
		src/kid/kid-jump.c src/kid/kid-jump.h src/kid/kid-vjump.c					\
		src/kid/kid-vjump.h src/kid/kid-run-jump.c src/kid/kid-run-jump.h	\
		src/kid/kid-misstep.c src/kid/kid-misstep.h src/kid/kid-hang.c		\
		src/kid/kid-hang.h src/kid/kid-hang-wall.c												\
		src/kid/kid-hang-wall.h src/kid/kid-hang-free.c										\
		src/kid/kid-hang-free.h src/kid/kid-climb.c src/kid/kid-climb.h		\
		src/kid/kid-unclimb.c src/kid/kid-unclimb.h src/kid/kid-fall.c		\
		src/kid/kid-fall.h src/kid/kid-drink.c src/kid/kid-drink.h				\
		src/kid/kid-raise-sword.c src/kid/kid-raise-sword.h								\
		src/kid/kid-keep-sword.c src/kid/kid-keep-sword.h									\
		src/kid/kid-take-sword.c src/kid/kid-take-sword.h									\
		src/kid/kid-sword-normal.c src/kid/kid-sword-normal.h							\
		src/kid/kid-sword-walkf.c src/kid/kid-sword-walkf.h								\
		src/kid/kid-sword-walkb.c src/kid/kid-sword-walkb.h								\
		src/kid/kid-sword-defense.c src/kid/kid-sword-defense.h						\
		src/kid/kid-sword-attack.c src/kid/kid-sword-attack.h							\
		src/kid/kid-sword-hit.c src/kid/kid-sword-hit.h src/kid/kid-die.c	\
		src/kid/kid-die.h src/kid/kid-stairs.c src/kid/kid-stairs.h				\
		src/guard/guard.c src/guard/guard.h src/guard/guard-normal.c			\
		src/guard/guard-normal.h src/guard/guard-vigilant.c								\
		src/guard/guard-vigilant.h src/guard/guard-hit.c									\
		src/guard/guard-hit.h src/guard/guard-die.c src/guard/guard-die.h	\
		src/guard/guard-fall.c src/guard/guard-fall.h											\
		src/guard/guard-walkf.c src/guard/guard-walkf.h										\
		src/guard/guard-walkb.c src/guard/guard-walkb.h										\
		src/guard/guard-attack.c src/guard/guard-attack.h									\
		src/guard/guard-defense.c src/guard/guard-defense.h								\
		src/levels/native-level.c src/levels/native-level.h								\
		src/levels/consistency-level.c src/levels/consistency-level.h			\
		src/levels/legacy-level.c src/levels/legacy-level.h								\
		src/levels/dat-level.c src/levels/dat-level.h											\
		src/levels/plv-level.c src/levels/plv-level.h src/kernel/timer.c	\
		src/kernel/timer.h src/kernel/event.c src/kernel/event.h					\
		src/kernel/video.c src/kernel/video.h src/kernel/audio.c					\
		src/kernel/audio.h src/kernel/memory.c src/kernel/memory.h				\
		src/kernel/gamepad.c src/kernel/gamepad.h src/kernel/xmouse.c			\
		src/kernel/xmouse.h src/kernel/random.c src/kernel/random.h				\
		src/kernel/array.c src/kernel/array.h src/kernel/xstdio.c					\
		src/kernel/xstdio.h src/kernel/file.c src/kernel/file.h						\
		src/kernel/dialog.c src/kernel/dialog.h src/kernel/threads.c			\
		src/kernel/threads.h src/kernel/xconfig.c src/kernel/xconfig.h		\
		src/kernel/diff.c src/kernel/diff.h src/menu.c src/menu.h					\
		src/editor.c src/editor.h src/debug.c src/debug.h src/undo.c			\
		src/undo.h src/multi-room.c src/multi-room.h

${mininim_OBJECTS}: ${top_builddir}/src/mininim.h.gch

${top_builddir}/src/mininim.h.gch: ${mininim_SOURCES:%.c=${srcdir}/%.h}
	$(AM_V_CC) $(COMPILE) -o $@ -c $<

CLEANFILES = ${top_builddir}/src/mininim.h.gch

AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\"												\
-DPKGDATADIR=\"$(pkgdatadir)\" -I$(top_builddir)/gnulib						\
-I$(top_srcdir)/gnulib -I${builddir}/src -I${srcdir}/src					\
-I${srcdir}/src/kernel -I${srcdir}/src/guard -I${srcdir}/src/kid	\
-I${srcdir}/src/levels -I${srcdir}/src/wall

AM_CFLAGS = ${ALLEGRO_CFLAGS} ${ALLEGRO_IMAGE_CFLAGS}	\
	${ALLEGRO_AUDIO_CFLAGS} ${ALLEGRO_ACODEC_CFLAGS}		\
	${ALLEGRO_FONT_CFLAGS} ${ALLEGRO_PRIMITIVES_CFLAGS}	\
	${ALLEGRO_DIALOG_CFLAGS}

if DEBUG
  CFLAGS += -O0 -Wall -ggdb3 -Werror -Wno-error=unused-function	\
  -Wno-error=unused-variable -Wno-error=unused-but-set-variable
else
  CFLAGS += -O3 -g0
endif

if OPTIMIZATIONS
 CFLAGS += -O3 -march=native
endif

LIBS = -lm ${ALLEGRO_LIBS} ${ALLEGRO_IMAGE_LIBS}										\
  ${ALLEGRO_AUDIO_LIBS} ${ALLEGRO_ACODEC_LIBS} ${ALLEGRO_FONT_LIBS}	\
  ${ALLEGRO_PRIMITIVES_LIBS} ${ALLEGRO_DIALOG_LIBS} ${mingw_ldadd}
LDADD = gnulib/libgnu.a

gnulib/libgnu.a:
	cd gnulib && $(MAKE)

if mingw_build
  LDADD += mininim.rc.$(OBJEXT)
  # AM_CFLAGS += -mwindows
endif
SUFFIXES = .rc
WINDRES = windres
%.rc.$(OBJEXT): %.rc
	$(WINDRES) $(WINDRESFLAGS) -o $@ $<

install-data-hook:
	$(MKDIR_P) "${DESTDIR}${pkgdatadir}"
	chmod u+w "${DESTDIR}${pkgdatadir}"
	cp -rv "${top_srcdir}/data" "${DESTDIR}${pkgdatadir}"

uninstall-local:
	find "${DESTDIR}${pkgdatadir}/data" \( -type f -or -type d \) \
		! -perm -u+w -exec chmod u+w {} ';'
	rm -rf "${DESTDIR}${pkgdatadir}/data"




#######################
# Automake directives #
#######################

ACLOCAL_AMFLAGS = -I gnulib/m4 -I build-aux
SUBDIRS = gnulib
if !mingw_build
SUBDIRS += doc
endif
EXTRA_DIST = ${top_level_doc} ChangeLog data




#########################
# Release documentation #
#########################

release_texi = ${abs_top_srcdir}/doc/release/release.texi.in
description_texi = ${abs_top_srcdir}/doc/release/description.texi

README : ${description_texi}
ANNOUNCEMENT : ${description_texi}

### Template recipe intended to generate the top-level release
### documentation files from the respective Texinfo sources.
define release_doc_recipe
${1} : $${top_srcdir}/doc/release/${1}.texi $${release_texi}
	$${AM_V_GEN}
	$${AM_V_at} $${makeinfo} -D file --plaintext \
	  --output=$$@ $$<
endef

### Instantiate the above recipe template for each top-level
### documentation file.
$(foreach f,${release_doc},$(eval $(call release_doc_recipe,$f)))

## Change log build recipe
ChangeLog :
	${AM_V_GEN}
	${AM_V_at} cd ${srcdir} && build-aux/git2cl > ChangeLog





############################
# Philosophy documentation #
############################

essay_header = ${abs_top_srcdir}/doc/philosophy/essay-header.texi

### Template recipe intended to generate the top-level release
### documentation files from the respective Texinfo sources.
define philosophy_doc_recipe
$(call uc,$1) : $${top_srcdir}/doc/philosophy/${1}.texi $${essay_header}
	$${AM_V_GEN}
	$${AM_V_at} $${makeinfo} -D GNUphiStandalone --plaintext \
	  --output=$$@ $$<
endef

### Instantiate the above recipe template for each top-level
### documentation file.
$(foreach f,${philosophy_doc},$(eval $(call philosophy_doc_recipe,$f)))





################
# Distribution #
################

# 'po/LINGUAS' file is generated automatically at configuration time
# and thus should not be included.
dist-hook :
	$(RM) ${distdir}/po/LINGUAS





###############
# Maintenance #
###############

fetchpo : ensure-vcs
	${AM_V_at} echo 'Fetching PO files...'
	${AM_V_at} $(RSYNC) -Lrtvz translationproject.org::tp/latest/mininim/ po

distgnu : ensure-vcs distcheck
	${AM_V_at} PATH=build-aux:${PATH} gnupload \
	  --to ftp.gnu.org:${PACKAGE_TARNAME} ${PACKAGE_TARNAME}-${PACKAGE_VERSION}.tar.gz

export PACKAGE_NAME PACKAGE_VERSION PACKAGE_MAILING_LIST

announcegnu : ensure-vcs ANNOUNCEMENT
	${AM_V_at} echo -n 'Sending announcement...'
	${AM_V_at} build-aux/make-announcement-mail | \
	  msmtp --read-recipients --account=gnu
	${AM_V_at} echo ' done.'

ensure-vcs :
	${AM_V_at} if test x${VCS} = xn; then \
	  echo "***** Sorry, the requested target can only be made from \
	VCS checkouts. *****"; exit 1; fi

.PHONY : ensure-vcs
