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

static ALLEGRO_COLOR v_palette (ALLEGRO_COLOR c, void *data);
/* static ALLEGRO_COLOR e_palette (ALLEGRO_COLOR c); */
/* static ALLEGRO_COLOR c_palette (ALLEGRO_COLOR c); */

struct pos *
get_kid_start_pos (struct pos *p)
{
  if (is_valid_pos (&start_pos) && replay_mode == NO_REPLAY)
    *p = start_pos;
  else *p = global_level.start_pos;
  return p;
}

struct actor *
create_kid (struct actor *k0, struct actor *k1, struct pos *p, enum dir dir)
{
  if (k0) {
    k1->shadow_of = k0->id;
    k1->shadow = true;
  } else {
    k1->shadow_of = -1;
    k1->f.b = actor_bitmap (NULL, "KID", "NORMAL", 0);
    k1->fo.b = actor_bitmap (NULL, "KID", "NORMAL", 0);
    k1->action = kid_normal;
    invalid_pos (&k1->item_pos);
    k1->total_hp = KID_INITIAL_TOTAL_HP;
    k1->current_hp = KID_INITIAL_CURRENT_HP;
    k1->fight = true;
    k1->enemy_id = -1;
    k1->skill.counter_attack_prob = -1;
    k1->skill.counter_defense_prob = -1;

    int dx = k1->f.dir == LEFT ? +22 : +28;
    int dy = +15;
    place_actor (k1, p, dx, dy, "KID", "NORMAL", 0);

    update_depressible_floor (k1, -4, -10);
  }

  return k1;
}

void
destroy_kid (struct actor *k)
{
  int i;
  if (current_kid_id == k->id)
    for (i = 0; i < actor_nmemb; i++) {
      struct actor *a = &actor[i];
      if (a->type == KID && a->controllable)
        current_kid_id = a->id;
    }
}

palette
get_kid_palette (void)
{
  /* switch (vm) { */
  /* case CGA: return c_palette; */
  /* case EGA: return e_palette; */
  /* case VGA: */
    return v_palette;
  /* } */
  /* return NULL; */
}

palette
get_shadow_palette (void)
{
  /* switch (vm) { */
  /* case CGA: return c_phantom_shadow_palette; */
  /* case EGA: return e_phantom_shadow_palette; */
  /* case VGA: */
    return v_phantom_shadow_palette;
  /* } */
  /* return NULL; */
}

void
draw_kid_frame (ALLEGRO_BITMAP *bitmap, struct actor *k)
{
  /* push_reset_clipping_rectangle (bitmap); */
  draw_actor_part (bitmap, "KID", "MAIN", k);
  /* pop_clipping_rectangle (); */

  /* if (k->invisible) return; */

  /* struct frame f = k->f; */
  /* struct frame_offset xf = k->xf; */

  /* palette pal = get_kid_palette (); */
  /* f.b = apply_palette (f.b, pal); */
  /* xf.b = apply_palette (xf.b, pal); */

  /* if (k->shadow) { */
  /*   f.b = apply_guard_palette (f.b, SHADOW, k->style); */
  /*   xf.b = apply_guard_palette (xf.b, SHADOW, k->style); */
  /*   palette pals = get_shadow_palette (vm); */
  /*   f.b = apply_palette (f.b, pals); */
  /*   xf.b = apply_palette (xf.b, pals); */
  /* } */

  /* if (hgc) { */
  /*   f.b = apply_palette (f.b, hgc_palette); */
  /*   xf.b = apply_palette (xf.b, hgc_palette); */
  /* } */

  /* draw_xframe (bitmap, &f, &xf); */
  /* draw_frame (bitmap, &f); */
}

void
draw_start_kid (ALLEGRO_BITMAP *bitmap)
{
  /* kid */
  struct frame f;
  f.c.room = global_level.start_pos.room;
  palette pal = get_kid_palette ();
  f.b = actor_bitmap (NULL, "KID", "NORMAL", 0);
  f.b = apply_palette (f.b, pal);
  /* if (hgc) f.b = apply_palette (f.b, hgc_palette); */
  f.b = apply_palette (f.b, start_actor_palette);
  f.flip = (global_level.start_dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;
  place_frame (&f, &f, f.b, &global_level.start_pos,
               global_level.start_dir == LEFT ? +28 : +22, +15);
  draw_frame (bitmap, &f);

  /* sword */
  if (global_level.has_sword)
    draw_sword (bitmap, &global_level.start_pos);
}

ALLEGRO_COLOR
colorful_shadow_palette (ALLEGRO_COLOR c, void *data)
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
v_phantom_shadow_palette (ALLEGRO_COLOR c, void *data)
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
e_phantom_shadow_palette (ALLEGRO_COLOR c, void *data)
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
c_phantom_shadow_palette (ALLEGRO_COLOR c, void *data)
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
v_palette (ALLEGRO_COLOR c, void *data)
{
  if (color_eq (c, HP_COLOR_01)) return V_BLOOD_COLOR_01;
  if (color_eq (c, HP_COLOR_02)) return V_BLOOD_COLOR_02;
  return c;
}

/* static ALLEGRO_COLOR */
/* e_palette (ALLEGRO_COLOR c) */
/* { */
/*   if (color_eq (c, V_KID_HAIR_COLOR_01) */
/*       || color_eq (c, V_KID_HAIR_COLOR_02)) return E_KID_HAIR_COLOR; */
/*   if (color_eq (c, V_KID_SKIN_COLOR_01) */
/*       || color_eq (c, V_KID_SKIN_COLOR_02)) return E_KID_SKIN_COLOR; */
/*   if (color_eq (c, V_KID_CLOTHES_COLOR_01)) return E_KID_CLOTHES_COLOR_01; */
/*   if (color_eq (c, V_KID_CLOTHES_COLOR_02)) return E_KID_CLOTHES_COLOR_02; */
/*   if (color_eq (c, V_KID_EYE_COLOR)) return E_KID_EYE_COLOR; */
/*   if (color_eq (c, V_BLOOD_COLOR_01)) return E_BLOOD_COLOR_01; */
/*   if (color_eq (c, V_BLOOD_COLOR_02)) return E_BLOOD_COLOR_02; */
/*   if (color_eq (c, HP_COLOR_01)) return E_BLOOD_COLOR_01; */
/*   if (color_eq (c, HP_COLOR_02)) return E_BLOOD_COLOR_02; */
/*   return c; */
/* } */

/* static ALLEGRO_COLOR */
/* c_palette (ALLEGRO_COLOR c) */
/* { */
/*   if (color_eq (c, V_KID_HAIR_COLOR_01)) return C_KID_HAIR_COLOR_01; */
/*   if (color_eq (c, V_KID_HAIR_COLOR_02)) return C_KID_HAIR_COLOR_02; */
/*   if (color_eq (c, V_KID_SKIN_COLOR_01) */
/*       || color_eq (c, V_KID_SKIN_COLOR_02)) return C_KID_SKIN_COLOR; */
/*   if (color_eq (c, V_KID_CLOTHES_COLOR_01)) return C_KID_CLOTHES_COLOR_01; */
/*   if (color_eq (c, V_KID_CLOTHES_COLOR_02)) return C_KID_CLOTHES_COLOR_02; */
/*   if (color_eq (c, V_KID_EYE_COLOR)) return C_KID_EYE_COLOR; */
/*   if (color_eq (c, V_BLOOD_COLOR_01) */
/*       || color_eq (c, V_BLOOD_COLOR_02)) return C_BLOOD_COLOR; */
/*   if (color_eq (c, HP_COLOR_01)) return C_BLOOD_COLOR; */
/*   if (color_eq (c, HP_COLOR_02)) return C_BLOOD_COLOR; */
/*   return c; */
/* } */

void
place_kid (struct actor *k, int room, int floor, int place)
{
  enum tile_fg tl, tr;

  struct pos p;
  new_pos (&p, k->f.c.l, -1, -1, -1);
  for (p.room = room; p.room < ROOMS; p.room++)
    for (p.floor = floor; p.floor < FLOORS; p.floor++)
      for (p.place = place; p.place < PLACES; p.place++) {
        enum tile_fg f = fg (&p);
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

  int dx = +15;
  int dy = +15;
  place_actor (k, &p, dx, dy, "KID", "NORMAL", 0);
}

void
draw_kid_hp (ALLEGRO_BITMAP *bitmap, struct actor *k)
{
  push_reset_clipping_rectangle (bitmap);
  draw_actor_part (bitmap, "KID", "HP", k);
  pop_clipping_rectangle ();
}

void
increase_kid_current_hp (struct actor *k)
{
  if (k->current_hp <= 0
      || k->current_hp >= k->total_hp) return;

  k->current_hp++;
  if (! is_audio_source_playing (&small_hp_potion_audio))
    play_audio (&small_hp_potion_audio, NULL, k->id);
  if (k->id == current_kid_id) {
    mr.flicker = 8;
    mr.color = get_flicker_blood_color ();
  }

  kid_haptic (k, KID_HAPTIC_DRINK);
}

void
increase_kid_total_hp (struct actor *k)
{
  if ((k->total_hp >= MAX_HP && k->current_hp >= k->total_hp)
      || k->current_hp <= 0)
    return;

  if (k->total_hp < 10) k->total_hp++;
  k->current_hp = k->total_hp;
  if (! is_audio_source_playing (&big_hp_potion_audio))
    play_audio (&big_hp_potion_audio, NULL, k->id);
  if (k->id == current_kid_id) {
    mr.flicker = 8;
    mr.color = get_flicker_blood_color ();
  }

  kid_haptic (k, KID_HAPTIC_STRONG_DRINK);
}

void
float_kid (struct actor *k)
{
  if (k->current_hp <= 0 && k->action != kid_fall) return;
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
  /* switch (vm) { */
  /* case CGA: */
  /*   if (hgc) return H_BLOOD_COLOR; */
  /*   else return C_BLOOD_COLOR; */
  /* case EGA: return E_BLOOD_COLOR_01; */
  /* case VGA: */
    return V_BLOOD_COLOR_01;
  /* } */
  /* return V_BLOOD_COLOR_01; */
}

ALLEGRO_COLOR
get_flicker_raise_sword_color (void)
{
  /* switch (vm) { */
  /* case CGA: */
  /*   if (hgc) return H_FLICKER_RAISE_SWORD_COLOR; */
  /*   else return C_FLICKER_RAISE_SWORD_COLOR; */
  /* case EGA: return E_FLICKER_RAISE_SWORD_COLOR; */
  /* case VGA: */
    return V_FLICKER_RAISE_SWORD_COLOR;
  /* } */
  /* return V_FLICKER_RAISE_SWORD_COLOR; */
}

ALLEGRO_COLOR
get_flicker_float_color (void)
{
  /* switch (vm) { */
  /* case CGA: */
  /*   if (hgc) return H_FLICKER_FLOAT_COLOR; */
  /*   else return C_FLICKER_FLOAT_COLOR; */
  /* case EGA: return E_FLICKER_FLOAT_COLOR; */
  /* case VGA: */
    return V_FLICKER_FLOAT_COLOR;
  /* } */
  /* return V_FLICKER_FLOAT_COLOR; */
}

void
kid_haptic (struct actor *k, double cycles)
{
  if (! k || k->id != current_kid_id) return;
  request_gamepad_rumble (1.0, cycles / DEFAULT_HZ);
}

void
kid_haptic_for_range (struct pos *p, coord_f cf, lua_Number r, double cycles)
{
  struct actor *k = get_actor_by_id (current_kid_id);
  struct pos pk;
  struct coord ck, cp;
  survey (_m, pos, &k->f, &ck, &pk, NULL);
  tile_coord (p, cf, &cp);
  coord2room (&ck, cp.room, &ck);
  if (pk.floor != p->floor) return;
  int d = fabs (ck.x - cp.x);
  if (d < r) kid_haptic (k, (1.0 - (d / r)) * cycles);
}

void
kid_debug (void)
{
  if (! DEBUG || cutscene) return;

  struct actor *k = get_actor_by_id (current_kid_id);

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
    int dl = dist_tile (&k->f, _bf, pos, -4, false, LOOSE_FLOOR);
    int dcl = dist_tile (&k->f, _bf, pos, -4, false, CLOSER_FLOOR);
    int dch = dist_chomper (&k->f, false);
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
  size_t i;
  fellow_shadow_id[0] = 0;
  for (i = 1; i < FELLOW_SHADOW_NMEMB; i++) fellow_shadow_id[i] = -1;
  last_fellow_shadow_id = 0;
}

int
fellow_shadow_count (void)
{
  size_t i;
  int count = 0;
  for (i = 1; i < FELLOW_SHADOW_NMEMB; i++)
    if (fellow_shadow_id[i] > 0) count++;
  return count;
}

int
next_fellow_shadow_style (void)
{
  int style = prandom (GUARD_STYLES_NMEMB - 1);
  int max_tries = GUARD_STYLES_NMEMB;

  size_t i;
 restart:
  for (i = 1; i < FELLOW_SHADOW_NMEMB; i++) {
    if (fellow_shadow_id[i] < 0) continue;
    struct actor *k = get_actor_by_id (fellow_shadow_id[i]);
    if (! k->id) continue;
    if (k->style == style) {
      style = (style + 1) % GUARD_STYLES_NMEMB;
      if (! max_tries--) assert (false);
      goto restart;
    }
  }

  return style;
}

void
next_fellow_shadow (int d)
{
  if (! last_fellow_shadow_id) {
    ui_msg (0, "NO FELLOW SHADOW");
    return;
  }

  size_t i;
  for (i = 1;
       fellow_shadow_id[i] != last_fellow_shadow_id;
       i = next_remainder (i, FELLOW_SHADOW_NMEMB, 1, d));

  do {
    i = next_remainder (i, FELLOW_SHADOW_NMEMB, 1, d);
    if (fellow_shadow_id[i] < 0) continue;

    struct actor *k = get_actor_by_id (fellow_shadow_id[i]);

    if (fellow_shadow_id[i] == last_fellow_shadow_id) {
      ui_msg (0, "NO FELLOW SHADOW");
      return;
    } else {
      if (k->current_hp > 0) select_controllable_by_id (k->id);
      else continue;
      return;
    }
  } while (true);
}

void
current_fellow_shadow (void)
{
  if (current_kid_id) select_controllable_by_id (0);
  else {
    if (last_fellow_shadow_id) {
      select_controllable_by_id (last_fellow_shadow_id);
      return;
    } else ui_msg (0, "NO FELLOW SHADOW");
  }
}

void
create_fellow_shadow (bool select)
{
  size_t i;
  for (i = 1; i < FELLOW_SHADOW_NMEMB; i++) {
    if (fellow_shadow_id[i] < 0) {
      struct actor *k0 = get_actor_by_id (0);
      if (k0->current_hp <= 0) return;

      int required_strength = fellow_shadow_count () + 1;

      if (required_strength >= k0->current_hp) {
        ui_msg (0, "NO STRENGTH");
        return;
      } else k0->current_hp += -required_strength;

      int id = create_actor (k0, 0, NULL, 0);

      fellow_shadow_id[i] = id;
      struct actor *k = get_actor_by_id (id);
      /* necessary so next_fellow_shadow doesn't consider its color */
      k->style = -1;
      k->style = next_fellow_shadow_style ();
      k->current_hp += required_strength;
      mr.flicker = 12;
      mr.color = get_flicker_raise_sword_color ();
      L_play_audio (main_L, "SUSPENSE", NULL, k->id);

      if (select) select_controllable_by_id (id);
      else {
        struct actor *ke = get_actor_by_id (k->enemy_id);
        if (ke) ke->enemy_id = id;
        last_fellow_shadow_id = id;
        k->selection_cycle = anim_cycle;
        k->death_timer = 0;
      }

      return;
    }
  }
  ui_msg (0, "NO FELLOW SHADOW");
}
