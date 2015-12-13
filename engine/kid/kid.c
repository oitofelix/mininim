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

struct pos item_pos = {.room = -1};
bool turn = false;

int kid_total_lives = KID_INITIAL_TOTAL_LIVES,
  kid_current_lives = KID_INITIAL_CURRENT_LIVES;

ALLEGRO_BITMAP *kid_full_life, *kid_empty_life;

ALLEGRO_SAMPLE *step_sample, *hit_ground_sample, *hit_wall_sample,
  *hang_on_fall_sample, *drink_sample, *glory_sample,
  *take_sword_sample, *sword_attack_sample,
  *action_not_allowed_sample;

bool sample_step, sample_hit_ground, sample_hit_wall,
  sample_hang_on_fall, sample_drink, sample_glory,
  sample_take_sword, sample_sword_attack, sample_action_not_allowed;

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
  load_kid_drink ();
  load_kid_raise_sword ();
  load_kid_keep_sword ();
  load_kid_take_sword ();
  load_kid_sword_normal ();
  load_kid_sword_walkf ();
  load_kid_sword_walkb ();
  load_kid_sword_defense ();
  load_kid_sword_attack ();

  /* bitmap */
  kid_full_life = load_bitmap (KID_FULL_LIFE);
  kid_empty_life = load_bitmap (KID_EMPTY_LIFE);

  /* sound */
  step_sample = load_sample (STEP_SAMPLE);
  hit_ground_sample = load_sample (HIT_GROUND_SAMPLE);
  hit_wall_sample = load_sample (HIT_WALL_SAMPLE);
  hang_on_fall_sample = load_sample (HANG_ON_FALL_SAMPLE);
  drink_sample = load_sample (DRINK_SAMPLE);
  glory_sample = load_sample (GLORY_SAMPLE);
  take_sword_sample = load_sample (TAKE_SWORD_SAMPLE);
  sword_attack_sample = load_sample (SWORD_ATTACK_SAMPLE);
  action_not_allowed_sample = load_sample (ACTION_NOT_ALLOWED_SAMPLE);

  /* kid himself */
  kid.f.id = &kid;
  kid.f.b = kid_normal_00;
  kid.f.dir = LEFT;
  kid.fo.b = kid_normal_00;
  kid.fo.dx = kid.fo.dy = +0;
  kid.action = kid_normal;
  update_depressible_floor (&kid, -4, -10);

  place_kid (2, 0, 0);
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
  unload_kid_fall ();
  unload_kid_drink ();
  unload_kid_raise_sword ();
  unload_kid_keep_sword ();
  unload_kid_take_sword ();
  unload_kid_sword_normal ();
  unload_kid_sword_walkf ();
  unload_kid_sword_walkb ();
  unload_kid_sword_defense ();
  unload_kid_sword_attack ();

  /* bitmaps */
  al_destroy_bitmap (kid_full_life);
  al_destroy_bitmap (kid_empty_life);

  /* sounds */
  al_destroy_sample (step_sample);
  al_destroy_sample (hit_ground_sample);
  al_destroy_sample (hit_wall_sample);
  al_destroy_sample (hang_on_fall_sample);
  al_destroy_sample (drink_sample);
  al_destroy_sample (glory_sample);
  al_destroy_sample (take_sword_sample);
  al_destroy_sample (sword_attack_sample);
  al_destroy_sample (action_not_allowed_sample);
}


void
draw_kid (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord nc;
  struct pos np, pbl, pmbo, pbr, pml, pm, pmr, ptl, pmt, ptr;
  survey (_bl, pos, &kid.f, &nc, &pbl, &np);
  survey (_mbo, pos, &kid.f, &nc, &pmbo, &np);
  survey (_br, pos, &kid.f, &nc, &pbr, &np);
  survey (_ml, pos, &kid.f, &nc, &pml, &np);
  survey (_m, pos, &kid.f, &nc, &pm, &np);
  survey (_mr, pos, &kid.f, &nc, &pmr, &np);
  survey (_tl, pos, &kid.f, &nc, &ptl, &np);
  survey (_mt, pos, &kid.f, &nc, &pmt, &np);
  survey (_tr, pos, &kid.f, &nc, &ptr, &np);

  if (! peq (&pbl, p)
      && ! peq (&pmbo, p)
      && ! peq (&pbr, p)
      && ! peq (&pml, p)
      && ! peq (&pm, p)
      && ! peq (&pmr, p)
      && ! peq (&ptl, p)
      && ! peq (&pmt, p)
      && ! peq (&ptr, p)) return;

  draw_frame (bitmap, &kid.f);
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

  place_frame (&kid.f, &kid.f, kid_normal_00, &p, +15, +15);
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
  if (sample_action_not_allowed)
    play_sample (action_not_allowed_sample);

  sample_step = sample_hit_ground = sample_hit_wall =
    sample_hang_on_fall = sample_drink = sample_glory =
    sample_take_sword = sample_sword_attack =
    sample_action_not_allowed = false;
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
