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

void
play_dat_level (int number)
{
  next_dat_level (number);
  play_level (&legacy_level);
}

void
next_dat_level (int number)
{
  if (number < 1) number = 14;
  else if (number > 14) number = 1;
  load_dat_level (number);
}

void
load_dat_level (int number)
{
  char *filename;
  xasprintf (&filename, levels_dat_filename);

  int8_t *dat =
    load_resource (filename, (load_resource_f) load_file);

  if (! dat)
    error (-1, 0, "cannot read dat level file %s", filename);

  al_free (filename);

  int8_t *offset;
  int16_t size;
  dat_getres (dat, 2000 + number, &offset, &size);

  if (! offset)
    error (-1, 0, "incorrect format for dat level file %s", filename);

  memcpy (&lv, offset, sizeof (lv));

  interpret_legacy_level (number);
  legacy_level.next_level = next_dat_level;

  al_free (dat);
}
