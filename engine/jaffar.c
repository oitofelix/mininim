/*
  jaffar.c -- jaffar module;

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

#include "kernel/video.h"
#include "anim.h"
#include "jaffar.h"

struct anim jaffar;

#define WALK_FRAMESET_NMEMB 8
#define OPEN_ARMS_FRAMESET_NMEMB 2
#define RAISE_ARMS_FRAMESET_NMEMB 11
#define LOWER_ARMS_FRAMESET_NMEMB 6
#define TURN_WALK_FRAMESET_NMEMB 10

static struct frameset walk_frameset[WALK_FRAMESET_NMEMB];
static struct frameset open_arms_frameset[OPEN_ARMS_FRAMESET_NMEMB];
static struct frameset raise_arms_frameset[RAISE_ARMS_FRAMESET_NMEMB];
static struct frameset lower_arms_frameset[RAISE_ARMS_FRAMESET_NMEMB];
static struct frameset turn_walk_frameset[TURN_WALK_FRAMESET_NMEMB];

static void init_walk_frameset (void);
static void init_open_arms_frameset (void);
static void init_raise_arms_frameset (void);
static void init_lower_arms_frameset (void);
static void init_turn_walk_frameset (void);

/* variables */
ALLEGRO_BITMAP *jaffar_normal_00,
  *jaffar_walk_01, *jaffar_walk_02, *jaffar_walk_03, *jaffar_walk_04, *jaffar_walk_05,
  *jaffar_walk_06, *jaffar_walk_08, *jaffar_walk_09,
  *jaffar_turn_walk_10, *jaffar_turn_walk_11, *jaffar_turn_walk_12,
  *jaffar_turn_walk_13, *jaffar_turn_walk_14, *jaffar_turn_walk_15,
  *jaffar_turn_walk_16, *jaffar_turn_walk_17, *jaffar_turn_walk_18,
  *jaffar_turn_walk_19,
  *jaffar_open_arms_38, *jaffar_open_arms_20,
  *jaffar_raise_arms_21, *jaffar_raise_arms_22, *jaffar_raise_arms_23,
  *jaffar_raise_arms_24, *jaffar_raise_arms_25, *jaffar_raise_arms_26,
  *jaffar_raise_arms_27, *jaffar_raise_arms_28, *jaffar_raise_arms_36,
  *jaffar_raise_arms_37, *jaffar_raise_arms_29,
  *jaffar_lower_arms_30, *jaffar_lower_arms_31, *jaffar_lower_arms_32,
  *jaffar_lower_arms_33, *jaffar_lower_arms_34, *jaffar_lower_arms_35;




void
load_jaffar (void)
{
  jaffar_normal_00 = load_bitmap (JAFFAR_NORMAL_00);
  jaffar_walk_01 = load_bitmap (JAFFAR_WALK_01);
  jaffar_walk_02 = load_bitmap (JAFFAR_WALK_02);
  jaffar_walk_03 = load_bitmap (JAFFAR_WALK_03);
  jaffar_walk_04 = load_bitmap (JAFFAR_WALK_04);
  jaffar_walk_05 = load_bitmap (JAFFAR_WALK_05);
  jaffar_walk_06 = load_bitmap (JAFFAR_WALK_06);
  jaffar_walk_08 = load_bitmap (JAFFAR_WALK_09);
  jaffar_walk_09 = load_bitmap (JAFFAR_WALK_09);
  jaffar_turn_walk_10 = load_bitmap (JAFFAR_TURN_WALK_10);
  jaffar_turn_walk_11 = load_bitmap (JAFFAR_TURN_WALK_11);
  jaffar_turn_walk_12 = load_bitmap (JAFFAR_TURN_WALK_12);
  jaffar_turn_walk_13 = load_bitmap (JAFFAR_TURN_WALK_13);
  jaffar_turn_walk_14 = load_bitmap (JAFFAR_TURN_WALK_14);
  jaffar_turn_walk_15 = load_bitmap (JAFFAR_TURN_WALK_15);
  jaffar_turn_walk_16 = load_bitmap (JAFFAR_TURN_WALK_16);
  jaffar_turn_walk_17 = load_bitmap (JAFFAR_TURN_WALK_17);
  jaffar_turn_walk_18 = load_bitmap (JAFFAR_TURN_WALK_18);
  jaffar_turn_walk_19 = load_bitmap (JAFFAR_TURN_WALK_19);
  jaffar_open_arms_38 = load_bitmap (JAFFAR_OPEN_ARMS_38);
  jaffar_open_arms_20 = load_bitmap (JAFFAR_OPEN_ARMS_20);
  jaffar_raise_arms_21 = load_bitmap (JAFFAR_RAISE_ARMS_21);
  jaffar_raise_arms_22 = load_bitmap (JAFFAR_RAISE_ARMS_22);
  jaffar_raise_arms_23 = load_bitmap (JAFFAR_RAISE_ARMS_23);
  jaffar_raise_arms_24 = load_bitmap (JAFFAR_RAISE_ARMS_24);
  jaffar_raise_arms_25 = load_bitmap (JAFFAR_RAISE_ARMS_25);
  jaffar_raise_arms_26 = load_bitmap (JAFFAR_RAISE_ARMS_26);
  jaffar_raise_arms_27 = load_bitmap (JAFFAR_RAISE_ARMS_27);
  jaffar_raise_arms_28 = load_bitmap (JAFFAR_RAISE_ARMS_28);
  jaffar_raise_arms_36 = load_bitmap (JAFFAR_RAISE_ARMS_36);
  jaffar_raise_arms_37 = load_bitmap (JAFFAR_RAISE_ARMS_37);
  jaffar_raise_arms_29 = load_bitmap (JAFFAR_RAISE_ARMS_29);
  jaffar_lower_arms_30 = load_bitmap (JAFFAR_LOWER_ARMS_30);
  jaffar_lower_arms_31 = load_bitmap (JAFFAR_LOWER_ARMS_31);
  jaffar_lower_arms_32 = load_bitmap (JAFFAR_LOWER_ARMS_32);
  jaffar_lower_arms_33 = load_bitmap (JAFFAR_LOWER_ARMS_33);
  jaffar_lower_arms_34 = load_bitmap (JAFFAR_LOWER_ARMS_34);
  jaffar_lower_arms_35 = load_bitmap (JAFFAR_LOWER_ARMS_35);

  /* framesets */
  init_walk_frameset ();
  init_open_arms_frameset ();
  init_raise_arms_frameset ();
  init_lower_arms_frameset ();
  init_turn_walk_frameset ();
}

void
unload_jaffar (void)
{
  al_destroy_bitmap (jaffar_normal_00);
  al_destroy_bitmap (jaffar_walk_01);
  al_destroy_bitmap (jaffar_walk_02);
  al_destroy_bitmap (jaffar_walk_03);
  al_destroy_bitmap (jaffar_walk_04);
  al_destroy_bitmap (jaffar_walk_05);
  al_destroy_bitmap (jaffar_walk_06);
  al_destroy_bitmap (jaffar_walk_08);
  al_destroy_bitmap (jaffar_walk_09);
  al_destroy_bitmap (jaffar_turn_walk_10);
  al_destroy_bitmap (jaffar_turn_walk_11);
  al_destroy_bitmap (jaffar_turn_walk_12);
  al_destroy_bitmap (jaffar_turn_walk_13);
  al_destroy_bitmap (jaffar_turn_walk_14);
  al_destroy_bitmap (jaffar_turn_walk_15);
  al_destroy_bitmap (jaffar_turn_walk_16);
  al_destroy_bitmap (jaffar_turn_walk_17);
  al_destroy_bitmap (jaffar_turn_walk_18);
  al_destroy_bitmap (jaffar_turn_walk_19);
  al_destroy_bitmap (jaffar_open_arms_38);
  al_destroy_bitmap (jaffar_open_arms_20);
  al_destroy_bitmap (jaffar_raise_arms_21);
  al_destroy_bitmap (jaffar_raise_arms_22);
  al_destroy_bitmap (jaffar_raise_arms_23);
  al_destroy_bitmap (jaffar_raise_arms_24);
  al_destroy_bitmap (jaffar_raise_arms_25);
  al_destroy_bitmap (jaffar_raise_arms_26);
  al_destroy_bitmap (jaffar_raise_arms_27);
  al_destroy_bitmap (jaffar_raise_arms_28);
  al_destroy_bitmap (jaffar_raise_arms_36);
  al_destroy_bitmap (jaffar_raise_arms_37);
  al_destroy_bitmap (jaffar_raise_arms_29);
  al_destroy_bitmap (jaffar_lower_arms_30);
  al_destroy_bitmap (jaffar_lower_arms_31);
  al_destroy_bitmap (jaffar_lower_arms_32);
  al_destroy_bitmap (jaffar_lower_arms_33);
  al_destroy_bitmap (jaffar_lower_arms_34);
  al_destroy_bitmap (jaffar_lower_arms_35);
}



void
init_walk_frameset (void)
{
  struct frameset frameset[WALK_FRAMESET_NMEMB] =
    {{jaffar_walk_01,-2,0},{jaffar_walk_02,-4,0},{jaffar_walk_03,-13,0},
     {jaffar_walk_04,-1,0},{jaffar_walk_05,+2,0},{jaffar_walk_06,-1,0},
     {jaffar_walk_08,-2,0},{jaffar_walk_09,+0,0}};

  memcpy (&walk_frameset, &frameset,
          WALK_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_open_arms_frameset (void)
{
  struct frameset frameset[OPEN_ARMS_FRAMESET_NMEMB] =
    {{jaffar_open_arms_38,-2,0},{jaffar_open_arms_20,+0,0}};

  memcpy (&open_arms_frameset, &frameset,
          OPEN_ARMS_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_raise_arms_frameset (void)
{
  struct frameset frameset[RAISE_ARMS_FRAMESET_NMEMB] =
    {{jaffar_raise_arms_21,+0,0},{jaffar_raise_arms_22,+0,0},
     {jaffar_raise_arms_23,+2,0},{jaffar_raise_arms_24,-1,0},
     {jaffar_raise_arms_25,-5,0},{jaffar_raise_arms_26,+0,0},
     {jaffar_raise_arms_27,+9,0},{jaffar_raise_arms_28,-2,0},
     {jaffar_raise_arms_36,+1,0},{jaffar_raise_arms_37,+0,0},
     {jaffar_raise_arms_29,+0,0}};

  memcpy (&raise_arms_frameset, &frameset,
          RAISE_ARMS_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_lower_arms_frameset (void)
{
  struct frameset frameset[LOWER_ARMS_FRAMESET_NMEMB] =
    {{jaffar_lower_arms_30,+2,0},{jaffar_lower_arms_31,+0,0},
     {jaffar_lower_arms_32,-2,0},{jaffar_lower_arms_33,-2,0},
     {jaffar_lower_arms_34,+0,0},{jaffar_lower_arms_35,+7,0}};

  memcpy (&lower_arms_frameset, &frameset,
          LOWER_ARMS_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_turn_walk_frameset (void)
{
  struct frameset frameset[TURN_WALK_FRAMESET_NMEMB] =
    {{jaffar_turn_walk_10,-1,+1},{jaffar_turn_walk_11,+1,0},
     {jaffar_turn_walk_12,+0,0},{jaffar_turn_walk_13,-2,0},
     {jaffar_turn_walk_14,-1,0},{jaffar_turn_walk_15,-4,0},
     {jaffar_turn_walk_16,+2,0},{jaffar_turn_walk_17,+7,0},
     {jaffar_turn_walk_18,+3,0},{jaffar_turn_walk_19,+1,0}};

  memcpy (&turn_walk_frameset, &frameset,
          TURN_WALK_FRAMESET_NMEMB * sizeof (struct frameset));
}



void
jaffar_normal (struct anim *jaffar)
{
  jaffar->oaction = jaffar->action;
  jaffar->action = jaffar_normal;
  jaffar->f.flip = (jaffar->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  jaffar->fo.b = jaffar_normal_00;
  jaffar->fo.dx = jaffar->fo.dy = +0;

  if (jaffar->f.b == lower_arms_frameset[5].frame)
    jaffar->fo.dx = -2, jaffar->fo.dy = +0;

  next_frame (&jaffar->f, &jaffar->f, &jaffar->fo);
}

void
jaffar_walk (struct anim *jaffar)
{
  jaffar->oaction = jaffar->action;
  jaffar->action = jaffar_walk;
  jaffar->f.flip = (jaffar->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (jaffar->oaction != jaffar_walk) jaffar->i = -1;

  if (jaffar->i < 5) jaffar->i++;
  else if (jaffar->repeat > 0) jaffar->i = 0, jaffar->repeat--;
  else if (jaffar->i < 7) jaffar->i++;
  else {
    jaffar_normal (jaffar);
    return;
  }

  select_frame (jaffar, walk_frameset, jaffar->i);

  if (jaffar->f.b == jaffar_normal_00) jaffar->fo.dx = -4;
  if (jaffar->f.b == turn_walk_frameset[9].frame)
    jaffar->fo.dx = +17, jaffar->fo.dy = -1,
      next_frame_inv = true;

  next_frame (&jaffar->f, &jaffar->f, &jaffar->fo);

  if (next_frame_inv) next_frame_inv = false;
}

void
jaffar_open_arms (struct anim *jaffar)
{
  jaffar->oaction = jaffar->action;
  jaffar->action = jaffar_open_arms;
  jaffar->f.flip = (jaffar->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (jaffar->oaction != jaffar_open_arms) jaffar->i = -1;

  if (jaffar->i < 1) jaffar->i++;

  select_frame (jaffar, open_arms_frameset, jaffar->i);

  if (jaffar->i == 1 && jaffar->f.b == open_arms_frameset[0].frame)
    jaffar->fo.dx = -5;

  next_frame (&jaffar->f, &jaffar->f, &jaffar->fo);
}

void
jaffar_raise_arms (struct anim *jaffar)
{
  jaffar->oaction = jaffar->action;
  jaffar->action = jaffar_raise_arms;
  jaffar->f.flip = (jaffar->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (jaffar->oaction != jaffar_raise_arms) jaffar->i = -1;

  if (jaffar->i < 10) jaffar->i++;

  select_frame (jaffar, raise_arms_frameset, jaffar->i);

  next_frame (&jaffar->f, &jaffar->f, &jaffar->fo);
}

void
jaffar_lower_arms (struct anim *jaffar)
{
  jaffar->oaction = jaffar->action;
  jaffar->action = jaffar_lower_arms;
  jaffar->f.flip = (jaffar->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (jaffar->oaction != jaffar_lower_arms) jaffar->i = -1;

  if (jaffar->i < 5) jaffar->i++;
  else {
    jaffar_normal (jaffar);
    return;
  }

  select_frame (jaffar, lower_arms_frameset, jaffar->i);

  next_frame (&jaffar->f, &jaffar->f, &jaffar->fo);
}

void
jaffar_turn_walk (struct anim *jaffar)
{
  jaffar->oaction = jaffar->action;
  jaffar->action = jaffar_turn_walk;

  if (jaffar->oaction != jaffar_turn_walk) {
    jaffar->f.dir = (jaffar->f.dir == RIGHT) ? LEFT : RIGHT;
    jaffar->i = -1;
  }

  jaffar->f.flip = (jaffar->f.dir == RIGHT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

  if (jaffar->i < 9) jaffar->i++;
  else {
    jaffar_walk (jaffar);
    return;
  }

  select_frame (jaffar, turn_walk_frameset, jaffar->i);

  next_frame_inv = true;
  next_frame (&jaffar->f, &jaffar->f, &jaffar->fo);
  next_frame_inv = false;
}
