/*
  kid-drink.c -- kid drink module;

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

static bool flow (struct actor *k);
static bool physics_in (struct actor *k);
static void physics_out (struct actor *k);

void
kid_drink (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_drink;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  struct pos p; new_pos (&p, &global_level, -1, -1, -1);

  if (k->oaction != kid_drink)
    k->i = -1, k->wait = 4, k->reverse = false;

  if (k->i < 14 && ! k->reverse) k->i++;
  else if (k->wait > 0) k->wait--;
  else if (k->i == 14 && k->wait == 0) k->reverse = true, k->i = 10;
  else if (k->i == 10 && k->reverse) k->i = 7;
  else {
    kid_normal (k);
    invalid_pos (&k->item_pos);
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
      play_audio (&harm_audio, NULL, k->id);
      kid_haptic (k, KID_HAPTIC_HARM);
      if (k->current_lives == 0) k->death_reason = POTION_DEATH;
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
      play_audio (&harm_audio, NULL, k->id);
      if (k->id == current_kid_id) {
        mr.flicker = 2;
        mr.color = get_flicker_blood_color ();
      }
      break;
    case FLOAT_POTION: float_kid (k); break;
    case FLIP_POTION:
      if (potion_flags) potion_flags = 0;
      else {
        switch (prandom_pos (&k->item_pos, 2)) {
        case 0: potion_flags ^= ALLEGRO_FLIP_VERTICAL; break;
        case 1: potion_flags ^= ALLEGRO_FLIP_HORIZONTAL; break;
        case 2: potion_flags ^= ALLEGRO_FLIP_VERTICAL
            | ALLEGRO_FLIP_HORIZONTAL; break;
        }
      }
      kid_haptic (k, KID_HAPTIC_DRINK);
      break;
    case ACTIVATION_POTION: p.room = 8, p.floor = p.place = 0;
      activate_tile (&p);
      kid_haptic (k, KID_HAPTIC_DRINK);
      break;
    default: break;
    }

  select_actor_frame (k, "KID", "DRINK", k->i);

  if (k->i == 14 && k->wait < 4) k->fo.dx = 0;
  if (k->i == 10 && k->reverse) k->fo.dx = -2, k->fo.dy = +1;
  if (k->i == 7 && k->reverse) k->fo.dx = +1;

  return true;
}

static bool
physics_in (struct actor *k)
{
  return true;
}

static void
physics_out (struct actor *k)
{
  /* depressible floors */
  keep_depressible_floor (k);

  /* sound */
  if (k->i == 7 && ! k->reverse)
    play_audio (&drink_audio, NULL, k->id);

  /* consume bottle */
  if (k->i == 0) {
    k->item = ext (&k->item_pos);
    register_tile_undo (&undo, &k->item_pos,
                       MIGNORE, MIGNORE, NO_ITEM, MIGNORE,
                       NULL, false, "CONSUME POTION");
  }
}
