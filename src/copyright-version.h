/*
  copyright-version.h -- Copyright and version strings;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MININIM_COPYRIGHT_VERSION_H
#define MININIM_COPYRIGHT_VERSION_H

#define COPYRIGHT_MARK "Copyright (C)"
#define COPYRIGHT_YEARS "2015--" CURRENT_YEAR

#if WINDOWS_EDITION
# define COPYRIGHT_HOLDER "Bruno Felix Rezende Ribeiro"
#else
# define COPYRIGHT_HOLDER "Bruno Félix Rezende Ribeiro"
#endif

#if GNU_EDITION
# define EDITION_NAME "GNU Edition"
#elif WINDOWS_EDITION
# define EDITION_NAME "Windows Edition"
#elif MACOSX_EDITION
# define EDITION_NAME "macOS Edition"
#else
# error "Unknown Edition"
#endif

#define LICENSING_MESSAGE "MININIM is free software under GPLv3+.\n"	\
  "You are free to change and redistribute it.\n"			\
  "There is NO WARRANTY, to the extent permitted by law."

#define FUNDING_MESSAGE "Please, support MININIM development!\n" \
  "http://oitofelix.github.io/funding.html"

#define FORUM_URL "http://forum.princed.org/"

struct allegro_version
{
  int major, minor, revision, release;
};

struct allegro_version get_allegro_version ();
const char *copyright_version_info ();

#endif	/* MININIM_COPYRIGHT_VERSION_H */
