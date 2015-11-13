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
#include "loose-floor.h"
#include "opener-floor.h"
#include "pillar.h"
#include "wall.h"
#include "spikes.h"
#include "door.h"
#include "room.h"

ALLEGRO_BITMAP *bricks_01, *bricks_02, *bricks_03, *bricks_04,
  *window, *torch;

/* room bitmap */
ALLEGRO_BITMAP *room_bg;

/* current room */
static int current_room = -1;

void
load_room (void)
{
  room_bg = create_bitmap (ORIGINAL_WIDTH, ORIGINAL_HEIGHT);

  switch (level->type) {
  case DUNGEON:
    switch (video_mode) {
    case VGA:
      load_vdungeon_floor ();
      load_vdungeon_loose_floor ();
      load_vdungeon_spikes ();
      load_vdungeon_wall ();
      load_vdungeon_pillar ();
      load_vdungeon_door ();
      bricks_01 = load_bitmap (VDUNGEON_BRICKS_01);
      bricks_02 = load_bitmap (VDUNGEON_BRICKS_02);
      bricks_03 = load_bitmap (VDUNGEON_BRICKS_03);
      bricks_04 = load_bitmap (VDUNGEON_BRICKS_04);
      torch = load_bitmap (VDUNGEON_TORCH);
      window = load_bitmap (VDUNGEON_WINDOW);
      break;
    default:
      error (-1, 0, "%s: unknown video mode (%i)", __func__, video_mode);
    }
    break;
  case PALACE:
    break;
  default:
    error (-1, 0, "%s: unknown level type (%i)", __func__, level->type);
  }

  load_opener_floor_sounds ();
}

void
unload_room (void)
{
  unload_floor ();
  unload_loose_floor ();
  unload_spikes ();
  unload_wall ();
  unload_pillar ();
  unload_door ();

  /* sounds */
  unload_opener_floor_sounds ();

  /* bitmaps */
  al_destroy_bitmap (room_bg);
  al_destroy_bitmap (bricks_01);
  al_destroy_bitmap (bricks_02);
  al_destroy_bitmap (bricks_03);
  al_destroy_bitmap (bricks_04);
  al_destroy_bitmap (torch);
  al_destroy_bitmap (window);
}

struct coord
nbitmap_coord (struct coord c, int w, int h)
{
  struct coord tl = c;
  struct coord tr = c;
  struct coord bl = c;
  struct coord br = c;

  tr.x = tl.x + w - 1;
  bl.y = tl.y + h - 1;
  br.x = tl.x + w - 1;
  br.y = tl.y + h - 1;

  struct coord ntl = ncoord (tl);
  struct coord ntr = ncoord (tr);
  struct coord nbl = ncoord (bl);
  struct coord nbr = ncoord (br);

  if (ntl.room == room_view) c = ntl;
  else if (ntr.room == room_view) {
    c = ntr;
    c.x = ntr.x - w + 1;
  } else if (nbl.room == room_view) {
    c = nbl;
    c.y = nbl.y - h + 1;
  } else if (nbr.room == room_view) {
    c = nbr;
    c.x = nbr.x - w + 1;
    c.y = nbr.y - h + 1;
  }

  return c;
}

void
draw_bitmapc (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
              struct coord c, int flags)
{
  int w = al_get_bitmap_width (from);
  int h = al_get_bitmap_height (from);

  if (! cutscene) {
    c = nbitmap_coord (c, w, h);
    if ((to == screen || to == room_bg)
        && c.room != room_view) return;
  }

  draw_bitmap (from, to, c.x, c.y, flags);
}

void
draw_bitmap_regionc (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to, float sx, float sy,
                     float sw, float sh, struct coord c, int flags)
{
  if (! cutscene) {
    c = nbitmap_coord (c, sw, sh);
    if ((to == screen || to == room_bg)
        && c.room != room_view) return;
  }

  draw_bitmap_region (from, to, sx, sy, sw, sh, c.x, c.y, flags);
}

void
draw_room (int room)
{
  if (room_view != room) return;

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
  case NO_FLOOR: break;
  case FLOOR: draw_floor (bitmap, p); break;
  case BROKEN_FLOOR: draw_broken_floor (bitmap, p); break;
  case LOOSE_FLOOR: draw_floor (bitmap, p); break;
  case SPIKES_FLOOR: draw_spikes_floor (bitmap, p); break;
  case OPENER_FLOOR: draw_opener_floor (bitmap, p); break;
  case PILLAR: draw_pillar (bitmap, p); break;
  case WALL: draw_wall (bitmap, p); break;
  case DOOR: draw_door (bitmap, p); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, construct (p).fg);
  }
}

void
draw_construct_bg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  switch (construct (p).bg) {
  case NO_BG: break;
  case BRICKS_01: draw_bricks_01 (bitmap, p); break;
  case BRICKS_02: draw_bricks_02 (bitmap, p); break;
  case BRICKS_03: draw_bricks_03 (bitmap, p); break;
  case BRICKS_04: draw_bricks_04 (bitmap, p); break;
  case TORCH: draw_torch (bitmap, p); break;
  case WINDOW: draw_window (bitmap, p); break;
  default:
    error (-1, 0, "%s: unknown background (%i)",
           __func__, construct (p).bg);
  }
}

void
draw_construct_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  switch (construct (p).fg) {
  case NO_FLOOR: break;
  case FLOOR: draw_floor_left (bitmap, p); break;
  case BROKEN_FLOOR: draw_broken_floor_left (bitmap, p); break;
  case LOOSE_FLOOR: draw_floor_left (bitmap, p); break;
  case SPIKES_FLOOR: draw_spikes_floor_left (bitmap, p); break;
  case OPENER_FLOOR: draw_opener_floor_left (bitmap, p); break;
  case PILLAR: draw_pillar_left (bitmap, p); break;
  case WALL: draw_wall (bitmap, p); break;
  case DOOR: draw_door_left (bitmap, p); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, construct (p).fg);
  }
}

void
draw_construct_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  switch (construct (p).fg) {
  case NO_FLOOR: break;
  case FLOOR: draw_floor_right (bitmap, p); break;
  case BROKEN_FLOOR: draw_broken_floor_right (bitmap, p); break;
  case LOOSE_FLOOR: draw_floor_right (bitmap, p); break;
  case SPIKES_FLOOR: draw_spikes_floor_right (bitmap, p); break;
  case OPENER_FLOOR: draw_opener_floor_right (bitmap, p); break;
  case PILLAR: draw_pillar_right (bitmap, p); break;
  case WALL: draw_wall (bitmap, p); break;
  case DOOR: draw_door_right (bitmap, p); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, construct (p).fg);
  }
}

void
draw_no_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  struct coord c = floor_base_coord (p);
  int w = al_get_bitmap_width (normal_floor_base);
  int h = al_get_bitmap_height (normal_floor_base);

  al_set_target_bitmap (bitmap);
  al_set_clipping_rectangle (c.x, c.y, w, h);
  al_clear_to_color (BLACK);
  al_reset_clipping_rectangle ();
}

void
draw_no_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  struct coord c = floor_left_coord (p);
  c.y--; /* take opener floor into account */
  int wl = al_get_bitmap_width (normal_floor_left);
  int wr = al_get_bitmap_width (normal_floor_right);
  int w = wl + wr;
  int hl = al_get_bitmap_height (normal_floor_left);
  int hr = al_get_bitmap_height (normal_floor_right);
  int hb = al_get_bitmap_height (normal_floor_base);
  int h = max (hl, hr) + hb;

  al_set_target_bitmap (bitmap);
  al_set_clipping_rectangle (c.x, c.y, w, h);
  al_clear_to_color (BLACK);
  al_reset_clipping_rectangle ();

  draw_construct_right (bitmap, prel (p, 0, -1));
  draw_construct_right (bitmap, prel (p, +1, -1));
  draw_construct_left (bitmap, prel (p, 0, +1));
}

void
draw_bricks_01 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (bricks_01, bitmap, bricks_coord (p) , 0);
}

void
draw_bricks_02 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (bricks_02, bitmap, bricks_coord (p) , 0);
}

void
draw_bricks_03 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (bricks_03, bitmap, bricks_coord (p) , 0);
}

void
draw_bricks_04 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (bricks_04, bitmap, bricks_coord (p) , 0);
}

struct coord
bricks_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 15;
  c.room = p.room;
  return c;
}

void
draw_torch (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (torch, bitmap, torch_coord (p), 0);
}

struct coord
torch_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 22;
  c.room = p.room;
  return c;
}

void
draw_window (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (window, bitmap, window_coord (p), 0);
}

struct coord
window_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 5;
  c.room = p.room;
  return c;
}

void
draw_room_anim_fg (struct anim a)
{
  draw_room_fg (posf (coord_bl (a)));
  draw_room_fg (posf (coord_br (a)));
  draw_room_fg (posf (coord_m (a)));
  draw_room_fg (posf (coord_tl (a)));
  draw_room_fg (posf (coord_tr (a)));
}

void
draw_room_fg (struct pos p)
{
  draw_room_fg_0 (prel (p, 0, -1));
  draw_room_fg_0 (p);
}

void
draw_room_fg_0 (struct pos p)
{
  switch (construct (p).fg) {
  case NO_FLOOR: break;
  case FLOOR: draw_floor_fg (screen, p); break;
  case BROKEN_FLOOR: draw_broken_floor_fg (screen, p); break;
  case LOOSE_FLOOR: draw_floor_fg (screen, p); break;
  case SPIKES_FLOOR: draw_spikes_fg (screen, p); break;
  case OPENER_FLOOR: draw_floor_fg (screen, p); break;
  case PILLAR: draw_pillar_fg (screen, p); break;
  case WALL: draw_wall_fg (screen, p); break;
  case DOOR: draw_door_fg (screen, p); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, construct (p).fg);
  }
}
