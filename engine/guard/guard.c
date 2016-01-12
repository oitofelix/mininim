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

#include <stdio.h>
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
#include "engine/sword.h"
#include "engine/fight.h"
#include "guard.h"

/* bitmaps */
ALLEGRO_BITMAP *guard_life, *guard_splash;

/* sounds */
ALLEGRO_SAMPLE *guard_hit_sample;

void
load_guard (void)
{
  load_guard_normal ();
  load_guard_vigilant ();
  load_guard_hit ();
  load_guard_die ();
  load_guard_fall ();
  load_guard_walkf ();

  /* bitmaps */
  guard_life = load_bitmap (GUARD_LIFE);
  guard_splash = load_bitmap (GUARD_SPLASH);

  /* sounds */
  guard_hit_sample = load_sample (GUARD_HIT_SAMPLE);
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

  /* bitmaps */
  al_destroy_bitmap (guard_life);
  al_destroy_bitmap (guard_splash);

  /* sounds */
  al_destroy_sample (guard_hit_sample);
}

struct anim *
create_guard (struct anim *g0, struct anim *g1, struct pos *p, enum dir dir)
{
  if (! g0) {
    g1->f.b = guard_normal_00;
    g1->fo.b = guard_normal_00;
    g1->action = guard_normal;
    g1->total_lives = 3;
    g1->current_lives = 3;
    g1->enemy_id = -1;
    g1->fight = true;
    g1->skill.defense_prob = 70;
    g1->skill.counter_attack_prob = 70;
    g1->skill.counter_defense_prob = 70;

    place_frame (&g1->f, &g1->f, guard_normal_00, p,
                 g1->f.dir == LEFT ? +PLACE_WIDTH + 2 : +3, +15);
    update_depressible_floor (g1, -4, -10);
  }

  return g1;
}

static ALLEGRO_COLOR
v_light_blue_palette (ALLEGRO_COLOR c)
{
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
e_red_palette (ALLEGRO_COLOR c)
{
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
c_red_palette (ALLEGRO_COLOR c)
{
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

static palette
get_palette (int style, enum vm vm)
{
  switch (vm) {
  case CGA:
    switch (style) {
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

void
draw_guard_frame (ALLEGRO_BITMAP *bitmap, struct anim *g, enum vm vm)
{
  struct coord c;

  if (g->invisible) return;

  struct frame f = g->f;
  struct frame_offset xf = g->xf;

  palette pal = get_palette (g->style, vm);
  f.b = apply_palette (f.b, pal);
  xf.b = apply_palette (xf.b, pal);

  if (hgc) {
    f.b = apply_palette (f.b, hgc_palette);
    xf.b = apply_palette (xf.b, hgc_palette);
  }

  draw_xframe (bitmap, &f, &xf);
  draw_frame (bitmap, &f);

  if (g->splash) {
    ALLEGRO_BITMAP *splash = apply_palette (guard_splash, pal);
    if (hgc) splash = apply_palette (splash, hgc_palette);
    draw_bitmapc (splash, bitmap, splash_coord (&g->f, &c), g->f.flip);
  }
}
