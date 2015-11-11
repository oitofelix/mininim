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

#include <stdio.h>
#include <error.h>
#include "kernel/video.h"
#include "kernel/audio.h"
#include "kernel/random.h"
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
  *floor_loose_right_02, *floor_loose_base_02, *floor_loose_02,
  *floor_corner_01, *floor_corner_02, *floor_corner_03,
  *floor_spikes_left, *floor_spikes_right;

ALLEGRO_SAMPLE *loose_floor_01, *loose_floor_02, *loose_floor_03, *broken_floor_sound;

static struct loose_floor loose_floor[PLACES * FLOORS];
static struct loose_floor *cfloor;

void
load_vdungeon_floor (void)
{
  /* bitmaps */
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
  floor_corner_01 = load_bitmap (VDUNGEON_FLOOR_CORNER_01);
  floor_corner_02 = load_bitmap (VDUNGEON_FLOOR_CORNER_02);
  floor_corner_03 = load_bitmap (VDUNGEON_FLOOR_CORNER_03);
  floor_spikes_left = load_bitmap (VDUNGEON_FLOOR_SPIKES_LEFT);
  floor_spikes_right = load_bitmap (VDUNGEON_FLOOR_SPIKES_RIGHT);

  /* used for loose floor falling animation */
  floor_loose_02 = create_floor_loose_02_bitmap ();

  /* sounds */
  loose_floor_01 = load_sample (LOOSE_FLOOR_01);
  loose_floor_02 = load_sample (LOOSE_FLOOR_02);
  loose_floor_03 = load_sample (LOOSE_FLOOR_03);
  broken_floor_sound = load_sample (BROKEN_FLOOR_SOUND);
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
  al_destroy_bitmap (floor_corner_01);
  al_destroy_bitmap (floor_corner_02);
  al_destroy_bitmap (floor_corner_03);
  al_destroy_bitmap (floor_spikes_left);
  al_destroy_bitmap (floor_spikes_right);

  /* sounds */
  al_destroy_sample (loose_floor_01);
  al_destroy_sample (loose_floor_02);
  al_destroy_sample (loose_floor_03);
  al_destroy_sample (broken_floor_sound);
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
  clear_bitmap (bitmap, al_map_rgba (0, 0, 0, 0));
  draw_bitmap (floor_loose_base_02, bitmap, 0, 14, 0);
  draw_bitmap (floor_loose_left_02, bitmap, 0, 1, 0);
  draw_bitmap (floor_loose_right_02, bitmap, 32, 0, 0);

  return bitmap;
}

void
draw_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (floor_normal_base, bitmap, floor_base_coord (p), 0);
}

void
draw_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (floor_normal_left, bitmap, floor_left_coord (p), 0);
  draw_bitmapc (floor_normal_right, bitmap, floor_right_coord (p), 0);
}

void
draw_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (floor_normal_left, bitmap, floor_left_coord (p), 0);
}

void
draw_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (floor_normal_right, bitmap, floor_right_coord (p), 0);
}

void
draw_broken_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (floor_broken_left, bitmap, floor_left_coord (p), 0);
  draw_bitmapc (floor_broken_right, bitmap, floor_right_coord (p), 0);
}

void
draw_broken_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (floor_broken_left, bitmap, floor_left_coord (p), 0);
}

void
draw_broken_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (floor_broken_right, bitmap, floor_right_coord (p), 0);
}

void
draw_broken_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (floor_broken_front, bitmap,
                  broken_floor_front_coord (p), 0);
}

void
draw_spikes_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (floor_spikes_left, bitmap, floor_left_coord (p), 0);
  draw_bitmapc (floor_spikes_right, bitmap, floor_right_coord (p), 0);
}

void
draw_spikes_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (floor_spikes_left, bitmap, floor_left_coord (p), 0);
}

void
draw_spikes_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_floor_base (bitmap, p);
  draw_bitmapc (floor_spikes_right, bitmap, floor_right_coord (p), 0);
}

void
draw_floor_corner_01 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (floor_corner_01, bitmap, floor_corner_01_coord (p), 0);
}

void
draw_floor_corner_02 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (floor_corner_02, bitmap, floor_corner_02_coord (p), 0);
}

void
draw_floor_corner_03 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (floor_corner_03, bitmap, floor_corner_03_coord (p), 0);
}

void
draw_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  struct coord c = coord_bb (kid);

  /* if (c.y - 6 <= floor_left_coord (p).y */
  /*     || c.x >= floor_right_coord (p).x) return; */

  if (c.y - 6 <= floor_left_coord (p).y) return;
  /* else if (c.x >= floor_right_coord (p).x) return; */

  if (is_kid_hang () && kid.dir == LEFT) return;
  if (is_kid_climb () && kid.dir == LEFT) return;

  if (is_kid_climb ()) {
      draw_floor_base (bitmap, p);
      if (kid.frame == kid_climb_03
          || kid.frame == kid_climb_09
               || kid.frame == kid_climb_10)
        draw_floor_corner_03 (bitmap, p);
      else if (kid.frame == kid_climb_04
               || kid.frame == kid_climb_06
               || kid.frame == kid_climb_07)
        draw_floor_corner_01 (bitmap, p);
      else if (kid.frame == kid_climb_08
               || kid.frame == kid_climb_05)
        draw_floor_corner_02 (bitmap, p);
  } else {
    draw_floor (bitmap, p);

  /* draw next floor left side to cover right edge of the current
     one */
  draw_construct_left (bitmap, prel (p, 0, +1));
  }
}

struct coord
floor_base_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * (p.floor + 1);
  c.room = p.room;
  return c;
}

struct coord
floor_left_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 50;
  c.room = p.room;
  return c;
}

struct coord
floor_right_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 50;
  c.room = p.room;
  return c;
}

struct coord
broken_floor_front_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 54;
  c.room = p.room;
  return c;
}

struct coord
floor_corner_01_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 53;
  c.room = p.room;
  return c;
}

struct coord
floor_corner_02_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 53;
  c.room = p.room;
  return c;
}

struct coord
floor_corner_03_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 55;
  c.room = p.room;
  return c;
}

void
release_loose_floor (struct pos p)
{
  size_t i = 0;

  for (i = 0; i < PLACES * FLOORS; i++) if (peq (loose_floor[i].p, p)) return;
  for (i = 0; loose_floor[i].p.room && i < PLACES * FLOORS; i++);
  if (i == PLACES * FLOORS)
    error (-1, 0, "%s: no free loose floor release slot (%i)",
           __func__, construct (p).fg);

  loose_floor[i].p = p;
  loose_floor[i].i = 0;
  loose_floor[i].resist = 4;
}

void
draw_shake_floor (void)
{
  static int i = 0;
  if (! is_visible (kid)
      || (i == 0
          && kid.frame != kid_jump_13
          && (! kid.just_fall || kid.frame != kid_couch_02)
          && kid.frame != kid_vjump_19
          && (kid.frame != kid_couch_11
              || kid.draw != draw_kid_misstep))) return;

  struct pos p = pos (coord_tf (kid));

  for (p.place = -1; p.place < PLACES; p.place++)
    if (construct (p).fg == LOOSE_FLOOR) {
      draw_no_floor (screen, p);
      switch (i) {
      case 0: draw_loose_floor_01 (screen, p);
        play_sample (loose_floor_sample ()); break;
      case 1: draw_floor (screen, p);
        play_sample (loose_floor_sample ()); break;
      case 2: draw_loose_floor_02 (screen, p);
        play_sample (loose_floor_sample ()); break;
      case 3: draw_floor (screen, p);
        play_sample (loose_floor_sample ()); break;
      }
      i = (i < 3)? i + 1 : 0;
      draw_construct_left (screen, prel (p, 0, +1));
      redraw_anim (kid);
    }
}

ALLEGRO_SAMPLE *
loose_floor_sample (void)
{
  switch (prandom (2)) {
  case 0: return loose_floor_01;
  case 1: return loose_floor_02;
  case 2: return loose_floor_03;
  }
  return loose_floor_01;
}

void
draw_release_loose_floor (void)
{
  size_t i;
  for (i = 0; i < PLACES * FLOORS; i++)
    if (loose_floor[i].p.room) {
      if (loose_floor[i].resist-- > 0) return;
      struct pos p = loose_floor[i].p;
      if (loose_floor[i].i == 0) draw_no_floor (room_bg, p);
      switch (loose_floor[i].i) {
      case 0: draw_loose_floor_01 (screen, p);
        play_sample (loose_floor_sample ()); break;
      case 1: draw_floor (screen, p);
        play_sample (loose_floor_sample ()); break;
      case 2: draw_loose_floor_02 (screen, p);
        play_sample (loose_floor_sample ()); break;
      case 3: draw_loose_floor_02 (screen, p);
        play_sample (loose_floor_sample ()); break;
      case 4: draw_floor (screen, p);
        play_sample (loose_floor_sample ()); break;
      case 5: draw_floor (screen, p); break;
      case 6: draw_floor (screen, p); break;
      case 7: draw_loose_floor_02 (screen, p);
        play_sample (loose_floor_sample ()); break;
      case 8: draw_loose_floor_02 (screen, p); break;
      case 9:
        set_construct_fg (p, NO_FLOOR);
        draw_loose_floor_02 (screen, p); break;
      case 10:
        draw_loose_floor_02 (screen, p);
        struct coord c = floor_left_coord (loose_floor[i].p);
        loose_floor[i].a.id = NULL;
        loose_floor[i].a.frame = floor_loose_02;
        loose_floor[i].a.c.room = loose_floor[i].p.room;
        loose_floor[i].a.c.x = c.x;
        loose_floor[i].a.c.y = c.y;
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
        draw_construct_left (screen, prel (p, 0, +1));
        redraw_anim (kid);
      }
    }
}

void
draw_floor_fall (void)
{
  int j;
  int speed;

  j = cfloor->i++ - 10;
  speed = 3 * j;
  if (speed > 33) speed = 33;

  struct pos pmt = pos (coord_mt (cfloor->a));
  struct pos p = pos (coord_mbo (cfloor->a));
  struct pos pn =
    pos (coord_mbo (next_anim (cfloor->a, cfloor->a.frame, 0, speed)));
  struct pos pk = pos (coord_mbo (kid));

  if (construct (p).fg == NO_FLOOR
      || peq (p, pn)) {
    draw_anim (&cfloor->a, floor_loose_02, 0, speed);
    if (is_visible (cfloor->a)) {
      /* redraw construct on the floor's right */
      struct pos pc = prel (pmt, 0, +1);
      draw_construct_left (screen, pc);
      /* redraw kid if he's over it */
      if (peq (pc, pk))
        redraw_anim (kid);
    }
  } else { /* the floor hit the ground */
    play_sample (broken_floor_sound);
    set_construct_fg (p, BROKEN_FLOOR);
    cfloor->p.room = 0;
    draw_construct (room_bg, p);
    draw_construct_left (room_bg, prel (p, 0, +1));
    draw_construct (screen, p);
    draw_construct_left (screen, prel (p, 0, +1));
    redraw_anim (kid);
  }
}

void
draw_loose_floor_01 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (floor_loose_base_01, bitmap, floor_base_coord (p), 0);
  draw_bitmapc (floor_loose_left_01, bitmap, floor_loose_left_coord (p), 0);
  draw_bitmapc (floor_loose_right_01, bitmap, floor_loose_right_coord (p), 0);
}

void
draw_loose_floor_02 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (floor_loose_base_02, bitmap, floor_base_coord (p), 0);
  draw_bitmapc (floor_loose_left_02, bitmap, floor_left_coord (p), 0);
  draw_bitmapc (floor_loose_right_02, bitmap, floor_loose_right_coord (p), 0);
}

struct coord
floor_loose_left_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 50 - 1;
  c.room = p.room;
  return c;
}

struct coord
floor_loose_right_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 50 - 1;
  c.room = p.room;
  return c;
}
