##
## Makefile.am -- Process this file with automake to produce
##                Makefile.in.
##
#
# Copyright (C) 2016, 2017 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>
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
mininim_SOURCES = src/mininim.c src/mininim.h src/compatibility.h			\
	  src/ui-command.c src/ui-command.h src/hotkeys.c src/hotkeys.h			\
	  src/constants.h src/types.h src/colors.h src/anim.c src/anim.h		\
	  src/actor.c src/actor.h src/pos.c src/pos.h src/physics.c					\
	  src/physics.h src/level.c src/level.h src/fix.c src/fix.h					\
	  src/loose-floor.c src/loose-floor.h src/opener-floor.c						\
	  src/opener-floor.h src/closer-floor.c src/closer-floor.h					\
	  src/spikes-floor.c src/spikes-floor.h src/door.c src/door.h				\
	  src/level-door.c src/level-door.h src/chomper.c src/chomper.h			\
	  src/carpet.c src/carpet.h src/mirror.c src/mirror.h src/room.c		\
	  src/room.h src/fire.c src/fire.h src/potion.c src/potion.h				\
	  src/sword.c src/sword.h src/clock.c src/clock.h src/princess.c		\
	  src/princess.h src/jaffar.c src/jaffar.h src/mouse.c src/mouse.h	\
	  src/audio-data.c src/audio-data.h src/cutscenes.c src/cutscenes.h	\
	  src/fight.c src/fight.h src/dat.c src/dat.h src/wall.c src/wall.h	\
	  src/kid/kid.c src/kid/kid.h src/kid/kid-normal.c									\
	  src/kid/kid-normal.h src/kid/kid-walk.c src/kid/kid-walk.h				\
	  src/kid/kid-start-run.c src/kid/kid-start-run.h src/kid/kid-run.c	\
	  src/kid/kid-run.h src/kid/kid-stop-run.c src/kid/kid-stop-run.h		\
	  src/kid/kid-turn-run.c src/kid/kid-turn-run.h											\
	  src/kid/kid-crouch.c src/kid/kid-crouch.h src/kid/kid-turn.c			\
	  src/kid/kid-turn.h src/kid/kid-stabilize.c												\
	  src/kid/kid-stabilize.h src/kid/kid-jump.c src/kid/kid-jump.h			\
	  src/kid/kid-vjump.c src/kid/kid-vjump.h src/kid/kid-run-jump.c		\
	  src/kid/kid-run-jump.h src/kid/kid-misstep.c											\
	  src/kid/kid-misstep.h src/kid/kid-hang.c src/kid/kid-hang.h				\
	  src/kid/kid-hang-free.c src/kid/kid-hang-free.h										\
	  src/kid/kid-hang-non-free.c src/kid/kid-hang-non-free.h						\
	  src/kid/kid-climb.c src/kid/kid-climb.h src/kid/kid-unclimb.c			\
	  src/kid/kid-unclimb.h src/kid/kid-fall.c src/kid/kid-fall.h				\
	  src/kid/kid-drink.c src/kid/kid-drink.h src/kid/kid-raise-sword.c	\
	  src/kid/kid-raise-sword.h src/kid/kid-keep-sword.c								\
	  src/kid/kid-keep-sword.h src/kid/kid-take-sword.c									\
	  src/kid/kid-take-sword.h src/kid/kid-sword-normal.c								\
	  src/kid/kid-sword-normal.h src/kid/kid-sword-walkf.c							\
	  src/kid/kid-sword-walkf.h src/kid/kid-sword-walkb.c								\
	  src/kid/kid-sword-walkb.h src/kid/kid-sword-defense.c							\
	  src/kid/kid-sword-defense.h src/kid/kid-sword-attack.c						\
	  src/kid/kid-sword-attack.h src/kid/kid-sword-hit.c								\
	  src/kid/kid-sword-hit.h src/kid/kid-die.c src/kid/kid-die.h				\
	  src/kid/kid-stairs.c src/kid/kid-stairs.h src/guard/guard.c				\
	  src/guard/guard.h src/guard/guard-normal.c												\
	  src/guard/guard-normal.h src/guard/guard-sword-normal.c						\
	  src/guard/guard-sword-normal.h src/guard/guard-sword-hit.c				\
	  src/guard/guard-sword-hit.h src/guard/guard-die.c									\
	  src/guard/guard-die.h src/guard/guard-fall.c											\
	  src/guard/guard-fall.h src/guard/guard-sword-walkf.c							\
	  src/guard/guard-sword-walkf.h src/guard/guard-sword-walkb.c				\
	  src/guard/guard-sword-walkb.h src/guard/guard-sword-attack.c			\
	  src/guard/guard-sword-attack.h src/guard/guard-sword-defense.c		\
	  src/guard/guard-sword-defense.h src/levels/native-level.c					\
	  src/levels/native-level.h src/levels/consistency-level.c					\
	  src/levels/consistency-level.h src/levels/legacy-level.c					\
	  src/levels/legacy-level.h src/levels/dat-level.c									\
	  src/levels/dat-level.h src/levels/plv-level.c											\
	  src/levels/plv-level.h src/kernel/event.c src/kernel/event.h			\
	  src/kernel/video.c src/kernel/video.h src/kernel/audio.c					\
	  src/kernel/audio.h src/kernel/memory.c src/kernel/memory.h				\
	  src/kernel/gamepad.c src/kernel/gamepad.h src/kernel/pointer.c		\
	  src/kernel/pointer.h src/kernel/random.c src/kernel/random.h			\
	  src/kernel/array.c src/kernel/array.h src/kernel/file.c						\
	  src/kernel/file.h src/kernel/dialog.c src/kernel/dialog.h					\
	  src/kernel/xconfig.c src/kernel/xconfig.h src/kernel/diff.c				\
	  src/kernel/diff.h src/kernel/xmath.c src/kernel/xmath.h						\
	  src/kernel/xstring.c src/kernel/xstring.h src/kernel/tree.c				\
	  src/kernel/tree.h src/bmenu.c src/bmenu.h src/editor.c						\
	  src/editor.h src/debug.c src/debug.h src/undo.c src/undo.h				\
	  src/multi-room.c src/multi-room.h src/sel-ring.c src/sel-ring.h		\
	  src/view-ring.c src/view-ring.h src/replay.c src/replay.h					\
	  src/script/repl.c src/script/repl.h src/script/script.c						\
	  src/script/script.h src/script/L_mininim.c src/script/L_mininim.h	\
	  src/script/L_mininim.level.c src/script/L_mininim.level.h					\
	  src/script/L_mininim.level.position.c															\
	  src/script/L_mininim.level.position.h															\
	  src/script/L_mininim.actor.c src/script/L_mininim.actor.h					\
	  src/script/L_mininim.audio.c src/script/L_mininim.audio.h					\
	  src/script/L_mininim.audio.source.c																\
	  src/script/L_mininim.audio.source.h src/script/L_mininim.video.c	\
	  src/script/L_mininim.video.h src/script/L_mininim.video.bitmap.c	\
	  src/script/L_mininim.video.bitmap.h																\
	  src/script/L_mininim.video.font.c																	\
	  src/script/L_mininim.video.font.h																	\
	  src/script/L_mininim.video.color.c																\
	  src/script/L_mininim.video.color.h																\
	  src/script/L_mininim.video.rectangle.c														\
	  src/script/L_mininim.video.rectangle.h														\
	  src/script/L_mininim.video.shader.c																\
	  src/script/L_mininim.video.shader.h																\
	  src/script/L_mininim.settings.c src/script/L_mininim.settings.h		\
	  src/script/L_mininim.mouse.c src/script/L_mininim.mouse.h					\
	  src/script/L_mininim.multiroom.c src/script/L_mininim.multiroom.h	\
	  src/script/L_mininim.profiler.c src/script/L_mininim.profiler.h		\
	  src/script/L_mininim.debugger.c src/script/L_mininim.debugger.h		\
	  src/script/L_mininim.math.c src/script/L_mininim.math.h						\
	  src/gui/gui.c src/gui/gui.h src/gui/gui-icons.h										\
	  src/gui/gui-menu.c src/gui/gui-menu.h src/gui/gui-editor-dialog.c	\
	  src/gui/gui-editor-dialog.h src/gui/gui-undo-control.c						\
	  src/gui/gui-undo-control.h src/gui/gui-tile-part-control.c				\
	  src/gui/gui-tile-part-control.h src/gui/gui-tile-part-dialog.c		\
	  src/gui/gui-tile-part-dialog.h																		\
	  src/gui/gui-tile-clipboard-control.c															\
	  src/gui/gui-tile-clipboard-control.h															\
	  src/gui/gui-tile-transform-control.c															\
	  src/gui/gui-tile-transform-control.h															\
	  src/gui/gui-tile-mirror-control.c																	\
	  src/gui/gui-tile-mirror-control.h src/gui/gui-tile-move-control.c	\
	  src/gui/gui-tile-move-control.h src/gui/gui-position-control.c		\
	  src/gui/gui-position-control.h																		\
	  src/gui/gui-editor-events-control.c																\
	  src/gui/gui-editor-events-control.h																\
	  src/gui/gui-editor-room-control.c																	\
	  src/gui/gui-editor-room-control.h																	\
	  src/gui/gui-editor-toolbar-control.c															\
	  src/gui/gui-editor-toolbar-control.h															\
	  src/gui/gui-directional-control.c																	\
	  src/gui/gui-directional-control.h																	\
	  src/gui/gui-coordinates-control.c																	\
	  src/gui/gui-coordinates-control.h src/gui/gui-zoom-control.c			\
	  src/gui/gui-zoom-control.h src/gui/gui-place-control.c						\
	  src/gui/gui-place-control.h src/gui/gui-mr-origin-control.c				\
	  src/gui/gui-mr-origin-control.h src/gui/gui-row-control.c					\
	  src/gui/gui-row-control.h src/gui/gui-page-control.c							\
	  src/gui/gui-page-control.h src/gui/gui-editor-tabs.c							\
	  src/gui/gui-editor-tabs.h src/gui/gui-editor-tile-control.c				\
	  src/gui/gui-editor-tile-control.h																	\
	  src/gui/gui-editor-view-nav-control.c															\
	  src/gui/gui-editor-view-nav-control.h

CLEANFILES = ${top_builddir}/src/mininim.h.gch

AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\"											\
-DPKGDATADIR=\"$(pkgdatadir)\" -I$(top_builddir)/gnulib					\
${LUA_INCLUDE} -I$(top_srcdir)/gnulib -I${builddir}/src					\
-I${srcdir}/src -I${srcdir}/src/kernel -I${srcdir}/src/guard		\
-I${srcdir}/src/kid -I${srcdir}/src/levels -I${srcdir}/src/wall	\
-I${srcdir}/src/script -I${srcdir}/src/gui

AM_CFLAGS = ${ALLEGRO_CFLAGS} ${ALLEGRO_IMAGE_CFLAGS}	\
	${ALLEGRO_AUDIO_CFLAGS} ${ALLEGRO_ACODEC_CFLAGS}		\
	${ALLEGRO_FONT_CFLAGS} ${ALLEGRO_PRIMITIVES_CFLAGS}	\
	${ALLEGRO_MAIN_CFLAGS} ${ALLEGRO_DIALOG_CFLAGS}			\
	${ALLEGRO_COLOR_CFLAGS}

if DEBUG
  CFLAGS += -O0 -Wall -ggdb3
else
  CFLAGS += -O3 -g0
endif

if OPTIMIZATIONS
 CFLAGS += -O3 -march=native
endif

if PROFILING
 CFLAGS += -O0 -pg --coverage
endif

LIBS = -lm -lreadline ${ALLEGRO_LIBS} ${ALLEGRO_IMAGE_LIBS}				\
${ALLEGRO_AUDIO_LIBS} ${ALLEGRO_ACODEC_LIBS} ${ALLEGRO_FONT_LIBS}	\
${ALLEGRO_PRIMITIVES_LIBS} ${ALLEGRO_MAIN_LIBS}										\
${ALLEGRO_DIALOG_LIBS} ${ALLEGRO_COLOR_LIBS} ${mingw_ldadd}				\
${LUA_LIB}

LDADD = gnulib/libgnu.a $(LIBSOCKET) $(LIB_SELECT) $(LTLIBINTL)	\
$(LTLIBTHREAD)

if mingw_build
  LDADD += mininim.rc.$(OBJEXT) -liup
# AM_CFLAGS += -mwindows

# Use this for linking IUP statically:
#
# -l:libiup.a -l:libgdi32.a -l:libcomdlg32.a -l:libcomctl32.a \
# -l:libuuid.a -l:liboleaut32.a -l:libole32.a
else
	LDADD	+= -liupmot
endif
SUFFIXES = .rc
WINDRES = ${host}-windres

${mininim_OBJECTS}: ${top_builddir}/src/mininim.h.gch

${top_builddir}/src/mininim.h.gch: ${mininim_SOURCES:%.c=${srcdir}/%.h}
	$(AM_V_CC) $(COMPILE) ${AM_CFLAGS} -o $@ -c $<

gnulib/libgnu.a:
	cd gnulib && $(MAKE)

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
if MAKEINFO
SUBDIRS += doc
endif
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
