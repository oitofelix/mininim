/*
  copyright.c -- Copyright and version information;

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

#include "mininim.h"

static const char *copyright_version_info_text;

struct allegro_version
get_allegro_version ()
{
  uint32_t allegro_version = al_get_allegro_version ();
  return (struct allegro_version)
    {
      .major = allegro_version >> 24,
      .minor = (allegro_version >> 16) & 255,
      .revision = (allegro_version >> 8) & 255,
      .release = allegro_version & 255
    };
}

const char *
copyright_version_info ()
{
  if (copyright_version_info_text)
    return copyright_version_info_text;
  struct allegro_version allegro_version = get_allegro_version ();
  return copyright_version_info_text = xasprintf
    ("%s"			/* package name */
     " %s "			/* version */
     "(%s)\n\n"			/* edition */
     "Website: %s\n"		/* website */
     "Forum: %s\n\n"		/* forum */
     "%s"			/* mark */
     " %s"			/* years */
     " %s"			/* holder */
     " <%s>\n\n"		/* email */
     "%s\n\n"			/* licensing */
     "%s\n\n"			/* funding */
     "Allegro %i.%i.%i\n"	/* Allegro version */
     "%s\n"			/* Lua version and copyright */
     "IUP %s " IUP_COPYRIGHT "\n", /* IUP version and copyright */
     PACKAGE_NAME,
     PACKAGE_VERSION,
     EDITION_NAME,
     PACKAGE_URL,
     FORUM_URL,
     COPYRIGHT_MARK,
     COPYRIGHT_YEARS,
     COPYRIGHT_HOLDER,
     PACKAGE_BUGREPORT,
     LICENSING_MESSAGE,
     FUNDING_MESSAGE,
     allegro_version.major,
     allegro_version.minor,
     allegro_version.revision,
     LUA_COPYRIGHT,
     IupVersion ());
}
