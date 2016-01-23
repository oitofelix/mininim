/*
  kid.h -- kid module;

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

#ifndef MININIM_KID_H
#define MININIM_KID_H

#include "video.h"
#include "audio.h"

/* kid actions */
#include "kid-normal.h"
#include "kid-walk.h"
#include "kid-start-run.h"
#include "kid-run.h"
#include "kid-stop-run.h"
#include "kid-turn-run.h"
#include "kid-couch.h"
#include "kid-turn.h"
#include "kid-stabilize.h"
#include "kid-jump.h"
#include "kid-vjump.h"
#include "kid-run-jump.h"
#include "kid-misstep.h"
#include "kid-hang.h"
#include "kid-hang-wall.h"
#include "kid-hang-free.h"
#include "kid-climb.h"
#include "kid-unclimb.h"
#include "kid-fall.h"
#include "kid-drink.h"
#include "kid-raise-sword.h"
#include "kid-keep-sword.h"
#include "kid-take-sword.h"
#include "kid-sword-normal.h"
#include "kid-sword-walkf.h"
#include "kid-sword-walkb.h"
#include "kid-sword-defense.h"
#include "kid-sword-attack.h"
#include "kid-sword-hit.h"
#include "kid-die.h"
#include "kid-stairs.h"

/* bitmaps */
#define V_KID_FULL_LIFE "data/life/v-kid-full.png"
#define V_KID_EMPTY_LIFE "data/life/v-kid-empty.png"
#define V_KID_SPLASH "data/splash/v-kid.png"

/* functions */
void load_kid (void);
void unload_kid (void);
struct anim *create_kid (struct anim *k0, struct anim *k1,
                         struct pos *p, enum dir dir);
void destroy_kid (struct anim *k);
void draw_kid_frame (ALLEGRO_BITMAP *bitmap, struct anim *k,
                     enum vm);
void draw_kid_lives (ALLEGRO_BITMAP *bitmap, struct anim *k,
                     enum vm vm);
void draw_kid_if_at_pos (ALLEGRO_BITMAP *bitmap, struct anim *k, struct pos *p,
                         enum vm vm);
palette get_kid_palette (enum vm vm);
palette get_shadow_palette (enum vm vm);
ALLEGRO_COLOR colorful_shadow_palette (ALLEGRO_COLOR c);
ALLEGRO_COLOR v_phantom_shadow_palette (ALLEGRO_COLOR c);
ALLEGRO_COLOR e_phantom_shadow_palette (ALLEGRO_COLOR c);
ALLEGRO_COLOR c_phantom_shadow_palette (ALLEGRO_COLOR c);
void increase_kid_current_lives (struct anim *k);
void increase_kid_total_lives (struct anim *k);
void float_kid (struct anim *k);
ALLEGRO_COLOR get_flicker_blood_color (void);
ALLEGRO_COLOR get_flicker_raise_sword_color (void);
ALLEGRO_COLOR get_flicker_float_color (void);
void kid_debug (void);
bool kid_door_split_collision (struct anim *k);

/* variables */
extern int current_kid_id;

extern ALLEGRO_BITMAP *v_kid_full_life, *v_kid_empty_life, *v_kid_splash;

#endif	/* MININIM_KID_H */
