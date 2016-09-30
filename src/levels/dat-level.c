/*
  dat-level.c -- dat level module;

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

#include "mininim.h"

struct level *
next_dat_level (struct level *l, int n)
{
  if (n < 1) n = 14;
  else if (n > 14) n = 1;
  return load_dat_level (l, n);
}

struct level *
load_dat_level (struct level *l, int n)
{
  char *filename;
  xasprintf (&filename, levels_dat_filename);

  int8_t *dat =
    load_resource (filename, (load_resource_f) load_file);

  if (! dat) {
    error (0, 0, "cannot read dat level file %s", filename);
    return NULL;
  }

  al_free (filename);

  int8_t *offset;
  int16_t size;
  dat_getres (dat, 2000 + n, &offset, &size);

  if (! offset) {
    error (0, 0, "incorrect format for dat level file %s", filename);
    return NULL;
  }

  memcpy (&lv, offset, sizeof (lv));

  interpret_legacy_level (l, n);
  l->next_level = next_dat_level;

  al_free (dat);
  return l;
}
