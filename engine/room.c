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
#include "room.h"
#include "level.h"

static ALLEGRO_BITMAP *floor_normal_left, *floor_normal_right, *floor_normal_base,
  *floor_broken_left, *floor_broken_right,
  *wall_left, *wall_center, *wall_right, *wall_single, *wall_face, *wall_face_top,
  *wall_left_base, *wall_center_base, *wall_right_base, *wall_single_base,
  *wall_gray_block, *wall_wide_divider, *wall_narrow_divider, *wall_mark_top_left,
  *wall_mark_top_right, *wall_mark_bottom_left, *wall_mark_bottom_right, *pillar_left,
  *pillar_right, *pillar_top, *pillar, *bricks_1, *torch;

/* room bitmap */
static ALLEGRO_BITMAP *room_bg;

/* current room */
static unsigned int room = -1;

static void draw_room_bg (void);
static void draw_floor (struct level_pos pos, enum floor_type floor, enum bg_type bg);
static void draw_pillar (struct level_pos pos);
static void draw_wall (ALLEGRO_BITMAP *bitmap, struct level_pos pos);
static enum wall_correlation wall_correlation (struct level_pos pos);
static void draw_wall_randomization (ALLEGRO_BITMAP *bitmap, struct level_pos pos);
static void draw_wall_left_mark (ALLEGRO_BITMAP *bitmap, struct level_pos,
                                 int random_0, int random_1, int random_2);
static void draw_wall_right_mark (ALLEGRO_BITMAP *bitmap, struct level_pos pos,
                                  int random_0, int random_1);

void
load_room (void)
{
  room_bg = create_bitmap (ORIGINAL_WIDTH, ORIGINAL_HEIGHT);

  switch (level->type)
    {
    case DUNGEON:
      switch (video_mode) {
      case VGA:
        floor_normal_left = load_bitmap (VDUNGEON_FLOOR_NORMAL_LEFT);
        floor_normal_right = load_bitmap (VDUNGEON_FLOOR_NORMAL_RIGHT);
        floor_normal_base = load_bitmap (VDUNGEON_FLOOR_NORMAL_BASE);
        floor_broken_left = load_bitmap (VDUNGEON_FLOOR_BROKEN_LEFT);
        floor_broken_right = load_bitmap (VDUNGEON_FLOOR_BROKEN_RIGHT);
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
        pillar_left = load_bitmap (VDUNGEON_PILLAR_LEFT);
        pillar_right = load_bitmap (VDUNGEON_PILLAR_RIGHT);
        pillar_top = load_bitmap (VDUNGEON_PILLAR_TOP);
        pillar = load_bitmap (VDUNGEON_PILLAR);
        bricks_1 = load_bitmap (VDUNGEON_BRICKS_1);
        torch = load_bitmap (VDUNGEON_TORCH);
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
  /* bitmaps */
  al_destroy_bitmap (floor_normal_left);
  al_destroy_bitmap (floor_normal_right);
  al_destroy_bitmap (floor_normal_base);
  al_destroy_bitmap (floor_broken_left);
  al_destroy_bitmap (floor_broken_right);
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
  al_destroy_bitmap (pillar_left);
  al_destroy_bitmap (pillar_right);
  al_destroy_bitmap (pillar_top);
  al_destroy_bitmap (pillar);
  al_destroy_bitmap (bricks_1);
  al_destroy_bitmap (torch);
}

void
draw_room (int _room)
{
  if (_room != room) {
    room = _room;
    draw_room_bg ();
  }
  draw_bitmap (room_bg, screen, 0, 0, 0);
}


static void
draw_room_bg (void)
{
  struct level_pos pos;
  pos.room = room;

  clear_bitmap (room_bg, BLACK);

  for (pos.floor = LEVEL_FLOORS - 1; pos.floor >= -1; pos.floor--)
    for (pos.place = -1; pos.place < LEVEL_PLACES; pos.place++) {
      switch (level_obj (pos)) {
      case NO_FLOOR:
        draw_floor (pos, NO_FLOOR_TYPE, NO_BG);
        break;
      case NORMAL_FLOOR:
        draw_floor (pos, NORMAL, NO_BG);
        break;
      case BROKEN_FLOOR:
        draw_floor (pos, BROKEN, NO_BG);
        break;
      case LOOSE_FLOOR:
        draw_floor (pos, LOOSE, NO_BG);
        break;
      case NORMAL_FLOOR_BRICKS_1:
        draw_floor (pos, NORMAL, BRICKS_1);
        break;
      case NORMAL_FLOOR_TORCH:
        draw_floor (pos, NORMAL, TORCH);
        break;
      case PILLAR:
        draw_pillar (pos);
        break;
      case WALL:
        draw_wall (room_bg, pos);
        break;
      default:
        error (-1, 0, "%s: unknown level object (%i)", __func__, level_obj (pos));
      }
    }
}

static void
draw_floor (struct level_pos pos, enum floor_type floor, enum bg_type bg)
{
  ALLEGRO_BITMAP *floor_left;
  ALLEGRO_BITMAP *floor_right;
  ALLEGRO_BITMAP *floor_base;

  switch (floor)
    {
    case NO_FLOOR_TYPE:
      break;
    case NORMAL:
      floor_left = floor_normal_left;
      floor_right = floor_normal_right;
      floor_base = floor_normal_base;
      break;
    case BROKEN:
      floor_left = floor_broken_left;
      floor_right = floor_broken_right;
      floor_base = floor_normal_base;
      break;
    case LOOSE:
      floor_left = floor_normal_left;
      floor_right = floor_normal_right;
      floor_base = floor_normal_base;
      break;
    default:
      error (-1, 0, "%s: unknown floor type (%u)", __func__, floor);
    }

  if (floor != NO_FLOOR_TYPE)
    {
      if (pos.place >= 0)
        draw_bitmap (floor_base, room_bg,
                     32 * pos.place, 63 * (pos.floor + 1), 0);
      if (pos.floor >= 0 && pos.place >= 0)
        draw_bitmap (floor_left, room_bg,
                     32 * pos.place, 63 * pos.floor + 50, 0);
      if (pos.floor >= 0 && pos.place + 1 < LEVEL_PLACES)
        draw_bitmap (floor_right, room_bg,
                     32 * (pos.place + 1), 63 * pos.floor + 50, 0);
    }

  switch (bg)
    {
    case NO_BG:
      break;
    case BRICKS_1:
      if (pos.floor >= 0 && pos.place + 1 < LEVEL_PLACES)
        draw_bitmap (bricks_1, room_bg,
                     32 * (pos.place + 1), 63 * pos.floor + 15, 0);
      break;
    case TORCH:
      if (pos.floor >= 0 && pos.place + 1 < LEVEL_PLACES)
        draw_bitmap (torch, room_bg,
                     32 * (pos.place + 1), 63 * pos.floor + 22, 0);
      break;
    default:
      error (-1, 0, "%s: unknown background type (%u)", __func__, floor);
    }
}

static void
draw_pillar (struct level_pos pos)
{
  if (pos.place >= 0)
    draw_bitmap (floor_normal_base, room_bg,
                 32 * pos.place, 63 * (pos.floor + 1), 0);
  if (pos.floor >= 0 && pos.place >= 0)
    draw_bitmap (pillar_left, room_bg,
                 32 * pos.place, 63 * pos.floor + 3, 0);
  if (pos.floor >= 0 && pos.place + 1 < LEVEL_PLACES)
    draw_bitmap (pillar_right, room_bg,
                 32 * (pos.place + 1), 63 * pos.floor + 3, 0);
  if (pos.floor > 0 && pos.place + 1 < LEVEL_PLACES)
    draw_bitmap (pillar_top, room_bg,
                 32 * (pos.place + 1), 63 * pos.floor - 4, 0);
}

static void
draw_wall (ALLEGRO_BITMAP *bitmap, struct level_pos pos)
{
  switch (wall_correlation (pos)) {
  case SWS:
    if (pos.place >= 0)
      draw_bitmap (wall_single_base, bitmap,
                   32 * pos.place, 63 * (pos.floor + 1), 0);
    if (pos.floor >= 0 && pos.place >= 0)
      draw_bitmap (wall_single, bitmap,
                   32 * pos.place, 63 * pos.floor + 3, 0);
    if (pos.floor >= 0 && pos.place + 1 < LEVEL_PLACES)
      draw_bitmap (wall_face, bitmap,
                   32 * (pos.place + 1), 63 * pos.floor + 3, 0);
    if (pos.floor > 0 && pos.place + 1 < LEVEL_PLACES)
      draw_bitmap (wall_face_top, bitmap,
                   32 * (pos.place + 1), 63 * pos.floor - 9, 0);
    break;
  case SWW:
    if (pos.place >= 0)
      draw_bitmap (wall_left_base, bitmap,
                   32 * pos.place, 63 * (pos.floor + 1), 0);
    if (pos.floor >= 0 && pos.place >= 0)
      draw_bitmap (wall_left, bitmap,
                   32 * pos.place, 63 * pos.floor + 3, 0);
    break;
  case WWS:
    if (pos.place >= 0)
      draw_bitmap (wall_right_base, bitmap,
                   32 * pos.place, 63 * (pos.floor + 1), 0);
    if (pos.floor >= 0 && pos.place >= 0)
      draw_bitmap (wall_right, bitmap,
                   32 * pos.place, 63 * pos.floor + 3, 0);
    if (pos.floor >= 0 && pos.place + 1 < LEVEL_PLACES)
      draw_bitmap (wall_face, bitmap,
                   32 * (pos.place + 1), 63 * pos.floor + 3, 0);
    if (pos.floor > 0 && pos.place + 1 < LEVEL_PLACES)
      draw_bitmap (wall_face_top, bitmap,
                   32 * (pos.place + 1), 63 * pos.floor - 9, 0);
    break;
  case WWW:
    if (pos.place >= 0)
      draw_bitmap (wall_center_base, bitmap,
                   32 * pos.place, 63 * (pos.floor + 1), 0);
    if (pos.floor >= 0 && pos.place >= 0)
      draw_bitmap (wall_center, bitmap,
                   32 * pos.place, 63 * pos.floor + 3, 0);
    break;
  default:
    error (-1, 0, "%s: unknown wall correlation (%u, %u. %u)",
           __func__, pos.room, pos.floor, pos.place);
  }

  draw_wall_randomization (bitmap, pos);
}

enum wall_correlation
wall_correlation (struct level_pos pos)
{
  if (level_obj (pos) != WALL)
    error (-1, 0, "%s: requested wall correlation on non-wall (%u, %u. %u)",
           __func__, pos.room, pos.floor, pos.place);

  if (level_obj_rel (pos, 0, -1) != WALL
      && level_obj_rel (pos, 0, +1) != WALL) return SWS;
  else if (level_obj_rel (pos, 0, -1) != WALL
           && level_obj_rel (pos, 0, +1) == WALL) return SWW;
  else if (level_obj_rel (pos, 0, -1) == WALL
           && level_obj_rel (pos, 0, +1) != WALL) return WWS;
  else if (level_obj_rel (pos, 0, -1) == WALL
           && level_obj_rel (pos, 0, +1) == WALL) return WWW;
  else
    error (-1, 0, "%s: unknown wall correlation (%u, %u. %u)",
           __func__, pos.room, pos.floor, pos.place);

  return -1;
}


void
draw_wall_randomization (ALLEGRO_BITMAP *bitmap, struct level_pos pos)
{
  if (level->type != DUNGEON && video_mode == VGA) return;

  uint32_t random_seed_backup = random_seed;
  random_seed = pos.room + pos.floor * LEVEL_PLACES + pos.place;
  prandom (1);
  unsigned int r0 = prandom(1);
  unsigned int r1 = prandom(4);
  unsigned int r2 = prandom(1);
  unsigned int r3 = prandom(4);

  ALLEGRO_BITMAP *wall_divider_0 = r2 ? wall_narrow_divider : wall_wide_divider;
  ALLEGRO_BITMAP *wall_divider_1 = r0 ? wall_narrow_divider : wall_wide_divider;
  unsigned int wall_divider_0_w = r2 ? 8 : 9;

  enum wall_correlation wc = wall_correlation (pos);

  switch (wc) {
  case WWW:
    /* gray block randomization  */
    if (prandom (4) == 0 && pos.floor >= 0 && pos.place >= 0)
      draw_bitmap (wall_gray_block, bitmap,
                   32 * pos.place, 63 * pos.floor + 3, 0);

    /* wall divider randomization  */
    if (pos.floor >= 0 && pos.place >= 0)
      draw_bitmap (wall_divider_1, bitmap,
                   32 * pos.place + 8 + r1, 63 * pos.floor + 24, 0);

    /* wall divider randomization  */
    if (pos.floor >= 0 && pos.place >= 0)
      draw_bitmap (wall_divider_0, bitmap,
                   32 * pos.place + r3, 63 * pos.floor + 45, 0);

    /* wall divider randomization (room above)  */
    if (pos.floor < 0 && pos.place >=0)
      draw_bitmap_region (wall_divider_0, bitmap, 0, 18, wall_divider_0_w, 3,
                          32 * pos.place + r3, 0, 0);

    /* wall mark randomization */
    if (level->type == DUNGEON) {
      if (prandom(4) == 0) draw_wall_right_mark (bitmap, pos, r1, prandom (3));
      if (prandom(4) == 0) draw_wall_left_mark (bitmap, pos, r3 - r2, r1 - r0, prandom (4));
    }
    break;
  case SWS:
    /* wall mark randomization */
    if (level->type == DUNGEON && prandom (6) == 0)
      draw_wall_left_mark (bitmap, pos, r3 - r2, r1 - r0, prandom (1));
    break;
  case SWW:
    /* gray block randomization  */
    if (prandom (4) == 0 && pos.floor >= 0 && pos.place >= 0)
      draw_bitmap (wall_gray_block, bitmap,
                   32 * pos.place, 63 * pos.floor + 3, 0);

    /* wall divider randomization  */
    if (pos.floor >= 0 && pos.place >= 0)
      draw_bitmap (wall_divider_1, bitmap,
                   32 * pos.place + 8 + r1, 63 * pos.floor + 24, 0);

    /* wall mark randomization */
    if (level->type == DUNGEON) {
      if (prandom(4) == 0) draw_wall_right_mark (bitmap, pos, r1, prandom (3));
      if (prandom(4) == 0) draw_wall_left_mark (bitmap, pos, r3 - r2, r1 - r0, prandom (3));
    }
    break;
  case WWS:
    /* wall divider randomization  */
    if (pos.floor >= 0 && pos.place >= 0)
      draw_bitmap (wall_divider_1, bitmap,
                   32 * pos.place + 8 + r1, 63 * pos.floor + 24, 0);

    /* wall divider randomization  */
    if (pos.floor >= 0 && pos.place >= 0)
      draw_bitmap (wall_divider_0, bitmap,
                   32 * pos.place + r3, 63 * pos.floor + 45, 0);

    /* wall divider randomization (room above)  */
    if (pos.floor < 0 && pos.place >=0)
      draw_bitmap_region (wall_divider_0, bitmap, 0, 18, wall_divider_0_w, 3,
                          32 * pos.place + r3, 0, 0);


    /* wall mark randomization */
    if (level->type == DUNGEON) {
      if (prandom(4) == 0) draw_wall_right_mark (bitmap, pos, r1, prandom (1) + 2);
      if (prandom(4) == 0) draw_wall_left_mark (bitmap, pos, r3 - r2, r1 - r0, prandom (4));
    }
    break;
  default:
    error (-1, 0, "%s: unknown wall correlation (%u)", __func__, wc);
  }

  random_seed = random_seed_backup;
}

void
draw_wall_left_mark (ALLEGRO_BITMAP *bitmap, struct level_pos pos,
                     int random_0, int random_1, int random_2)
{
  ALLEGRO_BITMAP *wall_mark = wall_mark_top_left;
  const unsigned int floor_offset[5] = {58, 41, 37, 20, 16};
  unsigned int place_offset = 0;

  if (random_2 % 2) wall_mark = wall_mark_bottom_left;
  if (random_2 > 3) place_offset = random_0 + 6;
  else if (random_2 > 1) place_offset = random_1 + 6;

  if (pos.floor >= 0 && pos.place >= 0)
    draw_bitmap (wall_mark, bitmap,
                 32 * pos.place + place_offset +
                 8 * (((random_2 == 2) || (random_2 == 3)) ? 1 : 0),
                 63 * pos.floor + 61 - floor_offset[random_2], 0);
}


void
draw_wall_right_mark (ALLEGRO_BITMAP *bitmap, struct level_pos pos,
                      int random_0, int random_1)
{
  ALLEGRO_BITMAP *wall_mark;
  const unsigned int floor_offset[4] = {52, 42, 31, 21};

  if (random_1 % 2) wall_mark = wall_mark_bottom_right;
  else wall_mark = wall_mark_top_right;

  if (random_1 < 2) random_0 = 24;
  else random_0 -= 3;

  if (pos.floor >= 0 && pos.place >= 0)
    draw_bitmap (wall_mark, bitmap,
                 32 * pos.place + 8 * ((random_1 > 1) ? 1 : 0) + random_0,
                 63 * pos.floor + 56 - floor_offset[random_1], 0);
}

void
draw_room_fg (room)
{
  struct level_pos pos;
  pos.room = room;

  for (pos.floor = LEVEL_FLOORS - 1; pos.floor >= 0; pos.floor--)
    for (pos.place = 0; pos.place < LEVEL_PLACES; pos.place++) {
      switch (level_obj (pos)) {
      case PILLAR:
        if (pos.floor >= 0 && pos.place >= 0)
          draw_bitmap (pillar, screen,
                       32 * pos.place + 8, 63 * pos.floor + 3, 0);
        break;
      case WALL:
        switch (wall_correlation (pos)) {
        case SWS:
          draw_wall (screen, pos);
        case SWW:
          draw_wall (screen, pos);
        default:
          break;
        }
      default:
          break;
      }
    }
}
