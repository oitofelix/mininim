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

#include "mininim.h"

struct anim princess;

#define TURN_FRAMESET_NMEMB 8
#define STEP_BACK_FRAMESET_NMEMB 6
#define LOOK_DOWN_FRAMESET_NMEMB 2
#define COUCH_FRAMESET_NMEMB 11
#define STROKE_FRAMESET_NMEMB 3
#define TURN_EMBRACE_FRAMESET_NMEMB 14

static struct frameset turn_frameset[TURN_FRAMESET_NMEMB];
static struct frameset step_back_frameset[STEP_BACK_FRAMESET_NMEMB];
static struct frameset look_down_frameset[LOOK_DOWN_FRAMESET_NMEMB];
static struct frameset couch_frameset[COUCH_FRAMESET_NMEMB];
static struct frameset stroke_frameset[STROKE_FRAMESET_NMEMB];
static struct frameset turn_embrace_frameset[TURN_EMBRACE_FRAMESET_NMEMB];

static void init_turn_frameset (void);
static void init_step_back_frameset (void);
static void init_look_down_frameset (void);
static void init_couch_frameset (void);
static void init_stroke_frameset (void);
static void init_turn_embrace_frameset (void);

/* variables */
ALLEGRO_BITMAP *princess_normal_00,
  *princess_turn_00, *princess_turn_01, *princess_turn_02, *princess_turn_03,
  *princess_turn_04, *princess_turn_05, *princess_turn_06, *princess_turn_07,
  *princess_step_back_00, *princess_step_back_01, *princess_step_back_02,
  *princess_step_back_03, *princess_step_back_04, *princess_step_back_05,
  *princess_look_down_00, *princess_look_down_01, *princess_rest_00,
  *princess_couch_00, *princess_couch_01, *princess_couch_02,
  *princess_couch_03, *princess_couch_04, *princess_couch_05,
  *princess_couch_06, *princess_couch_07, *princess_couch_08,
  *princess_couch_09, *princess_couch_10,
  *princess_stroke_00, *princess_stroke_01, *princess_stroke_02,
  *princess_turn_embrace_00, *princess_turn_embrace_01, *princess_turn_embrace_02,
  *princess_turn_embrace_03, *princess_turn_embrace_04, *princess_turn_embrace_05,
  *princess_turn_embrace_06, *princess_turn_embrace_07, *princess_turn_embrace_08,
  *princess_turn_embrace_09, *princess_turn_embrace_10, *princess_turn_embrace_11,
  *princess_turn_embrace_12, *princess_turn_embrace_13;

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
  princess_couch_00 = load_bitmap (PRINCESS_COUCH_00);
  princess_couch_01 = load_bitmap (PRINCESS_COUCH_01);
  princess_couch_02 = load_bitmap (PRINCESS_COUCH_02);
  princess_couch_03 = load_bitmap (PRINCESS_COUCH_03);
  princess_couch_04 = load_bitmap (PRINCESS_COUCH_04);
  princess_couch_05 = load_bitmap (PRINCESS_COUCH_05);
  princess_couch_06 = load_bitmap (PRINCESS_COUCH_06);
  princess_couch_07 = load_bitmap (PRINCESS_COUCH_07);
  princess_couch_08 = load_bitmap (PRINCESS_COUCH_08);
  princess_couch_09 = load_bitmap (PRINCESS_COUCH_09);
  princess_couch_10 = load_bitmap (PRINCESS_COUCH_10);
  princess_stroke_00 = load_bitmap (PRINCESS_STROKE_00);
  princess_stroke_01 = load_bitmap (PRINCESS_STROKE_01);
  princess_stroke_02 = load_bitmap (PRINCESS_STROKE_02);
  princess_turn_embrace_00 = load_bitmap (PRINCESS_TURN_EMBRACE_00);
  princess_turn_embrace_01 = load_bitmap (PRINCESS_TURN_EMBRACE_01);
  princess_turn_embrace_02 = load_bitmap (PRINCESS_TURN_EMBRACE_02);
  princess_turn_embrace_03 = load_bitmap (PRINCESS_TURN_EMBRACE_03);
  princess_turn_embrace_04 = load_bitmap (PRINCESS_TURN_EMBRACE_04);
  princess_turn_embrace_05 = load_bitmap (PRINCESS_TURN_EMBRACE_05);
  princess_turn_embrace_06 = load_bitmap (PRINCESS_TURN_EMBRACE_06);
  princess_turn_embrace_07 = load_bitmap (PRINCESS_TURN_EMBRACE_07);
  princess_turn_embrace_08 = load_bitmap (PRINCESS_TURN_EMBRACE_08);
  princess_turn_embrace_09 = load_bitmap (PRINCESS_TURN_EMBRACE_09);
  princess_turn_embrace_10 = load_bitmap (PRINCESS_TURN_EMBRACE_10);
  princess_turn_embrace_11 = load_bitmap (PRINCESS_TURN_EMBRACE_11);
  princess_turn_embrace_12 = load_bitmap (PRINCESS_TURN_EMBRACE_12);
  princess_turn_embrace_13 = load_bitmap (PRINCESS_TURN_EMBRACE_13);

  /* framesets */
  init_turn_frameset ();
  init_step_back_frameset ();
  init_look_down_frameset ();
  init_couch_frameset ();
  init_stroke_frameset ();
  init_turn_embrace_frameset ();
}

void
unload_princess (void)
{
  destroy_bitmap (princess_normal_00);
  destroy_bitmap (princess_turn_00);
  destroy_bitmap (princess_turn_01);
  destroy_bitmap (princess_turn_02);
  destroy_bitmap (princess_turn_03);
  destroy_bitmap (princess_turn_04);
  destroy_bitmap (princess_turn_05);
  destroy_bitmap (princess_turn_06);
  destroy_bitmap (princess_turn_07);
  destroy_bitmap (princess_step_back_00);
  destroy_bitmap (princess_step_back_01);
  destroy_bitmap (princess_step_back_02);
  destroy_bitmap (princess_step_back_03);
  destroy_bitmap (princess_step_back_04);
  destroy_bitmap (princess_step_back_05);
  destroy_bitmap (princess_look_down_00);
  destroy_bitmap (princess_look_down_01);
  destroy_bitmap (princess_rest_00);
  destroy_bitmap (princess_couch_00);
  destroy_bitmap (princess_couch_01);
  destroy_bitmap (princess_couch_02);
  destroy_bitmap (princess_couch_03);
  destroy_bitmap (princess_couch_04);
  destroy_bitmap (princess_couch_05);
  destroy_bitmap (princess_couch_06);
  destroy_bitmap (princess_couch_07);
  destroy_bitmap (princess_couch_08);
  destroy_bitmap (princess_couch_09);
  destroy_bitmap (princess_couch_10);
  destroy_bitmap (princess_stroke_00);
  destroy_bitmap (princess_stroke_01);
  destroy_bitmap (princess_stroke_02);
  destroy_bitmap (princess_turn_embrace_00);
  destroy_bitmap (princess_turn_embrace_01);
  destroy_bitmap (princess_turn_embrace_02);
  destroy_bitmap (princess_turn_embrace_03);
  destroy_bitmap (princess_turn_embrace_04);
  destroy_bitmap (princess_turn_embrace_05);
  destroy_bitmap (princess_turn_embrace_06);
  destroy_bitmap (princess_turn_embrace_07);
  destroy_bitmap (princess_turn_embrace_08);
  destroy_bitmap (princess_turn_embrace_09);
  destroy_bitmap (princess_turn_embrace_10);
  destroy_bitmap (princess_turn_embrace_11);
  destroy_bitmap (princess_turn_embrace_12);
  destroy_bitmap (princess_turn_embrace_13);
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
init_couch_frameset (void)
{
  struct frameset frameset[COUCH_FRAMESET_NMEMB] =
    {{princess_couch_00,+0,0},{princess_couch_01,-3,0},{princess_couch_02,+0,0},
     {princess_couch_03,+0,0},{princess_couch_04,-2,0},{princess_couch_05,+1,0},
     {princess_couch_06,+0,0},{princess_couch_07,+0,0},{princess_couch_08,+0,0},
     {princess_couch_09,-2,0},{princess_couch_10,+4,0}};

  memcpy (&couch_frameset, &frameset,
          COUCH_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_stroke_frameset (void)
{
  struct frameset frameset[STROKE_FRAMESET_NMEMB] =
    {{princess_stroke_00,+0,0},{princess_stroke_01,-2,0},
     {princess_stroke_02,-3,0}};

  memcpy (&stroke_frameset, &frameset,
          STROKE_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_turn_embrace_frameset (void)
{
  struct frameset frameset[TURN_EMBRACE_FRAMESET_NMEMB] =
    {{princess_turn_embrace_00,+0,0},{princess_turn_embrace_01,+0,0},
     {princess_turn_embrace_02,-1,0},{princess_turn_embrace_03,-2,0},
     {princess_turn_embrace_04,-6,0},{princess_turn_embrace_05,+1,0},
     {princess_turn_embrace_06,-8,0},{princess_turn_embrace_07,-8,0},
     {princess_turn_embrace_08,-25,0},{princess_turn_embrace_09,+2,0},
     {princess_turn_embrace_10,-1,0},{princess_turn_embrace_11,-2,0},
     {princess_turn_embrace_12,+2,0},{princess_turn_embrace_13,+0,0}};

  memcpy (&turn_embrace_frameset, &frameset,
          TURN_EMBRACE_FRAMESET_NMEMB * sizeof (struct frameset));
}


void
princess_normal (struct anim *princess)
{
  princess->oaction = princess->action;
  princess->action = princess_normal;
  princess->f.flip = (princess->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  princess->fo.b = princess_normal_00;
  princess->fo.dx = princess->fo.dy = +0;

  if (princess->oaction == princess_uncouch) princess->fo.dx = -1;

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

void
princess_couch (struct anim *princess)
{
  princess->oaction = princess->action;
  princess->action = princess_couch;
  princess->f.flip = (princess->f.dir == RIGHT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

  if (princess->oaction != princess_couch) {
    princess->i = -1;
    princess->j = 0;
  }

  if (princess->i == 10) princess->j = 1;
  if (princess->i < 10) princess->i++;

  select_frame (princess, couch_frameset, princess->i);

  if (princess->j) princess->fo.dx = 0;

  next_frame (&princess->f, &princess->f, &princess->fo);
}

void
princess_uncouch (struct anim *princess)
{
  princess->oaction = princess->action;
  princess->action = princess_uncouch;
  princess->f.flip = (princess->f.dir == RIGHT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

  if (princess->oaction != princess_uncouch) princess->i = 10;

  if (princess->i == 0) {
    princess_normal (princess);
    return;
  }

  princess->fo.b = couch_frameset[--princess->i].frame;
  princess->fo.dx = -couch_frameset[princess->i + 1].dx;
  princess->fo.dy = -couch_frameset[princess->i + 1].dy;

  next_frame (&princess->f, &princess->f, &princess->fo);
}

void
princess_stroke (struct anim *princess)
{
  princess->oaction = princess->action;
  princess->action = princess_stroke;
  princess->f.flip = (princess->f.dir == RIGHT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

  if (princess->oaction != princess_stroke) {
    princess->i = -1;
    princess->reverse = false;
    princess->wait = 0;
  }

  if (princess->wait++ % 3) return;

  if (princess->i == 2
      && ! princess->reverse)
    princess->reverse = true;

  if (princess->i == 0
      && princess->reverse)
    princess->reverse = false;

  if (princess->reverse) {
    princess->i--;
    princess->fo.b = stroke_frameset[princess->i].frame;
    princess->fo.dx = -stroke_frameset[princess->i + 1].dx;
    princess->fo.dy = -stroke_frameset[princess->i + 1].dy;
  } else {
    princess->i++;
    select_frame (princess, stroke_frameset, princess->i);
  }

  next_frame (&princess->f, &princess->f, &princess->fo);
}

void
princess_turn_embrace (struct anim *princess)
{
  princess->oaction = princess->action;
  princess->action = princess_turn_embrace;

  if (princess->oaction != princess_turn_embrace) {
    princess->f.dir = (princess->f.dir == RIGHT) ? LEFT : RIGHT;
    princess->i = -1;
  }

  princess->f.flip = (princess->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (princess->i < 13) princess->i++;

  select_frame (princess, turn_embrace_frameset, princess->i);
  next_frame_inv = true;
  next_frame (&princess->f, &princess->f, &princess->fo);
  next_frame_inv = false;
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
  pal = get_kid_palette (vm);
  f.b = apply_palette (f.b, pal);
  pal = get_princess_palette (vm);
  f.b = apply_palette (f.b, pal);

  if (hgc) f.b = apply_palette (f.b, hgc_palette);

  draw_frame (bitmap, &f);
}
