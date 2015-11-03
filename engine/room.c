/*
  room.c -- room module;

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
#include <allegro5/allegro.h>

#include "prince.h"
#include "kernel/video.h"
#include "kernel/random.h"
#include "physics.h"
#include "level.h"
#include "kid.h"
#include "floor.h"
#include "pillar.h"
#include "wall.h"
#include "room.h"


ALLEGRO_BITMAP *bricks_01, *bricks_02, *bricks_03, *bricks_04,
  *window, *torch;

/* room bitmap */
ALLEGRO_BITMAP *room_bg;

/* current room */
static unsigned int current_room = -1;

void
load_room (void)
{
  room_bg = create_bitmap (ORIGINAL_WIDTH, ORIGINAL_HEIGHT);

  switch (level->type) {
  case DUNGEON:
    switch (video_mode) {
    case VGA:
      load_vdungeon_floor ();
      load_vdungeon_wall ();
      load_vdungeon_pillar ();
      bricks_01 = load_bitmap (VDUNGEON_BRICKS_01);
      bricks_02 = load_bitmap (VDUNGEON_BRICKS_02);
      bricks_03 = load_bitmap (VDUNGEON_BRICKS_03);
      bricks_04 = load_bitmap (VDUNGEON_BRICKS_04);
      torch = load_bitmap (VDUNGEON_TORCH);
      window = load_bitmap (VDUNGEON_WINDOW);
      break;
    default:
      error (-1, 0, "%s: unknown video mode (%u)", __func__, video_mode);
    }
    break;
  case PALACE:
    break;
  default:
    error (-1, 0, "%s: unknown level type (%i)", __func__, level->type);
  }
}

void
unload_room (void)
{
  unload_floor ();
  unload_wall ();
  unload_pillar ();

  /* bitmaps */
  al_destroy_bitmap (bricks_01);
  al_destroy_bitmap (bricks_02);
  al_destroy_bitmap (bricks_03);
  al_destroy_bitmap (bricks_04);
  al_destroy_bitmap (torch);
  al_destroy_bitmap (window);
}

void
draw_room (int room)
{
  if (room != current_room) {
    current_room = room;
    draw_room_bg ();
  }
  draw_bitmap (room_bg, screen, 0, 0, 0);
}


void
draw_room_bg (void)
{
  struct pos p;
  p.room = current_room;

  clear_bitmap (room_bg, BLACK);

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++)
      draw_construct (room_bg, p);
}

void
draw_construct (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_construct_fg (bitmap, p);
  draw_construct_bg (bitmap, p);
}

void
draw_construct_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  switch (construct (p).fg) {
  case NO_FLOOR:
    break;
  case FLOOR:
    draw_floor (bitmap, p);
    break;
  case BROKEN_FLOOR:
    draw_broken_floor (bitmap, p);
    break;
  case LOOSE_FLOOR:
    draw_floor (bitmap, p);
    break;
  case PILLAR:
    draw_pillar (bitmap, p);
    break;
  case WALL:
    draw_wall (bitmap, p);
    break;
  default:
    error (-1, 0, "%s: unknown foreground (%u)",
           __func__, construct (p).fg);
  }
}

void
draw_construct_bg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  switch (construct (p).bg) {
  case NO_BG:
    break;
  case BRICKS_01:
    draw_bricks_01 (bitmap, p);
    break;
  case BRICKS_02:
    draw_bricks_02 (bitmap, p);
    break;
  case BRICKS_03:
    draw_bricks_03 (bitmap, p);
    break;
  case BRICKS_04:
    draw_bricks_04 (bitmap, p);
    break;
  case TORCH:
    draw_torch (bitmap, p);
    break;
  case WINDOW:
    draw_window (bitmap, p);
    break;
  default:
    error (-1, 0, "%s: unknown background (%u)",
           __func__, construct (p).bg);
  }
}

void
draw_construct_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  switch (construct (p).fg) {
  case NO_FLOOR:
    break;
  case FLOOR:
    draw_floor_left (bitmap, p);
    break;
  case BROKEN_FLOOR:
    draw_broken_floor_left (bitmap, p);
    break;
  case LOOSE_FLOOR:
    draw_floor_left (bitmap, p);
    break;
  case PILLAR:
    draw_pillar_left (bitmap, p);
    break;
  case WALL:
    draw_wall (bitmap, p);
    break;
  default:
    error (-1, 0, "%s: unknown foreground (%u)",
           __func__, construct (p).fg);
  }
}

void
draw_construct_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  switch (construct (p).fg) {
  case NO_FLOOR:
    break;
  case FLOOR:
    draw_floor_right (bitmap, p);
    break;
  case BROKEN_FLOOR:
    draw_broken_floor_right (bitmap, p);
    break;
  case LOOSE_FLOOR:
    draw_floor_right (bitmap, p);
    break;
  case PILLAR:
    draw_pillar_right (bitmap, p);
    break;
  case WALL:
    draw_wall (bitmap, p);
    break;
  default:
    error (-1, 0, "%s: unknown foreground (%u)",
           __func__, construct (p).fg);
  }
}

void
draw_no_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  struct xy xy = floor_base_xy (p);
  int w = al_get_bitmap_width (floor_normal_base);
  int h = al_get_bitmap_height (floor_normal_base);

  al_set_target_bitmap (bitmap);
  al_set_clipping_rectangle (xy.x, xy.y, w, h);
  al_clear_to_color (BLACK);
  al_reset_clipping_rectangle ();
}

void
draw_no_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  struct xy xy = floor_left_xy (p);
  int wl = al_get_bitmap_width (floor_normal_left);
  int wr = al_get_bitmap_width (floor_normal_right);
  int w = wl + wr;
  int hl = al_get_bitmap_height (floor_normal_left);
  int hr = al_get_bitmap_height (floor_normal_right);
  int hb = al_get_bitmap_height (floor_normal_base);
  int h = max (hl, hr) + hb;

  al_set_target_bitmap (bitmap);
  al_set_clipping_rectangle (xy.x, xy.y, w, h);
  al_clear_to_color (BLACK);
  al_reset_clipping_rectangle ();

  draw_construct_right (bitmap, pos_rel (p, 0, -1));
  draw_construct_right (bitmap, pos_rel (p, +1, -1));
  draw_construct_left (bitmap, pos_rel (p, 0, +1));
}

void
draw_bricks_01 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (bricks_01, bitmap, bricks_xy (p) , 0);
}

void
draw_bricks_02 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (bricks_02, bitmap, bricks_xy (p) , 0);
}

void
draw_bricks_03 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (bricks_03, bitmap, bricks_xy (p) , 0);
}

void
draw_bricks_04 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (bricks_04, bitmap, bricks_xy (p) , 0);
}

struct xy
bricks_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * (p.place + 1);
  xy.y = PLACE_HEIGHT * p.floor + 15;
  return xy;
}

void
draw_torch (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (torch, bitmap, torch_xy (p), 0);
}

struct xy
torch_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * (p.place + 1);
  xy.y = PLACE_HEIGHT * p.floor + 22;
  return xy;
}

void
draw_window (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (window, bitmap, window_xy (p), 0);
}

struct xy
window_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * (p.place + 1);
  xy.y = PLACE_HEIGHT * p.floor + 5;
  return xy;
}

void
draw_room_anim_fg (struct anim a)
{
  draw_room_fg (room_pos_bl (a));
  draw_room_fg (room_pos_br (a));
  draw_room_fg (room_pos_mid (a));
  draw_room_fg (room_pos_tl (a));
  draw_room_fg (room_pos_tr (a));
}

void
draw_room_fg (struct pos p)
{
  switch (construct (p).fg) {
  case NO_FLOOR:
    break;
  case FLOOR:
    draw_floor_fg (screen, p);
    break;
  case BROKEN_FLOOR:
    draw_broken_floor_fg (screen, p);
    break;
  case LOOSE_FLOOR:
    draw_floor_fg (screen, p);
    break;
  case PILLAR:
    draw_pillar_fg (screen, p);
    break;
  case WALL:
    draw_wall_fg (screen, p);
    break;
  default:
    error (-1, 0, "%s: unknown foreground (%u)",
           __func__, construct (p).fg);
  }
}

struct pos
room_pos_bl (struct anim a)
{
  int h = al_get_bitmap_height (a.frame);
  return room_pos_xy (a.room, a.x, a.y + h - 1);
}

struct pos
room_pos_br (struct anim a)
{
  int w = al_get_bitmap_width (a.frame);
  int h = al_get_bitmap_height (a.frame);
  return room_pos_xy (a.room, a.x + w - 1, a.y + h - 1);
}

struct pos
room_pos_mid (struct anim a)
{
  int w = al_get_bitmap_width (a.frame);
  int h = al_get_bitmap_height (a.frame);
  return room_pos_xy (a.room, a.x + w / 2, a.y + h / 2);
}

struct pos
room_pos_tl (struct anim a)
{
  return room_pos_xy (a.room, a.x, a.y);
}

struct pos
room_pos_tr (struct anim a)
{
  int w = al_get_bitmap_width (a.frame);
  return room_pos_xy (a.room, a.x + w - 1, a.y);
}

struct pos
room_pos_xy (unsigned int room, int x, int y)
{
  struct pos p;

  unsigned int qy = y / PLACE_HEIGHT;
  unsigned int ry = y % PLACE_HEIGHT;

  p.room = room;
  p.place = x / PLACE_WIDTH;
  p.floor = (ry < 3) ? qy - 1 : qy;

  if (x < 0) p.place = -1;
  if (y < 0) p.floor = -1;

  return p;
}
