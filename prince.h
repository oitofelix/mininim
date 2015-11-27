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
#define TRANSPARENT (al_map_rgba (0, 0, 0, 0))
#define BLACK (al_map_rgb (0, 0, 0))
#define RED (al_map_rgb (255, 0, 0))
#define GREEN (al_map_rgb (0, 255, 0))
#define BLUE (al_map_rgb (0, 0, 255))
#define YELLOW (al_map_rgb (255, 255, 0))
#define WHITE (al_map_rgb (255, 255, 255))

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

struct anim {
  struct anim *id;
  struct coord c;
  ALLEGRO_BITMAP *frame;
  enum dir dir;
  int repeat;
  int flip;
  void (*action) (void);

  /* depressible floor */
  struct pos df_pos[2];
  struct pos df_posb[2];

  ALLEGRO_BITMAP *xframe;
  int xdx, xdy;
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

struct survey {
  struct coord m, mt, mbo, ml, mr, mf, mba,
    tl, tr, tf, tb,
    bl, br, bf, bb;
  struct pos pm, pmt, pmbo, pml, pmr, pmf, pmba,
    ptl, ptr, ptf, ptb,
    pbl, pbr, pbf, pbb;
  enum confg cm, cmt, cmbo, cml, cmr, cmf, cmba,
    ctl, ctr, ctf, ctb,
    cbl, cbr, cbf, cbb;
};

/* current video mode */
extern enum video_mode video_mode;

#endif	/* FREEPOP_PRINCE_H */
