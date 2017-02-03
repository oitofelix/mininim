/*
  kid.c -- kid module;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

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

int current_kid_id;

ALLEGRO_BITMAP *v_kid_full_life, *v_kid_empty_life, *v_kid_splash;

static ALLEGRO_COLOR v_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR e_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR c_palette (ALLEGRO_COLOR c);

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
  load_kid_sword_hit ();
  load_kid_die ();
  load_kid_stairs ();

  /* bitmap */
  v_kid_full_life = load_bitmap (V_KID_FULL_LIFE);
  v_kid_empty_life = load_bitmap (V_KID_EMPTY_LIFE);
  v_kid_splash = load_bitmap (V_KID_SPLASH);
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
  unload_kid_sword_hit ();
  unload_kid_die ();
  unload_kid_stairs ();

  /* bitmaps */
  destroy_bitmap (v_kid_full_life);
  destroy_bitmap (v_kid_empty_life);
  destroy_bitmap (v_kid_splash);
}

struct pos *
get_kid_start_pos (struct pos *p)
{
  if (is_valid_pos (&start_pos) && replay_mode == NO_REPLAY)
    *p = start_pos;
  else *p = global_level.start_pos;
  return p;
}

struct anim *
create_kid (struct anim *k0, struct anim *k1, struct pos *p, enum dir dir)
{
  if (k0) {
    k1->shadow_of = k0->id;
    k1->shadow = true;
  } else {
    k1->shadow_of = -1;
    k1->f.b = kid_normal_00;
    k1->fo.b = kid_normal_00;
    k1->action = kid_normal;
    invalid_pos (&k1->item_pos);
    k1->total_lives = KID_INITIAL_TOTAL_LIVES;
    k1->current_lives = KID_INITIAL_CURRENT_LIVES;
    k1->fight = true;
    k1->enemy_id = -1;
    k1->skill.counter_attack_prob = -1;
    k1->skill.counter_defense_prob = -1;

    place_frame (&k1->f, &k1->f, kid_normal_00, p,
                 k1->f.dir == LEFT ? +22 : +28, +15);
    update_depressible_floor (k1, -4, -10);
  }

  return k1;
}

void
destroy_kid (struct anim *k)
{
  int i;
  if (current_kid_id == k->id)
    for (i = 0; i < anima_nmemb; i++) {
      struct anim *a = &anima[i];
      if (a->type == KID && a->controllable)
        current_kid_id = a->id;
    }
}

palette
get_kid_palette (enum vm vm)
{
  switch (vm) {
  case CGA: return c_palette;
  case EGA: return e_palette;
  case VGA: return v_palette;
  }
  return NULL;
}

palette
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

  if (k->invisible) return;

  struct frame f = k->f;
  struct frame_offset xf = k->xf;

  palette pal = get_kid_palette (vm);
  f.b = apply_palette (f.b, pal);
  xf.b = apply_palette (xf.b, pal);

  if (k->shadow) {
    f.b = apply_guard_palette (f.b, SHADOW, k->style, vm);
    xf.b = apply_guard_palette (xf.b, SHADOW, k->style, vm);
    /* palette pals = get_shadow_palette (vm); */
    /* f.b = apply_palette (f.b, pals); */
    /* xf.b = apply_palette (xf.b, pals); */
  }

  if (hgc) {
    f.b = apply_palette (f.b, hgc_palette);
    xf.b = apply_palette (xf.b, hgc_palette);
  }

  draw_frame (bitmap, &f);
  draw_xframe (bitmap, &f, &xf);

  if (k->splash) {
    ALLEGRO_BITMAP *splash = apply_palette (v_kid_splash, pal);
    if (hgc) splash = apply_palette (splash, hgc_palette);
    draw_bitmapc (splash, bitmap, splash_coord (&k->f, &c), k->f.flip);
  }

}

void
draw_start_kid (ALLEGRO_BITMAP *bitmap, enum vm vm)
{
  /* kid */
  struct frame f;
  f.c.room = global_level.start_pos.room;
  palette pal = get_kid_palette (vm);
  f.b = kid_normal_00;
  f.b = apply_palette (f.b, pal);
  if (hgc) f.b = apply_palette (f.b, hgc_palette);
  f.b = apply_palette (f.b, start_anim_palette);
  f.flip = (global_level.start_dir == LEFT) ? ALLEGRO_FLIP_HORIZONTAL : 0;
  place_frame (&f, &f, f.b, &global_level.start_pos,
               global_level.start_dir == LEFT ? +28 : +22, +15);
  draw_frame (bitmap, &f);

  /* sword */
  if (global_level.has_sword) draw_sword (bitmap, &global_level.start_pos, vm, true);
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
    case 0: return TRED_COLOR;
    case 1: return TGREEN_COLOR;
    case 2: return TBLUE_COLOR;
    }

  switch (mrandom (9)) {
  case 0: return RRED_COLOR;
  case 1: return RGREEN_COLOR;
  case 2: return RBLUE_COLOR;
  case 3: case 4: case 5: case 6: case 7: case 8: case 9:
    return TRANSPARENT_COLOR;
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
    return C_KID_SHADOW_SKIN_COLOR;
  if (color_eq (c, C_KID_EYE_COLOR))
    return C_KID_SHADOW_EYE_COLOR;
  return c;
}

static ALLEGRO_COLOR
v_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, LIFE_COLOR_01)) return V_BLOOD_COLOR_01;
  if (color_eq (c, LIFE_COLOR_02)) return V_BLOOD_COLOR_02;
  return c;
}

static ALLEGRO_COLOR
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
  if (color_eq (c, LIFE_COLOR_01)) return E_BLOOD_COLOR_01;
  if (color_eq (c, LIFE_COLOR_02)) return E_BLOOD_COLOR_02;
  return c;
}

static ALLEGRO_COLOR
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
  if (color_eq (c, LIFE_COLOR_01)) return C_BLOOD_COLOR;
  if (color_eq (c, LIFE_COLOR_02)) return C_BLOOD_COLOR;
  return c;
}

void
place_kid (struct anim *k, int room, int floor, int place)
{
  enum confg tl, tr;

  struct pos p;
  new_pos (&p, k->f.c.l, -1, -1, -1);
  for (p.room = room; p.room < ROOMS; p.room++)
    for (p.floor = floor; p.floor < FLOORS; p.floor++)
      for (p.place = place; p.place < PLACES; p.place++) {
        enum confg f = fg (&p);
        if (f == FLOOR || f == BROKEN_FLOOR
            || f == SKELETON_FLOOR || f == DOOR) goto end;
      }

 end:
  tl = fg_rel (&p, 0, -1);
  tr = fg_rel (&p, 0, +1);

  if (k->f.dir == LEFT
      && tl == WALL
      && tl == DOOR) k->f.dir = RIGHT;

  if (k->f.dir == RIGHT
      && tr == WALL
      && tr == DOOR) k->f.dir = LEFT;

  place_frame (&k->f, &k->f, kid_normal_00, &p, +15, +15);
}


void
draw_kid_lives (ALLEGRO_BITMAP *bitmap, struct anim *k,
                enum vm vm)
{
  if (k->dont_draw_lives) return;
  if (k->current_lives <= 0) return;

  int current_lives = (k->current_lives < 0) ? 0 : k->current_lives;
  current_lives = (k->current_lives > 10) ? 10 : k->current_lives;

  int total_lives = (k->total_lives < 0) ? 0 : k->total_lives;
  total_lives = (k->total_lives > 10) ? 10 : k->total_lives;

  ALLEGRO_COLOR bg_color;

  switch (vm) {
  case CGA: bg_color = C_LIVES_RECT_COLOR; break;
  case EGA: bg_color = E_LIVES_RECT_COLOR; break;
  case VGA: bg_color = V_LIVES_RECT_COLOR; break;
  }

  push_reset_clipping_rectangle (bitmap);

  draw_filled_rectangle (bitmap, 0, CUTSCENE_HEIGHT - 8,
                         7 * total_lives - 1,
                         CUTSCENE_HEIGHT - 1, bg_color);

  ALLEGRO_BITMAP *empty = NULL,
    *full = NULL;

  if (k->shadow) {
    if (k->style) {
      empty = apply_guard_palette (v_kid_empty_life, SHADOW, k->style, vm);
      full = apply_guard_palette (v_kid_full_life, SHADOW, k->style, vm);
    } else {
      palette pal = get_shadow_life_palette (vm);
      empty = apply_palette (v_kid_empty_life, pal);
      full = apply_palette (v_kid_full_life, pal);
    }
  } else {
    palette pal = get_kid_palette (vm);
    empty = apply_palette (v_kid_empty_life, pal);
    full = apply_palette (v_kid_full_life, pal);
  }

  if (hgc) {
    empty = apply_palette (empty, hgc_palette);
    full = apply_palette (full, hgc_palette);
  }

  int i;
  for (i = 0; i < total_lives; i++)
    draw_bitmap (empty, bitmap, 7 * i, CUTSCENE_HEIGHT - 6, 0);

  if (current_lives <= KID_MINIMUM_LIVES_TO_BLINK && anim_cycle % 2) {
      pop_clipping_rectangle ();
      return;
  }

  for (i = 0; i < current_lives; i++)
    draw_bitmap (full, bitmap, 7 * i, CUTSCENE_HEIGHT - 6, 0);

  pop_clipping_rectangle ();
}

void
increase_kid_current_lives (struct anim *k)
{
  if (k->current_lives <= 0
      || k->current_lives >= k->total_lives) return;

  k->current_lives++;
  if (! is_audio_source_playing (&small_life_potion_audio))
    play_audio (&small_life_potion_audio, NULL, k->id);
  if (k->id == current_kid_id) {
    mr.flicker = 8;
    mr.color = get_flicker_blood_color ();
  }

  kid_haptic (k, KID_HAPTIC_DRINK);
}

void
increase_kid_total_lives (struct anim *k)
{
  if ((k->total_lives >= MAX_LIVES && k->current_lives >= k->total_lives)
      || k->current_lives <= 0)
    return;

  if (k->total_lives < 10) k->total_lives++;
  k->current_lives = k->total_lives;
  if (! is_audio_source_playing (&big_life_potion_audio))
    play_audio (&big_life_potion_audio, NULL, k->id);
  if (k->id == current_kid_id) {
    mr.flicker = 8;
    mr.color = get_flicker_blood_color ();
  }

  kid_haptic (k, KID_HAPTIC_STRONG_DRINK);
}

void
float_kid (struct anim *k)
{
  if (k->current_lives <= 0 && ! is_kid_fall (&k->f)) return;
  k->float_timer = 1;
  stop_audio_instance (&scream_audio, NULL, k->id);
  while (stop_audio_instance (&floating_audio, NULL, k->id));
  play_audio (&floating_audio, NULL, k->id);
  if (k->id == current_kid_id) {
    mr.flicker = 8;
    mr.color = get_flicker_float_color ();
  }
}

ALLEGRO_COLOR
get_flicker_blood_color (void)
{
  switch (vm) {
  case CGA:
    if (hgc) return H_BLOOD_COLOR;
    else return C_BLOOD_COLOR;
  case EGA: return E_BLOOD_COLOR_01;
  case VGA: return V_BLOOD_COLOR_01;
  }
  return V_BLOOD_COLOR_01;
}

ALLEGRO_COLOR
get_flicker_raise_sword_color (void)
{
  switch (vm) {
  case CGA:
    if (hgc) return H_FLICKER_RAISE_SWORD_COLOR;
    else return C_FLICKER_RAISE_SWORD_COLOR;
  case EGA: return E_FLICKER_RAISE_SWORD_COLOR;
  case VGA: return V_FLICKER_RAISE_SWORD_COLOR;
  }
  return V_FLICKER_RAISE_SWORD_COLOR;
}

ALLEGRO_COLOR
get_flicker_float_color (void)
{
  switch (vm) {
  case CGA:
    if (hgc) return H_FLICKER_FLOAT_COLOR;
    else return C_FLICKER_FLOAT_COLOR;
  case EGA: return E_FLICKER_FLOAT_COLOR;
  case VGA: return V_FLICKER_FLOAT_COLOR;
  }
  return V_FLICKER_FLOAT_COLOR;
}

void
kid_haptic (struct anim *k, double cycles)
{
  if (! k || k->id != current_kid_id) return;
  request_gamepad_rumble (1.0, cycles / DEFAULT_HZ);
}

void
kid_haptic_for_range (struct pos *p, coord_f cf, double r, double cycles)
{
  struct anim *k = get_anim_by_id (current_kid_id);
  struct pos pk;
  struct coord ck, cp;
  survey (_m, pos, &k->f, &ck, &pk, NULL);
  con_coord (p, cf, &cp);
  coord2room (&ck, cp.room, &ck);
  if (pk.floor != p->floor) return;
  int d = abs (ck.x - cp.x);
  if (d < r) kid_haptic (k, (1.0 - (d / r)) * cycles);
}

void
kid_debug (void)
{
  if (! DEBUG || cutscene) return;

  struct anim *k = get_anim_by_id (current_kid_id);

  /* begin kid hack */
  if (was_key_pressed (0, ALLEGRO_KEY_DELETE)) k->f.c.x--;
  if (was_key_pressed (0, ALLEGRO_KEY_PGDN)) k->f.c.x++;

  if (! cutscene && was_key_pressed (0, ALLEGRO_KEY_F1)) {
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
    survey (_bf, pos, &k->f, &bf, &pbf, &npbf);

    struct pos ptf;
    survey (_tf, pos, &k->f, NULL, &ptf, NULL);
    struct pos pm;
    survey (_m, pos, &k->f, NULL, &pm, NULL);

    int dn = dist_next_place (&k->f, _tb, pos, 0, false);
    int dp = dist_next_place (&k->f, _tf, pos, 0, true);
    int dc = dist_collision (&k->f, _bf, -4, -4, &k->ci);
    int dcb = dist_collision (&k->f, _bb, +0, +0, &k->ci);
    int df = dist_fall (&k->f, false);
    int dl = dist_con (&k->f, _bf, pos, -4, false, LOOSE_FLOOR);
    int dcl = dist_con (&k->f, _bf, pos, -4, false, CLOSER_FLOOR);
    int dch = dist_chopper (&k->f, false);
    int de = dist_enemy (k);

      printf ("\
f = %i, p = %i, dn = %i, dp = %i, dc = %i, dcb = %i, df = %i, dl = %i, dcl = %i, dch = %i, de = %i\n",
              pbf.floor, pbf.place, dn, dp, dc, dcb, df, dl, dcl, dch, de);

  }
  /* end kid hack */
}





int fellow_shadow_id[9];
int last_fellow_shadow_id;

void
init_fellow_shadow_id (void)
{
  memset (&fellow_shadow_id[0], 0, sizeof (fellow_shadow_id));
  last_fellow_shadow_id = 0;
}

int
next_fellow_shadow_style (void)
{
  int style = mrandom (GUARD_STYLES_NMEMB - 1);
  int max_tries = GUARD_STYLES_NMEMB;

  size_t i;
 restart:
  for (i = 1; i < FELLOW_SHADOW_NMEMB; i++) {
    struct anim *k = get_anim_by_id (fellow_shadow_id[i]);
    if (! k->id) continue;
    if (k->style == style) {
      style = (style + 1) % GUARD_STYLES_NMEMB;
      if (! max_tries--) assert (false);
      goto restart;
    }
  }

  return style;
}
