/*
  xerror.c -- error module;

  Copyright (C) 2015, 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

#define _GNU_SOURCE

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include "array.h"

extern char *program_name;

void
xerror (int status, int errnum, const char *format, ...)
{
  va_list ap;
  va_start (ap, format);

  char *_format;

  if (errnum)
    xasprintf (&_format, "%s: %s: %s\n", program_name, format,
               strerror (errnum));
  else
    xasprintf (&_format, "%s: %s\n", program_name, format);

  vfprintf (stderr, _format, ap);

  va_end (ap);

  al_free (_format);

  if (status) exit (status);
}
