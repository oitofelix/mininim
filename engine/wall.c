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
    error (-1, 0, "%s: unknown wall correlation (%u, %u. %u)",
           __func__, p.room, p.floor, p.place);
  }
  draw_wall_randomization (bitmap, p);
}

void
draw_wall_sws (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (wall_single_base, bitmap, wall_base_xy (p), 0);
  draw_bitmap_xy (wall_single, bitmap, wall_xy (p), 0);
  draw_bitmap_xy (wall_face, bitmap, wall_face_xy (p), 0);
  draw_bitmap_xy (wall_face_top, bitmap, wall_face_top_xy (p), 0);
}

void
draw_wall_sww (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (wall_left_base, bitmap, wall_base_xy (p), 0);
  draw_bitmap_xy (wall_left, bitmap, wall_xy (p), 0);
}

void
draw_wall_wws (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (wall_right_base, bitmap, wall_base_xy (p), 0);
  draw_bitmap_xy (wall_right, bitmap, wall_xy (p), 0);
  draw_bitmap_xy (wall_face, bitmap, wall_face_xy (p), 0);
  draw_bitmap_xy (wall_face_top, bitmap, wall_face_top_xy (p), 0);
}

void
draw_wall_www (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmap_xy (wall_center_base, bitmap, wall_base_xy (p), 0);
  draw_bitmap_xy (wall_center, bitmap, wall_xy (p), 0);
}

void
draw_wall_fg (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  switch (wall_correlation (p)) {
  case SWS: draw_wall (screen, p); break;
  case SWW: draw_wall (screen, p); break;
  case WWS: break;
  case WWW: break;
  default:
    error (-1, 0, "%s: unknown wall correlation (%u, %u. %u)",
           __func__, p.room, p.floor, p.place);
    break;
  }
}

struct xy
wall_base_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * p.place;
  xy.y = PLACE_HEIGHT * (p.floor + 1);
  return xy;
}

struct xy
wall_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * p.place;
  xy.y = PLACE_HEIGHT * p.floor + 3;
  return xy;
}

struct xy
wall_face_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * (p.place + 1);
  xy.y = PLACE_HEIGHT * p.floor + 3;
  return xy;
}

struct xy
wall_face_top_xy (struct pos p)
{
  struct xy xy;
  xy.x = PLACE_WIDTH * (p.place + 1);
  xy.y = PLACE_HEIGHT * p.floor - 9;
  return xy;
}

enum wall_correlation
wall_correlation (struct pos p)
{
  if (construct (p).fg != WALL)
    error (-1, 0, "%s: requested wall correlation on non-wall (%u, %u. %u)",
           __func__, p.room, p.floor, p.place);

  if (construct_rel (p, 0, -1).fg != WALL
      && construct_rel (p, 0, +1).fg != WALL) return SWS;
  else if (construct_rel (p, 0, -1).fg != WALL
           && construct_rel (p, 0, +1).fg == WALL) return SWW;
  else if (construct_rel (p, 0, -1).fg == WALL
           && construct_rel (p, 0, +1).fg != WALL) return WWS;
  else if (construct_rel (p, 0, -1).fg == WALL
           && construct_rel (p, 0, +1).fg == WALL) return WWW;
  else
    error (-1, 0, "%s: unknown wall correlation (%u, %u. %u)",
           __func__, p.room, p.floor, p.place);

  return -1;
}

void
draw_wall_randomization (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (level->type != DUNGEON && video_mode == VGA) return;

  uint32_t random_seed_backup = random_seed;
  random_seed = p.room + p.floor * PLACES + p.place;
  prandom (1);
  unsigned int r0 = prandom(1);
  unsigned int r1 = prandom(4);
  unsigned int r2 = prandom(1);
  unsigned int r3 = prandom(4);

  ALLEGRO_BITMAP *wall_divider_0 = r2 ? wall_narrow_divider : wall_wide_divider;
  ALLEGRO_BITMAP *wall_divider_1 = r0 ? wall_narrow_divider : wall_wide_divider;
  unsigned int wall_divider_0_w = r2 ? 8 : 9;

  enum wall_correlation wc = wall_correlation (p);

  switch (wc) {
  case WWW:
    /* gray block randomization  */
    if (prandom (4) == 0 && p.floor >= 0 && p.place >= 0)
      draw_bitmap (wall_gray_block, bitmap,
                   32 * p.place, 63 * p.floor + 3, 0);

    /* wall divider randomization  */
    if (p.floor >= 0 && p.place >= 0)
      draw_bitmap (wall_divider_1, bitmap,
                   32 * p.place + 8 + r1, 63 * p.floor + 24, 0);

    /* wall divider randomization  */
    if (p.floor >= 0 && p.place >= 0)
      draw_bitmap (wall_divider_0, bitmap,
                   32 * p.place + r3, 63 * p.floor + 45, 0);

    /* wall divider randomization (room above)  */
    if (p.floor < 0 && p.place >=0)
      draw_bitmap_region (wall_divider_0, bitmap, 0, 18, wall_divider_0_w, 3,
                          32 * p.place + r3, 0, 0);

    /* wall mark randomization */
    if (level->type == DUNGEON) {
      if (prandom(4) == 0) draw_wall_right_mark (bitmap, p, r1, prandom (3));
      if (prandom(4) == 0) draw_wall_left_mark (bitmap, p, r3 - r2, r1 - r0, prandom (4));
    }
    break;
  case SWS:
    /* wall mark randomization */
    if (level->type == DUNGEON && prandom (6) == 0)
      draw_wall_left_mark (bitmap, p, r3 - r2, r1 - r0, prandom (1));
    break;
  case SWW:
    /* gray block randomization  */
    if (prandom (4) == 0 && p.floor >= 0 && p.place >= 0)
      draw_bitmap (wall_gray_block, bitmap,
                   32 * p.place, 63 * p.floor + 3, 0);

    /* wall divider randomization  */
    if (p.floor >= 0 && p.place >= 0)
      draw_bitmap (wall_divider_1, bitmap,
                   32 * p.place + 8 + r1, 63 * p.floor + 24, 0);

    /* wall mark randomization */
    if (level->type == DUNGEON) {
      if (prandom(4) == 0) draw_wall_right_mark (bitmap, p, r1, prandom (3));
      if (prandom(4) == 0) draw_wall_left_mark (bitmap, p, r3 - r2, r1 - r0, prandom (3));
    }
    break;
  case WWS:
    /* wall divider randomization  */
    if (p.floor >= 0 && p.place >= 0)
      draw_bitmap (wall_divider_1, bitmap,
                   32 * p.place + 8 + r1, 63 * p.floor + 24, 0);

    /* wall divider randomization  */
    if (p.floor >= 0 && p.place >= 0)
      draw_bitmap (wall_divider_0, bitmap,
                   32 * p.place + r3, 63 * p.floor + 45, 0);

    /* wall divider randomization (room above)  */
    if (p.floor < 0 && p.place >=0)
      draw_bitmap_region (wall_divider_0, bitmap, 0, 18, wall_divider_0_w, 3,
                          32 * p.place + r3, 0, 0);


    /* wall mark randomization */
    if (level->type == DUNGEON) {
      if (prandom(4) == 0) draw_wall_right_mark (bitmap, p, r1, prandom (1) + 2);
      if (prandom(4) == 0) draw_wall_left_mark (bitmap, p, r3 - r2, r1 - r0, prandom (4));
    }
    break;
  default:
    error (-1, 0, "%s: unknown wall correlation (%u)", __func__, wc);
  }

  random_seed = random_seed_backup;
}

void
draw_wall_left_mark (ALLEGRO_BITMAP *bitmap, struct pos p,
                     int random_0, int random_1, int random_2)
{
  ALLEGRO_BITMAP *wall_mark = wall_mark_top_left;
  const unsigned int floor_offset[5] = {58, 41, 37, 20, 16};
  unsigned int place_offset = 0;

  if (random_2 % 2) wall_mark = wall_mark_bottom_left;
  if (random_2 > 3) place_offset = random_0 + 6;
  else if (random_2 > 1) place_offset = random_1 + 6;

  if (p.floor >= 0 && p.place >= 0)
    draw_bitmap (wall_mark, bitmap,
                 32 * p.place + place_offset +
                 8 * (((random_2 == 2) || (random_2 == 3)) ? 1 : 0),
                 63 * p.floor + 61 - floor_offset[random_2], 0);
}

void
draw_wall_right_mark (ALLEGRO_BITMAP *bitmap, struct pos p,
                      int random_0, int random_1)
{
  ALLEGRO_BITMAP *wall_mark;
  const unsigned int floor_offset[4] = {52, 42, 31, 21};

  if (random_1 % 2) wall_mark = wall_mark_bottom_right;
  else wall_mark = wall_mark_top_right;

  if (random_1 < 2) random_0 = 24;
  else random_0 -= 3;

  if (p.floor >= 0 && p.place >= 0)
    draw_bitmap (wall_mark, bitmap,
                 32 * p.place + 8 * ((random_1 > 1) ? 1 : 0) + random_0,
                 63 * p.floor + 56 - floor_offset[random_1], 0);
}
