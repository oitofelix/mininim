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

#include "mininim.h"

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
  *jaffar_walk_00, *jaffar_walk_01, *jaffar_walk_02, *jaffar_walk_03, *jaffar_walk_04,
  *jaffar_walk_05, *jaffar_walk_06, *jaffar_walk_07,
  *jaffar_turn_walk_00, *jaffar_turn_walk_01, *jaffar_turn_walk_02,
  *jaffar_turn_walk_03, *jaffar_turn_walk_04, *jaffar_turn_walk_05,
  *jaffar_turn_walk_06, *jaffar_turn_walk_07, *jaffar_turn_walk_08,
  *jaffar_turn_walk_09,
  *jaffar_open_arms_00, *jaffar_open_arms_01,
  *jaffar_raise_arms_00, *jaffar_raise_arms_01, *jaffar_raise_arms_02,
  *jaffar_raise_arms_03, *jaffar_raise_arms_04, *jaffar_raise_arms_05,
  *jaffar_raise_arms_06, *jaffar_raise_arms_07, *jaffar_raise_arms_08,
  *jaffar_raise_arms_09, *jaffar_raise_arms_10,
  *jaffar_lower_arms_00, *jaffar_lower_arms_01, *jaffar_lower_arms_02,
  *jaffar_lower_arms_03, *jaffar_lower_arms_04, *jaffar_lower_arms_05;




void
load_jaffar (void)
{
  jaffar_normal_00 = load_bitmap (JAFFAR_NORMAL_00);
  jaffar_walk_00 = load_bitmap (JAFFAR_WALK_00);
  jaffar_walk_01 = load_bitmap (JAFFAR_WALK_01);
  jaffar_walk_02 = load_bitmap (JAFFAR_WALK_02);
  jaffar_walk_03 = load_bitmap (JAFFAR_WALK_03);
  jaffar_walk_04 = load_bitmap (JAFFAR_WALK_04);
  jaffar_walk_05 = load_bitmap (JAFFAR_WALK_05);
  jaffar_walk_06 = load_bitmap (JAFFAR_WALK_07);
  jaffar_walk_07 = load_bitmap (JAFFAR_WALK_07);
  jaffar_turn_walk_00 = load_bitmap (JAFFAR_TURN_WALK_00);
  jaffar_turn_walk_01 = load_bitmap (JAFFAR_TURN_WALK_01);
  jaffar_turn_walk_02 = load_bitmap (JAFFAR_TURN_WALK_02);
  jaffar_turn_walk_03 = load_bitmap (JAFFAR_TURN_WALK_03);
  jaffar_turn_walk_04 = load_bitmap (JAFFAR_TURN_WALK_04);
  jaffar_turn_walk_05 = load_bitmap (JAFFAR_TURN_WALK_05);
  jaffar_turn_walk_06 = load_bitmap (JAFFAR_TURN_WALK_06);
  jaffar_turn_walk_07 = load_bitmap (JAFFAR_TURN_WALK_07);
  jaffar_turn_walk_08 = load_bitmap (JAFFAR_TURN_WALK_08);
  jaffar_turn_walk_09 = load_bitmap (JAFFAR_TURN_WALK_09);
  jaffar_open_arms_00 = load_bitmap (JAFFAR_OPEN_ARMS_00);
  jaffar_open_arms_01 = load_bitmap (JAFFAR_OPEN_ARMS_01);
  jaffar_raise_arms_00 = load_bitmap (JAFFAR_RAISE_ARMS_00);
  jaffar_raise_arms_01 = load_bitmap (JAFFAR_RAISE_ARMS_01);
  jaffar_raise_arms_02 = load_bitmap (JAFFAR_RAISE_ARMS_02);
  jaffar_raise_arms_03 = load_bitmap (JAFFAR_RAISE_ARMS_03);
  jaffar_raise_arms_04 = load_bitmap (JAFFAR_RAISE_ARMS_04);
  jaffar_raise_arms_05 = load_bitmap (JAFFAR_RAISE_ARMS_05);
  jaffar_raise_arms_06 = load_bitmap (JAFFAR_RAISE_ARMS_06);
  jaffar_raise_arms_07 = load_bitmap (JAFFAR_RAISE_ARMS_07);
  jaffar_raise_arms_08 = load_bitmap (JAFFAR_RAISE_ARMS_08);
  jaffar_raise_arms_09 = load_bitmap (JAFFAR_RAISE_ARMS_09);
  jaffar_raise_arms_10 = load_bitmap (JAFFAR_RAISE_ARMS_10);
  jaffar_lower_arms_00 = load_bitmap (JAFFAR_LOWER_ARMS_00);
  jaffar_lower_arms_01 = load_bitmap (JAFFAR_LOWER_ARMS_01);
  jaffar_lower_arms_02 = load_bitmap (JAFFAR_LOWER_ARMS_02);
  jaffar_lower_arms_03 = load_bitmap (JAFFAR_LOWER_ARMS_03);
  jaffar_lower_arms_04 = load_bitmap (JAFFAR_LOWER_ARMS_04);
  jaffar_lower_arms_05 = load_bitmap (JAFFAR_LOWER_ARMS_05);

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
  destroy_bitmap (jaffar_normal_00);
  destroy_bitmap (jaffar_walk_00);
  destroy_bitmap (jaffar_walk_01);
  destroy_bitmap (jaffar_walk_02);
  destroy_bitmap (jaffar_walk_03);
  destroy_bitmap (jaffar_walk_04);
  destroy_bitmap (jaffar_walk_05);
  destroy_bitmap (jaffar_walk_06);
  destroy_bitmap (jaffar_walk_07);
  destroy_bitmap (jaffar_turn_walk_00);
  destroy_bitmap (jaffar_turn_walk_01);
  destroy_bitmap (jaffar_turn_walk_02);
  destroy_bitmap (jaffar_turn_walk_03);
  destroy_bitmap (jaffar_turn_walk_04);
  destroy_bitmap (jaffar_turn_walk_05);
  destroy_bitmap (jaffar_turn_walk_06);
  destroy_bitmap (jaffar_turn_walk_07);
  destroy_bitmap (jaffar_turn_walk_08);
  destroy_bitmap (jaffar_turn_walk_09);
  destroy_bitmap (jaffar_open_arms_00);
  destroy_bitmap (jaffar_open_arms_01);
  destroy_bitmap (jaffar_raise_arms_00);
  destroy_bitmap (jaffar_raise_arms_01);
  destroy_bitmap (jaffar_raise_arms_02);
  destroy_bitmap (jaffar_raise_arms_03);
  destroy_bitmap (jaffar_raise_arms_04);
  destroy_bitmap (jaffar_raise_arms_05);
  destroy_bitmap (jaffar_raise_arms_06);
  destroy_bitmap (jaffar_raise_arms_07);
  destroy_bitmap (jaffar_raise_arms_08);
  destroy_bitmap (jaffar_raise_arms_09);
  destroy_bitmap (jaffar_raise_arms_10);
  destroy_bitmap (jaffar_lower_arms_00);
  destroy_bitmap (jaffar_lower_arms_01);
  destroy_bitmap (jaffar_lower_arms_02);
  destroy_bitmap (jaffar_lower_arms_03);
  destroy_bitmap (jaffar_lower_arms_04);
  destroy_bitmap (jaffar_lower_arms_05);
}



void
init_walk_frameset (void)
{
  struct frameset frameset[WALK_FRAMESET_NMEMB] =
    {{jaffar_walk_00,-2,0},{jaffar_walk_01,-4,0},{jaffar_walk_02,-13,0},
     {jaffar_walk_03,-1,0},{jaffar_walk_04,+2,0},{jaffar_walk_05,-1,0},
     {jaffar_walk_06,-2,0},{jaffar_walk_07,+0,0}};

  memcpy (&walk_frameset, &frameset,
          WALK_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_open_arms_frameset (void)
{
  struct frameset frameset[OPEN_ARMS_FRAMESET_NMEMB] =
    {{jaffar_open_arms_00,-2,0},{jaffar_open_arms_01,+0,0}};

  memcpy (&open_arms_frameset, &frameset,
          OPEN_ARMS_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_raise_arms_frameset (void)
{
  struct frameset frameset[RAISE_ARMS_FRAMESET_NMEMB] =
    {{jaffar_raise_arms_00,+0,0},{jaffar_raise_arms_01,+0,0},
     {jaffar_raise_arms_02,+2,0},{jaffar_raise_arms_03,-1,0},
     {jaffar_raise_arms_04,-5,0},{jaffar_raise_arms_05,+0,0},
     {jaffar_raise_arms_06,+9,0},{jaffar_raise_arms_07,-2,0},
     {jaffar_raise_arms_08,+1,0},{jaffar_raise_arms_09,+0,0},
     {jaffar_raise_arms_10,+0,0}};

  memcpy (&raise_arms_frameset, &frameset,
          RAISE_ARMS_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_lower_arms_frameset (void)
{
  struct frameset frameset[LOWER_ARMS_FRAMESET_NMEMB] =
    {{jaffar_lower_arms_00,+2,0},{jaffar_lower_arms_01,+0,0},
     {jaffar_lower_arms_02,-2,0},{jaffar_lower_arms_03,-2,0},
     {jaffar_lower_arms_04,+0,0},{jaffar_lower_arms_05,+7,0}};

  memcpy (&lower_arms_frameset, &frameset,
          LOWER_ARMS_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_turn_walk_frameset (void)
{
  struct frameset frameset[TURN_WALK_FRAMESET_NMEMB] =
    {{jaffar_turn_walk_00,-1,+1},{jaffar_turn_walk_01,+1,0},
     {jaffar_turn_walk_02,+0,0},{jaffar_turn_walk_03,-2,0},
     {jaffar_turn_walk_04,-1,0},{jaffar_turn_walk_05,-4,0},
     {jaffar_turn_walk_06,+2,0},{jaffar_turn_walk_07,+7,0},
     {jaffar_turn_walk_08,+3,0},{jaffar_turn_walk_09,+1,0}};

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



void
draw_jaffar_frame (ALLEGRO_BITMAP *bitmap, struct anim *j, enum vm vm)
{
  if (j->invisible) return;

  struct frame f = j->f;

  palette pal = NULL;
  pal = get_guard_palette (j->style, vm);
  f.b = apply_palette (f.b, pal);

  if (hgc) f.b = apply_palette (f.b, hgc_palette);

  draw_frame (bitmap, &f);
}
