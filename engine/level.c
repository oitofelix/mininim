/*
  level.c -- level module;

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

#define _GNU_SOURCE

#include <error.h>
#include <stdio.h>

#include "kernel/video.h"
#include "kernel/keyboard.h"
#include "kernel/array.h"
#include "anim.h"
#include "physics.h"
#include "room.h"
#include "fire.h"
#include "potion.h"
#include "sword.h"
#include "kid/kid.h"
#include "loose-floor.h"
#include "opener-floor.h"
#include "closer-floor.h"
#include "spikes-floor.h"
#include "door.h"
#include "level-door.h"
#include "chopper.h"
#include "consistency.h"
#include "level.h"

/* functions */
static void load_level (void);
static void unload_level (void);
static void draw_level (void);
static void sample_level (void);
static void compute_level (void);

struct level *level;
static bool no_room_drawing = false;
int room_view = 1;
int draw_cycle;

void
play_level (struct level *_level)
{
  cutscene = false;

  level = _level;
  load_level ();

  register_cons ();
  play_anim (draw_level, sample_level, compute_level, 12);

  unload_level ();
}

void
register_cons (void)
{
  struct pos p;
  for (p.room = 0; p.room < ROOMS; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++)
        switch (con (&p)->fg) {
        case LOOSE_FLOOR: register_loose_floor (&p); break;
        case OPENER_FLOOR: register_opener_floor (&p); break;
        case CLOSER_FLOOR: register_closer_floor (&p); break;
        case SPIKES_FLOOR: register_spikes_floor (&p); break;
        case DOOR: register_door (&p); break;
        case LEVEL_DOOR: register_level_door (&p); break;
        case CHOPPER: register_chopper (&p); break;
        default: break;
        }
}

static void
load_level (void)
{
  load_room ();
  load_fire ();
  load_potion ();
  load_sword ();
  load_kid ();

  create_kid ();
  create_kid ();

  current_kid = &kid[0];
  current_kid->current = true;
  kid[1].shadow = true;
}

static void
unload_level (void)
{
  unload_room ();
  unload_fire ();
  unload_potion ();
  unload_sword ();
  unload_kid ();
}

static void
compute_level (void)
{
  int i;
  struct anim *k;

  int prev_room = current_kid->f.c.room;

  compute_loose_floors ();

  for (i = 0; i < kid_nmemb; i++) {
    k = &kid[i];
    k->splash = false;
    k->action (k);
  }

  if (current_kid->f.c.room != prev_room
      && current_kid->f.c.room != 0)  {
    room_view = current_kid->f.c.room;
    make_links_locally_consistent (prev_room, room_view);
  }

  compute_opener_floors ();
  compute_closer_floors ();
  compute_spikes_floors ();
  compute_doors ();
  compute_level_doors ();
  compute_choppers ();
}

static void
sample_level (void)
{
  sample_kid ();
  sample_loose_floors ();
  sample_opener_floors ();
  sample_closer_floors ();
  sample_spikes_floors ();
  sample_doors ();
  sample_level_doors ();
  sample_choppers ();
}

static void
draw_level (void)
{
  enum em em = level->em;
  enum vm vm = level->vm;

  char *text = NULL;

  int prev_room = room_view;

  if (was_key_pressed (ALLEGRO_KEY_HOME, 0, 0, true))
    room_view = current_kid->f.c.room;
  if (was_key_pressed (ALLEGRO_KEY_H, 0, 0, true))
    room_view = level->link[room_view].l;
  if (was_key_pressed (ALLEGRO_KEY_J, 0, 0, true))
    room_view = level->link[room_view].r;
  if (was_key_pressed (ALLEGRO_KEY_U, 0, 0, true))
    room_view = level->link[room_view].a;
  if (was_key_pressed (ALLEGRO_KEY_N, 0, 0, true))
    room_view = level->link[room_view].b;

  if (was_key_pressed (ALLEGRO_KEY_B, 0, ALLEGRO_KEYMOD_SHIFT, true))
    no_room_drawing = ! no_room_drawing;

  if (was_key_pressed (ALLEGRO_KEY_A, 0, 0, true)) {
    current_kid->current = false;
    current_kid = &kid[(current_kid - kid + 1) % kid_nmemb];
    current_kid->current = true;
    room_view = current_kid->f.c.room;
  }

  if (was_key_pressed (ALLEGRO_KEY_R, 0, 0, true)
      && is_kid_dead (&current_kid->f))
    kid_resurrect (current_kid);

  if (was_key_pressed (ALLEGRO_KEY_I, 0, 0, true)) {
    current_kid->immortal = ! current_kid->immortal;
    xasprintf (&text, "%s MODE", current_kid->immortal
               ? "IMMORTAL" : "MORTAL");
    draw_bottom_text (NULL, text);
    al_free (text);
  }

  if (room_view == 0) room_view = prev_room;

  /* drawing */
  struct pos p;
  p.room = room_view;

  clear_bitmap (screen, BLACK);

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      draw_fire (screen, &p, draw_cycle, vm);
    }

  if (! no_room_drawing) draw_room (screen, room_view, em, vm);

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      draw_falling_loose_floor (screen, &p, em, vm);
    }

  draw_kids (screen, em, vm);

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      draw_potion (screen, &p, draw_cycle, em, vm);
      draw_sword (screen, &p, draw_cycle, vm);
    }

  unpress_opener_floors ();
  unpress_closer_floors ();

  draw_kid_lives (screen, current_kid, draw_cycle);

  if (was_key_pressed (ALLEGRO_KEY_C, 0, 0, true)) {
    int s = room_view;
    int l = roomd (room_view, LEFT);
    int r = roomd (room_view, RIGHT);
    int a = roomd (room_view, ABOVE);
    int b = roomd (room_view, BELOW);
    int al = roomd (a, LEFT);
    int ar = roomd (a, RIGHT);
    int bl = roomd (b, LEFT);
    int br = roomd (b, RIGHT);

    xasprintf (&text, "S%i L%i R%i A%i B%i AL%i AR%i BL%i BR%i",
               s, l, r, a, b, al, ar, bl, br);
    draw_bottom_text (NULL, text);
    al_free (text);
  }

  if (was_key_pressed (ALLEGRO_KEY_V, 0, ALLEGRO_KEYMOD_CTRL, true)) {
    xasprintf (&text, "MININIM 0.9");
    draw_bottom_text (NULL, text);
    al_free (text);
  }

  if (was_key_pressed (ALLEGRO_KEY_S, 0, ALLEGRO_KEYMOD_CTRL, true)) {
    audio_enabled = ! audio_enabled;
    enable_audio (audio_enabled);
    xasprintf (&text, "SOUND %s", audio_enabled ? "ON" : "OFF");
    draw_bottom_text (NULL, text);
    al_free (text);
  }

  if (was_key_pressed (ALLEGRO_KEY_I, 0, ALLEGRO_KEYMOD_SHIFT, true)) {
    char *flip = "NONE";
    switch (screen_flags) {
    case 0:
      screen_flags = ALLEGRO_FLIP_VERTICAL;
      flip = "VERTICAL"; break;
    case ALLEGRO_FLIP_VERTICAL:
      screen_flags = ALLEGRO_FLIP_HORIZONTAL;
      flip = "HORIZONTAL"; break;
    case ALLEGRO_FLIP_HORIZONTAL:
      screen_flags = ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL;
      flip = "VERTICAL + HORIZONTAL"; break;
    case ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL:
      screen_flags = 0;
      break;
    }
    xasprintf (&text, "DISPLAY FLIP: %s", flip);
    draw_bottom_text (NULL, text);
    al_free (text);
  }

  if (was_key_pressed (ALLEGRO_KEY_F11, 0, 0, true)) {
    char *em_str = NULL;

    switch (level->em) {
    case DUNGEON: level->em = PALACE; em_str = "PALACE"; break;
    case PALACE: level->em = DUNGEON; em_str = "DUNGEON"; break;
    }

    xasprintf (&text, "ENVIRONMENT MODE: %s", em_str);
    draw_bottom_text (NULL, text);
    al_free (text);
  }

  if (was_key_pressed (ALLEGRO_KEY_F12, 0, 0, true)) {
    char *vm_str = NULL;

    switch (level->vm) {
    case CGA: break;
    case EGA: level->vm = VGA; vm_str = "VGA"; break;
    case VGA: level->vm = EGA; vm_str = "EGA"; break;
    }

    xasprintf (&text, "VIDEO MODE: %s", vm_str);
    draw_bottom_text (NULL, text);
    al_free (text);
  }

  draw_cycle++;
}
