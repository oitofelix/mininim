/*
  spikes.c -- spikes module;

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

#include "kernel/audio.h"
#include "physics.h"
#include "room.h"
#include "level.h"
#include "kid.h"
#include "spikes.h"

ALLEGRO_BITMAP *spikes_left_01, *spikes_right_01,
  *spikes_left_02, *spikes_right_02, *spikes_left_03, *spikes_right_03,
  *spikes_left_04, *spikes_right_04, *spikes_left_05, *spikes_right_05,
  *spikes_01_fg, *spikes_02_fg, *spikes_03_fg, *spikes_04_fg,
  *spikes_05_fg;

ALLEGRO_SAMPLE *spikes_sound;

static int current_room = -1;

static struct spikes {
  int i, wait, fg;
} spikes[FLOORS][PLACES + 1];

void
load_vdungeon_spikes (void)
{
  /* bitmaps */
  spikes_left_01 = load_bitmap (VDUNGEON_SPIKES_LEFT_01);
  spikes_right_01 = load_bitmap (VDUNGEON_SPIKES_RIGHT_01);
  spikes_left_02 = load_bitmap (VDUNGEON_SPIKES_LEFT_02);
  spikes_right_02 = load_bitmap (VDUNGEON_SPIKES_RIGHT_02);
  spikes_left_03 = load_bitmap (VDUNGEON_SPIKES_LEFT_03);
  spikes_right_03 = load_bitmap (VDUNGEON_SPIKES_RIGHT_03);
  spikes_left_04 = load_bitmap (VDUNGEON_SPIKES_LEFT_04);
  spikes_right_04 = load_bitmap (VDUNGEON_SPIKES_RIGHT_04);
  spikes_left_05 = load_bitmap (VDUNGEON_SPIKES_LEFT_05);
  spikes_right_05 = load_bitmap (VDUNGEON_SPIKES_RIGHT_05);
  spikes_01_fg = load_bitmap (VDUNGEON_SPIKES_01_FG);
  spikes_02_fg = load_bitmap (VDUNGEON_SPIKES_02_FG);
  spikes_03_fg = load_bitmap (VDUNGEON_SPIKES_03_FG);
  spikes_04_fg = load_bitmap (VDUNGEON_SPIKES_04_FG);
  spikes_05_fg = load_bitmap (VDUNGEON_SPIKES_05_FG);

  /* sounds */
  spikes_sound = load_sample (SPIKES_SOUND);
}

void
unload_spikes (void)
{
  /* bitmaps */
  al_destroy_bitmap (spikes_left_01);
  al_destroy_bitmap (spikes_right_01);
  al_destroy_bitmap (spikes_left_02);
  al_destroy_bitmap (spikes_right_02);
  al_destroy_bitmap (spikes_left_03);
  al_destroy_bitmap (spikes_right_03);
  al_destroy_bitmap (spikes_left_04);
  al_destroy_bitmap (spikes_right_04);
  al_destroy_bitmap (spikes_left_05);
  al_destroy_bitmap (spikes_right_05);
  al_destroy_bitmap (spikes_01_fg);
  al_destroy_bitmap (spikes_02_fg);
  al_destroy_bitmap (spikes_03_fg);
  al_destroy_bitmap (spikes_04_fg);
  al_destroy_bitmap (spikes_05_fg);

  /* sounds */
  al_destroy_sample (spikes_sound);
}

bool
should_spikes_raise_for_pos (struct pos p, struct pos pk)
{
  return peq (pk, p) || peq (pk, prel (p, -1, 0))
    || (peq (pk, prel (p, -2, 0))
        && construct (prel (p, -1, 0)).fg != WALL);
}

bool
should_spikes_raise (struct pos p)
{
  struct pos pkf = pos (coord_mf (kid));
  struct pos pkb = pos (coord_mba (kid));

  return should_spikes_raise_for_pos (p, pkf)
    || should_spikes_raise_for_pos (p, pkb);
}

void
draw_spikes (void)
{
  struct pos p;
  p.room = room_view;

  /* reset the spikes state when entering a new room */
  if (current_room != room_view) {
    current_room = room_view;
    for (p.floor = FLOORS - 1; p.floor >= 0; p.floor--)
      for (p.place = -1; p.place < PLACES; p.place++)
        spikes [p.floor][p.place + 1].i = 0;
  }

  struct spikes *s;

  for (p.floor = FLOORS - 1; p.floor >= 0; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++)
      switch (construct (p).fg) {
      case SPIKES_FLOOR:
        s = &spikes [p.floor][p.place + 1];
        switch (s->i) {
        case 0: if (should_spikes_raise (p)) {
            draw_spikes_01 (screen, p);
            play_sample (spikes_sound);
            s->i++;
            s->wait = 12;
            s->fg = 1;
          } else s->fg = 0;
          break;
        case 1: draw_spikes_02 (screen, p);
          s->i++; s->fg = 2; break;
        case 2: draw_spikes_03 (screen, p);
          s->i++; s->fg = 3; break;
        case 3: draw_spikes_04 (screen, p);
          s->i++; s->fg = 4; break;
        case 4: if (! should_spikes_raise (p)) {
            if (s->wait-- == 0) {
              draw_spikes_03 (screen, p);
              s->i++;
              s->fg = 3;
            } else {
              draw_spikes_05 (screen, p);
              s->fg = 5;
            }
          } else {
            draw_spikes_05 (screen, p);
            s->fg = 5;
          }
          break;
        case 5: draw_spikes_02 (screen, p);
          s->i++; s->fg = 2; break;
        case 6: draw_spikes_01 (screen, p);
          s->i = 0; s->fg = 1; break;
        }
        draw_construct_left (screen, prel (p, 0, +1));
        break;
      default:
        break;
      }
}

void
draw_spikes_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  switch (spikes [p.floor][p.place + 1].fg) {
  case 0: break;
  case 1: draw_spikes_01_fg (bitmap, p); break;
  case 2: draw_spikes_02_fg (bitmap, p); break;
  case 3: draw_spikes_03_fg (bitmap, p); break;
  case 4: draw_spikes_04_fg (bitmap, p); break;
  case 5: draw_spikes_05_fg (bitmap, p); break;
  }
}

void
draw_spikes_01 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_spikes_left_01 (bitmap, p);
  draw_spikes_right_01 (bitmap, p);
}

void
draw_spikes_left_01 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (spikes_left_01, bitmap, spikes_left_01_coord (p), 0);
}

void
draw_spikes_right_01 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (spikes_right_01, bitmap, spikes_right_01_coord (p), 0);
}

void
draw_spikes_01_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (spikes_01_fg, bitmap, spikes_01_fg_coord (p), 0);
}

struct coord
spikes_left_01_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 51;
  c.room = p.room;
  return c;
}

struct coord
spikes_right_01_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 53;
  c.room = p.room;
  return c;
}

struct coord
spikes_01_fg_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 56;
  c.room = p.room;
  return c;
}

void
draw_spikes_02 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_spikes_left_02 (bitmap, p);
  draw_spikes_right_02 (bitmap, p);
}

void
draw_spikes_left_02 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (spikes_left_02, bitmap, spikes_left_02_coord (p), 0);
}

void
draw_spikes_right_02 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (spikes_right_02, bitmap, spikes_right_02_coord (p), 0);
}

void
draw_spikes_02_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (spikes_02_fg, bitmap, spikes_02_fg_coord (p), 0);
}

struct coord
spikes_left_02_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 41;
  c.room = p.room;
  return c;
}

struct coord
spikes_right_02_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 41;
  c.room = p.room;
  return c;
}

struct coord
spikes_02_fg_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 48;
  c.room = p.room;
  return c;
}

void
draw_spikes_03 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_spikes_left_03 (bitmap, p);
  draw_spikes_right_03 (bitmap, p);
}

void
draw_spikes_left_03 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (spikes_left_03, bitmap, spikes_left_03_coord (p), 0);
}

void
draw_spikes_right_03 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (spikes_right_03, bitmap, spikes_right_03_coord (p), 0);
}

void
draw_spikes_03_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (spikes_03_fg, bitmap, spikes_03_fg_coord (p), 0);
}

struct coord
spikes_left_03_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 34;
  c.room = p.room;
  return c;
}

struct coord
spikes_right_03_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 41;
  c.room = p.room;
  return c;
}

struct coord
spikes_03_fg_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 37;
  c.room = p.room;
  return c;
}

void
draw_spikes_04 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_spikes_left_04 (bitmap, p);
  draw_spikes_right_04 (bitmap, p);
}

void
draw_spikes_left_04 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (spikes_left_04, bitmap, spikes_left_04_coord (p), 0);
}

void
draw_spikes_right_04 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (spikes_right_04, bitmap, spikes_right_04_coord (p), 0);
}

void
draw_spikes_04_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (spikes_04_fg, bitmap, spikes_04_fg_coord (p), 0);
}

struct coord
spikes_left_04_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 35;
  c.room = p.room;
  return c;
}

struct coord
spikes_right_04_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 36;
  c.room = p.room;
  return c;
}

struct coord
spikes_04_fg_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 38;
  c.room = p.room;
  return c;
}

void
draw_spikes_05 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_spikes_left_05 (bitmap, p);
  draw_spikes_right_05 (bitmap, p);
}

void
draw_spikes_left_05 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (spikes_left_05, bitmap, spikes_left_05_coord (p), 0);
}

void
draw_spikes_right_05 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (spikes_right_05, bitmap, spikes_right_05_coord (p), 0);
}

void
draw_spikes_05_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (spikes_05_fg, bitmap, spikes_05_fg_coord (p), 0);
}

struct coord
spikes_left_05_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 35;
  c.room = p.room;
  return c;
}

struct coord
spikes_right_05_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 37;
  c.room = p.room;
  return c;
}

struct coord
spikes_05_fg_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 40;
  c.room = p.room;
  return c;
}
