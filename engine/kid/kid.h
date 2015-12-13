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

#ifndef FREEPOP_KID_H
#define FREEPOP_KID_H

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

/* bitmaps */
#define KID_FULL_LIFE "dat/kid/objects/full live.png"
#define KID_EMPTY_LIFE "dat/kid/objects/empty live.png"

/* sounds */
#define STEP_SAMPLE "dat/digisnd1/running 2.ogg"
#define HIT_GROUND_SAMPLE "dat/digisnd1/running 1.ogg"
#define HIT_WALL_SAMPLE "dat/digisnd1/touching a wall.ogg"
#define HANG_ON_FALL_SAMPLE "dat/digisnd1/normal falling.ogg"
#define DRINK_SAMPLE "dat/digisnd3/drinking.ogg"
#define GLORY_SAMPLE "dat/midisnd1/guard death 1.ogg"
#define TAKE_SWORD_SAMPLE "dat/digisnd1/taking out the sword.ogg"
#define SWORD_ATTACK_SAMPLE "dat/digisnd1/sword moving.ogg"
#define ACTION_NOT_ALLOWED_SAMPLE "dat/digisnd1/drinking potion.ogg"

/* functions */
void load_kid (void);
void unload_kid (void);
void sample_kid (void);
void draw_kid_lives (ALLEGRO_BITMAP *bitmap, int j);
void draw_kid (ALLEGRO_BITMAP *bitmap, struct pos *p);

/* variables */
extern struct anim kid; /* kid animation object */

extern int kid_total_lives, kid_current_lives;

extern ALLEGRO_BITMAP *kid_full_life, *kid_empty_life;

extern bool sample_step, sample_hit_ground, sample_hit_wall,
  sample_hang_on_fall, sample_drink, sample_glory,
  sample_take_sword, sample_sword_attack, sample_action_not_allowed;

#endif	/* FREEPOP_KID_H */
