/*
  loose-floor.c -- loose floor module;

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
#include "prince.h"
#include "kernel/video.h"
#include "kernel/random.h"
#include "kernel/array.h"
#include "anim.h"
#include "physics.h"
#include "kid.h"
#include "room.h"
#include "floor.h"
#include "loose-floor.h"
#include "opener-floor.h"

ALLEGRO_BITMAP *loose_floor_left_01, *loose_floor_right_01, *loose_floor_base_01,
  *loose_floor_left_02, *loose_floor_right_02, *loose_floor_base_02,
  *loose_floor_02;

ALLEGRO_SAMPLE *loose_floor_01_sound, *loose_floor_02_sound, *loose_floor_03_sound,
  *broken_floor_sound;

static struct loose_floor *loose_floor = NULL;
static size_t loose_floor_nmemb = 0;

void
load_vdungeon_loose_floor (void)
{
  loose_floor_left_01 = load_bitmap (VDUNGEON_LOOSE_FLOOR_LEFT_01);
  loose_floor_right_01 = load_bitmap (VDUNGEON_LOOSE_FLOOR_RIGHT_01);
  loose_floor_base_01 = load_bitmap (VDUNGEON_LOOSE_FLOOR_BASE_01);
  loose_floor_left_02 = load_bitmap (VDUNGEON_LOOSE_FLOOR_LEFT_02);
  loose_floor_right_02 = load_bitmap (VDUNGEON_LOOSE_FLOOR_RIGHT_02);
  loose_floor_base_02 = load_bitmap (VDUNGEON_LOOSE_FLOOR_BASE_02);
  /* used for loose floor falling animation */
  loose_floor_02 = create_loose_floor_02_bitmap ();
}

void
unload_loose_floor (void)
{
  al_destroy_bitmap (loose_floor_left_01);
  al_destroy_bitmap (loose_floor_right_01);
  al_destroy_bitmap (loose_floor_base_01);
  al_destroy_bitmap (loose_floor_left_02);
  al_destroy_bitmap (loose_floor_right_02);
  al_destroy_bitmap (loose_floor_base_02);
  al_destroy_bitmap (loose_floor_02);
}

void
load_loose_floor_sounds (void)
{
  loose_floor_01_sound = load_sample (LOOSE_FLOOR_01_SOUND);
  loose_floor_02_sound = load_sample (LOOSE_FLOOR_02_SOUND);
  loose_floor_03_sound = load_sample (LOOSE_FLOOR_03_SOUND);
  broken_floor_sound = load_sample (BROKEN_FLOOR_SOUND);
}

void
unload_loose_floor_sounds (void)
{
  al_destroy_sample (loose_floor_01_sound);
  al_destroy_sample (loose_floor_02_sound);
  al_destroy_sample (loose_floor_03_sound);
  al_destroy_sample (broken_floor_sound);
}

ALLEGRO_BITMAP *
create_loose_floor_02_bitmap (void)
{
  int wl = al_get_bitmap_width (loose_floor_left_02);
  int wr = al_get_bitmap_width (loose_floor_right_02);
  int w = wl + wr;
  int hl = al_get_bitmap_height (loose_floor_left_02);
  int hr = al_get_bitmap_height (loose_floor_right_02);
  int hb = al_get_bitmap_height (loose_floor_base_02);
  int h = max (hl, hr) + hb;

  ALLEGRO_BITMAP *bitmap = create_bitmap (w, h);
  clear_bitmap (bitmap, al_map_rgba (0, 0, 0, 0));
  draw_bitmap (loose_floor_base_02, bitmap, 0, 14, 0);
  draw_bitmap (loose_floor_left_02, bitmap, 0, 1, 0);
  draw_bitmap (loose_floor_right_02, bitmap, 32, 0, 0);

  return bitmap;
}

void
register_loose_floor (struct pos p)
{
  struct loose_floor l;

  l.p = p;
  l.i = 0;
  l.resist = LOOSE_FLOOR_RESISTENCE;
  l.action = NO_LOOSE_FLOOR_ACTION;
  l.draw = draw_loose_floor;
  l.draw_left = draw_loose_floor_left;
  l.draw_right = draw_loose_floor_right;

  struct coord c = floor_left_coord (p);
  l.a.frame = loose_floor_02;
  l.a.c.room = p.room;
  l.a.c.x = c.x;
  l.a.c.y = c.y + 3;
  l.a.flip = 0;

  loose_floor =
    add_to_array (&l, 1, loose_floor, &loose_floor_nmemb,
                  loose_floor_nmemb, sizeof (l));
}

struct loose_floor *
loose_floor_at_pos (struct pos p)
{
  size_t i;
  for (i = 0; i < loose_floor_nmemb; i++)
    if (peq (loose_floor[i].p, p)) return &loose_floor[i];
  error (-1, 0, "%s: no loose floor at position has been registered (%i, %i, %i)",
         __func__, p.room, p.floor, p.place);
  return &loose_floor[0];
}

void
release_loose_floor (struct pos p)
{
  struct loose_floor *l = loose_floor_at_pos (p);
  if (l->action != RELEASE_LOOSE_FLOOR
      && l->action != FALL_LOOSE_FLOOR) {
    l->action = RELEASE_LOOSE_FLOOR;
    l->i = 0;
  }
}

void
draw_loose_floors (void)
{
  if (loose_floor_nmemb == 0) return;

  size_t i;
  for (i = loose_floor_nmemb - 1; (int) i >= 0; i--) {
    struct loose_floor *l = &loose_floor[i];
    if (is_pos_visible (l->p))
      switch (l->action) {
      case SHAKE_LOOSE_FLOOR: draw_loose_floor_shake (l); break;
      case RELEASE_LOOSE_FLOOR: draw_loose_floor_release (l, i); break;
      case FALL_LOOSE_FLOOR: draw_loose_floor_fall (l, i); break;
      default: draw_loose_floor (screen, l); break;
      }
  }
}

void
draw_loose_floor_shake (struct loose_floor *l)
{
  switch (l->i) {
  case 0: draw_loose_floor_01 (screen, l);
    play_sample (loose_floor_sample ()); l->i++; break;
  case 1: draw_loose_floor (screen, l); l->i++; break;
  case 2: draw_loose_floor_02 (screen, l);
    play_sample (loose_floor_sample ()); l->i++; break;
  case 3: draw_loose_floor (screen, l);
    l->action = NO_LOOSE_FLOOR_ACTION; l->i = 0; break;
  }
}

void
draw_loose_floor_release (struct loose_floor *l, size_t i)
{
  if (l->resist-- > 0) {
    l->draw (screen, l);
    return;
  }
  switch (l->i) {
  case 0: draw_loose_floor_01 (screen, l);
    play_sample (loose_floor_sample ()); l->i++; break;
  case 1: draw_loose_floor (screen, l); l->i++; break;
  case 2: draw_loose_floor_02 (screen, l);
    play_sample (loose_floor_sample ()); l->i++; break;
  case 3: draw_loose_floor_02 (screen, l); l->i++; break;
  case 4: draw_loose_floor (screen, l); l->i++; break;
  case 5: draw_loose_floor (screen, l); l->i++; break;
  case 6: draw_loose_floor (screen, l); l->i++; break;
  case 7: draw_loose_floor_02 (screen, l);
    play_sample (loose_floor_sample ()); l->i++; break;
  case 8: draw_loose_floor_02 (screen, l); l->i++; break;
  case 9: draw_loose_floor_02 (screen, l); l->i++; break;
  case 10:
    draw_loose_floor_02 (screen, l);
    l->i = 0;
    l->a.id = &l->a;
    l->action = FALL_LOOSE_FLOOR;
    l->resist = LOOSE_FLOOR_RESISTENCE;
    break;
  }
}

void
draw_loose_floor_fall (struct loose_floor *l, size_t i)
{
  if (l->i == 0) set_confg (l->p, NO_FLOOR);

  int speed = 3 * l->i++;
  if (speed > 33) speed = 33;

  struct survey s = survey (l->a, posf);
  struct survey t = survey (next_anim (l->a, l->a.frame, 0, speed), posf);

  if (s.cmbo == NO_FLOOR
      || peq (s.pmbo, t.pmbo)) {
    l->a = next_anim (l->a, loose_floor_02, 0, speed);
    draw_anim (l->a);
    draw_con_left (screen, s.ptr);
    draw_con_left (screen, s.pbr);
  } else { /* the floor hit the ground */
    play_sample (broken_floor_sound);
    if (s.cmbo == OPENER_FLOOR) break_opener_floor (s.pmbo);
    set_confg (s.pmbo, BROKEN_FLOOR);
    remove_from_array (loose_floor, &loose_floor_nmemb, i, 1, sizeof (*l));
    shake_loose_floor_row (s.pmbo);
    draw_con (room_bg, s.pmbo);
    draw_con_left (room_bg, prel (s.pmbo, 0, +1));
    draw_con (screen, s.pmbo);
    draw_con_left (screen, prel (s.pmbo, 0, +1));
  }
}

void
shake_loose_floor_row (struct pos p)
{
  struct loose_floor *l;
  for (p.place = PLACES - 1; p.place >= 0; p.place--)
    if (con (p).fg == LOOSE_FLOOR) {
      l = loose_floor_at_pos (p);
      if (l->action == NO_LOOSE_FLOOR_ACTION) {
        l->action = SHAKE_LOOSE_FLOOR;
        l->i = 0;
      }
    }
}

ALLEGRO_SAMPLE *
loose_floor_sample (void)
{
  switch (prandom (2)) {
  case 0: return loose_floor_01_sound;
  case 1: return loose_floor_02_sound;
  case 2: return loose_floor_03_sound;
  }
  return loose_floor_01_sound;
}

void
draw_loose_floor (ALLEGRO_BITMAP *bitmap, struct loose_floor *l)
{
  l->draw = draw_loose_floor;
  l->draw_left = draw_loose_floor_left;
  l->draw_right = draw_loose_floor_right;

  if (bitmap == room_bg) return;
  draw_floor (bitmap, l->p);
  draw_con_left (bitmap, prel (l->p, 0, +1));
}

void
draw_loose_floor_left (ALLEGRO_BITMAP *bitmap, struct loose_floor *l)
{
  l->draw = draw_loose_floor;
  l->draw_left = draw_loose_floor_left;
  l->draw_right = draw_loose_floor_right;

  if (bitmap == room_bg) return;
  draw_floor_left (bitmap, l->p);
}

void
draw_loose_floor_right (ALLEGRO_BITMAP *bitmap, struct loose_floor *l)
{
  l->draw = draw_loose_floor;
  l->draw_left = draw_loose_floor_left;
  l->draw_right = draw_loose_floor_right;

  if (bitmap == room_bg) return;
  draw_floor_right (bitmap, l->p);
  draw_con_left (bitmap, prel (l->p, 0, +1));
}

void
draw_loose_floor_01 (ALLEGRO_BITMAP *bitmap, struct loose_floor *l)
{
  l->draw = draw_loose_floor_01;
  l->draw_left = draw_loose_floor_01_left;
  l->draw_right = draw_loose_floor_01_right;

  if (bitmap == room_bg) return;
  draw_bitmapc (loose_floor_base_01, bitmap, floor_base_coord (l->p), 0);
  draw_bitmapc (loose_floor_left_01, bitmap, loose_floor_left_coord (l->p), 0);
  draw_bitmapc (loose_floor_right_01, bitmap, loose_floor_right_coord (l->p), 0);

  draw_con_left (bitmap, prel (l->p, 0, +1));
}

void
draw_loose_floor_01_left (ALLEGRO_BITMAP *bitmap, struct loose_floor *l)
{
  l->draw = draw_loose_floor_01;
  l->draw_left = draw_loose_floor_01_left;
  l->draw_right = draw_loose_floor_01_right;

  if (bitmap == room_bg) return;
  draw_bitmapc (loose_floor_base_01, bitmap, floor_base_coord (l->p), 0);
  draw_bitmapc (loose_floor_left_01, bitmap, loose_floor_left_coord (l->p), 0);
}

void
draw_loose_floor_01_right (ALLEGRO_BITMAP *bitmap, struct loose_floor *l)
{
  l->draw = draw_loose_floor_01;
  l->draw_left = draw_loose_floor_01_left;
  l->draw_right = draw_loose_floor_01_right;

  if (bitmap == room_bg) return;
  draw_bitmapc (loose_floor_base_01, bitmap, floor_base_coord (l->p), 0);
  draw_bitmapc (loose_floor_right_01, bitmap, loose_floor_right_coord (l->p), 0);

  draw_con_left (bitmap, prel (l->p, 0, +1));
}

void
draw_loose_floor_02 (ALLEGRO_BITMAP *bitmap, struct loose_floor *l)
{
  l->draw = draw_loose_floor_02;
  l->draw_left = draw_loose_floor_02_left;
  l->draw_right = draw_loose_floor_02_right;

  if (bitmap == room_bg) return;
  draw_bitmapc (loose_floor_base_02, bitmap, floor_base_coord (l->p), 0);
  draw_bitmapc (loose_floor_left_02, bitmap, floor_left_coord (l->p), 0);
  draw_bitmapc (loose_floor_right_02, bitmap, loose_floor_right_coord (l->p), 0);

  draw_con_left (bitmap, prel (l->p, 0, +1));
}

void
draw_loose_floor_02_left (ALLEGRO_BITMAP *bitmap, struct loose_floor *l)
{
  l->draw = draw_loose_floor_02;
  l->draw_left = draw_loose_floor_02_left;
  l->draw_right = draw_loose_floor_02_right;

  if (bitmap == room_bg) return;
  draw_bitmapc (loose_floor_base_02, bitmap, floor_base_coord (l->p), 0);
  draw_bitmapc (loose_floor_left_02, bitmap, floor_left_coord (l->p), 0);
}

void
draw_loose_floor_02_right (ALLEGRO_BITMAP *bitmap, struct loose_floor *l)
{
  l->draw = draw_loose_floor_02;
  l->draw_left = draw_loose_floor_02_left;
  l->draw_right = draw_loose_floor_02_right;

  if (bitmap == room_bg) return;
  draw_bitmapc (loose_floor_base_02, bitmap, floor_base_coord (l->p), 0);
  draw_bitmapc (loose_floor_right_02, bitmap, loose_floor_right_coord (l->p), 0);

  draw_con_left (bitmap, prel (l->p, 0, +1));
}

struct coord
loose_floor_left_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 50 - 1;
  c.room = p.room;
  return c;
}

struct coord
loose_floor_right_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 50 - 1;
  c.room = p.room;
  return c;
}
