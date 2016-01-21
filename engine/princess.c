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
  *princess_turn_00, *princess_turn_01, *princess_turn_02, *princess_turn_03,
  *princess_turn_04, *princess_turn_05, *princess_turn_06, *princess_turn_07,
  *princess_step_back_00, *princess_step_back_01, *princess_step_back_02,
  *princess_step_back_03, *princess_step_back_04, *princess_step_back_05,
  *princess_look_down_00, *princess_look_down_01, *princess_rest_00;

void
load_princess (void)
{
  princess_normal_00 = load_bitmap (PRINCESS_NORMAL_00);
  princess_turn_00 = load_bitmap (PRINCESS_TURN_00);
  princess_turn_01 = load_bitmap (PRINCESS_TURN_01);
  princess_turn_02 = load_bitmap (PRINCESS_TURN_02);
  princess_turn_03 = load_bitmap (PRINCESS_TURN_03);
  princess_turn_04 = load_bitmap (PRINCESS_TURN_04);
  princess_turn_05 = load_bitmap (PRINCESS_TURN_05);
  princess_turn_06 = load_bitmap (PRINCESS_TURN_06);
  princess_turn_07 = load_bitmap (PRINCESS_TURN_07);
  princess_step_back_00 = load_bitmap (PRINCESS_STEP_BACK_00);
  princess_step_back_01 = load_bitmap (PRINCESS_STEP_BACK_01);
  princess_step_back_02 = load_bitmap (PRINCESS_STEP_BACK_02);
  princess_step_back_03 = load_bitmap (PRINCESS_STEP_BACK_03);
  princess_step_back_04 = load_bitmap (PRINCESS_STEP_BACK_04);
  princess_step_back_05 = load_bitmap (PRINCESS_STEP_BACK_05);
  princess_look_down_00 = load_bitmap (PRINCESS_LOOK_DOWN_00);
  princess_look_down_01 = load_bitmap (PRINCESS_LOOK_DOWN_01);
  princess_rest_00 = load_bitmap (PRINCESS_REST_00);

  /* framesets */
  init_turn_frameset ();
  init_step_back_frameset ();
  init_look_down_frameset ();
}

void
unload_princess (void)
{
  al_destroy_bitmap (princess_normal_00);
  al_destroy_bitmap (princess_turn_00);
  al_destroy_bitmap (princess_turn_01);
  al_destroy_bitmap (princess_turn_02);
  al_destroy_bitmap (princess_turn_03);
  al_destroy_bitmap (princess_turn_04);
  al_destroy_bitmap (princess_turn_05);
  al_destroy_bitmap (princess_turn_06);
  al_destroy_bitmap (princess_turn_07);
  al_destroy_bitmap (princess_step_back_00);
  al_destroy_bitmap (princess_step_back_01);
  al_destroy_bitmap (princess_step_back_02);
  al_destroy_bitmap (princess_step_back_03);
  al_destroy_bitmap (princess_step_back_04);
  al_destroy_bitmap (princess_step_back_05);
  al_destroy_bitmap (princess_look_down_00);
  al_destroy_bitmap (princess_look_down_01);
  al_destroy_bitmap (princess_rest_00);
}



void
init_turn_frameset (void)
{
  struct frameset frameset[TURN_FRAMESET_NMEMB] =
    {{princess_turn_00,+0,0},{princess_turn_01,-1,0},{princess_turn_02,+0,0},
     {princess_turn_03,+2,0},{princess_turn_04,-6,0},{princess_turn_05,-2,0},
     {princess_turn_06,+6,0},{princess_turn_07,-2,0}};

  memcpy (&turn_frameset, &frameset,
          TURN_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_step_back_frameset (void)
{
  struct frameset frameset[STEP_BACK_FRAMESET_NMEMB] =
    {{princess_step_back_00,-6,0},{princess_step_back_01,-4,0},
     {princess_step_back_02,-1,0},{princess_step_back_03,-7,0},
     {princess_step_back_04,-2,0},{princess_step_back_05,+0,0}};

  memcpy (&step_back_frameset, &frameset,
          STEP_BACK_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_look_down_frameset (void)
{
  struct frameset frameset[LOOK_DOWN_FRAMESET_NMEMB] =
    {{princess_look_down_00,+0,0},{princess_look_down_01,+0,0}};

  memcpy (&look_down_frameset, &frameset,
          LOOK_DOWN_FRAMESET_NMEMB * sizeof (struct frameset));
}



void
princess_normal (struct anim *princess)
{
  princess->oaction = princess->action;
  princess->action = princess_normal;
  princess->f.flip = (princess->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  princess->fo.b = princess_normal_00;
  princess->fo.dx = princess->fo.dy = +0;

  next_frame (&princess->f, &princess->f, &princess->fo);
}

void
princess_turn (struct anim *princess)
{
  princess->oaction = princess->action;
  princess->action = princess_turn;

  if (princess->oaction != princess_turn) {
    princess->f.dir = (princess->f.dir == RIGHT) ? LEFT : RIGHT;
    princess->i = -1;
  }

  princess->f.flip = (princess->f.dir == RIGHT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

  if (princess->i < 7) princess->i++;
  else {
    princess_normal (princess);
    return;
  }

  select_frame (princess, turn_frameset, princess->i);
  next_frame_inv = true;
  next_frame (&princess->f, &princess->f, &princess->fo);
  next_frame_inv = false;
}

void
princess_step_back (struct anim *princess)
{
  princess->oaction = princess->action;
  princess->action = princess_step_back;
  princess->f.flip = (princess->f.dir == RIGHT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

  if (princess->oaction != princess_step_back) princess->i = -1;

  if (princess->i < 5) princess->i++;

  select_frame (princess, step_back_frameset, princess->i);

  if (princess->i == 5
      && princess->f.b == step_back_frameset[4].frame) princess->fo.dx = +1;

  next_frame_inv = true;
  next_frame (&princess->f, &princess->f, &princess->fo);
  if (next_frame_inv) next_frame_inv = false;
}

void
princess_look_down (struct anim *princess)
{
  princess->oaction = princess->action;
  princess->action = princess_look_down;
  princess->f.flip = (princess->f.dir == RIGHT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

  if (princess->oaction != princess_look_down) princess->i = -1;

  if (princess->i < 1) princess->i++;

  select_frame (princess, look_down_frameset, princess->i);
  next_frame (&princess->f, &princess->f, &princess->fo);
}

void
princess_rest (struct anim *princess)
{
  princess->oaction = princess->action;
  princess->action = princess_rest;
  princess->f.flip = (princess->f.dir == RIGHT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

  princess->fo.b = princess_rest_00;
  princess->fo.dx = princess->fo.dy = +0;

  next_frame (&princess->f, &princess->f, &princess->fo);
}



ALLEGRO_COLOR
c_princess_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, PRINCESS_SKIN_COLOR_01)) return C_PRINCESS_SKIN_COLOR_01;
  if (color_eq (c, PRINCESS_SKIN_COLOR_02)) return C_PRINCESS_SKIN_COLOR_02;
  if (color_eq (c, PRINCESS_SKIN_COLOR_03)) return C_PRINCESS_SKIN_COLOR_03;
  if (color_eq (c, PRINCESS_DRESS_COLOR_01)) return C_PRINCESS_DRESS_COLOR_01;
  if (color_eq (c, PRINCESS_DRESS_COLOR_02)) return C_PRINCESS_DRESS_COLOR_02;
  if (color_eq (c, PRINCESS_DRESS_COLOR_03)) return C_PRINCESS_DRESS_COLOR_03;
  if (color_eq (c, PRINCESS_HAIR_COLOR_01)) return C_PRINCESS_HAIR_COLOR_01;
  if (color_eq (c, PRINCESS_HAIR_COLOR_02)) return C_PRINCESS_HAIR_COLOR_02;
  if (color_eq (c, PRINCESS_EYES_COLOR)) return C_PRINCESS_EYES_COLOR;
  return c;
}

ALLEGRO_COLOR
e_princess_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, PRINCESS_SKIN_COLOR_01)) return E_PRINCESS_SKIN_COLOR_01;
  if (color_eq (c, PRINCESS_SKIN_COLOR_02)) return E_PRINCESS_SKIN_COLOR_02;
  if (color_eq (c, PRINCESS_SKIN_COLOR_03)) return E_PRINCESS_SKIN_COLOR_03;
  if (color_eq (c, PRINCESS_DRESS_COLOR_01)) return E_PRINCESS_DRESS_COLOR_01;
  if (color_eq (c, PRINCESS_DRESS_COLOR_02)) return E_PRINCESS_DRESS_COLOR_02;
  if (color_eq (c, PRINCESS_DRESS_COLOR_03)) return E_PRINCESS_DRESS_COLOR_03;
  if (color_eq (c, PRINCESS_HAIR_COLOR_01)) return E_PRINCESS_HAIR_COLOR_01;
  if (color_eq (c, PRINCESS_HAIR_COLOR_02)) return E_PRINCESS_HAIR_COLOR_02;
  if (color_eq (c, PRINCESS_EYES_COLOR)) return E_PRINCESS_EYES_COLOR;
  return c;
}

ALLEGRO_COLOR
v_princess_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, PRINCESS_SKIN_COLOR_01)) return V_PRINCESS_SKIN_COLOR_01;
  if (color_eq (c, PRINCESS_SKIN_COLOR_02)) return V_PRINCESS_SKIN_COLOR_02;
  if (color_eq (c, PRINCESS_SKIN_COLOR_03)) return V_PRINCESS_SKIN_COLOR_03;
  if (color_eq (c, PRINCESS_DRESS_COLOR_01)) return V_PRINCESS_DRESS_COLOR_01;
  if (color_eq (c, PRINCESS_DRESS_COLOR_02)) return V_PRINCESS_DRESS_COLOR_02;
  if (color_eq (c, PRINCESS_DRESS_COLOR_03)) return V_PRINCESS_DRESS_COLOR_03;
  if (color_eq (c, PRINCESS_HAIR_COLOR_01)) return V_PRINCESS_HAIR_COLOR_01;
  if (color_eq (c, PRINCESS_HAIR_COLOR_02)) return V_PRINCESS_HAIR_COLOR_02;
  if (color_eq (c, PRINCESS_EYES_COLOR)) return V_PRINCESS_EYES_COLOR;
  return c;
}

palette
get_princess_palette (enum vm vm)
{
  switch (vm) {
  case CGA: return c_princess_palette;
  case EGA: return e_princess_palette;
  case VGA: return v_princess_palette;
  }
  return NULL;
}

void
draw_princess_frame (ALLEGRO_BITMAP *bitmap, struct anim *p, enum vm vm)
{
  if (p->invisible) return;

  struct frame f = p->f;

  palette pal = NULL;
  pal = get_princess_palette (vm);
  f.b = apply_palette (f.b, pal);

  if (hgc) f.b = apply_palette (f.b, hgc_palette);

  draw_frame (bitmap, &f);
}
