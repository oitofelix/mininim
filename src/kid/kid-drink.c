/*
  kid-drink.c -- kid drink module;

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

struct frameset kid_drink_frameset[KID_DRINK_FRAMESET_NMEMB];

static void init_kid_drink_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_drink_00, *kid_drink_01, *kid_drink_02,
  *kid_drink_03, *kid_drink_04, *kid_drink_05, *kid_drink_06,
  *kid_drink_07, *kid_drink_08, *kid_drink_09, *kid_drink_10,
  *kid_drink_11, *kid_drink_12, *kid_drink_13, *kid_drink_14;

static void
init_kid_drink_frameset (void)
{
  struct frameset frameset[KID_DRINK_FRAMESET_NMEMB] =
    {{kid_drink_00,-7,0},{kid_drink_01,+1,+1},{kid_drink_02,+1,-1},
     {kid_drink_03,+0,0},{kid_drink_04,+2,0},{kid_drink_05,-1,0},
     {kid_drink_06,+1,0},{kid_drink_07,+6,0},{kid_drink_08,-1,0},
     {kid_drink_09,+2,-1},{kid_drink_10,-2,+1},{kid_drink_11,+0,-1},
     {kid_drink_12,-1,0},{kid_drink_13,+1,0},{kid_drink_14,+1,0}};

  memcpy (&kid_drink_frameset, &frameset,
          KID_DRINK_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_drink (void)
{
  /* bitmaps */
  kid_drink_00 = load_bitmap (KID_DRINK_00);
  kid_drink_01 = load_bitmap (KID_DRINK_01);
  kid_drink_02 = load_bitmap (KID_DRINK_02);
  kid_drink_03 = load_bitmap (KID_DRINK_03);
  kid_drink_04 = load_bitmap (KID_DRINK_04);
  kid_drink_05 = load_bitmap (KID_DRINK_05);
  kid_drink_06 = load_bitmap (KID_DRINK_06);
  kid_drink_07 = load_bitmap (KID_DRINK_07);
  kid_drink_08 = load_bitmap (KID_DRINK_08);
  kid_drink_09 = load_bitmap (KID_DRINK_09);
  kid_drink_10 = load_bitmap (KID_DRINK_10);
  kid_drink_11 = load_bitmap (KID_DRINK_11);
  kid_drink_12 = load_bitmap (KID_DRINK_12);
  kid_drink_13 = load_bitmap (KID_DRINK_13);
  kid_drink_14 = load_bitmap (KID_DRINK_14);

  /* frameset */
  init_kid_drink_frameset ();
}

void
unload_kid_drink (void)
{
  destroy_bitmap (kid_drink_00);
  destroy_bitmap (kid_drink_01);
  destroy_bitmap (kid_drink_02);
  destroy_bitmap (kid_drink_03);
  destroy_bitmap (kid_drink_04);
  destroy_bitmap (kid_drink_05);
  destroy_bitmap (kid_drink_06);
  destroy_bitmap (kid_drink_07);
  destroy_bitmap (kid_drink_08);
  destroy_bitmap (kid_drink_09);
  destroy_bitmap (kid_drink_10);
  destroy_bitmap (kid_drink_11);
  destroy_bitmap (kid_drink_12);
  destroy_bitmap (kid_drink_13);
  destroy_bitmap (kid_drink_14);
}

void
kid_drink (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_drink;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  struct pos p;

  if (k->oaction != kid_drink)
    k->i = -1, k->wait = 4, k->reverse = false;

  if (k->i < 14 && ! k->reverse) k->i++;
  else if (k->wait > 0) k->wait--;
  else if (k->i == 14 && k->wait == 0) k->reverse = true, k->i = 10;
  else if (k->i == 10 && k->reverse) k->i = 7;
  else {
    kid_normal (k);
    k->item_pos.room = -1;
    return false;
  }

  if (k->i == 14 && k->wait == 1)
    switch (k->item) {
    case EMPTY_POTION: break;
    case SMALL_LIFE_POTION: increase_kid_current_lives (k); break;
    case BIG_LIFE_POTION: increase_kid_total_lives (k); break;
    case SMALL_POISON_POTION:
      if (k->immortal
          || k->poison_immune) break;
      k->current_lives--;
      k->splash = true;
      if (k->current_lives == 0) k->death_reason = POTION_DEATH;
      play_sample (harm_sample, k->f.c.room);
      if (k->id == current_kid_id) {
        mr.flicker = 2;
        mr.color = get_flicker_blood_color ();
      }
      break;
    case BIG_POISON_POTION:
      if (k->immortal
          || k->poison_immune) break;
      k->current_lives = 0;
      k->splash = true;
      k->death_reason = POTION_DEATH;
      play_sample (harm_sample, k->f.c.room);
      if (k->id == current_kid_id) {
        mr.flicker = 2;
        mr.color = get_flicker_blood_color ();
      }
      break;
    case FLOAT_POTION: float_kid (k); break;
    case FLIP_POTION:
      if (screen_flags) screen_flags = 0;
      else {
        switch (prandom_pos (&k->item_pos, 2)) {
        case 0: screen_flags ^= ALLEGRO_FLIP_VERTICAL; break;
        case 1: screen_flags ^= ALLEGRO_FLIP_HORIZONTAL; break;
        case 2: screen_flags ^= ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL; break;
        }
      }
      break;
    case ACTIVATION_POTION: p.room = 8, p.floor = p.place = 0;
      activate_con (&p); break;
    default: break;
    }

  select_frame (k, kid_drink_frameset, k->i);

  if (k->i == 14 && k->wait < 4) k->fo.dx = 0;
  if (k->i == 10 && k->reverse) k->fo.dx = -2, k->fo.dy = +1;
  if (k->i == 7 && k->reverse) k->fo.dx = +1;

  return true;
}

static bool
physics_in (struct anim *k)
{
  return true;
}

static void
physics_out (struct anim *k)
{
  /* depressible floors */
  keep_depressible_floor (k);

  /* sound */
  if (k->i == 7 && ! k->reverse) play_sample (drink_sample, k->f.c.room);

  /* consume bottle */
  if (k->i == 0) {
    k->item = con (&k->item_pos)->ext.item;
    register_con_undo (&undo, &k->item_pos,
                       MIGNORE, MIGNORE, NO_ITEM,
                       false, false, false, false,
                       CHPOS_NONE,
                       "CONSUME POTION");
  }
}
