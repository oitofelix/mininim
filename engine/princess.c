/*
  princess.c -- princess module;

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
#include "kernel/video.h"
#include "anim.h"
#include "princess.h"

struct anim princess;

#define TURN_FRAMESET_NMEMB 8
#define STEP_BACK_FRAMESET_NMEMB 6
#define LOOK_DOWN_FRAMESET_NMEMB 2

static struct frameset turn_frameset[TURN_FRAMESET_NMEMB];
static struct frameset step_back_frameset[STEP_BACK_FRAMESET_NMEMB];
static struct frameset look_down_frameset[LOOK_DOWN_FRAMESET_NMEMB];

static void init_turn_frameset (void);
static void init_step_back_frameset (void);
static void init_look_down_frameset (void);

/* variables */
ALLEGRO_BITMAP *princess_normal_00,
  *princess_turn_02, *princess_turn_03, *princess_turn_04, *princess_turn_05,
  *princess_turn_06, *princess_turn_07, *princess_turn_08, *princess_turn_09,
  *princess_step_back_10, *princess_step_back_11, *princess_step_back_12,
  *princess_step_back_13, *princess_step_back_14, *princess_step_back_15,
  *princess_look_down_16, *princess_look_down_17;

void
load_princess (void)
{
  princess_normal_00 = load_bitmap (PRINCESS_NORMAL_00);
  princess_turn_02 = load_bitmap (PRINCESS_TURN_02);
  princess_turn_03 = load_bitmap (PRINCESS_TURN_03);
  princess_turn_04 = load_bitmap (PRINCESS_TURN_04);
  princess_turn_05 = load_bitmap (PRINCESS_TURN_05);
  princess_turn_06 = load_bitmap (PRINCESS_TURN_06);
  princess_turn_07 = load_bitmap (PRINCESS_TURN_07);
  princess_turn_08 = load_bitmap (PRINCESS_TURN_08);
  princess_turn_09 = load_bitmap (PRINCESS_TURN_09);
  princess_step_back_10 = load_bitmap (PRINCESS_STEP_BACK_10);
  princess_step_back_11 = load_bitmap (PRINCESS_STEP_BACK_11);
  princess_step_back_12 = load_bitmap (PRINCESS_STEP_BACK_12);
  princess_step_back_13 = load_bitmap (PRINCESS_STEP_BACK_13);
  princess_step_back_14 = load_bitmap (PRINCESS_STEP_BACK_14);
  princess_step_back_15 = load_bitmap (PRINCESS_STEP_BACK_15);
  princess_look_down_16 = load_bitmap (PRINCESS_LOOK_DOWN_16);
  princess_look_down_17 = load_bitmap (PRINCESS_LOOK_DOWN_17);

  /* framesets */
  init_turn_frameset ();
  init_step_back_frameset ();
  init_look_down_frameset ();
}

void
unload_princess (void)
{
  al_destroy_bitmap (princess_normal_00);
  al_destroy_bitmap (princess_turn_02);
  al_destroy_bitmap (princess_turn_03);
  al_destroy_bitmap (princess_turn_04);
  al_destroy_bitmap (princess_turn_05);
  al_destroy_bitmap (princess_turn_06);
  al_destroy_bitmap (princess_turn_07);
  al_destroy_bitmap (princess_turn_08);
  al_destroy_bitmap (princess_turn_09);
  al_destroy_bitmap (princess_step_back_10);
  al_destroy_bitmap (princess_step_back_11);
  al_destroy_bitmap (princess_step_back_12);
  al_destroy_bitmap (princess_step_back_13);
  al_destroy_bitmap (princess_step_back_14);
  al_destroy_bitmap (princess_step_back_15);
  al_destroy_bitmap (princess_look_down_16);
  al_destroy_bitmap (princess_look_down_17);
}



void
init_turn_frameset (void)
{
  struct frameset frameset[TURN_FRAMESET_NMEMB] =
    {{princess_turn_02,+0,0},{princess_turn_03,-1,0},{princess_turn_04,+0,0},
     {princess_turn_05,+2,0},{princess_turn_06,-6,0},{princess_turn_07,-2,0},
     {princess_turn_08,+6,0},{princess_turn_09,-2,0}};

  memcpy (&turn_frameset, &frameset,
          TURN_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_step_back_frameset (void)
{
  struct frameset frameset[STEP_BACK_FRAMESET_NMEMB] =
    {{princess_step_back_10,-6,0},{princess_step_back_11,-4,0},
     {princess_step_back_12,-1,0},{princess_step_back_13,-7,0},
     {princess_step_back_14,-2,0},{princess_step_back_15,+0,0}};

  memcpy (&step_back_frameset, &frameset,
          STEP_BACK_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_look_down_frameset (void)
{
  struct frameset frameset[LOOK_DOWN_FRAMESET_NMEMB] =
    {{princess_look_down_16,+0,0},{princess_look_down_17,+0,0}};

  memcpy (&look_down_frameset, &frameset,
          LOOK_DOWN_FRAMESET_NMEMB * sizeof (struct frameset));
}



void
princess_normal (void)
{
  princess.action = princess_normal;
  princess.f.flip = (princess.f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  ALLEGRO_BITMAP *frame = princess_normal_00;
  int dx = +0;
  int dy = +0;

  next_frame (&princess.f, &princess.f, frame, dx, dy);
}

void
princess_turn (void)
{
  void (*oaction) (void) = princess.action;
  princess.action = princess_turn;
  princess.f.flip = (princess.f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  static int i = 0;
  if (oaction != princess_turn) i = 0;

  ALLEGRO_BITMAP *frame = turn_frameset[i].frame;
  int dx = turn_frameset[i].dx;
  int dy = turn_frameset[i].dy;

  next_frame (&princess.f, &princess.f, frame, dx, dy);

  if (i < 7) i++;
  else {
    princess.f.dir = (princess.f.dir == RIGHT) ? LEFT : RIGHT;
    princess.action = princess_normal;
  }
}

void
princess_step_back (void)
{
  void (*oaction) (void) = princess.action;
  princess.action = princess_step_back;
  princess.f.flip = (princess.f.dir == RIGHT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

  static int i = 0;
  if (oaction != princess_step_back) i = 0;

  ALLEGRO_BITMAP *frame = step_back_frameset[i].frame;
  int dx = step_back_frameset[i].dx;
  int dy = step_back_frameset[i].dy;

  if (i == 5 && princess.f.b == step_back_frameset[4].frame) dx = +1;

  next_frame_inv = true;
  next_frame (&princess.f, &princess.f, frame, dx, dy);

  if (next_frame_inv) next_frame_inv = false;
  if (i < 5) i++;
}

void
princess_look_down (void)
{
  void (*oaction) (void) = princess.action;
  princess.action = princess_look_down;
  princess.f.flip = (princess.f.dir == RIGHT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

  static int i = 0;
  if (oaction != princess_look_down) i = 0;

  ALLEGRO_BITMAP *frame = look_down_frameset[i].frame;
  int dx = look_down_frameset[i].dx;
  int dy = look_down_frameset[i].dy;

  next_frame (&princess.f, &princess.f, frame, dx, dy);

  if (i < 1) i++;
}
