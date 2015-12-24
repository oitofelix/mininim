/*
  prince.h -- MININIM main module;

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

#ifndef MININIM_PRINCE_H
#define MININIM_PRINCE_H

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

#define V_KID_HAIR_COLOR_01 (al_map_rgb (184, 144, 0))
#define V_KID_HAIR_COLOR_02 (al_map_rgb (120, 92, 56))
#define V_KID_SKIN_COLOR_01 (al_map_rgb (220, 128, 108))
#define V_KID_SKIN_COLOR_02 (al_map_rgb (176, 112, 96))
#define V_KID_CLOTHES_COLOR_01 (al_map_rgb (252, 252, 216))
#define V_KID_CLOTHES_COLOR_02 (al_map_rgb (216, 184, 160))
#define V_KID_EYE_COLOR (al_map_rgb (136, 84, 76))
#define V_KID_NOSE_COLOR (al_map_rgb (176, 112, 96))

#define E_KID_HAIR_COLOR (al_map_rgb (170, 0, 0))
#define E_KID_SKIN_COLOR (al_map_rgb (170, 85, 0))
#define E_KID_EYE_COLOR (al_map_rgb (0, 0, 0))
#define E_KID_CLOTHES_COLOR_01 (al_map_rgb (255, 255, 255))
#define E_KID_CLOTHES_COLOR_02 (al_map_rgb (170, 170, 170))

#define C_KID_HAIR_COLOR_01 (al_map_rgb (255, 255, 255))
#define C_KID_HAIR_COLOR_02 (al_map_rgb (85, 255, 255))
#define C_KID_SKIN_COLOR (al_map_rgb (255, 85, 255))
#define C_KID_EYE_COLOR (al_map_rgb (0, 0, 0))
#define C_KID_CLOTHES_COLOR_01 (al_map_rgb (255, 255, 255))
#define C_KID_CLOTHES_COLOR_02 (al_map_rgb (85, 255, 255))

#define V_KID_SHADOW_CLOTHES_COLOR_01 (al_map_rgba (64, 64 , 64, 0))
#define V_KID_SHADOW_CLOTHES_COLOR_02 (al_map_rgba (32, 32, 32, 0))
#define V_KID_SHADOW_EYE_COLOR (al_map_rgba (184, 0, 0, 0))

#define E_KID_SHADOW_CLOTHES_COLOR_01 (al_map_rgb (170, 170 , 170))
#define E_KID_SHADOW_CLOTHES_COLOR_02 (al_map_rgb (85, 85, 85))
#define E_KID_SHADOW_SKIN_COLOR (al_map_rgb (255, 255, 255))
#define E_KID_SHADOW_HAIR_COLOR (al_map_rgb (85, 85, 85))
#define E_KID_SHADOW_EYE_COLOR (al_map_rgb (170, 0, 0))

#define C_KID_SHADOW_CLOTHES_COLOR_01 (al_map_rgb (85, 255, 255))
#define C_KID_SHADOW_CLOTHES_COLOR_02 (al_map_rgb (0, 0, 0))
#define C_KID_SHADOW_SKIN_COLOR (al_map_rgb (255, 255, 255))
#define C_KID_SHADOW_EYE_COLOR (al_map_rgb (255, 85, 255))

#define LIFE_POTION_BUBBLE_COLOR (al_map_rgb (224, 0, 48))
#define POISON_POTION_BUBBLE_COLOR (al_map_rgb (80, 84, 248))
#define FLOAT_POTION_BUBBLE_COLOR (al_map_rgb (84, 252, 84))
#define FLIP_POTION_BUBBLE_COLOR (al_map_rgb (255, 255, 0))

#define C_BLOOD_COLOR (al_map_rgb (255, 85, 255))
#define E_BLOOD_COLOR_01 (al_map_rgb (255, 85, 85))
#define E_BLOOD_COLOR_02 (al_map_rgb (170, 0, 0))
#define V_BLOOD_COLOR_01 (al_map_rgb (228, 0, 0))
#define V_BLOOD_COLOR_02 (al_map_rgb (184, 0, 0))

#define RRED (al_map_rgb (prandom (255), 0, 0))
#define RGREEN (al_map_rgb (0, prandom (255), 0))
#define RBLUE (al_map_rgb (0, 0, prandom (255)))

#define TRANSPARENT (al_map_rgba (0, 0, 0, 0))
#define TRED (al_map_rgba (255, 0, 0, 0))
#define TGREEN (al_map_rgba (0, 255, 0, 0))
#define TBLUE (al_map_rgba (0, 0, 255, 0))

#define PALACE_WALL_COLOR_00 (al_map_rgb (216, 168, 88))
#define PALACE_WALL_COLOR_01 (al_map_rgb (224, 164, 92))
#define PALACE_WALL_COLOR_02 (al_map_rgb (224, 168, 96))
#define PALACE_WALL_COLOR_03 (al_map_rgb (216, 160, 84))
#define PALACE_WALL_COLOR_04 (al_map_rgb (224, 164, 92))
#define PALACE_WALL_COLOR_05 (al_map_rgb (216, 164, 88))
#define PALACE_WALL_COLOR_06 (al_map_rgb (224, 168, 88))
#define PALACE_WALL_COLOR_07 (al_map_rgb (216, 168, 96))

#define C_FIRE_COLOR_01 (al_map_rgb (255, 85, 255))
#define C_FIRE_COLOR_02 (al_map_rgb (255, 255, 255))
#define E_FIRE_COLOR_01 (al_map_rgb (255, 85, 85))
#define E_FIRE_COLOR_02 (al_map_rgb (255, 255, 85))
#define V_FIRE_COLOR_01 (al_map_rgb (252, 132, 0))
#define V_FIRE_COLOR_02 (al_map_rgb (252, 252, 0))

#define DV_BRICKS_COLOR (al_map_rgb (48, 68, 88))
#define DE_BRICKS_COLOR (al_map_rgb (80, 80, 80))
#define DC_BRICKS_COLOR (al_map_rgb (85, 255, 255))

#define PV_BRICKS_COLOR (al_map_rgb (12, 56, 88))
#define PE_BRICKS_COLOR (al_map_rgb (0, 0, 160))
#define PC_BRICKS_COLOR (al_map_rgb (85, 255, 255))

#define C_LIVE_COLOR (al_map_rgb (255, 85, 255))
#define E_LIVE_COLOR_01 (al_map_rgb (255, 85, 85))
#define E_LIVE_COLOR_02 (al_map_rgb (170, 0, 0))
#define V_LIVE_COLOR_01 (al_map_rgb (228, 0, 0))
#define V_LIVE_COLOR_02 (al_map_rgb (184, 0, 0))

#define C_TEXT_LINE_COLOR (al_map_rgb (0, 0, 0))
#define V_TEXT_LINE_COLOR (al_map_rgb (0, 0, 0))

#define V_LIVES_RECT_COLOR (al_map_rgba (0, 0, 0, 170))
#define C_LIVES_RECT_COLOR (al_map_rgb (0, 0, 0))
#define E_LIVES_RECT_COLOR (al_map_rgb (0, 0, 0))

#define V_MSG_LINE_COLOR (al_map_rgba (0, 0, 0, 192))
#define E_MSG_LINE_COLOR (al_map_rgb (0, 0, 0))
#define C_MSG_LINE_COLOR (al_map_rgb (0, 0, 0))

/* environment mode */
enum em {
  DUNGEON, PALACE,
};

/* video mode */
enum vm {
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

struct rect {
  struct coord c;
  int w, h;
};

struct dim {
  int x, y, w, h, fx, bx;
};

enum dir {
  LEFT, RIGHT, ABOVE, BELOW
};

struct level {
  void (*start) (void);
  void (*special_events) (void);
  void (*next_level) (int, struct pos *);
  int number;

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
      BIG_PILLAR_BOTTOM,
      BIG_PILLAR_TOP,
      WALL,
      DOOR,
      LEVEL_DOOR,
      CHOPPER,
    } fg;
    enum conbg {
      NO_BG,
      BRICKS_01,
      BRICKS_02,
      BRICKS_03,
      BRICKS_04,
      NO_BRICKS,
      TORCH,
      WINDOW,
    } bg;
    union conext {
      int event;
      enum item {
        NO_ITEM,
        SMALL_LIFE_POTION,
        BIG_LIFE_POTION,
        SMALL_POISON_POTION,
        BIG_POISON_POTION,
        FLOAT_POTION,
        FLIP_POTION,
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
  int id;
  int shadow_of;

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
    keep_sword_fast, turn, shadow, current, splash,
    hit_by_loose_floor, invisible;

  ALLEGRO_TIMER *floating;

  int dc, df, dl, dcl, dch, dcd;

  bool immortal, poison_immune, loose_floor_immune, fall_immune,
    spikes_immune, chopper_immune;

  enum confg confg;

  enum item item;

  struct pos p, item_pos, hang_pos;

  /* depressible floor */
  struct pos df_pos[2];
  struct pos df_posb[2];
};

/* functions */
int max (int a, int b);
int min (int a, int b);

/* variables */
extern ALLEGRO_TIMER *play_time;
extern enum em em;
extern enum vm vm;

#endif	/* MININIM_PRINCE_H */
