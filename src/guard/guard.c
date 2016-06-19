/*
  guard.c -- guard module;

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

/* bitmaps */
ALLEGRO_BITMAP *guard_life, *guard_splash;

static struct coord *guard_life_coord (int i, struct coord *c);
static ALLEGRO_COLOR c_shadow_life_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR e_shadow_life_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR v_shadow_life_palette (ALLEGRO_COLOR c);
static palette get_shadow_life_palette (enum vm vm);
static ALLEGRO_COLOR skeleton_life_palette (ALLEGRO_COLOR c);

void
load_guard (void)
{
  load_guard_normal ();
  load_guard_vigilant ();
  load_guard_hit ();
  load_guard_die ();
  load_guard_fall ();
  load_guard_walkf ();
  load_guard_walkb ();
  load_guard_attack ();
  load_guard_defense ();

  /* bitmaps */
  guard_life = load_bitmap (GUARD_LIFE);
  guard_splash = load_bitmap (GUARD_SPLASH);
}

void
unload_guard (void)
{
  unload_guard_normal ();
  unload_guard_vigilant ();
  unload_guard_hit ();
  unload_guard_die ();
  unload_guard_fall ();
  unload_guard_walkf ();
  unload_guard_walkb ();
  unload_guard_attack ();
  unload_guard_defense ();

  /* bitmaps */
  destroy_bitmap (guard_life);
  destroy_bitmap (guard_splash);
}

struct anim *
create_guard (struct anim *g0, struct anim *g1, struct pos *p, enum dir dir)
{
  if (! g0) {
    g1->f.b = get_guard_normal_bitmap (g1->type);
    g1->fo.b = g1->f.b;
    g1->action = guard_normal;
    g1->total_lives = 3;
    g1->current_lives = 3;
    g1->enemy_id = -1;
    g1->oenemy_id = -1;
    g1->fight = true;
    g1->has_sword = true;
    g1->skill.counter_attack_prob = -1;
    g1->skill.counter_defense_prob = -1;
    g1->glory_sample = false;

    if (g1->type == SHADOW) g1->shadow = true;

    place_frame (&g1->f, &g1->f, g1->f.b, p,
                 g1->f.dir == LEFT ? +16 : +22, +14);
    place_on_the_ground (&g1->f, &g1->f.c);

    update_depressible_floor (g1, -7, -26);
  }

  return g1;
}

void
apply_guard_mode (struct anim *g, enum gm gm)
{
  if (! is_guard (g)) return;
  switch (gm) {
  case ORIGINAL_GM: g->type = g->original_type; break;
  case GUARD_GM: g->type = GUARD; break;
  case FAT_GUARD_GM: g->type = FAT_GUARD; break;
  case VIZIER_GM: g->type = VIZIER; break;
  case SKELETON_GM: g->type = SKELETON; break;
  case SHADOW_GM: g->type = SHADOW; break;
  }
}

static ALLEGRO_COLOR
v_salmon_palette (ALLEGRO_COLOR c)
{
  /* honor transparency */
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;

  /* KID */
  if (color_eq (c, V_KID_CLOTHES_COLOR_01)) return V_KID_SHADOW_CLOTHES_COLOR_01;
  if (color_eq (c, V_KID_CLOTHES_COLOR_02)) return V_KID_SHADOW_CLOTHES_COLOR_02;

  /* SKELETON */
  if (color_eq (c, SKELETON_COLOR)) return V_SKELETON_COLOR;

  /* GUARD, FAT_GUARD, VIZIER */
  if (color_eq (c, GUARD_SHOES_COLOR)) return V_SALMON_GUARD_SHOES_COLOR;
  if (color_eq (c, GUARD_PANTS_COLOR_01)) return V_SALMON_GUARD_PANTS_COLOR_01;
  if (color_eq (c, GUARD_PANTS_COLOR_02)) return V_SALMON_GUARD_PANTS_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_01)) return V_SALMON_GUARD_CAPE_COLOR_01;
  if (color_eq (c, GUARD_CAPE_COLOR_02)) return V_SALMON_GUARD_CAPE_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_03)) return V_SALMON_GUARD_CAPE_COLOR_03;
  if (color_eq (c, GUARD_BELT_COLOR)) return V_SALMON_GUARD_BELT_COLOR;
  if (color_eq (c, GUARD_HAT_COLOR_01)) return V_SALMON_GUARD_HAT_COLOR_01;
  if (color_eq (c, GUARD_HAT_COLOR_02)) return V_SALMON_GUARD_HAT_COLOR_02;
  if (color_eq (c, GUARD_SKIN_COLOR_01)) return V_SALMON_GUARD_SKIN_COLOR_01;
  if (color_eq (c, GUARD_SKIN_COLOR_02)) return V_SALMON_GUARD_SKIN_COLOR_02;
  if (color_eq (c, GUARD_EYES_COLOR)) return V_SALMON_GUARD_EYES_COLOR;
  if (color_eq (c, GUARD_CLOSED_EYES_COLOR)) return V_SALMON_GUARD_CLOSED_EYES_COLOR;
  if (color_eq (c, GUARD_BLOOD_COLOR)) return V_SALMON_GUARD_BLOOD_COLOR;
  if (color_eq (c, GUARD_SPLASH_COLOR)) return V_SALMON_GUARD_SPLASH_COLOR;
  return c;
}

static ALLEGRO_COLOR
v_light_blue_palette (ALLEGRO_COLOR c)
{
  /* honor transparency */
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;

  /* KID */
  if (color_eq (c, V_KID_CLOTHES_COLOR_01)) return V_LIGHT_BLUE_GUARD_PANTS_COLOR_01;
  if (color_eq (c, V_KID_CLOTHES_COLOR_02)) return V_LIGHT_BLUE_GUARD_PANTS_COLOR_02;

  /* SKELETON */
  if (color_eq (c, SKELETON_COLOR)) return V_LIGHT_BLUE_GUARD_PANTS_COLOR_01;

  /* GUARD, FAT_GUARD, VIZIER */
  if (color_eq (c, GUARD_SHOES_COLOR)) return V_LIGHT_BLUE_GUARD_SHOES_COLOR;
  if (color_eq (c, GUARD_PANTS_COLOR_01)) return V_LIGHT_BLUE_GUARD_PANTS_COLOR_01;
  if (color_eq (c, GUARD_PANTS_COLOR_02)) return V_LIGHT_BLUE_GUARD_PANTS_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_01)) return V_LIGHT_BLUE_GUARD_CAPE_COLOR_01;
  if (color_eq (c, GUARD_CAPE_COLOR_02)) return V_LIGHT_BLUE_GUARD_CAPE_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_03)) return V_LIGHT_BLUE_GUARD_CAPE_COLOR_03;
  if (color_eq (c, GUARD_BELT_COLOR)) return V_LIGHT_BLUE_GUARD_BELT_COLOR;
  if (color_eq (c, GUARD_HAT_COLOR_01)) return V_LIGHT_BLUE_GUARD_HAT_COLOR_01;
  if (color_eq (c, GUARD_HAT_COLOR_02)) return V_LIGHT_BLUE_GUARD_HAT_COLOR_02;
  if (color_eq (c, GUARD_SKIN_COLOR_01)) return V_LIGHT_BLUE_GUARD_SKIN_COLOR_01;
  if (color_eq (c, GUARD_SKIN_COLOR_02)) return V_LIGHT_BLUE_GUARD_SKIN_COLOR_02;
  if (color_eq (c, GUARD_EYES_COLOR)) return V_LIGHT_BLUE_GUARD_EYES_COLOR;
  if (color_eq (c, GUARD_CLOSED_EYES_COLOR))
    return V_LIGHT_BLUE_GUARD_CLOSED_EYES_COLOR;
  if (color_eq (c, GUARD_BLOOD_COLOR)) return V_LIGHT_BLUE_GUARD_BLOOD_COLOR;
  if (color_eq (c, GUARD_SPLASH_COLOR)) return V_LIGHT_BLUE_GUARD_SPLASH_COLOR;
  return c;
}

static ALLEGRO_COLOR
v_red_palette (ALLEGRO_COLOR c)
{
  /* honor transparency */
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;

  /* KID */
  if (color_eq (c, V_KID_CLOTHES_COLOR_01)) return V_RED_GUARD_PANTS_COLOR_01;
  if (color_eq (c, V_KID_CLOTHES_COLOR_02)) return V_RED_GUARD_PANTS_COLOR_02;

  /* SKELETON */
  if (color_eq (c, SKELETON_COLOR)) return V_RED_GUARD_PANTS_COLOR_01;

  /* GUARD, FAT_GUARD, VIZIER */
  if (color_eq (c, GUARD_SHOES_COLOR)) return V_RED_GUARD_SHOES_COLOR;
  if (color_eq (c, GUARD_PANTS_COLOR_01)) return V_RED_GUARD_PANTS_COLOR_01;
  if (color_eq (c, GUARD_PANTS_COLOR_02)) return V_RED_GUARD_PANTS_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_01)) return V_RED_GUARD_CAPE_COLOR_01;
  if (color_eq (c, GUARD_CAPE_COLOR_02)) return V_RED_GUARD_CAPE_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_03)) return V_RED_GUARD_CAPE_COLOR_03;
  if (color_eq (c, GUARD_BELT_COLOR)) return V_RED_GUARD_BELT_COLOR;
  if (color_eq (c, GUARD_HAT_COLOR_01)) return V_RED_GUARD_HAT_COLOR_01;
  if (color_eq (c, GUARD_HAT_COLOR_02)) return V_RED_GUARD_HAT_COLOR_02;
  if (color_eq (c, GUARD_SKIN_COLOR_01)) return V_RED_GUARD_SKIN_COLOR_01;
  if (color_eq (c, GUARD_SKIN_COLOR_02)) return V_RED_GUARD_SKIN_COLOR_02;
  if (color_eq (c, GUARD_EYES_COLOR)) return V_RED_GUARD_EYES_COLOR;
  if (color_eq (c, GUARD_CLOSED_EYES_COLOR)) return V_RED_GUARD_CLOSED_EYES_COLOR;
  if (color_eq (c, GUARD_BLOOD_COLOR)) return V_RED_GUARD_BLOOD_COLOR;
  if (color_eq (c, GUARD_SPLASH_COLOR)) return V_RED_GUARD_SPLASH_COLOR;
  return c;
}

static ALLEGRO_COLOR
v_orange_palette (ALLEGRO_COLOR c)
{
  /* honor transparency */
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;

  /* KID */
  if (color_eq (c, V_KID_CLOTHES_COLOR_01)) return V_ORANGE_GUARD_PANTS_COLOR_01;
  if (color_eq (c, V_KID_CLOTHES_COLOR_02)) return V_ORANGE_GUARD_PANTS_COLOR_02;

  /* SKELETON */
  if (color_eq (c, SKELETON_COLOR)) return V_ORANGE_GUARD_PANTS_COLOR_01;

  /* GUARD, FAT_GUARD, VIZIER */
  if (color_eq (c, GUARD_SHOES_COLOR)) return V_ORANGE_GUARD_SHOES_COLOR;
  if (color_eq (c, GUARD_PANTS_COLOR_01)) return V_ORANGE_GUARD_PANTS_COLOR_01;
  if (color_eq (c, GUARD_PANTS_COLOR_02)) return V_ORANGE_GUARD_PANTS_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_01)) return V_ORANGE_GUARD_CAPE_COLOR_01;
  if (color_eq (c, GUARD_CAPE_COLOR_02)) return V_ORANGE_GUARD_CAPE_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_03)) return V_ORANGE_GUARD_CAPE_COLOR_03;
  if (color_eq (c, GUARD_BELT_COLOR)) return V_ORANGE_GUARD_BELT_COLOR;
  if (color_eq (c, GUARD_HAT_COLOR_01)) return V_ORANGE_GUARD_HAT_COLOR_01;
  if (color_eq (c, GUARD_HAT_COLOR_02)) return V_ORANGE_GUARD_HAT_COLOR_02;
  if (color_eq (c, GUARD_SKIN_COLOR_01)) return V_ORANGE_GUARD_SKIN_COLOR_01;
  if (color_eq (c, GUARD_SKIN_COLOR_02)) return V_ORANGE_GUARD_SKIN_COLOR_02;
  if (color_eq (c, GUARD_EYES_COLOR)) return V_ORANGE_GUARD_EYES_COLOR;
  if (color_eq (c, GUARD_CLOSED_EYES_COLOR)) return V_ORANGE_GUARD_CLOSED_EYES_COLOR;
  if (color_eq (c, GUARD_BLOOD_COLOR)) return V_ORANGE_GUARD_BLOOD_COLOR;
  if (color_eq (c, GUARD_SPLASH_COLOR)) return V_ORANGE_GUARD_SPLASH_COLOR;
  return c;
}

static ALLEGRO_COLOR
v_green_palette (ALLEGRO_COLOR c)
{
  /* honor transparency */
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;

  /* KID */
  if (color_eq (c, V_KID_CLOTHES_COLOR_01)) return V_GREEN_GUARD_PANTS_COLOR_01;
  if (color_eq (c, V_KID_CLOTHES_COLOR_02)) return V_GREEN_GUARD_PANTS_COLOR_02;

  /* SKELETON */
  if (color_eq (c, SKELETON_COLOR)) return V_GREEN_GUARD_PANTS_COLOR_01;

  /* GUARD, FAT_GUARD, VIZIER */
  if (color_eq (c, GUARD_SHOES_COLOR)) return V_GREEN_GUARD_SHOES_COLOR;
  if (color_eq (c, GUARD_PANTS_COLOR_01)) return V_GREEN_GUARD_PANTS_COLOR_01;
  if (color_eq (c, GUARD_PANTS_COLOR_02)) return V_GREEN_GUARD_PANTS_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_01)) return V_GREEN_GUARD_CAPE_COLOR_01;
  if (color_eq (c, GUARD_CAPE_COLOR_02)) return V_GREEN_GUARD_CAPE_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_03)) return V_GREEN_GUARD_CAPE_COLOR_03;
  if (color_eq (c, GUARD_BELT_COLOR)) return V_GREEN_GUARD_BELT_COLOR;
  if (color_eq (c, GUARD_HAT_COLOR_01)) return V_GREEN_GUARD_HAT_COLOR_01;
  if (color_eq (c, GUARD_HAT_COLOR_02)) return V_GREEN_GUARD_HAT_COLOR_02;
  if (color_eq (c, GUARD_SKIN_COLOR_01)) return V_GREEN_GUARD_SKIN_COLOR_01;
  if (color_eq (c, GUARD_SKIN_COLOR_02)) return V_GREEN_GUARD_SKIN_COLOR_02;
  if (color_eq (c, GUARD_EYES_COLOR)) return V_GREEN_GUARD_EYES_COLOR;
  if (color_eq (c, GUARD_CLOSED_EYES_COLOR)) return V_GREEN_GUARD_CLOSED_EYES_COLOR;
  if (color_eq (c, GUARD_BLOOD_COLOR)) return V_GREEN_GUARD_BLOOD_COLOR;
  if (color_eq (c, GUARD_SPLASH_COLOR)) return V_GREEN_GUARD_SPLASH_COLOR;
  return c;
}

static ALLEGRO_COLOR
v_dark_blue_palette (ALLEGRO_COLOR c)
{
  /* honor transparency */
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;

  /* KID */
  if (color_eq (c, V_KID_CLOTHES_COLOR_01)) return V_DARK_BLUE_GUARD_PANTS_COLOR_01;
  if (color_eq (c, V_KID_CLOTHES_COLOR_02)) return V_DARK_BLUE_GUARD_PANTS_COLOR_02;

  /* SKELETON */
  if (color_eq (c, SKELETON_COLOR)) return V_DARK_BLUE_GUARD_PANTS_COLOR_01;

  /* GUARD, FAT_GUARD, VIZIER */
  if (color_eq (c, GUARD_SHOES_COLOR)) return V_DARK_BLUE_GUARD_SHOES_COLOR;
  if (color_eq (c, GUARD_PANTS_COLOR_01)) return V_DARK_BLUE_GUARD_PANTS_COLOR_01;
  if (color_eq (c, GUARD_PANTS_COLOR_02)) return V_DARK_BLUE_GUARD_PANTS_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_01)) return V_DARK_BLUE_GUARD_CAPE_COLOR_01;
  if (color_eq (c, GUARD_CAPE_COLOR_02)) return V_DARK_BLUE_GUARD_CAPE_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_03)) return V_DARK_BLUE_GUARD_CAPE_COLOR_03;
  if (color_eq (c, GUARD_BELT_COLOR)) return V_DARK_BLUE_GUARD_BELT_COLOR;
  if (color_eq (c, GUARD_HAT_COLOR_01)) return V_DARK_BLUE_GUARD_HAT_COLOR_01;
  if (color_eq (c, GUARD_HAT_COLOR_02)) return V_DARK_BLUE_GUARD_HAT_COLOR_02;
  if (color_eq (c, GUARD_SKIN_COLOR_01)) return V_DARK_BLUE_GUARD_SKIN_COLOR_01;
  if (color_eq (c, GUARD_SKIN_COLOR_02)) return V_DARK_BLUE_GUARD_SKIN_COLOR_02;
  if (color_eq (c, GUARD_EYES_COLOR)) return V_DARK_BLUE_GUARD_EYES_COLOR;
  if (color_eq (c, GUARD_CLOSED_EYES_COLOR))
    return V_DARK_BLUE_GUARD_CLOSED_EYES_COLOR;
  if (color_eq (c, GUARD_BLOOD_COLOR)) return V_DARK_BLUE_GUARD_BLOOD_COLOR;
  if (color_eq (c, GUARD_SPLASH_COLOR)) return V_DARK_BLUE_GUARD_SPLASH_COLOR;
  return c;
}

static ALLEGRO_COLOR
v_purple_palette (ALLEGRO_COLOR c)
{
  /* honor transparency */
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;

  /* KID */
  if (color_eq (c, V_KID_CLOTHES_COLOR_01)) return V_PURPLE_GUARD_PANTS_COLOR_01;
  if (color_eq (c, V_KID_CLOTHES_COLOR_02)) return V_PURPLE_GUARD_PANTS_COLOR_02;

  /* SKELETON */
  if (color_eq (c, SKELETON_COLOR)) return V_PURPLE_GUARD_PANTS_COLOR_01;

  /* GUARD, FAT_GUARD, VIZIER */
  if (color_eq (c, GUARD_SHOES_COLOR)) return V_PURPLE_GUARD_SHOES_COLOR;
  if (color_eq (c, GUARD_PANTS_COLOR_01)) return V_PURPLE_GUARD_PANTS_COLOR_01;
  if (color_eq (c, GUARD_PANTS_COLOR_02)) return V_PURPLE_GUARD_PANTS_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_01)) return V_PURPLE_GUARD_CAPE_COLOR_01;
  if (color_eq (c, GUARD_CAPE_COLOR_02)) return V_PURPLE_GUARD_CAPE_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_03)) return V_PURPLE_GUARD_CAPE_COLOR_03;
  if (color_eq (c, GUARD_BELT_COLOR)) return V_PURPLE_GUARD_BELT_COLOR;
  if (color_eq (c, GUARD_HAT_COLOR_01)) return V_PURPLE_GUARD_HAT_COLOR_01;
  if (color_eq (c, GUARD_HAT_COLOR_02)) return V_PURPLE_GUARD_HAT_COLOR_02;
  if (color_eq (c, GUARD_SKIN_COLOR_01)) return V_PURPLE_GUARD_SKIN_COLOR_01;
  if (color_eq (c, GUARD_SKIN_COLOR_02)) return V_PURPLE_GUARD_SKIN_COLOR_02;
  if (color_eq (c, GUARD_EYES_COLOR)) return V_PURPLE_GUARD_EYES_COLOR;
  if (color_eq (c, GUARD_CLOSED_EYES_COLOR)) return V_PURPLE_GUARD_CLOSED_EYES_COLOR;
  if (color_eq (c, GUARD_BLOOD_COLOR)) return V_PURPLE_GUARD_BLOOD_COLOR;
  if (color_eq (c, GUARD_SPLASH_COLOR)) return V_PURPLE_GUARD_SPLASH_COLOR;
  return c;
}

static ALLEGRO_COLOR
v_yellow_palette (ALLEGRO_COLOR c)
{
  /* honor transparency */
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;

  /* KID */
  if (color_eq (c, V_KID_CLOTHES_COLOR_01)) return V_YELLOW_GUARD_PANTS_COLOR_01;
  if (color_eq (c, V_KID_CLOTHES_COLOR_02)) return V_YELLOW_GUARD_PANTS_COLOR_02;

  /* SKELETON */
  if (color_eq (c, SKELETON_COLOR)) return V_YELLOW_GUARD_PANTS_COLOR_01;

  /* GUARD, FAT_GUARD, VIZIER */
  if (color_eq (c, GUARD_SHOES_COLOR)) return V_YELLOW_GUARD_SHOES_COLOR;
  if (color_eq (c, GUARD_PANTS_COLOR_01)) return V_YELLOW_GUARD_PANTS_COLOR_01;
  if (color_eq (c, GUARD_PANTS_COLOR_02)) return V_YELLOW_GUARD_PANTS_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_01)) return V_YELLOW_GUARD_CAPE_COLOR_01;
  if (color_eq (c, GUARD_CAPE_COLOR_02)) return V_YELLOW_GUARD_CAPE_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_03)) return V_YELLOW_GUARD_CAPE_COLOR_03;
  if (color_eq (c, GUARD_BELT_COLOR)) return V_YELLOW_GUARD_BELT_COLOR;
  if (color_eq (c, GUARD_HAT_COLOR_01)) return V_YELLOW_GUARD_HAT_COLOR_01;
  if (color_eq (c, GUARD_HAT_COLOR_02)) return V_YELLOW_GUARD_HAT_COLOR_02;
  if (color_eq (c, GUARD_SKIN_COLOR_01)) return V_YELLOW_GUARD_SKIN_COLOR_01;
  if (color_eq (c, GUARD_SKIN_COLOR_02)) return V_YELLOW_GUARD_SKIN_COLOR_02;
  if (color_eq (c, GUARD_EYES_COLOR)) return V_YELLOW_GUARD_EYES_COLOR;
  if (color_eq (c, GUARD_CLOSED_EYES_COLOR)) return V_YELLOW_GUARD_CLOSED_EYES_COLOR;
  if (color_eq (c, GUARD_BLOOD_COLOR)) return V_YELLOW_GUARD_BLOOD_COLOR;
  if (color_eq (c, GUARD_SPLASH_COLOR)) return V_YELLOW_GUARD_SPLASH_COLOR;
  return c;
}

static ALLEGRO_COLOR
e_dark_red_palette (ALLEGRO_COLOR c)
{
  /* honor transparency */
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;

  /* KID */
  if (color_eq (c, E_KID_CLOTHES_COLOR_01)) return E_KID_SHADOW_CLOTHES_COLOR_01;
  if (color_eq (c, E_KID_CLOTHES_COLOR_02)) return E_KID_SHADOW_CLOTHES_COLOR_02;
  if (color_eq (c, E_KID_SKIN_COLOR)) return E_KID_SHADOW_SKIN_COLOR;
  if (color_eq (c, E_KID_HAIR_COLOR)) return E_KID_SHADOW_HAIR_COLOR;
  if (color_eq (c, E_KID_EYE_COLOR)) return E_KID_SHADOW_EYE_COLOR;

  /* SKELETON */
  if (color_eq (c, SKELETON_COLOR)) return E_SKELETON_COLOR;

  /* GUARD, FAT_GUARD, VIZIER */
  if (color_eq (c, GUARD_SHOES_COLOR)) return E_DARK_RED_GUARD_SHOES_COLOR;
  if (color_eq (c, GUARD_PANTS_COLOR_01)) return E_DARK_RED_GUARD_PANTS_COLOR_01;
  if (color_eq (c, GUARD_PANTS_COLOR_02)) return E_DARK_RED_GUARD_PANTS_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_01)) return E_DARK_RED_GUARD_CAPE_COLOR_01;
  if (color_eq (c, GUARD_CAPE_COLOR_02)) return E_DARK_RED_GUARD_CAPE_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_03)) return E_DARK_RED_GUARD_CAPE_COLOR_03;
  if (color_eq (c, GUARD_BELT_COLOR)) return E_DARK_RED_GUARD_BELT_COLOR;
  if (color_eq (c, GUARD_HAT_COLOR_01)) return E_DARK_RED_GUARD_HAT_COLOR_01;
  if (color_eq (c, GUARD_HAT_COLOR_02)) return E_DARK_RED_GUARD_HAT_COLOR_02;
  if (color_eq (c, GUARD_SKIN_COLOR_01)) return E_DARK_RED_GUARD_SKIN_COLOR_01;
  if (color_eq (c, GUARD_SKIN_COLOR_02)) return E_DARK_RED_GUARD_SKIN_COLOR_02;
  if (color_eq (c, GUARD_EYES_COLOR)) return E_DARK_RED_GUARD_EYES_COLOR;
  if (color_eq (c, GUARD_CLOSED_EYES_COLOR)) return E_DARK_RED_GUARD_CLOSED_EYES_COLOR;
  if (color_eq (c, GUARD_BLOOD_COLOR)) return E_DARK_RED_GUARD_BLOOD_COLOR;
  if (color_eq (c, GUARD_SPLASH_COLOR)) return E_DARK_RED_GUARD_SPLASH_COLOR;
  return c;
}

static ALLEGRO_COLOR
e_red_palette (ALLEGRO_COLOR c)
{
  /* honor transparency */
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;

  /* KID */
  if (color_eq (c, E_KID_CLOTHES_COLOR_01)) return E_RED_GUARD_PANTS_COLOR_01;
  if (color_eq (c, E_KID_CLOTHES_COLOR_02)) return E_RED_GUARD_PANTS_COLOR_02;
  if (color_eq (c, E_KID_EYE_COLOR)) return E_RED_GUARD_EYES_COLOR;

  /* SKELETON */
  if (color_eq (c, SKELETON_COLOR)) return E_RED_GUARD_PANTS_COLOR_01;

  /* GUARD, FAT_GUARD, VIZIER */
  if (color_eq (c, GUARD_SHOES_COLOR)) return E_RED_GUARD_SHOES_COLOR;
  if (color_eq (c, GUARD_PANTS_COLOR_01)) return E_RED_GUARD_PANTS_COLOR_01;
  if (color_eq (c, GUARD_PANTS_COLOR_02)) return E_RED_GUARD_PANTS_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_01)) return E_RED_GUARD_CAPE_COLOR_01;
  if (color_eq (c, GUARD_CAPE_COLOR_02)) return E_RED_GUARD_CAPE_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_03)) return E_RED_GUARD_CAPE_COLOR_03;
  if (color_eq (c, GUARD_BELT_COLOR)) return E_RED_GUARD_BELT_COLOR;
  if (color_eq (c, GUARD_HAT_COLOR_01)) return E_RED_GUARD_HAT_COLOR_01;
  if (color_eq (c, GUARD_HAT_COLOR_02)) return E_RED_GUARD_HAT_COLOR_02;
  if (color_eq (c, GUARD_SKIN_COLOR_01)) return E_RED_GUARD_SKIN_COLOR_01;
  if (color_eq (c, GUARD_SKIN_COLOR_02)) return E_RED_GUARD_SKIN_COLOR_02;
  if (color_eq (c, GUARD_EYES_COLOR)) return E_RED_GUARD_EYES_COLOR;
  if (color_eq (c, GUARD_CLOSED_EYES_COLOR)) return E_RED_GUARD_CLOSED_EYES_COLOR;
  if (color_eq (c, GUARD_BLOOD_COLOR)) return E_RED_GUARD_BLOOD_COLOR;
  if (color_eq (c, GUARD_SPLASH_COLOR)) return E_RED_GUARD_SPLASH_COLOR;
  return c;
}

static ALLEGRO_COLOR
e_green_palette (ALLEGRO_COLOR c)
{
  /* honor transparency */
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;

  /* KID */
  if (color_eq (c, E_KID_CLOTHES_COLOR_01)) return E_GREEN_GUARD_PANTS_COLOR_01;
  if (color_eq (c, E_KID_CLOTHES_COLOR_02)) return E_GREEN_GUARD_PANTS_COLOR_02;
  if (color_eq (c, E_KID_EYE_COLOR)) return E_GREEN_GUARD_EYES_COLOR;

  /* SKELETON */
  if (color_eq (c, SKELETON_COLOR)) return E_GREEN_GUARD_PANTS_COLOR_01;

  /* GUARD, FAT_GUARD, VIZIER */
  if (color_eq (c, GUARD_SHOES_COLOR)) return E_GREEN_GUARD_SHOES_COLOR;
  if (color_eq (c, GUARD_PANTS_COLOR_01)) return E_GREEN_GUARD_PANTS_COLOR_01;
  if (color_eq (c, GUARD_PANTS_COLOR_02)) return E_GREEN_GUARD_PANTS_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_01)) return E_GREEN_GUARD_CAPE_COLOR_01;
  if (color_eq (c, GUARD_CAPE_COLOR_02)) return E_GREEN_GUARD_CAPE_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_03)) return E_GREEN_GUARD_CAPE_COLOR_03;
  if (color_eq (c, GUARD_BELT_COLOR)) return E_GREEN_GUARD_BELT_COLOR;
  if (color_eq (c, GUARD_HAT_COLOR_01)) return E_GREEN_GUARD_HAT_COLOR_01;
  if (color_eq (c, GUARD_HAT_COLOR_02)) return E_GREEN_GUARD_HAT_COLOR_02;
  if (color_eq (c, GUARD_SKIN_COLOR_01)) return E_GREEN_GUARD_SKIN_COLOR_01;
  if (color_eq (c, GUARD_SKIN_COLOR_02)) return E_GREEN_GUARD_SKIN_COLOR_02;
  if (color_eq (c, GUARD_EYES_COLOR)) return E_GREEN_GUARD_EYES_COLOR;
  if (color_eq (c, GUARD_CLOSED_EYES_COLOR)) return E_GREEN_GUARD_CLOSED_EYES_COLOR;
  if (color_eq (c, GUARD_BLOOD_COLOR)) return E_GREEN_GUARD_BLOOD_COLOR;
  if (color_eq (c, GUARD_SPLASH_COLOR)) return E_GREEN_GUARD_SPLASH_COLOR;
  return c;
}

static ALLEGRO_COLOR
c_green_palette (ALLEGRO_COLOR c)
{
  /* honor transparency */
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;

  /* KID */
  if (color_eq (c, C_KID_CLOTHES_COLOR_01)) return C_KID_SHADOW_CLOTHES_COLOR_01;
  if (color_eq (c, C_KID_CLOTHES_COLOR_02)) return C_KID_SHADOW_CLOTHES_COLOR_02;
  if (color_eq (c, C_KID_SKIN_COLOR)) return C_KID_SHADOW_SKIN_COLOR;
  if (color_eq (c, C_KID_EYE_COLOR)) return C_KID_SHADOW_EYE_COLOR;

  /* SKELETON */
  if (color_eq (c, SKELETON_COLOR)) return C_SKELETON_COLOR;

  /* GUARD, FAT_GUARD, VIZIER */
  if (color_eq (c, GUARD_SHOES_COLOR)) return C_GREEN_GUARD_SHOES_COLOR;
  if (color_eq (c, GUARD_PANTS_COLOR_01)) return C_GREEN_GUARD_PANTS_COLOR_01;
  if (color_eq (c, GUARD_PANTS_COLOR_02)) return C_GREEN_GUARD_PANTS_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_01)) return C_GREEN_GUARD_CAPE_COLOR_01;
  if (color_eq (c, GUARD_CAPE_COLOR_02)) return C_GREEN_GUARD_CAPE_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_03)) return C_GREEN_GUARD_CAPE_COLOR_03;
  if (color_eq (c, GUARD_BELT_COLOR)) return C_GREEN_GUARD_BELT_COLOR;
  if (color_eq (c, GUARD_HAT_COLOR_01)) return C_GREEN_GUARD_HAT_COLOR_01;
  if (color_eq (c, GUARD_HAT_COLOR_02)) return C_GREEN_GUARD_HAT_COLOR_02;
  if (color_eq (c, GUARD_SKIN_COLOR_01)) return C_GREEN_GUARD_SKIN_COLOR_01;
  if (color_eq (c, GUARD_SKIN_COLOR_02)) return C_GREEN_GUARD_SKIN_COLOR_02;
  if (color_eq (c, GUARD_EYES_COLOR)) return C_GREEN_GUARD_EYES_COLOR;
  if (color_eq (c, GUARD_CLOSED_EYES_COLOR)) return C_GREEN_GUARD_CLOSED_EYES_COLOR;
  if (color_eq (c, GUARD_BLOOD_COLOR)) return C_GREEN_GUARD_BLOOD_COLOR;
  if (color_eq (c, GUARD_SPLASH_COLOR)) return C_GREEN_GUARD_SPLASH_COLOR;
  return c;
}

static ALLEGRO_COLOR
c_red_palette (ALLEGRO_COLOR c)
{
  /* honor transparency */
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;

  /* KID */
  if (color_eq (c, C_KID_CLOTHES_COLOR_01)) return C_RED_GUARD_PANTS_COLOR_01;
  if (color_eq (c, C_KID_CLOTHES_COLOR_02)) return C_KID_SHADOW_CLOTHES_COLOR_02;
  if (color_eq (c, C_KID_SKIN_COLOR)) return C_RED_GUARD_SKIN_COLOR_01;
  if (color_eq (c, C_KID_EYE_COLOR)) return C_RED_GUARD_EYES_COLOR;

  /* SKELETON */
  if (color_eq (c, SKELETON_COLOR)) return C_RED_GUARD_PANTS_COLOR_01;

  /* GUARD, FAT_GUARD, VIZIER */
  if (color_eq (c, GUARD_SHOES_COLOR)) return C_RED_GUARD_SHOES_COLOR;
  if (color_eq (c, GUARD_PANTS_COLOR_01)) return C_RED_GUARD_PANTS_COLOR_01;
  if (color_eq (c, GUARD_PANTS_COLOR_02)) return C_RED_GUARD_PANTS_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_01)) return C_RED_GUARD_CAPE_COLOR_01;
  if (color_eq (c, GUARD_CAPE_COLOR_02)) return C_RED_GUARD_CAPE_COLOR_02;
  if (color_eq (c, GUARD_CAPE_COLOR_03)) return C_RED_GUARD_CAPE_COLOR_03;
  if (color_eq (c, GUARD_BELT_COLOR)) return C_RED_GUARD_BELT_COLOR;
  if (color_eq (c, GUARD_HAT_COLOR_01)) return C_RED_GUARD_HAT_COLOR_01;
  if (color_eq (c, GUARD_HAT_COLOR_02)) return C_RED_GUARD_HAT_COLOR_02;
  if (color_eq (c, GUARD_SKIN_COLOR_01)) return C_RED_GUARD_SKIN_COLOR_01;
  if (color_eq (c, GUARD_SKIN_COLOR_02)) return C_RED_GUARD_SKIN_COLOR_02;
  if (color_eq (c, GUARD_EYES_COLOR)) return C_RED_GUARD_EYES_COLOR;
  if (color_eq (c, GUARD_CLOSED_EYES_COLOR)) return C_RED_GUARD_CLOSED_EYES_COLOR;
  if (color_eq (c, GUARD_BLOOD_COLOR)) return C_RED_GUARD_BLOOD_COLOR;
  if (color_eq (c, GUARD_SPLASH_COLOR)) return C_RED_GUARD_SPLASH_COLOR;
  return c;
}

palette
get_guard_palette (int style, enum vm vm)
{
  switch (vm) {
  case CGA:
    switch (style) {
    case 0: return c_green_palette;
    case 1: default: return c_red_palette;
    case 2: return c_red_palette;
    case 3: return c_red_palette;
    case 4: return c_red_palette;
    case 5: return c_red_palette;
    case 6: return c_red_palette;
    case 7: return c_red_palette;
    }
  case EGA:
    switch (style) {
    case 0: return e_dark_red_palette;
    case 1: default: return e_red_palette;
    case 2: return e_red_palette;
    case 3: return e_red_palette;
    case 4: return e_green_palette;
    case 5: return e_red_palette;
    case 6: return e_green_palette;
    case 7: return e_green_palette;
    }
  case VGA:
    switch (style) {
    case 0: return v_salmon_palette;
    case 1: default: return v_light_blue_palette;
    case 2: return v_red_palette;
    case 3: return v_orange_palette;
    case 4: return v_green_palette;
    case 5: return v_dark_blue_palette;
    case 6: return v_purple_palette;
    case 7: return v_yellow_palette;
    }
  }
  return NULL;
}

ALLEGRO_BITMAP *
apply_guard_palette (ALLEGRO_BITMAP *bitmap, enum anim_type type,
                     int style, enum vm vm)
{
  palette pal = NULL;

  if (type == SHADOW) {
    pal = get_kid_palette (vm);
    bitmap = apply_palette (bitmap, pal);
  }

  if (is_guard_by_type (type)) {
    pal = get_guard_palette (style, vm);
    bitmap = apply_palette (bitmap, pal);
  } else return bitmap;

  if (hgc) bitmap = apply_palette (bitmap, hgc_palette);

  return bitmap;
}

void
draw_guard_frame (ALLEGRO_BITMAP *bitmap, struct anim *g, enum vm vm)
{
  struct coord c;

  if (! is_guard (g)) return;
  if (g->invisible) return;

  struct frame f = g->f;
  struct frame_offset xf = g->xf;

  f.b = apply_guard_palette (f.b, g->type, g->style, vm);
  xf.b = apply_guard_palette (xf.b, g->type, g->style, vm);

  draw_xframe (bitmap, &f, &xf);
  draw_frame (bitmap, &f);

  if (g->splash && g->type != SKELETON) {
    ALLEGRO_BITMAP *splash = (g->type == SHADOW) ? v_kid_splash : guard_splash;
    palette pal = get_guard_palette (g->style, vm);
    splash = apply_palette (splash, pal);
    if (hgc) splash = apply_palette (splash, hgc_palette);
    draw_bitmapc (splash, bitmap, splash_coord (&g->f, &c), g->f.flip);
  }
}

void
draw_start_guards (ALLEGRO_BITMAP *bitmap, enum vm vm)
{
  int i;
  for (i = 0; i < GUARDS; i++) {
    struct guard *g = &global_level.guard[i];
    if (g->type == NO_ANIM) continue;
    struct frame f;
    f.c.room = g->p.room;
    f.b = get_guard_normal_bitmap (g->type);
    f.b = apply_guard_palette (f.b, g->type, g->style, vm);
    f.b = apply_palette (f.b, start_anim_palette);
    f.flip = (g->dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;
    place_frame (&f, &f, f.b, &g->p,
                 g->dir == LEFT ? +16 : +22, +14);
    place_on_the_ground (&f, &f.c);
    draw_frame (bitmap, &f);
  }
}

bool
is_guard (struct anim *a)
{
  return is_guard_by_type (a->type);
}

bool
is_guard_by_type (enum anim_type t)
{
  return t == SHADOW
    || t == GUARD
    || t == FAT_GUARD
    || t == VIZIER
    || t == SKELETON;
}

void
draw_guard_lives (ALLEGRO_BITMAP *bitmap, struct anim *g, enum vm vm)
{
  if (g->dont_draw_lives) return;
  if (g->current_lives <= 0) return;

  int current_lives = (g->current_lives > 10)
    ? 10 : g->current_lives;

  int i;
  struct coord c;
  struct rect r;
  new_rect (&r, room_view, ORIGINAL_WIDTH - 7 * current_lives,
            ORIGINAL_HEIGHT - 8, 7 * current_lives,
            ORIGINAL_HEIGHT - 1);

  ALLEGRO_COLOR bg_color;

  switch (vm) {
  case CGA: bg_color = C_LIVES_RECT_COLOR; break;
  case EGA: bg_color = E_LIVES_RECT_COLOR; break;
  case VGA: bg_color = V_LIVES_RECT_COLOR; break;
  }

  draw_filled_rect (bitmap, &r, bg_color);

  ALLEGRO_BITMAP *life = guard_life;
  palette pal = NULL;

  if ((g->type == SHADOW && g->style == 0)
      || g->type == KID) {
    pal = get_shadow_life_palette (vm);
    life = apply_palette (life, pal);
  } else if (g->type == SKELETON && g->style == 0)
    life = apply_palette (life, skeleton_life_palette);
  else if (is_guard (g)) {
    pal = get_guard_palette (g->style, vm);
    life = apply_palette (life, pal);
  }

  if (hgc) life = apply_palette (life, hgc_palette);

  for (i = 0; i < current_lives; i++)
    draw_bitmapc (life, bitmap, guard_life_coord (i, &c), 0);
}

static struct coord *
guard_life_coord (int i, struct coord *c)
{
  return
    new_coord (c, NULL, room_view,
               ORIGINAL_WIDTH - 7 * (i + 1) + 1,
               194);
}

static ALLEGRO_COLOR
c_shadow_life_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;

  if (color_eq (c, GUARD_LIFE_COLOR_01)) return C_KID_SHADOW_CLOTHES_COLOR_01;
  if (color_eq (c, GUARD_LIFE_COLOR_02)) return C_KID_SHADOW_CLOTHES_COLOR_02;

  return c;
}

static ALLEGRO_COLOR
e_shadow_life_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;

  if (color_eq (c, GUARD_LIFE_COLOR_01)) return E_KID_SHADOW_CLOTHES_COLOR_01;
  if (color_eq (c, GUARD_LIFE_COLOR_02)) return E_KID_SHADOW_CLOTHES_COLOR_02;

  return c;
}

static ALLEGRO_COLOR
v_shadow_life_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;

  if (color_eq (c, GUARD_LIFE_COLOR_01)) return V_KID_SHADOW_CLOTHES_COLOR_01;
  if (color_eq (c, GUARD_LIFE_COLOR_02)) return V_KID_SHADOW_CLOTHES_COLOR_02;

  return c;
}

static palette
get_shadow_life_palette (enum vm vm)
{
  switch (vm) {
  case CGA: return c_shadow_life_palette;
  case EGA: return e_shadow_life_palette;
  case VGA: return v_shadow_life_palette;
  }
  return NULL;
}

static ALLEGRO_COLOR
skeleton_life_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;
  return WHITE;
}
