/*
  dat.c -- dat module;

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
dat_getres (int8_t *dat, uint16_t id, int8_t **offset, int16_t *size)
{
  uint16_t i;

  *offset = NULL;
  *size = 0;

  /* TODO: make endianness independent */
  int8_t *index = dat + * (uint32_t *) dat + 2;
  uint16_t n = * (uint16_t *) (index - 2);
  for (i = 0; i < n; i++)
    if (* (uint16_t *) (index + 8 * i) == id) {
      *offset = (int8_t *) (dat + * (uint32_t *) (index + 8 * i + 2) + 1);
      *size = * (uint16_t *) (index + 8 * i + 6);
    }
}
