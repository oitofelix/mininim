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
#include "physics.h"
#include "stars.h"

/* functions */
static void draw_stars (ALLEGRO_BITMAP *bitmap, struct star star[], size_t count,
                        enum vm vm);
static ALLEGRO_COLOR get_star_color (int i, enum vm vm);
static struct star *star_coord (struct pos *p, int i, struct star *s);

static ALLEGRO_COLOR
get_star_color (int i, enum vm vm)
{
  switch (vm) {
  case CGA:
    switch (i) {
    case 0: return C_STAR_COLOR_01;
    case 1: return C_STAR_COLOR_02;
    case 2: return C_STAR_COLOR_03;
    }
  case EGA:
    switch (i) {
    case 0: return E_STAR_COLOR_01;
    case 1: return E_STAR_COLOR_02;
    case 2: return E_STAR_COLOR_03;
    }
  case VGA:
    switch (i) {
    case 0: return V_STAR_COLOR_01;
    case 1: return V_STAR_COLOR_02;
    case 2: return V_STAR_COLOR_03;
    }
  }
  return V_STAR_COLOR_03;
}

static void
draw_stars (ALLEGRO_BITMAP *bitmap, struct star star[], size_t count,
            enum vm vm)
{
  int i;

  for (i = 0; i < count; i++) {
    if (star[i].color >= 0 && star[i].color <= 2
        && prandom (12)) continue;

    switch (star[i].color)
      {
      case 0: case 2: star[i].color = 1; break;
      case 1: default: star[i].color = prandom (1) ? 0 : 2;
      }
  }

  al_set_target_bitmap (bitmap);
  for (i = 0; i < count; i++)
    al_put_pixel (star[i].x, star[i].y,
                  get_star_color (star[i].color, vm));
}

void
draw_princess_room_stars (void)
{
  static struct star star[6] = {
    {20,97,1}, {16,104,2}, {23,110,1},
    {17,116,2}, {24,120,3}, {18,128,3}};

  draw_stars (screen, star, 6, vm);
}

void
draw_balcony_stars (ALLEGRO_BITMAP *bitmap, struct pos *p,
                    enum vm vm)
{
  static struct star star[FLOORS + 2][PLACES + 1][5];

  if (con (p)->bg != BALCONY) return;

  star_coord (p, 0, &star[p->floor + 1][p->place + 1][0]);
  star_coord (p, 1, &star[p->floor + 1][p->place + 1][1]);
  star_coord (p, 2, &star[p->floor + 1][p->place + 1][2]);
  star_coord (p, 3, &star[p->floor + 1][p->place + 1][3]);
  star_coord (p, 4, &star[p->floor + 1][p->place + 1][4]);

  draw_stars (bitmap, star[p->floor + 1][p->place + 1], 5, vm);
}

static struct star *
star_coord (struct pos *p, int i, struct star *s)
{
  s->x = PLACE_WIDTH * p->place + 16 + prandom_pos_uniq (p, 2 * i, 1, 28);
  s->y = PLACE_HEIGHT * p->floor - 3 + prandom_pos_uniq (p, 2 * i + 1, 1, 21);
  return s;
}
