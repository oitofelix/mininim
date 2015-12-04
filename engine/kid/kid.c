/*
  kid.c -- kid module;

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

#include <stdio.h>
#include <error.h>
#include "prince.h"
#include "kernel/video.h"
#include "kernel/audio.h"
#include "kernel/keyboard.h"
#include "engine/anim.h"
#include "engine/physics.h"
#include "engine/level.h"
#include "engine/room.h"
#include "engine/door.h"
#include "engine/loose-floor.h"
#include "engine/opener-floor.h"
#include "engine/closer-floor.h"
#include "engine/potion.h"
#include "engine/sword.h"
#include "kid.h"

struct anim kid;

bool misstep = false;
bool uncouch_slowly = false;
bool critical_edge = false;
int inertia = 0;
struct pos item_pos = {.room = -1};
bool hang_limit;
bool turn = false;
bool keep_sword_fast = false;

int kid_total_lives = KID_INITIAL_TOTAL_LIVES,
  kid_current_lives = KID_INITIAL_CURRENT_LIVES;

struct frameset kid_drink_frameset[KID_DRINK_FRAMESET_NMEMB];
struct frameset kid_raise_sword_frameset[KID_RAISE_SWORD_FRAMESET_NMEMB];
struct frameset kid_keep_sword_frameset[KID_KEEP_SWORD_FRAMESET_NMEMB];
struct frameset kid_take_sword_frameset[KID_TAKE_SWORD_FRAMESET_NMEMB];
struct frameset kid_sword_walkf_frameset[KID_SWORD_WALKF_FRAMESET_NMEMB];
struct frameset kid_sword_walkb_frameset[KID_SWORD_WALKB_FRAMESET_NMEMB];
struct frameset kid_sword_defense_frameset[KID_SWORD_DEFENSE_FRAMESET_NMEMB];
struct frameset kid_sword_attack_frameset[KID_SWORD_ATTACK_FRAMESET_NMEMB];

static void init_kid_drink_frameset (void);
static void init_kid_raise_sword_frameset (void);
static void init_kid_keep_sword_frameset (void);
static void init_kid_take_sword_frameset (void);
static void init_kid_sword_walkf_frameset (void);
static void init_kid_sword_walkb_frameset (void);
static void init_kid_sword_defense_frameset (void);
static void init_kid_sword_attack_frameset (void);

ALLEGRO_BITMAP *kid_full_life, *kid_empty_life,
  *kid_drink_01, *kid_drink_02, *kid_drink_03, *kid_drink_04, *kid_drink_05,
  *kid_drink_06, *kid_drink_07, *kid_drink_08, *kid_drink_09, *kid_drink_10,
  *kid_drink_11, *kid_drink_12, *kid_drink_13, *kid_drink_14, *kid_drink_15,
  *kid_raise_sword_01, *kid_raise_sword_02, *kid_raise_sword_03, *kid_raise_sword_04,
  *kid_keep_sword_01, *kid_keep_sword_02, *kid_keep_sword_03, *kid_keep_sword_04,
  *kid_keep_sword_05, *kid_keep_sword_06, *kid_keep_sword_07, *kid_keep_sword_08,
  *kid_keep_sword_09, *kid_keep_sword_10,
  *kid_take_sword_01, *kid_take_sword_02, *kid_take_sword_03, *kid_take_sword_04,
  *kid_sword_normal_08,
  *kid_sword_walkf_14, *kid_sword_walkf_15,
  *kid_sword_walkb_10, *kid_sword_walkb_07,
  *kid_sword_defense_18, *kid_sword_defense_11,
  *kid_sword_attack_01, *kid_sword_attack_02, *kid_sword_attack_03,
  *kid_sword_attack_04, *kid_sword_attack_05, *kid_sword_attack_06,
  *kid_sword_attack_07;

ALLEGRO_SAMPLE *step_sample, *hit_ground_sample, *hit_wall_sample,
  *hang_on_fall_sample, *drink_sample, *glory_sample,
  *take_sword_sample, *sword_attack_sample;

bool sample_step, sample_hit_ground, sample_hit_wall,
  sample_hang_on_fall, sample_drink, sample_glory,
  sample_take_sword, sample_sword_attack;

static void place_kid (int room, int floor, int place);
static struct coord *kid_life_coord (int i, struct coord *c);

void
load_kid (void)
{
  load_kid_normal ();
  load_kid_walk ();
  load_kid_start_run ();
  load_kid_run ();
  load_kid_stop_run ();
  load_kid_turn_run ();
  load_kid_couch ();
  load_kid_turn ();
  load_kid_stabilize ();
  load_kid_jump ();
  load_kid_vjump ();
  load_kid_run_jump ();
  load_kid_misstep ();
  load_kid_hang ();
  load_kid_hang_wall ();
  load_kid_hang_free ();
  load_kid_climb ();
  load_kid_unclimb ();
  load_kid_fall ();

  /* bitmap */
  kid_full_life = load_bitmap (KID_FULL_LIFE);
  kid_empty_life = load_bitmap (KID_EMPTY_LIFE);
  kid_drink_01 = load_bitmap (KID_DRINK_01);
  kid_drink_02 = load_bitmap (KID_DRINK_02);
  kid_drink_03 = load_bitmap (KID_DRINK_03);
  kid_drink_04 = load_bitmap (KID_DRINK_04);
  kid_drink_05 = load_bitmap (KID_DRINK_05);
  kid_drink_06 = load_bitmap (KID_DRINK_06);
  kid_drink_07 = load_bitmap (KID_DRINK_07);
  kid_drink_08 = load_bitmap (KID_DRINK_08);
  kid_drink_09 = load_bitmap (KID_DRINK_09);
  kid_drink_10 = load_bitmap (KID_DRINK_10);
  kid_drink_11 = load_bitmap (KID_DRINK_11);
  kid_drink_12 = load_bitmap (KID_DRINK_12);
  kid_drink_13 = load_bitmap (KID_DRINK_13);
  kid_drink_14 = load_bitmap (KID_DRINK_14);
  kid_drink_15 = load_bitmap (KID_DRINK_15);
  kid_raise_sword_01 = load_bitmap (KID_RAISE_SWORD_01);
  kid_raise_sword_02 = load_bitmap (KID_RAISE_SWORD_02);
  kid_raise_sword_03 = load_bitmap (KID_RAISE_SWORD_03);
  kid_raise_sword_04 = load_bitmap (KID_RAISE_SWORD_04);
  kid_keep_sword_01 = load_bitmap (KID_KEEP_SWORD_01);
  kid_keep_sword_02 = load_bitmap (KID_KEEP_SWORD_02);
  kid_keep_sword_03 = load_bitmap (KID_KEEP_SWORD_03);
  kid_keep_sword_04 = load_bitmap (KID_KEEP_SWORD_04);
  kid_keep_sword_05 = load_bitmap (KID_KEEP_SWORD_05);
  kid_keep_sword_06 = load_bitmap (KID_KEEP_SWORD_06);
  kid_keep_sword_07 = load_bitmap (KID_KEEP_SWORD_07);
  kid_keep_sword_08 = load_bitmap (KID_KEEP_SWORD_08);
  kid_keep_sword_09 = load_bitmap (KID_KEEP_SWORD_09);
  kid_keep_sword_10 = load_bitmap (KID_KEEP_SWORD_10);
  kid_take_sword_01 = load_bitmap (KID_TAKE_SWORD_01);
  kid_take_sword_02 = load_bitmap (KID_TAKE_SWORD_02);
  kid_take_sword_03 = load_bitmap (KID_TAKE_SWORD_03);
  kid_take_sword_04 = load_bitmap (KID_TAKE_SWORD_04);
  kid_sword_normal_08 = load_bitmap (KID_SWORD_NORMAL_08);
  kid_sword_walkf_14 = load_bitmap (KID_SWORD_WALKF_14);
  kid_sword_walkf_15 = load_bitmap (KID_SWORD_WALKF_15);
  kid_sword_walkb_10 = load_bitmap (KID_SWORD_WALKB_10);
  kid_sword_walkb_07 = load_bitmap (KID_SWORD_WALKB_07);
  kid_sword_defense_18 = load_bitmap (KID_SWORD_DEFENSE_18);
  kid_sword_defense_11 = load_bitmap (KID_SWORD_DEFENSE_11);
  kid_sword_attack_01 = load_bitmap (KID_SWORD_ATTACK_01);
  kid_sword_attack_02 = load_bitmap (KID_SWORD_ATTACK_02);
  kid_sword_attack_03 = load_bitmap (KID_SWORD_ATTACK_03);
  kid_sword_attack_04 = load_bitmap (KID_SWORD_ATTACK_04);
  kid_sword_attack_05 = load_bitmap (KID_SWORD_ATTACK_05);
  kid_sword_attack_06 = load_bitmap (KID_SWORD_ATTACK_06);
  kid_sword_attack_07 = load_bitmap (KID_SWORD_ATTACK_07);

  /* sound */
  step_sample = load_sample (STEP_SAMPLE);
  hit_ground_sample = load_sample (HIT_GROUND_SAMPLE);
  hit_wall_sample = load_sample (HIT_WALL_SAMPLE);
  hang_on_fall_sample = load_sample (HANG_ON_FALL_SAMPLE);
  drink_sample = load_sample (DRINK_SAMPLE);
  glory_sample = load_sample (GLORY_SAMPLE);
  take_sword_sample = load_sample (TAKE_SWORD_SAMPLE);
  sword_attack_sample = load_sample (SWORD_ATTACK_SAMPLE);

  /* framesets */
  init_kid_drink_frameset ();
  init_kid_raise_sword_frameset ();
  init_kid_keep_sword_frameset ();
  init_kid_take_sword_frameset ();
  init_kid_sword_walkf_frameset ();
  init_kid_sword_walkb_frameset ();
  init_kid_sword_defense_frameset ();
  init_kid_sword_attack_frameset ();

  /* kid himself */
  kid.f.id = &kid;
  kid.f.b = kid_normal_00;
  kid.f.dir = LEFT;
  kid.action = kid_normal;
  update_depressible_floor (&kid, -4, -10);

place_kid (1, 0, 0);
}

void
unload_kid (void)
{
  unload_kid_normal ();
  unload_kid_walk ();
  unload_kid_start_run ();
  unload_kid_run ();
  unload_kid_stop_run ();
  unload_kid_turn_run ();
  unload_kid_couch ();
  unload_kid_turn ();
  unload_kid_stabilize ();
  unload_kid_jump ();
  unload_kid_vjump ();
  unload_kid_run_jump ();
  unload_kid_misstep ();
  unload_kid_hang ();
  unload_kid_hang_wall ();
  unload_kid_hang_free ();
  unload_kid_climb ();
  unload_kid_unclimb ();

  /* bitmaps */
  al_destroy_bitmap (kid_full_life);
  al_destroy_bitmap (kid_empty_life);
  al_destroy_bitmap (kid_drink_01);
  al_destroy_bitmap (kid_drink_02);
  al_destroy_bitmap (kid_drink_03);
  al_destroy_bitmap (kid_drink_04);
  al_destroy_bitmap (kid_drink_05);
  al_destroy_bitmap (kid_drink_06);
  al_destroy_bitmap (kid_drink_07);
  al_destroy_bitmap (kid_drink_08);
  al_destroy_bitmap (kid_drink_09);
  al_destroy_bitmap (kid_drink_10);
  al_destroy_bitmap (kid_drink_11);
  al_destroy_bitmap (kid_drink_12);
  al_destroy_bitmap (kid_drink_13);
  al_destroy_bitmap (kid_drink_14);
  al_destroy_bitmap (kid_drink_15);
  al_destroy_bitmap (kid_raise_sword_01);
  al_destroy_bitmap (kid_raise_sword_02);
  al_destroy_bitmap (kid_raise_sword_03);
  al_destroy_bitmap (kid_raise_sword_04);
  al_destroy_bitmap (kid_keep_sword_01);
  al_destroy_bitmap (kid_keep_sword_02);
  al_destroy_bitmap (kid_keep_sword_03);
  al_destroy_bitmap (kid_keep_sword_04);
  al_destroy_bitmap (kid_keep_sword_05);
  al_destroy_bitmap (kid_keep_sword_06);
  al_destroy_bitmap (kid_keep_sword_07);
  al_destroy_bitmap (kid_keep_sword_08);
  al_destroy_bitmap (kid_keep_sword_09);
  al_destroy_bitmap (kid_keep_sword_10);
  al_destroy_bitmap (kid_take_sword_01);
  al_destroy_bitmap (kid_take_sword_02);
  al_destroy_bitmap (kid_take_sword_03);
  al_destroy_bitmap (kid_take_sword_04);
  al_destroy_bitmap (kid_sword_normal_08);
  al_destroy_bitmap (kid_sword_walkf_14);
  al_destroy_bitmap (kid_sword_walkf_15);
  al_destroy_bitmap (kid_sword_walkb_10);
  al_destroy_bitmap (kid_sword_walkb_07);
  al_destroy_bitmap (kid_sword_defense_18);
  al_destroy_bitmap (kid_sword_defense_11);
  al_destroy_bitmap (kid_sword_attack_01);
  al_destroy_bitmap (kid_sword_attack_02);
  al_destroy_bitmap (kid_sword_attack_03);
  al_destroy_bitmap (kid_sword_attack_04);
  al_destroy_bitmap (kid_sword_attack_05);
  al_destroy_bitmap (kid_sword_attack_06);
  al_destroy_bitmap (kid_sword_attack_07);

  /* sounds */
  al_destroy_sample (step_sample);
  al_destroy_sample (hit_ground_sample);
  al_destroy_sample (hit_wall_sample);
  al_destroy_sample (hang_on_fall_sample);
  al_destroy_sample (drink_sample);
  al_destroy_sample (glory_sample);
  al_destroy_sample (take_sword_sample);
}

void
place_kid (int room, int floor, int place)
{
  struct pos p;

  for (p.room = room; p.room < ROOMS; p.room++)
    for (p.floor = floor; p.floor < FLOORS; p.floor++)
      for (p.place = place; p.place < PLACES; p.place++)
        if (con (&p)->fg == FLOOR
            || con (&p)->fg == BROKEN_FLOOR
            || con (&p)->fg == SKELETON_FLOOR
            || con (&p)->fg == DOOR) goto end;

  enum confg tl = crel (&p, 0, -1)->fg;
  enum confg tr = crel (&p, 0, +1)->fg;

 end:
  if (kid.f.dir == LEFT
      && tl == WALL
      && tl == DOOR) kid.f.dir = RIGHT;

  if (kid.f.dir == RIGHT
      && tr == WALL
      && tr == DOOR) kid.f.dir = LEFT;

  kid.f.c.room = p.room;
  kid.f.c.x = PLACE_WIDTH * p.place + 15;
  kid.f.c.y = PLACE_HEIGHT * p.floor + 15;
}


void
sample_kid (void)
{
  if (sample_step) play_sample (step_sample);
  if (sample_hit_ground) play_sample (hit_ground_sample);
  if (sample_hit_wall) play_sample (hit_wall_sample);
  if (sample_hang_on_fall) play_sample (hang_on_fall_sample);
  if (sample_drink) play_sample (drink_sample);
  if (sample_glory) play_sample (glory_sample);
  if (sample_take_sword) play_sample (take_sword_sample);
  if (sample_sword_attack) play_sample (sword_attack_sample);

  sample_step = sample_hit_ground = sample_hit_wall =
    sample_hang_on_fall = sample_drink = sample_glory =
    sample_take_sword = sample_sword_attack = false;
}



void
init_kid_drink_frameset (void)
{
  struct frameset frameset[KID_DRINK_FRAMESET_NMEMB] =
    {{kid_drink_01,-7,0},{kid_drink_02,+1,+1},{kid_drink_03,+1,-1},
     {kid_drink_04,+0,0},{kid_drink_05,+2,0},{kid_drink_06,-1,0},
     {kid_drink_07,+1,0},{kid_drink_08,+6,0},{kid_drink_09,-1,0},
     {kid_drink_10,+2,-1},{kid_drink_11,-2,+1},{kid_drink_12,+0,-1},
     {kid_drink_13,-1,0},{kid_drink_14,+1,0},{kid_drink_15,+1,0}};

  memcpy (&kid_drink_frameset, &frameset,
          KID_DRINK_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_kid_raise_sword_frameset (void)
{
  struct frameset frameset[KID_RAISE_SWORD_FRAMESET_NMEMB] =
    {{kid_raise_sword_01,-4,0},{kid_raise_sword_02,+0,0},
     {kid_raise_sword_03,+1,0},{kid_raise_sword_04,-1,0}};

  memcpy (&kid_raise_sword_frameset, &frameset,
          KID_RAISE_SWORD_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_kid_keep_sword_frameset (void)
{
  struct frameset frameset[KID_KEEP_SWORD_FRAMESET_NMEMB] =
    {{kid_keep_sword_01,+2,0},{kid_keep_sword_02,-2,0},{kid_keep_sword_03,+0,0},
     {kid_keep_sword_04,+1,0},{kid_keep_sword_05,-6,0},{kid_keep_sword_06,+2,0},
     {kid_keep_sword_07,+3,0},{kid_keep_sword_08,+0,0},{kid_keep_sword_09,+3,0},
     {kid_keep_sword_10,+0,0}};

  memcpy (&kid_keep_sword_frameset, &frameset,
          KID_KEEP_SWORD_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_kid_take_sword_frameset (void)
{
  struct frameset frameset[KID_TAKE_SWORD_FRAMESET_NMEMB] =
    {{kid_take_sword_01,-6,0},{kid_take_sword_02,+0,0},
     {kid_take_sword_03,-4,0},{kid_take_sword_04,-6,0}};

  memcpy (&kid_take_sword_frameset, &frameset,
          KID_TAKE_SWORD_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_kid_sword_walkf_frameset (void)
{
  struct frameset frameset[KID_SWORD_WALKF_FRAMESET_NMEMB] =
    {{kid_sword_walkf_14,-14,0},{kid_sword_walkf_15,-4,0}};

  memcpy (&kid_sword_walkf_frameset, &frameset,
          KID_SWORD_WALKF_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_kid_sword_walkb_frameset (void)
{
  struct frameset frameset[KID_SWORD_WALKB_FRAMESET_NMEMB] =
    {{kid_sword_walkb_10,-1,0},{kid_sword_walkb_07,+11,0}};

  memcpy (&kid_sword_walkb_frameset, &frameset,
          KID_SWORD_WALKB_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_kid_sword_defense_frameset (void)
{
  struct frameset frameset[KID_SWORD_DEFENSE_FRAMESET_NMEMB] =
    {{kid_sword_defense_18,+0,0},{kid_sword_defense_11,+0,0}};

  memcpy (&kid_sword_defense_frameset, &frameset,
          KID_SWORD_DEFENSE_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
init_kid_sword_attack_frameset (void)
{
  struct frameset frameset[KID_SWORD_ATTACK_FRAMESET_NMEMB] =
    {{kid_sword_attack_01,+1,0},{kid_sword_attack_02,-8,0},
     {kid_sword_attack_03,-8,0},{kid_sword_attack_04,-9,0},
     {kid_sword_attack_05,+8,0},{kid_sword_attack_06,+8,0},
     {kid_sword_attack_07,+8,0}};

  memcpy (&kid_sword_attack_frameset, &frameset,
          KID_SWORD_ATTACK_FRAMESET_NMEMB * sizeof (struct frameset));
}



void
kid_drink (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_drink;
  kid.f.flip = (kid.f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  static bool reverse = false;
  static int i = 0, wait = 4;
  if (oaction != kid_drink) reverse = false, i = 0, wait = 4;

  ALLEGRO_BITMAP *frame = kid_drink_frameset[i].frame;
  int dx = kid_drink_frameset[i].dx;
  int dy = kid_drink_frameset[i].dy;

  if (i == 14 && wait < 4) dx = 0;
  if (i == 10 && reverse) dx = -2, dy = +1;
  if (i == 7 && reverse) dx = +1;

  next_frame (&kid.f, &kid.f, frame, dx, dy);

  /* depressible floors */
  keep_depressible_floor (&kid);

  /* sound */
  if (i == 7 && ! reverse) sample_drink = true;

  /* consume bottle */
  if (i == 0) con (&item_pos)->ext.item = NO_ITEM;

  /* next frame */
  if (i < 14 && ! reverse) i++;
  else if (wait > 0) wait--;
  else if (i == 14 && wait == 0) reverse = true, i = 10;
  else if (i == 10 && reverse) i = 7;
  else {
    kid.action = kid_normal;
    reverse = false, i = 0, wait = 4;
    item_pos.room = -1;
  }
}

void
kid_raise_sword (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_raise_sword;
  kid.f.flip = (kid.f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  static int i = 0, wait = 5, j = 0;
  if (oaction != kid_raise_sword) i = 0, wait = 5;

  j = 20 + i;

  ALLEGRO_BITMAP *frame = kid_raise_sword_frameset[i].frame;
  int dx = kid_raise_sword_frameset[i].dx;
  int dy = kid_raise_sword_frameset[i].dy;

  if (i == 0 && wait < 5) dx = 0;

  next_frame (&kid.f, &kid.f, frame, dx, dy);
  kid.xframe = sword_frameset[j].frame;
  kid.xdx = sword_frameset[j].dx;
  kid.xdy = sword_frameset[j].dy;

  /* sound */
  if (i == 0 && wait == 5) {
    video_effect.color = YELLOW;
    start_video_effect (VIDEO_FLICKERING, SECS_TO_VCYCLES (0.5));
    sample_glory = true;
  }

  /* depressible floors */
  keep_depressible_floor (&kid);

  /* consume sword */
  if (i == 0) con (&item_pos)->ext.item = NO_ITEM;

  /* next frame */
  if (i == 0 && wait > 0) wait--;
  else if (i < 3) i++;
  else {
    kid.action = kid_keep_sword;
    i = 0, wait = 5;
    item_pos.room = -1;
  }
}

void
kid_keep_sword (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_keep_sword;
  kid.f.flip = (kid.f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  static int i = 0, wait = 1, j = 0;
  if (oaction != kid_keep_sword) i = 0, wait = 1;
  if (keep_sword_fast && i % 2 == 0) i++, wait = 0;

  j = 24 + i;

  ALLEGRO_BITMAP *frame = kid_keep_sword_frameset[i].frame;
  int dx = kid_keep_sword_frameset[i].dx;
  int dy = kid_keep_sword_frameset[i].dy;

  if (i == 8 && wait < 1) dx = 0;
  if (i == 9 && wait < 2) dx = 0;
  if (keep_sword_fast && i % 2) dx += kid_keep_sword_frameset[i - 1].dx;
  if (kid.f.b == kid_sword_normal_08) dx = +8;

  next_frame (&kid.f, &kid.f, frame, dx, dy);
  if (i < 4) {
    kid.xframe = sword_frameset[j].frame;
    kid.xdx = sword_frameset[j].dx;
    kid.xdy = sword_frameset[j].dy;
  }

  /* depressible floors */
  keep_depressible_floor (&kid);

  /* next frame */
  if (i < 8) i++;
  else if (i == 8 && wait > 0) wait--;
  else if (i == 8 && wait == 0) i++, wait = 2;
  else if (i == 9 && wait > 0) wait--;
  else {
    if (keep_sword_fast) {
      kid.action = kid_normal;
      keep_sword_fast = false;
    }
    else {
      kid.action =  kid_turn;
      /* kid turn will invert kid's direction */
      kid.f.dir = (kid.f.dir == RIGHT) ? LEFT : RIGHT;
    }
    i = 0, wait = 1;
  }
}

void
kid_take_sword (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_take_sword;
  kid.f.flip = (kid.f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  struct coord nc; struct pos np;
  enum confg ctf;
  ctf = survey (_tf, pos, &kid.f, &nc, &np, &np)->fg;

  static int i = 0;
  if (oaction != kid_take_sword) i = 0;

  ALLEGRO_BITMAP *frame = kid_take_sword_frameset[i].frame;
  int dx = kid_take_sword_frameset[i].dx;
  int dy = kid_take_sword_frameset[i].dy;

  /* fall */
  if (ctf == NO_FLOOR) {
    kid_fall ();
    i = 0; return;
  }

  /* collision */
  if (is_colliding (&kid.f, _tf, pos, 0, false, -dx)) {
    kid_stabilize_collision ();
    i = 0; return;
  }

  next_frame (&kid.f, &kid.f, frame, dx, dy);

  /* sound */
  if (i == 0) sample_take_sword = true;

  /* depressible floors */
  keep_depressible_floor (&kid);

  /* next frame */
  if (i < 3) i++;
  else {
    kid.action = kid_sword_normal;
    i = 0;
  }
}

void
kid_sword_normal (void)
{
  kid.action = kid_sword_normal;
  kid.f.flip = (kid.f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  struct coord nc; struct pos np;
  enum confg cbf, cmbo, cbb;
  cbf = survey (_bf, pos, &kid.f, &nc, &np, &np)->fg;
  cmbo = survey (_mbo, pos, &kid.f, &nc, &np, &np)->fg;
  cbb = survey (_bb, pos, &kid.f, &nc, &np, &np)->fg;

  int j = 13;
  kid.xframe = sword_frameset[j].frame;
  kid.xdx = sword_frameset[j].dx;
  kid.xdy = sword_frameset[j].dy;
  int w = al_get_bitmap_width (kid.xframe);

  bool keep_sword = down_key;
  bool defense = up_key;
  bool attack = shift_key;
  bool walkf = ((kid.f.dir == RIGHT) && right_key)
    || ((kid.f.dir == LEFT) && left_key);
  bool walkb = ((kid.f.dir == RIGHT) && left_key)
    || ((kid.f.dir == LEFT) && right_key);

  if (kid.f.b == kid_sword_normal_08) {
    if (keep_sword) {
      /* keep_sword_fast = true; */
      kid_keep_sword ();
      return;
    } else if (defense) {
      kid_sword_defense ();
      return;
    } else if (attack) {
      kid_sword_attack ();
      return;
    } else if (walkf) {
      kid_sword_walkf ();
      return;
    } else if (walkb) {
      kid_sword_walkb ();
      return;
    }
  }

  /* depressible floors */
  if (kid.f.b == kid_take_sword_frameset[3].frame)
    update_depressible_floor (&kid, -2, -27);
  else keep_depressible_floor (&kid);

  ALLEGRO_BITMAP *frame = kid_sword_normal_08;
  int dx = +0;
  int dy = +0;

  if (kid.f.b == kid_take_sword_frameset[3].frame) dx = -4;
  if (kid.f.b == kid_sword_walkf_frameset[1].frame) dx = +5;
  if (kid.f.b == kid_sword_walkb_frameset[1].frame) dx = +2;

  /* collision */
  if (is_colliding (&kid.f, _tf, pos, 0, false, -dx + w / 2))
    to_collision_edge (&kid.f, frame, _tf, pos, 0, false, -dx + w / 2);

  /* fall */
  if (cbf == NO_FLOOR || cmbo == NO_FLOOR || cbb == NO_FLOOR) {
    kid_fall ();
    kid.xframe = NULL;
    return;
  }

  next_frame (&kid.f, &kid.f, frame, dx, dy);
}

void
kid_sword_walkf (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_sword_walkf;
  kid.f.flip = (kid.f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  struct coord nc; struct pos np;
  enum confg cbf, cmbo, cbb;
  cbf = survey (_bf, pos, &kid.f, &nc, &np, &np)->fg;
  cmbo = survey (_mbo, pos, &kid.f, &nc, &np, &np)->fg;
  cbb = survey (_bb, pos, &kid.f, &nc, &np, &np)->fg;

  static int i = 0, j = 0;
  if (oaction != kid_sword_walkf) i = 0;

  j = 13;
  kid.xframe = sword_frameset[j].frame;
  kid.xdx = sword_frameset[j].dx;
  kid.xdy = sword_frameset[j].dy;
  int w = al_get_bitmap_width (kid.xframe);
  if (i == 0) kid.xdx = -19;
  if (i == 1) kid.xdx = -21;

  ALLEGRO_BITMAP *frame = kid_sword_walkf_frameset[i].frame;
  int dx = kid_sword_walkf_frameset[i].dx;
  int dy = kid_sword_walkf_frameset[i].dy;

  /* fall */
  if (cbf == NO_FLOOR || cmbo == NO_FLOOR || cbb == NO_FLOOR) {
    kid_fall ();
    kid.xframe = NULL;
    return;
  }

  /* collision */
  if (is_colliding (&kid.f, _tf, pos, 0, false, -dx + w / 2))
    to_collision_edge (&kid.f, frame, _tf, pos, 0, false, -dx + w / 2);

  next_frame (&kid.f, &kid.f, frame, dx, dy);

  /* depressible floors */
  if (i == 1) update_depressible_floor (&kid, -7, -35);
  else keep_depressible_floor (&kid);

  /* next frame */
  if (i < 1) i++;
  else {
    kid.action = kid_sword_normal;
    i = 0;
  }
}

void
kid_sword_walkb (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_sword_walkb;
  kid.f.flip = (kid.f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  struct coord nc; struct pos np;
  enum confg cbf, cmbo, cbb;
  cbf = survey (_bf, pos, &kid.f, &nc, &np, &np)->fg;
  cmbo = survey (_mbo, pos, &kid.f, &nc, &np, &np)->fg;
  cbb = survey (_bb, pos, &kid.f, &nc, &np, &np)->fg;

  static int i = 0, j = 0;
  if (oaction != kid_sword_walkb) i = 0;
  if (kid.f.b == kid_sword_attack_frameset[5].frame) i = 1;

  if (i == 0) j = 10;
  if (i == 1) j = 17;
  kid.xframe = sword_frameset[j].frame;
  kid.xdx = sword_frameset[j].dx;
  kid.xdy = sword_frameset[j].dy;

  ALLEGRO_BITMAP *frame = kid_sword_walkb_frameset[i].frame;
  int dx = kid_sword_walkb_frameset[i].dx;
  int dy = kid_sword_walkb_frameset[i].dy;

  /* fall */
  if (cbf == NO_FLOOR || cmbo == NO_FLOOR || cbb == NO_FLOOR) {
    kid_fall ();
    kid.xframe = NULL;
    return;
  }

  /* collision */
  if (is_colliding (&kid.f, _bb, pos, 0, true, dx))
    to_collision_edge (&kid.f, frame, _bb, pos, 0, true, dx + 4);

  next_frame (&kid.f, &kid.f, frame, dx, dy);

  /* depressible floors */
  if (i == 1) update_depressible_floor (&kid, -1, -24);
  else keep_depressible_floor (&kid);

  /* next frame */
  if (i < 1) i++;
  else {
    kid.action = kid_sword_normal;
    i = 0;
  }
}

void
kid_sword_defense (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_sword_defense;
  kid.f.flip = (kid.f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  struct coord nc; struct pos np;
  enum confg cbf, cmbo, cbb;
  cbf = survey (_bf, pos, &kid.f, &nc, &np, &np)->fg;
  cmbo = survey (_mbo, pos, &kid.f, &nc, &np, &np)->fg;
  cbb = survey (_bb, pos, &kid.f, &nc, &np, &np)->fg;

  static int i = 0, j = 0;
  if (oaction != kid_sword_defense) i = 0;

  if (i == 0) j = 28;
  if (i == 1) j = 14;
  kid.xframe = sword_frameset[j].frame;
  kid.xdx = sword_frameset[j].dx;
  kid.xdy = sword_frameset[j].dy;

  ALLEGRO_BITMAP *frame = kid_sword_defense_frameset[i].frame;
  int dx = kid_sword_defense_frameset[i].dx;
  int dy = kid_sword_defense_frameset[i].dy;

  /* fall */
  if (cbf == NO_FLOOR || cmbo == NO_FLOOR || cbb == NO_FLOOR) {
    kid_fall ();
    kid.xframe = NULL;
    return;
  }

  /* collision */
  if (is_colliding (&kid.f, _bb, pos, 0, true, dx))
    to_collision_edge (&kid.f, frame, _bb, pos, 0, true, dx + 4);

  next_frame (&kid.f, &kid.f, frame, dx, dy);

  /* depressible floors */
  keep_depressible_floor (&kid);

  /* next frame */
  if (i < 1) i++;
  else {
    kid.action = kid_sword_normal;
    i = 0;
  }
}


void
kid_sword_attack (void)
{
  void (*oaction) (void) = kid.action;
  kid.action = kid_sword_attack;
  kid.f.flip = (kid.f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  struct coord nc; struct pos np;
  enum confg cbf, cmbo, cbb;
  cbf = survey (_bf, pos, &kid.f, &nc, &np, &np)->fg;
  cmbo = survey (_mbo, pos, &kid.f, &nc, &np, &np)->fg;
  cbb = survey (_bb, pos, &kid.f, &nc, &np, &np)->fg;

  static int i = 0, j = 0;
  if (oaction != kid_sword_attack) i = 0;

  if (i == 0) j = 1;
  if (i == 2 || i == 3) j = 19;
  if (i == 4 || i == 6) j = 17;
  if (i == 5) j = 7;
  kid.xframe = sword_frameset[j].frame;
  kid.xdx = sword_frameset[j].dx;
  kid.xdy = sword_frameset[j].dy;
  if (i == 1) kid.xframe = NULL;
  if (i == 3) kid.xdx = -21, kid.xdy = +7;
  if (i == 4) kid.xdx = -7, kid.xdy = +17;

  ALLEGRO_BITMAP *frame = kid_sword_attack_frameset[i].frame;
  int dx = kid_sword_attack_frameset[i].dx;
  int dy = kid_sword_attack_frameset[i].dy;

  /* fall */
  if (cbf == NO_FLOOR || cmbo == NO_FLOOR || cbb == NO_FLOOR) {
    kid_fall ();
    kid.xframe = NULL;
    return;
  }

  /* collision */
  if (is_colliding (&kid.f, _bb, pos, 0, true, dx))
    to_collision_edge (&kid.f, frame, _bb, pos, 0, true, dx + 4);

  next_frame (&kid.f, &kid.f, frame, dx, dy);

  /* sound */
  if (i == 3) sample_sword_attack = true;

  /* depressible floors */
  if (i == 2) update_depressible_floor (&kid, -8, -40);
  else if (i == 3) update_depressible_floor (&kid, -11, -47);
  else if (i == 5) update_depressible_floor (&kid, -4, -33);
  else if (i == 6) update_depressible_floor (&kid, -1, -24);
  else keep_depressible_floor (&kid);

  /* next frame */
  if (i < 6) i++;
  else {
    kid.action = kid_sword_normal;
    i = 0;
  }
}



bool
is_kid_fall (struct frame *f)
{
  return f->b == kid_fall_13
    || f->b == kid_fall_14
    || f->b == kid_fall_15
    || f->b == kid_fall_16
    || f->b == kid_fall_17;
}

bool
is_kid_hang_or_climb (struct frame *f)
{
  return f->b == kid_hang_00
    || f->b == kid_hang_01
    || f->b == kid_hang_02
    || f->b == kid_hang_03
    || f->b == kid_hang_04
    || f->b == kid_hang_05
    || f->b == kid_hang_06
    || f->b == kid_hang_07
    || f->b == kid_hang_08
    || f->b == kid_hang_09
    || f->b == kid_hang_10
    || f->b == kid_hang_11
    || f->b == kid_hang_12
    || f->b == kid_hang_14

    || f->b == kid_climb_01
    || f->b == kid_climb_02
    || f->b == kid_climb_03
    || f->b == kid_climb_04
    || f->b == kid_climb_05
    || f->b == kid_climb_06
    || f->b == kid_climb_07
    || f->b == kid_climb_08
    || f->b == kid_climb_09
    || f->b == kid_climb_10;
}

bool
is_kid_climb (struct frame *f)
{
  return f->b == kid_climb_03
    || f->b == kid_climb_04
    || f->b == kid_climb_05
    || f->b == kid_climb_06
    || f->b == kid_climb_07
    || f->b == kid_climb_08
    || f->b == kid_climb_09
    || f->b == kid_climb_10;
}

bool
is_kid_vjump (struct frame *f)
{
  int i;
  for (i = 0; i < KID_VJUMP_FRAMESET_NMEMB; i ++)
    if (f->b == kid_vjump_frameset[i].frame) return true;
  return false;
}

bool
is_kid_turn (struct frame *f)
{
  int i;
  for (i = 0; i < KID_TURN_FRAMESET_NMEMB; i ++)
    if (f->b == kid_turn_frameset[i].frame) return true;
  return false;
}

bool
is_kid_hanging_at_pos (struct frame *f, struct pos *p)
{
  struct coord bb; struct pos pbb, npbb, pr;
  survey (_bb, pos, f, &bb, &pbb, &npbb);

  int dir = (f->dir == LEFT) ? -1 : +1;
  return (is_kid_hang_or_climb (f)
          && peq (prel (&pbb, &pr, -1, dir), p));
}



void
draw_kid_lives (ALLEGRO_BITMAP *bitmap, int j)
{
  int i;
  struct coord c;

  al_draw_filled_rectangle (0, ORIGINAL_HEIGHT - 8,
                            7 * kid_total_lives, ORIGINAL_HEIGHT,
                            al_map_rgba (0, 0, 0, 170));

  for (i = 0; i < kid_total_lives; i++)
    draw_bitmapc (kid_empty_life, bitmap, kid_life_coord (i, &c), 0);

  if (kid_current_lives <= KID_MINIMUM_LIVES_TO_BLINK && j % 2) return;

  for (i = 0; i < kid_current_lives; i++)
    draw_bitmapc (kid_full_life, bitmap, kid_life_coord (i, &c), 0);
}

static struct coord *
kid_life_coord (int i, struct coord *c)
{
  c->x = 7 * i;
  c->y = 194;
  c->room = room_view;
  return c;
}
