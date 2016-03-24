/*
  plv-level.c -- plv level module;

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

void
play_plv_level (int number)
{
  next_plv_level (number);
  play_level (&legacy_level);
}

void
next_plv_level (int number)
{
  if (number < 1) number = 14;
  else if (number > 14) number = 1;
  load_plv_level (number);
}

void
load_plv_level (int number)
{
  char *filename;
  xasprintf (&filename, "data/plv-levels/%02d.plv", number);

  int8_t *plv =
    load_resource (filename, (load_resource_f) load_file);

  if (! plv)
    error (-1, 0, "cannot read plv level file %s", filename);

  al_free (filename);

  memcpy (&lv, plv + 19, sizeof (lv));

  interpret_legacy_level (number);
  legacy_level.next_level = next_plv_level;

  al_free (plv);
}
