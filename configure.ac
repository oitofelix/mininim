dnl
dnl configure.ac -- configuration script;
dnl
dnl Copyright (C) 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>
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
AC_INIT([MININIM], [0.10.0.1],
[oitofelix@gnu.org], [mininim],
[http://oitofelix.freeshell.org/mininim/])
AC_SUBST([PACKAGE_MAILING_LIST], [])
AC_DEFINE_UNQUOTED([PACKAGE_MAILING_LIST], ["$PACKAGE_MAILING_LIST"],
  [Help and support mailing list])
AC_DEFINE(PACKAGE_TRANSLATION_BUGREPORT,
["http://translationproject.org/team/"], [Translation bugs reporting address])
AC_DEFINE(PACKAGE_COPYRIGHT_HOLDER, ["Bruno Fe'lix Rezende Ribeiro"],
[Copyright holder's name])


# GNU Autoconf settings;
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux])
AC_PREREQ(2.60)
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
  [AC_DEFINE([NDEBUG], [1], [Make assert macro do nothing.])])

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

mingw_dir=/c/mingw
allegro_ver=5.0.10
dumb_ver=0.9.3
flac_ver=1.2.1
freetype_ver=2.4.8
vorbis_ver=1.3.2
ogg_ver=1.2.1
zlib_ver=1.2.5
openal_ver=1.14
allegro_minver=5.0.9

AM_CONDITIONAL([mingw_build], [expr "x$build_os" : "xmingw"])

# Check for Allegro library and its addons
AS_CASE([$host],

# MinGW
[*-*-mingw*],
  [ALLEGRO_CFLAGS="-I${mingw_dir}/include"
   ALLEGRO_LIBS="-lallegro-${allegro_ver}-static-mt"
   ALLEGRO_IMAGE_CFLAGS="${ALLEGRO_CFLAGS}"
   ALLEGRO_IMAGE_LIBS="-lallegro_image-${allegro_ver}-static-mt ${ALLEGRO_LIBS}"
   ALLEGRO_AUDIO_CFLAGS="${ALLEGRO_CFLAGS}"
   ALLEGRO_AUDIO_LIBS="-lallegro_audio-${allegro_ver}-static-mt ${ALLEGRO_LIBS}"
   ALLEGRO_ACODEC_CFLAGS="${ALLEGRO_CFLAGS}"
   ALLEGRO_ACODEC_LIBS="-lallegro_acodec-${allegro_ver}-static-mt ${ALLEGRO_AUDIO_LIBS}"
   ALLEGRO_FONT_CFLAGS="${ALLEGRO_CFLAGS}"
   ALLEGRO_FONT_LIBS="-lallegro_font-${allegro_ver}-static-mt ${ALLEGRO_LIBS}"
   ALLEGRO_PRIMITIVES_CFLAGS="${ALLEGRO_CFLAGS}"
   ALLEGRO_PRIMITIVES_LIBS="-lallegro_primitives-${allegro_ver}-static-mt ${ALLEGRO_LIBS}"
   ALLEGRO_DIALOG_CFLAGS="${ALLEGRO_CFLAGS}"
   ALLEGRO_DIALOG_LIBS="-lallegro_dialog-${allegro_ver}-static-mt ${ALLEGRO_LIBS}"
   mingw_ldadd="-ldumb-${dumb_ver}-static-mt \
      -lFLAC-${flac_ver}-static-mt \
      -lfreetype-${freetype_ver}-static-mt \
      -lvorbisfile-${vorbis_ver}-static-mt \
      -lvorbis-${vorbis_ver}-static-mt \
      -logg-${ogg_ver}-static-mt \
      -lzlib-${zlib_ver}-static-mt \
      -lopenal-${openal_ver}-static-mt \
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
  [allegro-5 >= $allegro_minver],,
  [PKG_CHECK_MODULES([ALLEGRO],
    [allegro-5.0 >= $allegro_minver],,
    AC_MSG_ERROR([MININIM requires Allegro $allegro_minver (or superior)]))])

  PKG_CHECK_MODULES([ALLEGRO_IMAGE],
   [allegro_image-5 >= $allegro_minver],,
   [PKG_CHECK_MODULES([ALLEGRO_IMAGE],
     [allegro_image-5.0 >= $allegro_minver],,
     AC_MSG_ERROR([MININIM requires Allegro $allegro_minver (or superior) image addon]))])

  PKG_CHECK_MODULES([ALLEGRO_AUDIO],
   [allegro_audio-5 >= $allegro_minver],,
   [PKG_CHECK_MODULES([ALLEGRO_AUDIO],
    [allegro_audio-5.0 >= $allegro_minver],,
    AC_MSG_ERROR([MININIM requires Allegro $allegro_minver (or superior) audio addon]))])

  PKG_CHECK_MODULES([ALLEGRO_ACODEC],
   [allegro_acodec-5 >= $allegro_minver],,
   [PKG_CHECK_MODULES([ALLEGRO_ACODEC],
    [allegro_acodec-5.0 >= $allegro_minver],,
    AC_MSG_ERROR([MININIM requires Allegro $allegro_minver (or superior) audio codecs addon]))])

  PKG_CHECK_MODULES([ALLEGRO_FONT],
   [allegro_font-5 >= $allegro_minver],,
   [PKG_CHECK_MODULES([ALLEGRO_FONT],
    [allegro_font-5.0 >= $allegro_minver],,
    AC_MSG_ERROR([MININIM requires Allegro $allegro_minver (or superior) font addon]))])

  PKG_CHECK_MODULES([ALLEGRO_PRIMITIVES],
   [allegro_primitives-5 >= $allegro_minver],,
   [PKG_CHECK_MODULES([ALLEGRO_PRIMITIVES],
    [allegro_primitives-5.0 >= $allegro_minver],,
    AC_MSG_ERROR([MININIM requires Allegro $allegro_minver (or superior) primitives addon]))])

  PKG_CHECK_MODULES([ALLEGRO_DIALOG],
   [allegro_dialog-5 >= $allegro_minver],,
    [PKG_CHECK_MODULES([ALLEGRO_DIALOG],
     [allegro_dialog-5.0 >= $allegro_minver],,
     AC_MSG_ERROR([MININIM requires Allegro $allegro_minver (or superior) dialogs addon]))])])

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
AC_SUBST([ALLEGRO_DIALOG_CFLAGS])
AC_SUBST([ALLEGRO_DIALOG_LIBS])
AC_SUBST([mingw_ldadd])

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
