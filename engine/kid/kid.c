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
#include "kernel/array.h"
#include "kernel/random.h"
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

struct anim *kid;
size_t kid_nmemb;
struct anim *current_kid;

ALLEGRO_BITMAP *kid_full_life, *kid_empty_life;

ALLEGRO_SAMPLE *step_sample, *hit_ground_sample, *hit_wall_sample,
  *hang_on_fall_sample, *drink_sample, *glory_sample,
  *take_sword_sample, *sword_attack_sample,
  *action_not_allowed_sample;

bool sample_step, sample_hit_ground, sample_hit_wall,
  sample_hang_on_fall, sample_drink, sample_glory,
  sample_take_sword, sample_sword_attack, sample_action_not_allowed;

static void place_kid (struct anim *kid, int room, int floor, int place);
static struct coord *kid_life_coord (int i, struct coord *c);
static int compare_kids (const void *k0, const void *k1);

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

int
create_kid (void)
{
  struct anim k;

  int i = kid_nmemb;

  k.f.b = kid_normal_00;
  k.f.c.room = room_view;
  k.f.dir = LEFT;
  k.fo.b = kid_normal_00;
  k.fo.dx = k.fo.dy = +0;
  k.action = kid_normal;
  k.item_pos.room = -1;
  k.total_lives = KID_INITIAL_TOTAL_LIVES,
  k.current_lives = KID_INITIAL_CURRENT_LIVES;
  k.misstep = k.hang_limit = k.uncouch_slowly = false;
  k.inertia = 0;
  k.shadow = false;
  k.xf.b = NULL;
  k.current = false;
  k.turn = false;

  place_kid (&k, 1, 0, 0);
  update_depressible_floor (&k, -4, -10);

  kid = add_to_array (&k, 1, kid, &kid_nmemb, i, sizeof (k));

  kid[i].f.id = &kid[i];

  return i;
}

void
draw_kids (ALLEGRO_BITMAP *bitmap)
{
  struct coord ml; struct pos pml, pmlr, pmlra;
  struct anim *k;

  qsort (kid, kid_nmemb, sizeof (*k), compare_kids);

  size_t i;
  for (i = 0; i < kid_nmemb; i++) {
    k = &kid[i];

    k->f.id = k;
    if (k->current) current_kid = k;

    _ml (&k->f, &ml); pos (&ml, &pml);
    prel (&pml, &pmlr, 0, +1);
    prel (&pml, &pmlra, -1, +1);

    draw_kid_frame (bitmap, k);
    draw_xframe (bitmap, &k->f, &k->xf);

    draw_falling_loose_floor (bitmap, &pmlr);
    draw_falling_loose_floor (bitmap, &pmlra);
    draw_room_anim_fg (bitmap, k);
    k->xf.b = NULL;
  }
}

int
compare_kids (const void *k0, const void *k1)
{
  struct coord nc;
  struct pos np, ptl0, ptl1, ptr0, ptr1,
    pbl0, pbl1, pbr0, pbr1;

  struct anim *_k0 = (struct anim *) k0;
  struct anim *_k1 = (struct anim *) k1;

  survey (_br, pos, &_k0->f, &nc, &pbr0, &np);
  survey (_br, pos, &_k1->f, &nc, &pbr1, &np);

  survey (_bl, pos, &_k0->f, &nc, &pbl0, &np);
  survey (_bl, pos, &_k1->f, &nc, &pbl1, &np);

  survey (_tl, pos, &_k0->f, &nc, &ptl0, &np);
  survey (_tl, pos, &_k1->f, &nc, &ptl1, &np);

  survey (_tr, pos, &_k0->f, &nc, &ptr0, &np);
  survey (_tr, pos, &_k1->f, &nc, &ptr1, &np);

  int cpbr = cpos (&pbr0, &pbr1);
  if (cpbr) return cpbr;

  int cpbl = cpos (&pbl0, &pbl1);
  if (cpbl) return cpbl;

  int cptr = cpos (&ptr0, &ptr1);
  if (cptr) return cptr;

  int cptl = cpos (&ptl0, &ptl1);
  if (cptl) return cptl;

  return 0;
}

void
draw_kid_frame (ALLEGRO_BITMAP *bitmap, struct anim *k)
{
  ALLEGRO_BITMAP *ob = NULL;

  if (k->shadow) {
    ob = k->f.b;
    k->f.b = apply_palette (k->f.b, shadow_palette);
  }

  draw_frame (bitmap, &k->f);

  if (k->shadow) {
    al_destroy_bitmap (k->f.b);
    k->f.b = ob;
  }
}

ALLEGRO_COLOR
shadow_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;
  if (color_eq (c, KID_SKIN_COLOR)
      || color_eq (c, KID_NOSE_COLOR))
    switch (draw_cycle % 3) {
    case 0: return TRED;
    case 1: return TGREEN;
    case 2: return TBLUE;
    }

  switch (prandom (9)) {
  case 0: return RRED;
  case 1: return RGREEN;
  case 2: return RBLUE;
  case 3: case 4: case 5: case 6: case 7: case 8: case 9:
    return TRANSPARENT;
  }
  return BLACK;
}

void
draw_kid (ALLEGRO_BITMAP *bitmap, struct anim *kid, struct pos *p)
{
  struct coord nc;
  struct pos np, pbl, pmbo, pbr, pml, pm, pmr, ptl, pmt, ptr;
  survey (_bl, pos, &kid->f, &nc, &pbl, &np);
  survey (_mbo, pos, &kid->f, &nc, &pmbo, &np);
  survey (_br, pos, &kid->f, &nc, &pbr, &np);
  survey (_ml, pos, &kid->f, &nc, &pml, &np);
  survey (_m, pos, &kid->f, &nc, &pm, &np);
  survey (_mr, pos, &kid->f, &nc, &pmr, &np);
  survey (_tl, pos, &kid->f, &nc, &ptl, &np);
  survey (_mt, pos, &kid->f, &nc, &pmt, &np);
  survey (_tr, pos, &kid->f, &nc, &ptr, &np);

  if (! peq (&pbl, p)
      && ! peq (&pmbo, p)
      && ! peq (&pbr, p)
      && ! peq (&pml, p)
      && ! peq (&pm, p)
      && ! peq (&pmr, p)
      && ! peq (&ptl, p)
      && ! peq (&pmt, p)
      && ! peq (&ptr, p)) return;

  draw_frame (bitmap, &kid->f);
}

void
place_kid (struct anim *kid, int room, int floor, int place)
{
  struct pos p;
  enum confg tl, tr;

  for (p.room = room; p.room < ROOMS; p.room++)
    for (p.floor = floor; p.floor < FLOORS; p.floor++)
      for (p.place = place; p.place < PLACES; p.place++)
        if (con (&p)->fg == FLOOR
            || con (&p)->fg == BROKEN_FLOOR
            || con (&p)->fg == SKELETON_FLOOR
            || con (&p)->fg == DOOR) goto end;

 end:
  tl = crel (&p, 0, -1)->fg;
  tr = crel (&p, 0, +1)->fg;

  if (kid->f.dir == LEFT
      && tl == WALL
      && tl == DOOR) kid->f.dir = RIGHT;

  if (kid->f.dir == RIGHT
      && tr == WALL
      && tr == DOOR) kid->f.dir = LEFT;

  place_frame (&kid->f, &kid->f, kid_normal_00, &p, +15, +15);
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
draw_kid_lives (ALLEGRO_BITMAP *bitmap, struct anim *kid, int j)
{
  int i;
  struct coord c;

  al_draw_filled_rectangle (0, ORIGINAL_HEIGHT - 8,
                            7 * kid->total_lives, ORIGINAL_HEIGHT,
                            al_map_rgba (0, 0, 0, 170));

  for (i = 0; i < kid->total_lives; i++)
    draw_bitmapc (kid_empty_life, bitmap, kid_life_coord (i, &c), 0);

  if (kid->current_lives <= KID_MINIMUM_LIVES_TO_BLINK && j % 2) return;

  for (i = 0; i < kid->current_lives; i++)
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
