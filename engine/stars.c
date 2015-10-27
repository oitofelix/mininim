/*
  stars.c -- stars module;

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

#include <stddef.h>
#include "kernel/video.h"
#include "kernel/random.h"
#include "stars.h"

/* functions */
static void draw_stars (struct star stars[], size_t count);

static void
draw_stars (struct star stars[], size_t count)
{
  static unsigned int i = 0;

  if (! count) return;

  unsigned int random = prandom_uniq (STARS_RANDOM_SEED + i, count - 1);
  struct star *star = &stars[random];

  switch (star->color)
    {
    case 1:
    case 3:
      star->color = 2;
      break;
    case 2:
    default:
      star->color = prandom (2) + 1;
    }

  size_t j;
  al_set_target_bitmap (screen);
  for (j = 0; j < count; j++) {
      unsigned char rgb = (stars[j].color * 255) / 3;
      al_put_pixel (stars[j].x, stars[j].y, al_map_rgb (rgb, rgb, rgb));
  }

  i++;
}

void
draw_princess_room_stars (void)
{
  static struct star stars[6] = {
    {20,97,1}, {16,104,2}, {23,110,1},
    {17,116,2}, {24,120,3}, {18,128,3}};

  draw_stars (stars, 6);
}
