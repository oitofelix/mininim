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

#define EFFECT_HZ 30
#define SECS_TO_VCYCLES(x) ((x) * EFFECT_HZ)
#define BLACK (al_map_rgb (0, 0, 0))
#define WHITE (al_map_rgb (255, 255, 255))


enum video_mode {
  CGA, EGA, VGA,
};

struct pos {
  int room, floor, place;
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

struct anim {
  struct anim *id;
  struct coord c;
  ALLEGRO_BITMAP *frame;
  enum dir dir;
  int repeat;
  int flip;
  bool just_fall;
  void (*draw) (void);
  void (*odraw) (void);
  void (*collision) (void);
  void (*back_collision) (void);
  void (*fall) (void);
  void (*ceiling) (void);
};

struct level {
  enum {
    DUNGEON, PALACE,
  } type;

  struct construct {
    enum construct_fg {
      NO_FLOOR,
      FLOOR,
      BROKEN_FLOOR,
      LOOSE_FLOOR,
      SPIKES_FLOOR,
      OPENER_FLOOR,
      PILLAR,
      WALL,
      DOOR,
    } fg;
    enum construct_bg {
      NO_BG,
      BRICKS_01,
      BRICKS_02,
      BRICKS_03,
      BRICKS_04,
      TORCH,
      WINDOW,
    } bg;
    int event;
  } construct[ROOMS][FLOORS][PLACES];

  struct {
    int l, r, a, b;
  } link[ROOMS];

  struct {
    struct pos p;
    bool next;
  } event[EVENTS];
};

struct survey {
  struct coord m, mt, mbo, ml, mr, mf, mba,
    tl, tr, tf, tb,
    bl, br, bf, bb;
  struct pos pm, pmt, pmbo, pml, pmr, pmf, pmba,
    ptl, ptr, ptf, ptb,
    pbl, pbr, pbf, pbb;
  enum construct_fg cm, cmt, cmbo, cml, cmr, cmf, cmba,
    ctl, ctr, ctf, ctb,
    cbl, cbr, cbf, cbb;
};

/* current video mode */
extern enum video_mode video_mode;

#endif	/* FREEPOP_PRINCE_H */
