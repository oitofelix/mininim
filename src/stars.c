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

static struct stars_bitmap {
  ALLEGRO_BITMAP *b;
  struct coord c;
} stars_bitmap[FLOORS + 2][PLACES + 1];

static struct star star[FLOORS + 2][PLACES + 1][STARS];

static enum vm last_vm = -1;

void draw_star (struct star *s, struct stars_bitmap *sb, enum vm vm);
static void draw_stars (ALLEGRO_BITMAP *bitmap, struct star s[],
                        struct stars_bitmap *sb, size_t count, enum vm vm);
static ALLEGRO_COLOR get_star_color (int i, enum vm vm);
static int next_color (int color);
static struct star *star_coord (struct pos *p, int i, struct star *s);
static void redraw_stars_bitmap (struct star s[], struct stars_bitmap *sb,
                                 int count, enum vm vm);

void
load_stars (void)
{
  /* callbacks */
  register_room_callback (compute_stars_position);
}

void
unload_stars (void)
{
  /* callbacks */
  remove_room_callback (compute_stars_position);
}

void
compute_stars_position (int last_room, int room)
{
  struct pos p;
  p.room = room;

  int i, min_x = INT_MAX, min_y = INT_MAX,
    max_x = INT_MIN, max_y = INT_MIN;

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      struct stars_bitmap *sb =
        &stars_bitmap[p.floor + 1][p.place + 1];
      if (sb->b) {
        destroy_bitmap (sb->b);
        sb->b = NULL;
      }
      if (con (&p)->bg != BALCONY) continue;

      for (i = 0; i < STARS; i++) {
        struct star *s = &star[p.floor + 1][p.place + 1][i];
        star_coord (&p, i, s);
        min_x = min_int (min_x, s->x);
        min_y = min_int (min_y, s->y);
        max_x = max_int (max_x, s->x);
        max_y = max_int (max_y, s->y);
        s->color = next_color (s->color);
      }

      sb->b = create_bitmap (max_x - min_x + 1, max_y - min_y + 1);
      clear_bitmap (sb->b, TRANSPARENT_COLOR);
      sb->c.room = room;
      sb->c.x = min_x;
      sb->c.y = min_y;

      redraw_stars_bitmap (star[p.floor + 1][p.place + 1], sb, STARS, vm);
    }
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
draw_stars (ALLEGRO_BITMAP *bitmap, struct star s[], struct stars_bitmap *sb,
            size_t count, enum vm vm)
{
  if (anim_cycle % 4 || game_paused) {
    draw_bitmapc (sb->b, bitmap, &sb->c, 0);
    return;
  }

  int i = prandom (count - 1);
  s[i].color = next_color (s[i].color);
  draw_star (&s[i], sb, vm);
  draw_bitmapc (sb->b, bitmap, &sb->c, 0);
}

void
draw_star (struct star *s, struct stars_bitmap *sb, enum vm vm)
{
  al_lock_bitmap (sb->b, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
  set_target_bitmap (sb->b);
  al_put_pixel (s->x - sb->c.x, s->y - sb->c.y, get_star_color (s->color, vm));
  al_unlock_bitmap (sb->b);
}

static void
redraw_stars_bitmap (struct star s[], struct stars_bitmap *sb,
                     int count, enum vm vm)
{
  int i;
  for (i = 0; i < count; i++) draw_star (&s[i], sb, vm);
}

void
draw_princess_room_stars (ALLEGRO_BITMAP *bitmap, enum vm vm)
{
  int i, min_x = INT_MAX, min_y = INT_MAX,
    max_x = INT_MIN, max_y = INT_MIN;

  static struct star s[6] = {
    {20,97,0}, {16,104,1}, {23,110,0},
    {17,116,1}, {24,120,2}, {18,128,2}};

  static struct stars_bitmap sb = {.b = NULL};

  if (! sb.b) {
    for (i = 0; i < 6; i++) {
      min_x = min_int (min_x, s[i].x);
      min_y = min_int (min_y, s[i].y);
      max_x = max_int (max_x, s[i].x);
      max_y = max_int (max_y, s[i].y);
    }

    sb.b = create_bitmap (max_x - min_x + 1, max_y - min_y + 1);
    clear_bitmap (sb.b, TRANSPARENT_COLOR);
    sb.c.x = min_x;
    sb.c.y = min_y;

    redraw_stars_bitmap (&s[0], &sb, 6, vm);
  }

  if (vm != last_vm) {
    redraw_stars_bitmap (&s[0], &sb, 6, vm);
    last_vm = vm;
  }

  draw_stars (bitmap, s, &sb, 6, vm);
}

void
draw_balcony_stars (ALLEGRO_BITMAP *bitmap, struct pos *p,
                    enum vm vm)
{
  if (con (p)->bg != BALCONY) return;

  if (vm != last_vm) {
    redraw_stars_bitmap (star[p->floor + 1][p->place + 1],
                         &stars_bitmap [p->floor + 1][p->place + 1], STARS, vm);
    last_vm = vm;
  }

  draw_stars (bitmap, star[p->floor + 1][p->place + 1],
              &stars_bitmap [p->floor + 1][p->place + 1], STARS, vm);
}

static struct star *
star_coord (struct pos *p, int i, struct star *s)
{
  s->x = PLACE_WIDTH * p->place + 16 + prandom_pos_uniq (p, 2 * i, 1, 28);
  s->y = PLACE_HEIGHT * p->floor - 3 + prandom_pos_uniq (p, 2 * i + 1, 1, 21);
  return s;
}
