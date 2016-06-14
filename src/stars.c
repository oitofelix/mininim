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

#include "mininim.h"

static void draw_star (struct stars *stars, int i, enum vm vm);
static void draw_stars (ALLEGRO_BITMAP *bitmap, struct stars *stars, enum vm vm);
static ALLEGRO_COLOR get_star_color (int i, enum vm vm);
static int next_color (int color);
static struct star *star_coord (struct pos *p, int i, struct star *s);
static void redraw_stars_bitmap (struct stars *stars, enum vm vm);

void
load_stars (void)
{
  /* nothing */
}

void
unload_stars (void)
{
  /* nothing */
}

void
generate_stars_for_pos (struct pos *p)
{
  struct pos np; npos (p, &np);
  int x, y;
  if (! mr_coord (np.room, -1, &x, &y)) return;

  struct stars *stars = &mr.cell[x][y].stars[np.floor][np.place];
  destroy_bitmap (stars->b);
  stars->b = NULL;
  if (stars->s) al_free (stars->s);
  stars->s = NULL;
  stars->count = 0;

  if (con (&np)->bg != BALCONY) return;

  stars->count = 3 + prandom_pos (&np, 5);
  stars->s = xcalloc (stars->count, sizeof (* stars->s));

  int i, min_x = INT_MAX, min_y = INT_MAX,
    max_x = INT_MIN, max_y = INT_MIN;

  for (i = 0; i < stars->count; i++) {
    struct star *s = &stars->s[i];
    star_coord (&np, i, s);
    min_x = min_int (min_x, s->x);
    min_y = min_int (min_y, s->y);
    max_x = max_int (max_x, s->x);
    max_y = max_int (max_y, s->y);
    s->color = next_color (s->color);
  }

  stars->b = create_bitmap (max_x - min_x + 1, max_y - min_y + 1);
  clear_bitmap (stars->b, TRANSPARENT_COLOR);
  new_coord (&stars->c, np.l, np.room, min_x, min_y);

  redraw_stars_bitmap (stars, vm);
}

void
generate_stars_for_room (int room)
{
  struct pos p; new_pos (&p, &global_level, room, -1, -1);
  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++)
      generate_stars_for_pos (&p);
}

void
generate_stars_for_cell (int x, int y)
{
  struct pos p; new_pos (&p, &global_level, mr.cell[x][y].room, -1, -1);
  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++)
      generate_stars_for_pos (&p);
}

void
generate_stars (void)
{
  int x, y;
  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++)
      if (mr.cell[x][y].room) generate_stars_for_cell (x, y);
}

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

static int
next_color (int c)
{
  switch (c) {
  case 0: case 2: return 1;
  case 1: default: return prandom (1) ? 0 : 2;
  }
  return 1;
}

static void
draw_stars (ALLEGRO_BITMAP *bitmap, struct stars *stars, enum vm vm)
{
  if (! stars->count) return;

  if (anim_cycle % 4 || game_paused) {
    draw_bitmapc (stars->b, bitmap, &stars->c, 0);
    return;
  }

  int i = prandom (stars->count - 1);
  stars->s[i].color = next_color (stars->s[i].color);
  draw_star (stars, i, vm);
  draw_bitmapc (stars->b, bitmap, &stars->c, 0);
}

void
draw_star (struct stars *stars, int i, enum vm vm)
{
  al_lock_bitmap (stars->b, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
  set_target_bitmap (stars->b);
  al_put_pixel (stars->s[i].x - stars->c.x, stars->s[i].y - stars->c.y,
                get_star_color (stars->s[i].color, vm));
  al_unlock_bitmap (stars->b);
}

static void
redraw_stars_bitmap (struct stars *stars, enum vm vm)
{
  int i;
  for (i = 0; i < stars->count; i++) draw_star (stars, i, vm);
}

void
draw_princess_room_stars (ALLEGRO_BITMAP *bitmap, enum vm vm)
{
  int i, min_x = INT_MAX, min_y = INT_MAX,
    max_x = INT_MIN, max_y = INT_MIN;

  static struct star s[6] = {
    {20,97,0}, {16,104,1}, {23,110,0},
    {17,116,1}, {24,120,2}, {18,128,2}};

  static struct stars stars = {.b = NULL, .s = s, .count = 6};

  if (! stars.b) {
    for (i = 0; i < 6; i++) {
      min_x = min_int (min_x, stars.s[i].x);
      min_y = min_int (min_y, stars.s[i].y);
      max_x = max_int (max_x, stars.s[i].x);
      max_y = max_int (max_y, stars.s[i].y);
    }

    stars.b = create_bitmap (max_x - min_x + 1, max_y - min_y + 1);
    clear_bitmap (stars.b, TRANSPARENT_COLOR);
    stars.c.x = min_x;
    stars.c.y = min_y;

    redraw_stars_bitmap (&stars, vm);
  }

  if (vm != mr.last.vm) redraw_stars_bitmap (&stars, vm);

  draw_stars (bitmap, &stars, vm);
}

void
draw_balcony_stars (ALLEGRO_BITMAP *bitmap, struct pos *p, enum vm vm)
{
  if (con (p)->bg != BALCONY) return;
  struct pos np; npos (p, &np);

  if (! np.room) return;

  struct stars *stars = &mr.cell[mr.dx][mr.dy].stars[np.floor][np.place];

  if (vm != mr.last.vm) redraw_stars_bitmap (stars, vm);

  draw_stars (bitmap, stars, vm);
}

static struct star *
star_coord (struct pos *p, int i, struct star *s)
{
  s->x = PLACE_WIDTH * p->place + 16 + prandom_pos_uniq (p, 2 * i, 1, 28);
  s->y = PLACE_HEIGHT * p->floor - 3 + prandom_pos_uniq (p, 2 * i + 1, 1, 21);
  return s;
}
