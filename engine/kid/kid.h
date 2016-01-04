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

#include "kernel/audio.h"

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
#include "kid-die.h"
#include "kid-stairs.h"

/* bitmaps */
#define V_KID_FULL_LIFE "data/life/v-kid-full.png"
#define V_KID_EMPTY_LIFE "data/life/v-kid-empty.png"
#define V_KID_SPLASH "data/splash/v-kid.png"

/* sounds */
#define STEP_SAMPLE "dat/digisnd1/running 2.ogg"
#define HIT_GROUND_SAMPLE "dat/digisnd1/running 1.ogg"
#define HIT_GROUND_HARM_SAMPLE "dat/digisnd1/loosing a life falling.ogg"
#define HIT_GROUND_FATAL_SAMPLE "dat/digisnd1/prince crashing into the floor.ogg"
#define HIT_WALL_SAMPLE "dat/digisnd1/touching a wall.ogg"
#define HANG_ON_FALL_SAMPLE "dat/digisnd1/normal falling.ogg"
#define DRINK_SAMPLE "dat/digisnd3/drinking.ogg"
#define GLORY_SAMPLE "dat/midisnd1/guard death 1.ogg"
#define TAKE_SWORD_SAMPLE "dat/digisnd1/taking out the sword.ogg"
#define SWORD_ATTACK_SAMPLE "dat/digisnd1/sword moving.ogg"
#define HARM_SAMPLE "dat/digisnd1/loosing a life from the enemy.ogg"
#define ACTION_NOT_ALLOWED_SAMPLE "dat/digisnd1/drinking potion.ogg"
#define SMALL_LIFE_POTION_SAMPLE "dat/midisnd1/potion 2.ogg"
#define BIG_LIFE_POTION_SAMPLE "dat/midisnd1/potion 1.ogg"
#define SCREAM_SAMPLE "dat/digisnd3/falling.ogg"
#define SPIKED_SAMPLE "dat/digisnd2/something spiked.ogg"
#define CHOPPED_SAMPLE "dat/digisnd2/something chopped.ogg"
#define FLOATING_SAMPLE "dat/midisnd1/floating.ogg"
#define DEATH_SAMPLE "dat/midisnd1/death.ogg"
#define PRESS_KEY_SAMPLE "dat/digisnd1/press-key.ogg"
#define MIRROR_SAMPLE "dat/digisnd2/mirror.ogg"
#define SUSPENSE_SAMPLE "dat/midisnd1/suspense.ogg"
#define SUCCESS_SAMPLE "dat/midisnd1/success.ogg"

/* functions */
void load_kid (void);
void unload_kid (void);
void draw_kid_lives (ALLEGRO_BITMAP *bitmap, struct anim *kid,
                     enum vm vm);
void draw_kid_if_at_pos (ALLEGRO_BITMAP *bitmap, struct anim *kid, struct pos *p,
                         enum vm vm);
int create_kid (struct anim *k);
void destroy_kid (struct anim *k);
void destroy_kids (void);
struct anim *get_kid_by_id (int id);
void clear_kids_keyboard_state (void);
void draw_kids (ALLEGRO_BITMAP *bitmap, enum em em, enum vm vm);
void draw_kid_frame (ALLEGRO_BITMAP *bitmap, struct anim *k,
                     enum vm);
ALLEGRO_COLOR colorful_shadow_palette (ALLEGRO_COLOR c);
ALLEGRO_COLOR v_phantom_shadow_palette (ALLEGRO_COLOR c);
ALLEGRO_COLOR e_phantom_shadow_palette (ALLEGRO_COLOR c);
ALLEGRO_COLOR c_phantom_shadow_palette (ALLEGRO_COLOR c);
ALLEGRO_COLOR v_palette (ALLEGRO_COLOR c);
ALLEGRO_COLOR e_palette (ALLEGRO_COLOR c);
ALLEGRO_COLOR c_palette (ALLEGRO_COLOR c);
void increase_kid_current_lives (struct anim *k);
void increase_kid_total_lives (struct anim *k);
void float_kid (struct anim *k);
ALLEGRO_COLOR get_flicker_blood_color (void);
ALLEGRO_COLOR get_flicker_raise_sword_color (void);
ALLEGRO_COLOR get_flicker_float_color (void);
void kid_debug (void);
bool kid_door_split_collision (struct anim *k);

/* variables */
extern struct anim *kida;
extern size_t kida_nmemb;
extern struct anim *current_kid;

extern ALLEGRO_SAMPLE *step_sample, *hit_ground_sample, *hit_ground_harm_sample,
  *hit_ground_fatal_sample, *hit_wall_sample, *hang_on_fall_sample,
  *drink_sample, *glory_sample, *take_sword_sample, *sword_attack_sample,
  *harm_sample, *action_not_allowed_sample, *small_life_potion_sample,
  *big_life_potion_sample, *scream_sample, *spiked_sample, *chopped_sample,
  *floating_sample, *death_sample, *press_key_sample, *mirror_sample,
  *suspense_sample, *success_sample;

#endif	/* MININIM_KID_H */
