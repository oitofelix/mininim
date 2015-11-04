/*
  floor.c -- floor module;

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

#include <error.h>
#include "kernel/video.h"
#include "physics.h"
#include "kid.h"
#include "pillar.h"
#include "wall.h"
#include "level.h"
#include "room.h"
#include "floor.h"

ALLEGRO_BITMAP *floor_normal_left, *floor_normal_right, *floor_normal_base,
  *floor_broken_left, *floor_broken_right, *floor_broken_front, *floor_loose_left_01,
  *floor_loose_right_01, *floor_loose_base_01, *floor_loose_left_02,
  *floor_loose_right_02, *floor_loose_base_02, *floor_loose_02;

void
load_vdungeon_floor (void)
{
  floor_normal_left = load_bitmap (VDUNGEON_FLOOR_NORMAL_LEFT);
  floor_normal_right = load_bitmap (VDUNGEON_FLOOR_NORMAL_RIGHT);
  floor_normal_base = load_bitmap (VDUNGEON_FLOOR_NORMAL_BASE);
  floor_broken_left = load_bitmap (VDUNGEON_FLOOR_BROKEN_LEFT);
  floor_broken_right = load_bitmap (VDUNGEON_FLOOR_BROKEN_RIGHT);
  floor_broken_front = load_bitmap (VDUNGEON_FLOOR_BROKEN_FRONT);
  floor_loose_left_01 = load_bitmap (VDUNGEON_FLOOR_LOOSE_LEFT_01);
  floor_loose_right_01 = load_bitmap (VDUNGEON_FLOOR_LOOSE_RIGHT_01);
  floor_loose_base_01 = load_bitmap (VDUNGEON_FLOOR_LOOSE_BASE_01);
  floor_loose_left_02 = load_bitmap (VDUNGEON_FLOOR_LOOSE_LEFT_02);
  floor_loose_right_02 = load_bitmap (VDUNGEON_FLOOR_LOOSE_RIGHT_02);
  floor_loose_base_02 = load_bitmap (VDUNGEON_FLOOR_LOOSE_BASE_02);

  /* used for loose floor falling animation */
  floor_loose_02 = create_floor_loose_02_bitmap ();
}

void
unload_floor (void)
{
  /* bitmaps */
  al_destroy_bitmap (floor_normal_left);
  al_destroy_bitmap (floor_normal_right);
  al_destroy_bitmap (floor_normal_base);
  al_destroy_bitmap (floor_broken_left);
  al_destroy_bitmap (floor_broken_right);
  al_destroy_bitmap (floor_broken_front);
  al_destroy_bitmap (floor_loose_left_01);
  al_destroy_bitmap (floor_loose_right_01);
  al_destroy_bitmap (floor_loose_base_01);
  al_destroy_bitmap (floor_loose_left_02);
  al_destroy_bitmap (floor_loose_right_02);
  al_destroy_bitmap (floor_loose_base_02);
  al_destroy_bitmap (floor_loose_02);
}

ALLEGRO_BITMAP *
create_floor_loose_02_bitmap (void)
{
  int wl = al_get_bitmap_width (floor_loose_left_02);
  int wr = al_get_bitmap_width (floor_loose_right_02);
  int w = wl + wr;
  int hl = al_get_bitmap_height (floor_loose_left_02);
  int hr = al_get_bitmap_height (floor_loose_right_02);
  int hb = al_get_bitmap_height (floor_loose_base_02);
  int h = max (hl, hr) + hb;

  ALLEGRO_BITMAP *bitmap = create_bitmap (w, h);
  draw_bitmap (floor_loose_base_02, bitmap, 0, 14, 0);
  draw_bitmap (floor_loose_left_02, bitmap, 0, 1, 0);
  draw_bitmap (floor_loose_right_02, bitmap, 32, 0, 0);

  return bitmap;
}

void
draw_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (floor_normal_base, bitmap, floor_base_xy (p), 0);
  draw_bitmap_xy (floor_normal_left, bitmap, floor_left_xy (p), 0);
  draw_bitmap_xy (floor_normal_right, bitmap, floor_right_xy (p), 0);
}

void
draw_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (floor_normal_base, bitmap, floor_base_xy (p), 0);
  draw_bitmap_xy (floor_normal_left, bitmap, floor_left_xy (p), 0);
}

void
draw_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (floor_normal_base, bitmap, floor_base_xy (p), 0);
  draw_bitmap_xy (floor_normal_right, bitmap, floor_right_xy (p), 0);
}

void
draw_broken_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (floor_normal_base, bitmap, floor_base_xy (p), 0);
  draw_bitmap_xy (floor_broken_left, bitmap, floor_left_xy (p), 0);
  draw_bitmap_xy (floor_broken_right, bitmap, floor_right_xy (p), 0);
}

void
draw_broken_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (floor_normal_base, bitmap, floor_base_xy (p), 0);
  draw_bitmap_xy (floor_broken_left, bitmap, floor_left_xy (p), 0);
}

void
draw_broken_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (floor_normal_base, bitmap, floor_base_xy (p), 0);
  draw_bitmap_xy (floor_broken_right, bitmap, floor_right_xy (p), 0);
}

void
draw_broken_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (floor_broken_front, bitmap,
                  broken_floor_front_xy (p), 0);
}

void
draw_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  /* fix a bug where the floor would be redraw over the last kid
     stabilization frame when turning on the edge */
  if (kid.frame == kid_stabilize_08) return;

  /* don't redraw floors while the kid is on them */
  if ((! is_falling (kid) && ! is_kid_fall () && ! is_kid_vjump ())

      /* don't draw floors over kid's foots when jumping from the
         edge */
      || (is_kid_normal () || is_kid_start_jump () || is_kid_jump ()
          || is_kid_start_couch () || is_kid_couch () || is_kid_stop_couch ())) return;

  draw_floor (bitmap, p);

  /* draw next floor left side to cover right edge of the current
     one */
  draw_construct_left (bitmap, pos_rel (p, 0, +1));
}

struct xy
floor_base_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * p.place;
  xy.y = PLACE_HEIGHT * (p.floor + 1);
  return xy;
}

struct xy
floor_left_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * p.place;
  xy.y = PLACE_HEIGHT * p.floor + 50;
  return xy;
}

struct xy
floor_right_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * (p.place + 1);
  xy.y = PLACE_HEIGHT * p.floor + 50;
  return xy;
}

struct xy
broken_floor_front_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * p.place;
  xy.y = PLACE_HEIGHT * p.floor + 54;
  return xy;
}

static struct loose_floor loose_floor[PLACES * FLOORS];

void
release_loose_floor (struct pos p)
{
  size_t i = 0;

  if (construct (p).fg != LOOSE_FLOOR)
    error (-1, 0, "%s: impossible to release non-loose floor (%u)",
           __func__, construct (p).fg);

  while (loose_floor[i].p.room && i < PLACES * FLOORS) {
    if (is_pos_eq (loose_floor[i].p, p)) return;
    i++;
  }
  if (i == PLACES * FLOORS)
    error (-1, 0, "%s: no free loose floor release slot (%u)",
           __func__, construct (p).fg);

  loose_floor[i].p = p;
  loose_floor[i].i = 0;
}

void
draw_shake_floor (void)
{
  static unsigned int i = 0;
  if (kid.room != room_view
      || (i == 0
          && kid.frame != kid_jump_13
          && (! kid.just_fall || kid.frame != kid_couch_02)
          && kid.frame != kid_vjump_19)) return;

  struct pos p = pos (kid);

  for (p.place = -1; p.place < PLACES; p.place++)
    if (construct (p).fg == LOOSE_FLOOR) {
      draw_no_floor (screen, p);
      switch (i) {
      case 0: draw_loose_floor_01 (screen, p); break;
      case 1: draw_floor (screen, p); break;
      case 2: draw_loose_floor_02 (screen, p); break;
      case 3: draw_floor (screen, p); break;
      }
      i = (i < 3)? i + 1 : 0;
      draw_construct_left (screen, pos_rel (p, 0, +1));
      draw_anim (&kid, kid.frame, 0, 0);
    }
}

static struct loose_floor *cfloor;

void
draw_release_loose_floor (void)
{
  size_t i;
  for (i = 0; i < PLACES * FLOORS; i++)
    if (loose_floor[i].p.room) {
      struct pos p = loose_floor[i].p;
      if (loose_floor[i].i == 0) draw_no_floor (room_bg, p);
      switch (loose_floor[i].i) {
      case 0: draw_loose_floor_01 (screen, p); break;
      case 1: draw_floor (screen, p); break;
      case 2: draw_loose_floor_02 (screen, p); break;
      case 3: draw_loose_floor_02 (screen, p); break;
      case 4: draw_floor (screen, p); break;
      case 5: draw_floor (screen, p); break;
      case 6: draw_floor (screen, p); break;
      case 7: draw_loose_floor_02 (screen, p); break;
      case 8: draw_loose_floor_02 (screen, p); break;
      case 9: draw_loose_floor_02 (screen, p); break;
      case 10:
        draw_loose_floor_02 (screen, p);
        set_construct_fg (p, NO_FLOOR);
        struct xy xy = floor_left_xy (loose_floor[i].p);
        loose_floor[i].a.frame = floor_loose_02;
        loose_floor[i].a.room = loose_floor[i].p.room;
        loose_floor[i].a.x = xy.x;
        loose_floor[i].a.y = xy.y;
        loose_floor[i].a.fall = draw_floor_fall;
        loose_floor[i].a.draw = draw_floor_fall;
      break;
      default:
        cfloor = &loose_floor[i];
        loose_floor[i].a.draw ();
        break;
      }
      if (loose_floor[i].i < 11) {
        loose_floor[i].i++;
        draw_construct_left (screen, pos_rel (p, 0, +1));
        draw_anim (&kid, kid.frame, 0, 0);
      }
    }
}

void
draw_floor_fall (void)
{
  unsigned int speed = 3 * (cfloor->i - 10);
  if (speed < 33) cfloor->i++;
  speed = (speed > 33) ? 33 : speed;

  struct pos p = room_pos_mid (cfloor->a);
  struct pos pn = room_pos_mid (next_anim (cfloor->a, cfloor->a.frame, 0, speed));
  struct pos pk = pos (kid);

  if (construct (p).fg == NO_FLOOR
      || is_pos_eq (p, pn)) {
    draw_anim (&cfloor->a, floor_loose_02, 0, speed);
    /* draw kid above the floor if he is higher */
    if (pk.place == p.place && kid.y < cfloor->a.y - 16)
      draw_anim (&kid, kid.frame, 0, 0);
    if (room_view == p.room) draw_construct_left (screen, pos_rel (p, 0, +1));
  } else {
    set_construct_fg (p, BROKEN_FLOOR);
    cfloor->p.room = 0;
    if (room_view == p.room) {
      draw_construct (room_bg, p);
      draw_construct_left (room_bg, pos_rel (p, 0, +1));
      draw_construct (screen, p);
      draw_construct_left (screen, pos_rel (p, 0, +1));
      draw_anim (&kid, kid.frame, 0, 0);
    }
  }
}

void
draw_loose_floor_01 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (floor_loose_base_01, bitmap, floor_base_xy (p), 0);
  draw_bitmap_xy (floor_loose_left_01, bitmap, floor_loose_left_xy (p), 0);
  draw_bitmap_xy (floor_loose_right_01, bitmap, floor_loose_right_xy (p), 0);
}

void
draw_loose_floor_02 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (floor_loose_base_02, bitmap, floor_base_xy (p), 0);
  draw_bitmap_xy (floor_loose_left_02, bitmap, floor_left_xy (p), 0);
  draw_bitmap_xy (floor_loose_right_02, bitmap, floor_loose_right_xy (p), 0);
}

struct xy
floor_loose_left_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * p.place;
  xy.y = PLACE_HEIGHT * p.floor + 50 - 1;
  return xy;
}

struct xy
floor_loose_right_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * (p.place + 1);
  xy.y = PLACE_HEIGHT * p.floor + 50 - 1;
  return xy;
}
