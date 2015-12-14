/*
  prince.h -- FreePoP main module;

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

#ifndef FREEPOP_PRINCE_H
#define FREEPOP_PRINCE_H

#include <allegro5/allegro.h>

#define ROOMS 25
#define FLOORS 3
#define PLACES 10
#define EVENTS 256

#define DISPLAY_WIDTH 640
#define DISPLAY_HEIGHT 400
#define ORIGINAL_WIDTH 320
#define ORIGINAL_HEIGHT 200

#define PLACE_WIDTH (ORIGINAL_WIDTH / PLACES)
#define PLACE_HEIGHT ((ORIGINAL_HEIGHT - 11) / FLOORS)

#define KID_INITIAL_TOTAL_LIVES 3
#define KID_INITIAL_CURRENT_LIVES 3
#define KID_MINIMUM_LIVES_TO_BLINK 1
#define BOTTOM_TEXT_DURATION 3

#define EFFECT_HZ 30
#define SECS_TO_VCYCLES(x) ((x) * EFFECT_HZ)

/* colors */
#define BLACK (al_map_rgb (0, 0, 0))
#define RED (al_map_rgb (255, 0, 0))
#define GREEN (al_map_rgb (0, 255, 0))
#define BLUE (al_map_rgb (0, 0, 255))
#define YELLOW (al_map_rgb (255, 255, 0))
#define WHITE (al_map_rgb (255, 255, 255))
#define KID_SKIN_COLOR (al_map_rgb (220, 128, 108))
#define KID_NOSE_COLOR (al_map_rgb (176, 112, 96))

#define RRED (al_map_rgb (prandom (255), 0, 0))
#define RGREEN (al_map_rgb (0, prandom (255), 0))
#define RBLUE (al_map_rgb (0, 0, prandom (255)))

#define TRANSPARENT (al_map_rgba (0, 0, 0, 0))
#define TRED (al_map_rgba (255, 0, 0, 0))
#define TGREEN (al_map_rgba (0, 255, 0, 0))
#define TBLUE (al_map_rgba (0, 0, 255, 0))

enum video_mode {
  CGA, EGA, VGA,
};

struct pos {
  int room, floor, place;
};

struct frameset {
  ALLEGRO_BITMAP *frame;
  int dx, dy;
};

struct coord {
  int room, x, y;
};

struct dim {
  int x, y, w, h, fx, bx;
};

enum dir {
  LEFT, RIGHT, ABOVE, BELOW
};

struct level {
  enum {
    DUNGEON, PALACE,
  } type;

  struct con {
    enum confg {
      NO_FLOOR,
      FLOOR,
      BROKEN_FLOOR,
      SKELETON_FLOOR,
      LOOSE_FLOOR,
      SPIKES_FLOOR,
      OPENER_FLOOR,
      CLOSER_FLOOR,
      PILLAR,
      WALL,
      DOOR,
    } fg;
    enum conbg {
      NO_BG,
      BRICKS_01,
      BRICKS_02,
      BRICKS_03,
      BRICKS_04,
      TORCH,
      WINDOW,
    } bg;
    union conext {
      int event;
      enum item {
        NO_ITEM,
        SMALL_LIFE_POTION,
        BIG_LIFE_POTION,
        SWORD,
      } item;
    } ext;
  } con[ROOMS][FLOORS][PLACES];

  struct {
    int l, r, a, b;
  } link[ROOMS];

  struct {
    struct pos p;
    bool next;
  } event[EVENTS];
};

/* avoid "'struct' declared inside parameter list" error for the
   ACTION definition */
struct anim *_action;
typedef void (*ACTION) (struct anim *a);

struct anim {
  struct frame {
    void *id;
    struct coord c;
    ALLEGRO_BITMAP *b;
    enum dir dir;
    int flip;
  } f;

  struct frame_offset {
    ALLEGRO_BITMAP *b;
    int dx, dy;
  } fo;

  struct frame_offset xf;

  struct collision_info {
    enum confg t;
    struct pos p;
  } ci;

  ACTION oaction;
  ACTION action;
  ACTION hang_caller;
  int i, j, wait, repeat, cinertia, inertia, walk,
    total_lives, current_lives;
  bool reverse, collision, fall, hit_ceiling,
    just_hanged, hang, hang_limit, misstep, uncouch_slowly,
    keep_sword_fast, turn, shadow, current;

  struct pos p, item_pos, hang_pos;

  /* depressible floor */
  struct pos df_pos[2];
  struct pos df_posb[2];
};

/* functions */
int max (int a, int b);
int min (int a, int b);

/* current video mode */
extern enum video_mode video_mode;

#endif	/* FREEPOP_PRINCE_H */
