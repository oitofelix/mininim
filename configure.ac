dnl
dnl configure.ac -- configuration script;
dnl
dnl Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>
dnl
dnl This program is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation, either version 3 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.
dnl


##################
# Initialization #
##################

# Release definitions;
AC_INIT([MININIM], m4_esyscmd_s([date -u +%-y.%-m.%-d.%-H.%-M]),
[oitofelix@gnu.org], [mininim],
[http://oitofelix.freeshell.org/mininim/])
AC_SUBST([PACKAGE_MAILING_LIST], [])
AC_DEFINE([CURRENT_YEAR],
          "m4_esyscmd_s([date -u +%Y])",
          [Last year in range for copyright purposes.])
AC_DEFINE_UNQUOTED([PACKAGE_MAILING_LIST], ["$PACKAGE_MAILING_LIST"],
  [Help and support mailing list])
AC_DEFINE(PACKAGE_TRANSLATION_BUGREPORT,
["http://translationproject.org/team/"], [Translation bugs reporting address])

# GNU Autoconf settings;
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux])
AC_PREREQ([2.64])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/mininim.c])

# GNU Automake settings;
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_SILENT_RULES([yes])
dnl AM_EXTRA_RECURSIVE_TARGETS([homepage install-man])
dnl AM_GNU_GETTEXT_VERSION([0.18.1])
dnl AM_GNU_GETTEXT([external])

# Decide whether you are configuring from VCS.
AS_IF([[test -d "$srcdir"/.git]], [[VCS=y]], [[VCS=n]])
AC_SUBST([VCS])

# Compute the canonical build-system variables.
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

# Debug option
AC_ARG_ENABLE([debug],
[AS_HELP_STRING([--enable-debug], [Turn on debugging])],
[case "${enableval}" in
yes) debug=true ;;
no)  debug=false ;;
*) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],[debug=false])
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

AS_IF([[test x$debug != xtrue]],
  [AC_DEFINE([NDEBUG], [1], [Define to 1 to disable assert macro.])],
  [debug_suffix=-debug]
  )

# Optimizations option
AC_ARG_ENABLE([optimizations],
[AS_HELP_STRING([--enable-optimizations],[Turn on optimizations])],
[case "${enableval}" in
yes) optimizations=true ;;
no)  optimizations=false ;;
*) AC_MSG_ERROR([bad value ${enableval} for --enable-optimizations]) ;;
esac],[optimizations=false])
AM_CONDITIONAL([OPTIMIZATIONS], [test x$optimizations = xtrue])

# Profiling option
AC_ARG_ENABLE([profiling],
[AS_HELP_STRING([--enable-profiling], [Turn on profiling])],
[case "${enableval}" in
yes) profiling=true ;;
no)  profiling=false ;;
*) AC_MSG_ERROR([bad value ${enableval} for --enable-profiling]) ;;
esac],[profiling=false])
AM_CONDITIONAL([PROFILING], [test x$profiling = xtrue])

# Automake conditional for platform specific builds
AM_CONDITIONAL([mingw_build], [expr "x$host_os" : "xmingw"])



################################
# Programs Search and Checking #
################################

# Check for `mkdir'.
AC_PROG_MKDIR_P

# Check for `ln'.
AC_PROG_LN_S

# Check for `grep'.
AC_PROG_GREP

# Check for `sed'.
AC_PROG_SED

# Check for `cc'.
AC_PROG_CC

# GNUlib;
gl_EARLY
gl_INIT





##########
### Check for Allegro library
######

allegro_minver=5.0.9

AS_CASE([$host],

# MinGW
[*-*-mingw*],
  dnl # Joystick in MinGW crashes for Allegro non-debug builds
  dnl [debug_suffix=-debug]

  [ALLEGRO_CFLAGS=""
   ALLEGRO_LIBS="-lallegro${debug_suffix}"
   ALLEGRO_IMAGE_CFLAGS="${ALLEGRO_CFLAGS}"
   ALLEGRO_IMAGE_LIBS="-lallegro_image${debug_suffix} ${ALLEGRO_LIBS}"
   ALLEGRO_AUDIO_CFLAGS="${ALLEGRO_CFLAGS}"
   ALLEGRO_AUDIO_LIBS="-lallegro_audio${debug_suffix} ${ALLEGRO_LIBS}"
   ALLEGRO_ACODEC_CFLAGS="${ALLEGRO_CFLAGS}"
   ALLEGRO_ACODEC_LIBS="-lallegro_acodec${debug_suffix} ${ALLEGRO_AUDIO_LIBS}"
   ALLEGRO_FONT_CFLAGS="${ALLEGRO_CFLAGS}"
   ALLEGRO_FONT_LIBS="-lallegro_font${debug_suffix} ${ALLEGRO_LIBS}"
   ALLEGRO_PRIMITIVES_CFLAGS="${ALLEGRO_CFLAGS}"
   ALLEGRO_PRIMITIVES_LIBS="-lallegro_primitives${debug_suffix} ${ALLEGRO_LIBS}"
   ALLEGRO_DIALOG_CFLAGS="${ALLEGRO_CFLAGS}"
   ALLEGRO_DIALOG_LIBS="-lallegro_dialog${debug_suffix} ${ALLEGRO_LIBS}"
   ALLEGRO_COLOR_CFLAGS="${ALLEGRO_CFLAGS}"
   ALLEGRO_COLOR_LIBS="-lallegro_color${debug_suffix} ${ALLEGRO_LIBS}"
   mingw_ldadd="dnl -lFLAC \
      dnl -lfreetype \
      dnl -lvorbisfile \
      dnl -lvorbis \
      -logg \
      -lz \
      -lgdiplus \
      -luuid \
      -lkernel32 \
      -lwinmm \
      -lpsapi \
      -lopengl32 \
      -lglu32 \
      -luser32 \
      -lcomdlg32 \
      -lgdi32 \
      -lshell32 \
      -lole32 \
      -ladvapi32 \
      -lws2_32 \
      -lshlwapi \
      -lstdc++"],

# Default
 [PKG_CHECK_MODULES([ALLEGRO],
  [allegro${debug_suffix}-5 >= $allegro_minver],,
  [PKG_CHECK_MODULES([ALLEGRO],
    [allegro${debug_suffix}-5.0 >= $allegro_minver],,
    AC_MSG_ERROR([MININIM requires Allegro $allegro_minver (or superior)]))])

  PKG_CHECK_MODULES([ALLEGRO_IMAGE],
   [allegro_image${debug_suffix}-5 >= $allegro_minver],,
   [PKG_CHECK_MODULES([ALLEGRO_IMAGE],
     [allegro_image${debug_suffix}-5.0 >= $allegro_minver],,
     AC_MSG_ERROR([MININIM requires Allegro $allegro_minver (or superior) image addon]))])

  PKG_CHECK_MODULES([ALLEGRO_AUDIO],
   [allegro_audio${debug_suffix}-5 >= $allegro_minver],,
   [PKG_CHECK_MODULES([ALLEGRO_AUDIO],
    [allegro_audio${debug_suffix}-5.0 >= $allegro_minver],,
    AC_MSG_ERROR([MININIM requires Allegro $allegro_minver (or superior) audio addon]))])

  PKG_CHECK_MODULES([ALLEGRO_ACODEC],
   [allegro_acodec${debug_suffix}-5 >= $allegro_minver],,
   [PKG_CHECK_MODULES([ALLEGRO_ACODEC],
    [allegro_acodec${debug_suffix}-5.0 >= $allegro_minver],,
    AC_MSG_ERROR([MININIM requires Allegro $allegro_minver (or superior) audio codecs addon]))])

  PKG_CHECK_MODULES([ALLEGRO_FONT],
   [allegro_font${debug_suffix}-5 >= $allegro_minver],,
   [PKG_CHECK_MODULES([ALLEGRO_FONT],
    [allegro_font${debug_suffix}-5.0 >= $allegro_minver],,
    AC_MSG_ERROR([MININIM requires Allegro $allegro_minver (or superior) font addon]))])

  PKG_CHECK_MODULES([ALLEGRO_PRIMITIVES],
   [allegro_primitives${debug_suffix}-5 >= $allegro_minver],,
   [PKG_CHECK_MODULES([ALLEGRO_PRIMITIVES],
    [allegro_primitives${debug_suffix}-5.0 >= $allegro_minver],,
    AC_MSG_ERROR([MININIM requires Allegro $allegro_minver (or superior) primitives addon]))])

  PKG_CHECK_MODULES([ALLEGRO_MAIN],
   [allegro_main${debug_suffix}-5 >= $allegro_minver],,
   [PKG_CHECK_MODULES([ALLEGRO_MAIN],
    [allegro_main${debug_suffix}-5.0 >= $allegro_minver],,
    AC_MSG_ERROR([MININIM requires Allegro $allegro_minver (or superior) main addon]))])

  PKG_CHECK_MODULES([ALLEGRO_DIALOG],
   [allegro_dialog${debug_suffix}-5 >= $allegro_minver],,
    [PKG_CHECK_MODULES([ALLEGRO_DIALOG],
     [allegro_dialog${debug_suffix}-5.0 >= $allegro_minver],,
     AC_MSG_ERROR([MININIM requires Allegro $allegro_minver (or superior) dialogs addon]))])

  PKG_CHECK_MODULES([ALLEGRO_COLOR],
   [allegro_color${debug_suffix}-5 >= $allegro_minver],,
    [PKG_CHECK_MODULES([ALLEGRO_COLOR],
     [allegro_color${debug_suffix}-5.0 >= $allegro_minver],,
     AC_MSG_ERROR([MININIM requires Allegro $allegro_minver (or superior) colors addon]))])

])

AC_SUBST([ALLEGRO_CFLAGS])
AC_SUBST([ALLEGRO_LIBS])
AC_SUBST([ALLEGRO_IMAGE_CFLAGS])
AC_SUBST([ALLEGRO_IMAGE_LIBS])
AC_SUBST([ALLEGRO_AUDIO_CFLAGS])
AC_SUBST([ALLEGRO_AUDIO_LIBS])
AC_SUBST([ALLEGRO_ACODEC_CFLAGS])
AC_SUBST([ALLEGRO_ACODEC_LIBS])
AC_SUBST([ALLEGRO_FONT_CFLAGS])
AC_SUBST([ALLEGRO_FONT_LIBS])
AC_SUBST([ALLEGRO_PRIMITIVES_CFLAGS])
AC_SUBST([ALLEGRO_PRIMITIVES_LIBS])
AC_SUBST([ALLEGRO_MAIN_CFLAGS])
AC_SUBST([ALLEGRO_MAIN_LIBS])
AC_SUBST([ALLEGRO_DIALOG_CFLAGS])
AC_SUBST([ALLEGRO_DIALOG_LIBS])
AC_SUBST([ALLEGRO_COLOR_CFLAGS])
AC_SUBST([ALLEGRO_COLOR_LIBS])
AC_SUBST([mingw_ldadd])





##########
### Check for Lua
######

AM_COND_IF([mingw_build],
           [LUA_EXEC_PREFIX="/usr/i686-w64-mingw32"
	    LUA_INCLUDE="-I/usr/i686-w64-mingw32/include"
	    LUA_LIB="-l:liblua.a"
	    LUA_PLATFORM="i686-w64-mingw32"
	    LUA_SHORT_VERSION="53"
	    LUA_VERSION="5.3"],
           [LUA_LIB="-llua5.3 -ldl"])

AX_PROG_LUA([5.3], [5.4], [],
  AC_MSG_ERROR([MININIM requires Lua 5.3]))
AX_LUA_HEADERS(, AC_MSG_ERROR([MININIM requires Lua headers]))
AX_LUA_LIBS(, AC_MSG_ERROR([MININIM requires Lua libraries]))
AX_LUA_READLINE(, AC_MSG_ERROR([MININIM requires GNU Readline]))



##########
### Check for programs needed to build documentation.
######

AC_MSG_NOTICE([checking programs needed in order to build documentation])

### Declare `MAKEINFO' as a precious variable.
AC_ARG_VAR([MAKEINFO], [Texinfo `makeinfo' program])

### Check for `makeinfo'.
AC_PATH_PROG([MAKEINFO], [makeinfo])
AM_CONDITIONAL([MAKEINFO], [test x$MAKEINFO != x])

# Check for GNU Texinfo program `makeinfo'.

### Define GNU Texinfo minimal version.
m4_define([TEXINFO_MINVER], 5.1)

# If GNU Texinfo was not found and you are not building from a VCS
# checkout, warn the user about what the lack of that optional
# dependence means.
AS_IF([[test x$MAKEINFO = x]],
  AC_MSG_WARN(
[[GNU Texinfo not found.  It is required at least version ]]TEXINFO_MINVER.
[[You can find it at https://gnu.org/software/texinfo/.]]
[[It will not be possible to rebuild the Info documentation
nor generate any other documentation format.]]))

### If `makeinfo' was found, test whether its version is at least
### TEXINFO_MINVER.
AS_IF([[test x$MAKEINFO != x]],
  # Put `makeinfo' version in variable `texinfo_version'.
  [[texinfo_version=`$MAKEINFO --version |
      $SED -n '1s/^.* \(@<:@@<:@:digit:@:>@.@:>@*\)$/\1/p'`]]

  # If `makeinfo' version is less than TEXINFO_VER, print an error
  # message and abort when building from VCS checkout or warn the user
  # otherwise.
  AS_VERSION_COMPARE([[$texinfo_version]], TEXINFO_MINVER,
    [AC_MSG_WARN(
[[GNU Texinfo is too old.  It is required at least version ]]TEXINFO_MINVER.
[[You can find it at https://gnu.org/software/texinfo/.]]
[[It will not be possible to rebuild the Info documentation
nor generate any other documentation format.]])]))

AM_MISSING_PROG([MAKEINFO], [makeinfo])

### Declare `HELP2MAN' as a precious variable.
AC_ARG_VAR([HELP2MAN], ['help2man' program])

### Check for `help2man'.
AC_PATH_PROG([HELP2MAN], [help2man])
AM_CONDITIONAL([HELP2MAN], [test x$HELP2MAN != x])

# Check for GNU help2man existence.  If not building from a VCS
# checkout, it is not required.
AS_IF([[test x$HELP2MAN = x]],
  AC_MSG_WARN(
[[GNU help2man not found.  You can get it at https://gnu.org/software/help2man/.]]))

# Check for GNU help2man existence.  It should only be used if not
# cross-compiling.
AS_IF([[test $cross_compiling = no]],
  [AM_MISSING_PROG([HELP2MAN], [help2man])], HELP2MAN=:)

# rsync is needed to fetch PO files from 'translationproject.org'.
AM_MISSING_PROG([RSYNC], [rsync])





####################
# Meta-information #
####################

AC_MSG_NOTICE([[Checking meta-information]])

dnl AC_MSG_CHECKING([available translations])
dnl AC_SUBST([LINGUAS], $(echo $(ls $srcdir/po/*.po | $GREP -v quot |
dnl   $SED 's,^.*po/\(.*\)\.po$,\1,')))
dnl AC_MSG_RESULT([$LINGUAS])

AS_IF([test x$VCS = xy],

  AC_MSG_CHECKING([release date])
  AC_SUBST([RELEASE_DATE], $(LINGUA=C LANG=C date '+%B %d, %Y'))
  AC_MSG_RESULT([$RELEASE_DATE])

  AC_MSG_CHECKING([Autoconf version])
  AC_SUBST([AUTOCONF_VERSION], m4_defn([AC_AUTOCONF_VERSION]))
  AC_MSG_RESULT([$AUTOCONF_VERSION])

  AC_MSG_CHECKING([Automake version])
  AC_SUBST([AUTOMAKE_VERSION], `automake --version |
    $SED -n 's/^automake.* \(@<:@@<:@:digit:@:>@.@:>@*\)$/\1/p'`)
  AC_MSG_RESULT([$AUTOMAKE_VERSION])

  AC_MSG_CHECKING([Texinfo version])
  AC_SUBST([TEXINFO_VERSION], `$MAKEINFO --version |
    $SED -n '1s/^.* \(@<:@@<:@:digit:@:>@.@:>@*\)$/\1/p'`)
  AC_MSG_RESULT([$TEXINFO_VERSION]))





################
# Output Files #
################

# Output files;
dnl po/Makefile.in
dnl po/LINGUAS
dnl doc/Doxyfile
dnl src/Makefile

AC_CONFIG_FILES(
[Makefile
doc/Makefile
doc/man/Makefile
doc/release/release.texi
gnulib/Makefile])

AC_OUTPUT





###########################
# Help about Make targets #
###########################

AS_ECHO("")
AS_BOX([[GNU Make Targets Table]])
AS_ECHO("")
AS_ECHO("")

AS_ECHO(["\
Build:

 all            Compile entire program.

 info, html, pdf, ps, dvi
                Build documentation in the respective format.

 doxygen-doc    Build source-code documentation.
 check          Run test suite.

 tags, ctags
                Build respective tags table file.


Installation:

 install-exec     Install architecture-dependent files.
 install-data     Install architecture-independent files.
 install          Same as \`install-exec' followed by \`install-data'.
 install-strip    Same as \`install' but strip executables.

 install-{info,html,pdf,ps,dvi}
                  Install documentation in the respective format.

 install-man      Install man pages.
 installdirs      Create the installation directory tree.
 installcheck     Perform installation tests.
 uninstall        Delete installed files.


Cleaning:

 mostlyclean         Delete files that \`make' built and usually
                      one would like to rebuild.
 clean               Same as \`mostlyclean' but delete also
                      any file \`make' built but was ignored by
                      \`mostlyclean'.
 distclean           Same as \`clean' but delete also files
                      generated by the configuration script.
 maintainer-clean    Same as \`distclean' but delete also
                      files maintainers built.


Distribution:

 distdir      Create distribution directory.
 dist         Same as \`distdir' but package the distribution
               directory into a tarball.
 distcheck    Same as \`dist' but also check generated
               tarball for correctness.


Maintenance:

 These can only be made from VCS checkouts.

 homepage     Generate package's homepage.
 distgnu      Same as \`distcheck' but also sign and upload the
               resulting tarball to the GNU ftp site.
 announcegnu  Send release announcement to 'info-gnu@gnu.org',
               '$PACKAGE_MAILING_LIST' and
               'coordinator@translationproject.org'
 fetchpo      Fetch the latest PO translation files from the
               Translation Project.
"])
