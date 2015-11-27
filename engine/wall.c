/*
  wall.c -- wall module;

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
#include "level.h"
#include "kid.h"
#include "room.h"
#include "wall.h"

ALLEGRO_BITMAP *wall_left, *wall_center, *wall_right, *wall_single, *wall_face,
  *wall_face_top, *wall_left_base, *wall_center_base, *wall_right_base,
  *wall_single_base, *wall_gray_block, *wall_wide_divider, *wall_narrow_divider,
  *wall_mark_top_left, *wall_mark_top_right, *wall_mark_bottom_left,
  *wall_mark_bottom_right;

void
load_vdungeon_wall (void)
{
  wall_left = load_bitmap (VDUNGEON_WALL_LEFT);
  wall_center = load_bitmap (VDUNGEON_WALL_CENTER);
  wall_right = load_bitmap (VDUNGEON_WALL_RIGHT);
  wall_single = load_bitmap (VDUNGEON_WALL_SINGLE);
  wall_face = load_bitmap (VDUNGEON_WALL_FACE);
  wall_face_top = load_bitmap (VDUNGEON_WALL_FACE_TOP);
  wall_left_base = load_bitmap (VDUNGEON_WALL_LEFT_BASE);
  wall_center_base = load_bitmap (VDUNGEON_WALL_CENTER_BASE);
  wall_right_base = load_bitmap (VDUNGEON_WALL_RIGHT_BASE);
  wall_single_base = load_bitmap (VDUNGEON_WALL_SINGLE_BASE);
  wall_gray_block = load_bitmap (VDUNGEON_WALL_GRAY_BLOCK);
  wall_wide_divider = load_bitmap (VDUNGEON_WALL_WIDE_DIVIDER);
  wall_narrow_divider = load_bitmap (VDUNGEON_WALL_NARROW_DIVIDER);
  wall_mark_top_left = load_bitmap (VDUNGEON_WALL_MARK_TOP_LEFT);
  wall_mark_top_right = load_bitmap (VDUNGEON_WALL_MARK_TOP_RIGHT);
  wall_mark_bottom_left = load_bitmap (VDUNGEON_WALL_MARK_BOTTOM_LEFT);
  wall_mark_bottom_right = load_bitmap (VDUNGEON_WALL_MARK_BOTTOM_RIGHT);
}

void
unload_wall (void)
{
  /* bitmaps */
  al_destroy_bitmap (wall_left);
  al_destroy_bitmap (wall_center);
  al_destroy_bitmap (wall_right);
  al_destroy_bitmap (wall_single);
  al_destroy_bitmap (wall_face);
  al_destroy_bitmap (wall_face_top);
  al_destroy_bitmap (wall_left_base);
  al_destroy_bitmap (wall_center_base);
  al_destroy_bitmap (wall_right_base);
  al_destroy_bitmap (wall_single_base);
  al_destroy_bitmap (wall_gray_block);
  al_destroy_bitmap (wall_wide_divider);
  al_destroy_bitmap (wall_narrow_divider);
  al_destroy_bitmap (wall_mark_top_left);
  al_destroy_bitmap (wall_mark_top_right);
  al_destroy_bitmap (wall_mark_bottom_left);
  al_destroy_bitmap (wall_mark_bottom_right);
}

void
draw_wall (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  switch (wall_correlation (p)) {
  case SWS: draw_wall_sws (bitmap, p); break;
  case SWW: draw_wall_sww (bitmap, p); break;
  case WWS: draw_wall_wws (bitmap, p); break;
  case WWW: draw_wall_www (bitmap, p); break;
  default:
    error (-1, 0, "%s: unknown wall correlation (%i, %i. %i)",
           __func__, p.room, p.floor, p.place);
  }
  draw_wall_randomization (bitmap, p);
}

void
draw_wall_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  switch (wall_correlation (p)) {
  case SWS: draw_wall_sws_base (bitmap, p); break;
  case SWW: draw_wall_sww_base (bitmap, p); break;
  case WWS: draw_wall_wws_base (bitmap, p); break;
  case WWW: draw_wall_www_base (bitmap, p); break;
  default:
    error (-1, 0, "%s: unknown wall correlation (%i, %i. %i)",
           __func__, p.room, p.floor, p.place);
  }
}

void
draw_wall_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  switch (wall_correlation (p)) {
  case SWS: draw_wall_sws_no_face (bitmap, p); break;
  case SWW: draw_wall_sww (bitmap, p); break;
  case WWS: draw_wall_wws_no_face (bitmap, p); break;
  case WWW: draw_wall_www (bitmap, p); break;
  default:
    error (-1, 0, "%s: unknown wall correlation (%i, %i. %i)",
           __func__, p.room, p.floor, p.place);
  }
  draw_wall_randomization (bitmap, p);
}

void
draw_wall_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_wall (bitmap, p);
  draw_con_left (bitmap, prel (p, 0, +1));
  draw_con_left (bitmap, prel (p, -1, +1));
}

void
draw_wall_sws (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_wall_sws_no_face (bitmap, p);
  draw_bitmapc (wall_face, bitmap, wall_face_coord (p), 0);
  draw_bitmapc (wall_face_top, bitmap, wall_face_top_coord (p), 0);
}

void
draw_wall_sws_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (wall_single_base, bitmap, wall_base_coord (p), 0);
}

void
draw_wall_sws_no_face (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_wall_sws_base (bitmap, p);
  draw_bitmapc (wall_single, bitmap, wall_coord (p), 0);
}

void
draw_wall_sww_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (wall_left_base, bitmap, wall_base_coord (p), 0);
}

void
draw_wall_sww (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_wall_sww_base (bitmap, p);
  draw_bitmapc (wall_left, bitmap, wall_coord (p), 0);
}

void
draw_wall_wws (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_wall_wws_no_face (bitmap, p);
  draw_bitmapc (wall_face, bitmap, wall_face_coord (p), 0);
  draw_bitmapc (wall_face_top, bitmap, wall_face_top_coord (p), 0);
}

void
draw_wall_wws_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (wall_right_base, bitmap, wall_base_coord (p), 0);
}

void
draw_wall_wws_no_face (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_wall_wws_base (bitmap, p);
  draw_bitmapc (wall_right, bitmap, wall_coord (p), 0);
}

void
draw_wall_www_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (wall_center_base, bitmap, wall_base_coord (p), 0);
}

void
draw_wall_www (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_wall_www_base (bitmap, p);
  draw_bitmapc (wall_center, bitmap, wall_coord (p), 0);
}

void
draw_wall_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_wall_left (bitmap, p);
}

struct coord
wall_base_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * (p.floor + 1);
  c.room = p.room;
  return c;
}

struct coord
wall_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 3;
  c.room = p.room;
  return c;
}

struct coord
wall_face_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 3;
  c.room = p.room;
  return c;
}

struct coord
wall_face_top_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor - 9;
  c.room = p.room;
  return c;
}

enum wall_correlation
wall_correlation (struct pos p)
{
  if (con (p)->fg != WALL)
    error (-1, 0, "%s: requested wall correlation on non-wall (%i, %i, %i)",
           __func__, p.room, p.floor, p.place);

  if (crel (p, 0, -1)->fg != WALL
      && crel (p, 0, +1)->fg != WALL) return SWS;
  else if (crel (p, 0, -1)->fg != WALL
           && crel (p, 0, +1)->fg == WALL) return SWW;
  else if (crel (p, 0, -1)->fg == WALL
           && crel (p, 0, +1)->fg != WALL) return WWS;
  else if (crel (p, 0, -1)->fg == WALL
           && crel (p, 0, +1)->fg == WALL) return WWW;
  else
    error (-1, 0, "%s: unknown wall correlation (%i, %i. %i)",
           __func__, p.room, p.floor, p.place);

  return -1;
}

/* wall randomization auxiliary numbers */
static int r0, r1, r2, r3;

/* wall randomization dividers */
static ALLEGRO_BITMAP *wall_divider_00, *wall_divider_01;

void
draw_wall_randomization (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (level->type != DUNGEON && video_mode == VGA) return;

  struct pos np = npos (p);
  seedp (np);
  prandom (1);
  r0 = prandom(1);
  r1 = prandom(4);
  r2 = prandom(1);
  r3 = prandom(4);

  wall_divider_00 = r2 ? wall_narrow_divider : wall_wide_divider;
  wall_divider_01 = r0 ? wall_narrow_divider : wall_wide_divider;

  enum wall_correlation wc = wall_correlation (p);

  switch (wc) {
  case WWW:
    if (prandom (4) == 0) draw_wall_gray_block (bitmap, p);
    draw_wall_divider_01 (bitmap, p);
    draw_wall_divider_00 (bitmap, p);
    if (level->type == DUNGEON) {
      if (prandom(4) == 0)
        draw_wall_right_mark (bitmap, p, prandom (3));
      if (prandom(4) == 0)
        draw_wall_left_mark (bitmap, p, prandom (4));
    }
    break;
  case SWS:
    if (level->type == DUNGEON && prandom (6) == 0)
      draw_wall_left_mark (bitmap, p, prandom (1));
    break;
  case SWW:
    if (prandom (4) == 0)
      draw_wall_gray_block (bitmap, p);
    draw_wall_divider_01 (bitmap, p);
    if (level->type == DUNGEON) {
      if (prandom(4) == 0)
        draw_wall_right_mark (bitmap, p, prandom (3));
      if (prandom(4) == 0)
        draw_wall_left_mark (bitmap, p, prandom (3));
    }
    break;
  case WWS:
    draw_wall_divider_01 (bitmap, p);
    draw_wall_divider_00 (bitmap, p);
    if (level->type == DUNGEON) {
      if (prandom(4) == 0)
        draw_wall_right_mark (bitmap, p, prandom (1) + 2);
      if (prandom(4) == 0)
        draw_wall_left_mark (bitmap, p, prandom (4));
    }
    break;
  default:
    error (-1, 0, "%s: unknown wall correlation (%i)", __func__, wc);
  }

  unseedp ();
}

void
draw_wall_left_mark (ALLEGRO_BITMAP *bitmap, struct pos p, int r)
{
  ALLEGRO_BITMAP *wall_mark = wall_mark_top_left;
  const int floor_offset[5] = {58, 41, 37, 20, 16};
  int place_offset = 0;

  if (r % 2) wall_mark = wall_mark_bottom_left;
  if (r > 3) place_offset = r3 - r2 + 6;
  else if (r > 1) place_offset = r1 - r0 + 6;

  draw_bitmap (wall_mark, bitmap,
               32 * p.place + place_offset +
               8 * (((r == 2) || (r == 3)) ? 1 : 0),
               63 * p.floor + 61 - floor_offset[r], 0);
}

void
draw_wall_right_mark (ALLEGRO_BITMAP *bitmap, struct pos p, int r)
{
  ALLEGRO_BITMAP *wall_mark;
  const int floor_offset[4] = {52, 42, 31, 21};

  if (r % 2) wall_mark = wall_mark_bottom_right;
  else wall_mark = wall_mark_top_right;

  draw_bitmap (wall_mark, bitmap,
               32 * p.place + 8 * ((r > 1) ? 1 : 0)
               + ((r < 2) ? 24 : r1 - 3),
               63 * p.floor + 56 - floor_offset[r], 0);
}

void
draw_wall_gray_block (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (wall_gray_block, bitmap, wall_gray_block_coord (p), 0);
}

struct coord
wall_gray_block_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 3;
  c.room = p.room;
  return c;
}

void
draw_wall_divider_00 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (wall_divider_00, bitmap, wall_divider_00_coord (p), 0);
}

struct coord
wall_divider_00_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place + r3;
  c.y = PLACE_HEIGHT * p.floor + 45;
  c.room = p.room;
  return c;
}

void
draw_wall_divider_01 (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (wall_divider_01, bitmap, wall_divider_01_coord (p), 0);
}

struct coord
wall_divider_01_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place + 8 + r1;
  c.y = PLACE_HEIGHT * p.floor + 24;
  c.room = p.room;
  return c;
}
