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
#include <string.h>
#include "prince.h"
#include "kernel/video.h"
#include "kernel/audio.h"
#include "kernel/keyboard.h"
#include "kernel/array.h"
#include "kernel/random.h"
#include "kernel/timer.h"
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

ALLEGRO_BITMAP *v_kid_full_life, *v_kid_empty_life, *v_kid_splash;

ALLEGRO_SAMPLE *step_sample, *hit_ground_sample, *hit_ground_harm_sample,
  *hit_ground_fatal_sample, *hit_wall_sample, *hang_on_fall_sample,
  *drink_sample, *glory_sample, *take_sword_sample, *sword_attack_sample,
  *harm_sample, *action_not_allowed_sample, *small_life_potion_sample,
  *big_life_potion_sample, *scream_sample, *spiked_sample, *chopped_sample,
  *floating_sample, *death_sample, *press_key_sample;

bool sample_step, sample_hit_ground, sample_hit_ground_harm,
  sample_hit_ground_fatal, sample_hit_wall, sample_hang_on_fall,
  sample_drink, sample_glory, sample_take_sword, sample_sword_attack,
  sample_harm, sample_action_not_allowed, sample_small_life_potion,
  sample_big_life_potion, sample_scream, sample_spiked, sample_chopped,
  sample_floating, sample_death, sample_press_key;

static void place_kid (struct anim *kid, int room, int floor, int place);
static struct coord *kid_life_coord (int i, struct coord *c);
static int compare_kids (const void *k0, const void *k1);
static struct coord * splash_coord (struct frame *f, struct coord *c);
static palette get_palette (enum vm vm);

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
  load_kid_die ();
  load_kid_stairs ();

  /* bitmap */
  v_kid_full_life = load_bitmap (V_KID_FULL_LIFE);
  v_kid_empty_life = load_bitmap (V_KID_EMPTY_LIFE);
  v_kid_splash = load_bitmap (V_KID_SPLASH);

  /* sound */
  step_sample = load_sample (STEP_SAMPLE);
  hit_ground_sample = load_sample (HIT_GROUND_SAMPLE);
  hit_ground_harm_sample = load_sample (HIT_GROUND_HARM_SAMPLE);
  hit_ground_fatal_sample = load_sample (HIT_GROUND_FATAL_SAMPLE);
  hit_wall_sample = load_sample (HIT_WALL_SAMPLE);
  hang_on_fall_sample = load_sample (HANG_ON_FALL_SAMPLE);
  drink_sample = load_sample (DRINK_SAMPLE);
  glory_sample = load_sample (GLORY_SAMPLE);
  take_sword_sample = load_sample (TAKE_SWORD_SAMPLE);
  sword_attack_sample = load_sample (SWORD_ATTACK_SAMPLE);
  harm_sample = load_sample (HARM_SAMPLE);
  action_not_allowed_sample = load_sample (ACTION_NOT_ALLOWED_SAMPLE);
  small_life_potion_sample = load_sample (SMALL_LIFE_POTION_SAMPLE);
  big_life_potion_sample = load_sample (BIG_LIFE_POTION_SAMPLE);
  scream_sample = load_sample (SCREAM_SAMPLE);
  spiked_sample = load_sample (SPIKED_SAMPLE);
  chopped_sample = load_sample (CHOPPED_SAMPLE);
  floating_sample = load_sample (FLOATING_SAMPLE);
  death_sample = load_sample (DEATH_SAMPLE);
  press_key_sample = load_sample (PRESS_KEY_SAMPLE);
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
  unload_kid_die ();
  unload_kid_stairs ();

  /* bitmaps */
  al_destroy_bitmap (v_kid_full_life);
  al_destroy_bitmap (v_kid_empty_life);
  al_destroy_bitmap (v_kid_splash);

  /* sounds */
  al_destroy_sample (step_sample);
  al_destroy_sample (hit_ground_sample);
  al_destroy_sample (hit_ground_harm_sample);
  al_destroy_sample (hit_ground_fatal_sample);
  al_destroy_sample (hit_wall_sample);
  al_destroy_sample (hang_on_fall_sample);
  al_destroy_sample (drink_sample);
  al_destroy_sample (glory_sample);
  al_destroy_sample (take_sword_sample);
  al_destroy_sample (sword_attack_sample);
  al_destroy_sample (harm_sample);
  al_destroy_sample (action_not_allowed_sample);
  al_destroy_sample (small_life_potion_sample);
  al_destroy_sample (big_life_potion_sample);
  al_destroy_sample (scream_sample);
  al_destroy_sample (spiked_sample);
  al_destroy_sample (chopped_sample);
  al_destroy_sample (floating_sample);
  al_destroy_sample (death_sample);
  al_destroy_sample (press_key_sample);
}

int
create_kid (struct anim *_k)
{
  struct anim k;

  int i = kid_nmemb;

  memset (&k, 0, sizeof (k));

  if (_k) {
    k = *_k;
    k.id = i;
    k.shadow_of = _k->id;
    k.shadow = true;
    k.floating = create_timer (1.0);
  } else {
    k.id = i;
    k.shadow_of = -1;
    k.f.b = kid_normal_00;
    k.f.c.room = room_view;
    k.f.dir = LEFT;
    k.fo.b = kid_normal_00;
    k.action = kid_normal;
    k.item_pos.room = -1;
    k.total_lives = KID_INITIAL_TOTAL_LIVES;
    k.current_lives = KID_INITIAL_CURRENT_LIVES;
    k.floating = create_timer (1.0);

    place_kid (&k, 1, 0, 0);
    update_depressible_floor (&k, -4, -10);
  }

  kid = add_to_array (&k, 1, kid, &kid_nmemb, i, sizeof (k));

  kid[i].f.id = &kid[i];

  return i;
}

struct anim *
get_kid_by_id (int id)
{
  int i;
  for (i = 0; i < kid_nmemb; i++)
    if (kid[i].id == id) return &kid[i];
  return NULL;
}

void
draw_kids (ALLEGRO_BITMAP *bitmap,
           enum em em, enum vm vm)
{
  struct coord ml; struct pos pml, pmlr, pmlra;
  struct anim *k;

  /* coord_wa = true; */

  qsort (kid, kid_nmemb, sizeof (*k), compare_kids);

  size_t i;
  for (i = 0; i < kid_nmemb; i++) {
    k = &kid[i];

    if (k->invisible) continue;

    k->f.id = k;
    if (k->current) current_kid = k;

    _ml (&k->f, &ml); pos (&ml, &pml);
    prel (&pml, &pmlr, 0, +1);
    prel (&pml, &pmlra, -1, +1);

    draw_kid_frame (bitmap, k, vm);

    draw_falling_loose_floor (bitmap, &pmlr, em, vm);
    draw_falling_loose_floor (bitmap, &pmlra, em, vm);
    draw_room_anim_fg (bitmap, em, vm, k);
    k->xf.b = NULL;
  }

  /* coord_wa = false; */
}

int
compare_kids (const void *k0, const void *k1)
{
  struct coord nc;
  struct pos np, ptl0, ptl1, ptr0, ptr1,
    pbl0, pbl1, pbr0, pbr1;

  struct anim *_k0 = (struct anim *) k0;
  struct anim *_k1 = (struct anim *) k1;

  survey (_br, pos, &_k0->f, &nc, &np, &pbr0);
  survey (_br, pos, &_k1->f, &nc, &np, &pbr1);

  survey (_bl, pos, &_k0->f, &nc, &np, &pbl0);
  survey (_bl, pos, &_k1->f, &nc, &np, &pbl1);

  survey (_tr, pos, &_k0->f, &nc, &np, &ptr0);
  survey (_tr, pos, &_k1->f, &nc, &np, &ptr1);

  survey (_tl, pos, &_k0->f, &nc, &np, &ptl0);
  survey (_tl, pos, &_k1->f, &nc, &np, &ptl1);

  int cpbr = cpos (&pbr0, &pbr1);
  if (cpbr && pbr0.room == pbr1.room) return cpbr;

  int cptr = cpos (&ptr0, &ptr1);
  if (cptr && ptr0.room == ptr1.room) return cptr;

  int cpbl = cpos (&pbl0, &pbl1);
  if (cpbl && pbl0.room == pbl1.room) return cpbl;

  int cptl = cpos (&ptl0, &ptl1);
  if (cptl && ptl0.room == ptl1.room) return cptl;

  return 0;
}

static palette
get_palette (enum vm vm)
{
  switch (vm) {
  case CGA: return c_palette;
  case EGA: return e_palette;
  case VGA: return v_palette;
  }
  return NULL;
}

static palette
get_shadow_palette (enum vm vm)
{
  switch (vm) {
  case CGA: return c_phantom_shadow_palette;
  case EGA: return e_phantom_shadow_palette;
  case VGA: return v_phantom_shadow_palette;
  }
  return NULL;
}

void
draw_kid_frame (ALLEGRO_BITMAP *bitmap, struct anim *k,
                enum vm vm)
{
  struct coord c;

  struct frame f = k->f;
  struct frame_offset xf = k->xf;

  palette pal = get_palette (vm);
  f.b = apply_palette (f.b, pal);
  xf.b = apply_palette (xf.b, pal);

  if (k->shadow) {
    palette pals = get_shadow_palette (vm);
    f.b = apply_palette (f.b, pals);
    xf.b = apply_palette (xf.b, pals);
  }

  draw_frame (bitmap, &f);
  draw_xframe (bitmap, &f, &xf);

  if (k->splash) {
    ALLEGRO_BITMAP *splash = apply_palette (v_kid_splash, pal);
    draw_bitmapc (splash, bitmap, splash_coord (&k->f, &c), k->f.flip);
  }

}

ALLEGRO_COLOR
colorful_shadow_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;
  if (color_eq (c, V_KID_SKIN_COLOR_01)
      || color_eq (c, V_KID_NOSE_COLOR))
    switch (anim_cycle % 3) {
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

ALLEGRO_COLOR
v_phantom_shadow_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;
  if (color_eq (c, V_KID_CLOTHES_COLOR_01))
    return V_KID_SHADOW_CLOTHES_COLOR_01;
  if (color_eq (c, V_KID_CLOTHES_COLOR_02))
    return V_KID_SHADOW_CLOTHES_COLOR_02;
  if (color_eq (c, V_KID_EYE_COLOR))
    return V_KID_SHADOW_EYE_COLOR;
  return al_map_rgba (r, g, b, 0);
}

ALLEGRO_COLOR
e_phantom_shadow_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;
  if (color_eq (c, E_KID_CLOTHES_COLOR_01))
    return E_KID_SHADOW_CLOTHES_COLOR_01;
  if (color_eq (c, E_KID_CLOTHES_COLOR_02))
    return E_KID_SHADOW_CLOTHES_COLOR_02;
  if (color_eq (c, E_KID_SKIN_COLOR))
    return E_KID_SHADOW_SKIN_COLOR;
  if (color_eq (c, E_KID_HAIR_COLOR))
    return E_KID_SHADOW_HAIR_COLOR;
  if (color_eq (c, E_KID_EYE_COLOR))
    return E_KID_SHADOW_EYE_COLOR;
  return c;
}

ALLEGRO_COLOR
c_phantom_shadow_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;
  if (color_eq (c, C_KID_CLOTHES_COLOR_01))
    return C_KID_SHADOW_CLOTHES_COLOR_01;
  if (color_eq (c, C_KID_CLOTHES_COLOR_02))
    return C_KID_SHADOW_CLOTHES_COLOR_02;
  if (color_eq (c, C_KID_SKIN_COLOR))
    return C_KID_SHADOW_SKIN_COLOR;;
  if (color_eq (c, C_KID_EYE_COLOR))
    return C_KID_SHADOW_EYE_COLOR;;
  return c;
}

ALLEGRO_COLOR
v_palette (ALLEGRO_COLOR c)
{
  return c;
}

ALLEGRO_COLOR
e_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, V_KID_HAIR_COLOR_01)
      || color_eq (c, V_KID_HAIR_COLOR_02)) return E_KID_HAIR_COLOR;
  if (color_eq (c, V_KID_SKIN_COLOR_01)
      || color_eq (c, V_KID_SKIN_COLOR_02)) return E_KID_SKIN_COLOR;
  if (color_eq (c, V_KID_CLOTHES_COLOR_01)) return E_KID_CLOTHES_COLOR_01;
  if (color_eq (c, V_KID_CLOTHES_COLOR_02)) return E_KID_CLOTHES_COLOR_02;
  if (color_eq (c, V_KID_EYE_COLOR)) return E_KID_EYE_COLOR;
  if (color_eq (c, V_BLOOD_COLOR_01)) return E_BLOOD_COLOR_01;
  if (color_eq (c, V_BLOOD_COLOR_02)) return E_BLOOD_COLOR_02;
  return c;
}

ALLEGRO_COLOR
c_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, V_KID_HAIR_COLOR_01)) return C_KID_HAIR_COLOR_01;
  if (color_eq (c, V_KID_HAIR_COLOR_02)) return C_KID_HAIR_COLOR_02;
  if (color_eq (c, V_KID_SKIN_COLOR_01)
      || color_eq (c, V_KID_SKIN_COLOR_02)) return C_KID_SKIN_COLOR;
  if (color_eq (c, V_KID_CLOTHES_COLOR_01)) return C_KID_CLOTHES_COLOR_01;
  if (color_eq (c, V_KID_CLOTHES_COLOR_02)) return C_KID_CLOTHES_COLOR_02;
  if (color_eq (c, V_KID_EYE_COLOR)) return C_KID_EYE_COLOR;
  if (color_eq (c, V_BLOOD_COLOR_01)
      || color_eq (c, V_BLOOD_COLOR_02)) return C_BLOOD_COLOR;
  return c;
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
  if (sample_hit_ground_harm) play_sample (hit_ground_harm_sample);
  if (sample_hit_ground_fatal) play_sample (hit_ground_fatal_sample);
  if (sample_hit_wall) play_sample (hit_wall_sample);
  if (sample_hang_on_fall) play_sample (hang_on_fall_sample);
  if (sample_drink) play_sample (drink_sample);
  if (sample_glory) play_sample (glory_sample);
  if (sample_take_sword) play_sample (take_sword_sample);
  if (sample_sword_attack) play_sample (sword_attack_sample);
  if (sample_harm) play_sample (harm_sample);
  if (sample_action_not_allowed)
    play_sample (action_not_allowed_sample);
  if (sample_small_life_potion)
    play_sample (small_life_potion_sample);
  if (sample_big_life_potion)
    play_sample (big_life_potion_sample);
  if (sample_scream) play_sample (scream_sample);
  if (sample_spiked) play_sample (spiked_sample);
  if (sample_chopped) play_sample (chopped_sample);
  if (sample_floating) play_sample (floating_sample);
  if (sample_death) play_sample (death_sample);
  if (sample_press_key) play_sample (press_key_sample);

  sample_step = sample_hit_ground = sample_hit_ground_harm =
    sample_hit_ground_fatal = sample_hit_wall =
    sample_hang_on_fall = sample_drink = sample_glory =
    sample_take_sword = sample_sword_attack = sample_harm =
    sample_action_not_allowed = sample_small_life_potion =
    sample_big_life_potion = sample_scream = sample_spiked =
    sample_chopped = sample_floating = sample_death =
    sample_press_key = false;
}

void
draw_kid_lives (ALLEGRO_BITMAP *bitmap, struct anim *kid,
                enum vm vm)
{
  int i;
  struct coord c;
  struct rect r =
    new_rect (room_view, 0, ORIGINAL_HEIGHT - 8,
              7 * kid->total_lives, ORIGINAL_HEIGHT - 1);

  ALLEGRO_COLOR bg_color;

  switch (vm) {
  case CGA: bg_color = C_LIVES_RECT_COLOR; break;
  case EGA: bg_color = E_LIVES_RECT_COLOR; break;
  case VGA: bg_color = V_LIVES_RECT_COLOR; break;
  }

  draw_filled_rect (bitmap, &r, bg_color);

  ALLEGRO_BITMAP *empty = NULL,
    *full = NULL;
  palette pal = get_palette (vm);
  empty = apply_palette (v_kid_empty_life, pal);
  full = apply_palette (v_kid_full_life, pal);

  for (i = 0; i < kid->total_lives; i++)
    draw_bitmapc (empty, bitmap, kid_life_coord (i, &c), 0);

  if (kid->current_lives <= KID_MINIMUM_LIVES_TO_BLINK && anim_cycle % 2) return;

  for (i = 0; i < kid->current_lives; i++)
    draw_bitmapc (full, bitmap, kid_life_coord (i, &c), 0);
}

static struct coord *
kid_life_coord (int i, struct coord *c)
{
  c->x = 7 * i;
  c->y = 194;
  c->room = room_view;
  return c;
}

static struct coord *
splash_coord (struct frame *f, struct coord *c)
{
  int w = al_get_bitmap_width (v_kid_splash);
  int h = al_get_bitmap_height (v_kid_splash);
  int fw = al_get_bitmap_width (f->b);
  int fh = al_get_bitmap_height (f->b);
  c->x = f->c.x + (fw / 2) - (w / 2);
  c->y = f->c.y + (fh / 2) - (h / 2);
  c->room = f->c.room;
  return c;
}

void
increase_kid_current_lives (struct anim *k)
{
  if (k->current_lives < k->total_lives) {
    k->current_lives++;
    sample_small_life_potion = true;
    video_effect.color = RED;
    start_video_effect (VIDEO_FLICKERING, SECS_TO_VCYCLES (0.3));
  }
}

void
increase_kid_total_lives (struct anim *k)
{
  if (k->total_lives < 10) {
    k->total_lives++;
    k->current_lives = k->total_lives;
    sample_big_life_potion = true;
    video_effect.color = RED;
    start_video_effect (VIDEO_FLICKERING, SECS_TO_VCYCLES (0.3));
  }
}

void
float_kid (struct anim *k)
{
  al_set_timer_count (k->floating, 0);
  al_start_timer (k->floating);
  sample_floating = true;
  video_effect.color = GREEN;
  start_video_effect (VIDEO_FLICKERING, SECS_TO_VCYCLES (0.3));
}

void
kid_debug (void)
{
  /* begin kid hack */
  if (! cutscene) {
    /* static int px = 0; */
    /* static int py = 0; */
    /* if (a_key) px--; */
    /* if (d_key) px++; */
    /* if (w_key) py--; */
    /* if (s_key) py++; */
    /* al_set_target_bitmap (screen); */
    /* al_put_pixel (px, py, al_map_rgb (0, 255, 255)); */

    /* printf ("x = %i, y = %i, floor = %i, place = %i\n", px, py, (py -3) / 63, (px - 15) / 32); */

    struct coord bf; struct pos pbf, npbf;
    survey (_bf, pos, &current_kid->f, &bf, &pbf, &npbf);
    if (delete_key) current_kid->f.c.x--;
    if (page_down_key) current_kid->f.c.x++;
    int dn = dist_next_place (&current_kid->f, _bf, pos, 0, false);
    int dp = dist_next_place (&current_kid->f, _bf, pos, 0, true);
    int dc = dist_collision (&current_kid->f, false, &kid->ci) + 4;
    int df = dist_fall (&current_kid->f, false);
    int dl = dist_con (&current_kid->f, _bf, pos, -4, false, LOOSE_FLOOR);
    int dcl = dist_con (&current_kid->f, _bf, pos, -4, false, CLOSER_FLOOR);
    int dch = dist_chopper (&current_kid->f, false);
    if (delete_key || page_down_key || enter_key)
      printf ("\
f = %i, p = %i, dn = %i, dp = %i, dc = %i, df = %i, dl = %i, dcl = %i, dch = %i\n",
              pbf.floor, pbf.place, dn, dp, dc, df, dl, dcl, dch);
  }
  /* end kid hack */
}
