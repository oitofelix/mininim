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

#include <error.h>
#include <stdio.h>

#include "kernel/video.h"
#include "kernel/keyboard.h"

#include "anim.h"
#include "physics.h"
#include "room.h"
#include "fire.h"
#include "potion.h"
#include "sword.h"
#include "kid.h"
#include "loose-floor.h"
#include "opener-floor.h"
#include "closer-floor.h"
#include "spikes-floor.h"
#include "door.h"
#include "consistency.h"
#include "level.h"

/* functions */
static void load_level (void);
static void unload_level (void);
static void level_anim (void);

/* current level */
struct level *level;

static bool no_room_drawing = false;

int room_view = 1;

void
play_level (struct level *_level)
{
  int i = 0;

  cutscene = false;

  level = _level;
  for (i = 0; i < 2; i++) fix_level ();
  load_level ();

  register_cons ();
  play_anim (level_anim, 12);

  unload_level ();
}

void
register_cons (void)
{
  struct pos p;
  for (p.room = 0; p.room < ROOMS; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++)
        switch (con (p)->fg) {
        case LOOSE_FLOOR: register_loose_floor (p); break;
        case OPENER_FLOOR: register_opener_floor (p); break;
        case CLOSER_FLOOR: register_closer_floor (p); break;
        case SPIKES_FLOOR: register_spikes_floor (p); break;
        case DOOR: register_door (p); break;
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
level_anim (void)
{
  static int i = 0;

  char bottom_message[ORIGINAL_WIDTH / 8 + 1];

  int prev_room = room_view;
  static bool show_coordinates = false;

  if (was_key_pressed (ALLEGRO_KEY_H, true))
    room_view = level->link[room_view].l;
  if (was_key_pressed (ALLEGRO_KEY_J, true))
    room_view = level->link[room_view].r;
  if (was_key_pressed (ALLEGRO_KEY_U, true))
    room_view = level->link[room_view].a;
  if (was_key_pressed (ALLEGRO_KEY_N, true))
    room_view = level->link[room_view].b;

  if (was_key_pressed (ALLEGRO_KEY_B, true))
    no_room_drawing = ! no_room_drawing;

  if (was_key_pressed (ALLEGRO_KEY_C, true))
    show_coordinates = ! show_coordinates;

  if (room_view == 0) room_view = prev_room;

  /* computation */
  kids = survey (kid, pos);
  kidsf = survey (kid, posf);
  prev_room = kid.c.room;
  kid.action ();
  if (prev_room != kid.c.room)  {
    room_view = kid.c.room;
    make_links_locally_consistent (prev_room, room_view);
  }
  compute_loose_floors ();
  compute_opener_floors ();
  compute_closer_floors ();
  compute_spikes_floors ();
  compute_doors ();

  /* drawing */
  struct pos p;
  p.room = room_view;

  clear_bitmap (screen, BLACK);

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      draw_fire (screen, p, i);
    }

  if (! no_room_drawing) draw_room (room_view);

  if (! no_room_drawing)
    for (p.floor = FLOORS; p.floor >= -1; p.floor--)
      for (p.place = -1; p.place < PLACES; p.place++) {
        draw_loose_floor (screen, p);
        draw_opener_floor (screen, p);
        draw_closer_floor (screen, p);
        draw_spikes_floor (screen, p);
        draw_door (screen, p);
      }

  draw_anim (screen, kid);
  draw_xanim (screen, kid);
  draw_room_anim_fg (kid);
  kid.xframe = NULL;

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      draw_potion (screen, p, i);
      draw_sword (screen, p, i);
    }

  unpress_opener_floors ();
  unpress_closer_floors ();

  if (show_coordinates) {
    int s = room_view;
    int l = roomd (room_view, LEFT);
    int r = roomd (room_view, RIGHT);
    int a = roomd (room_view, ABOVE);
    int b = roomd (room_view, BELOW);
    int al = roomd (a, LEFT);
    int ar = roomd (a, RIGHT);
    int bl = roomd (b, LEFT);
    int br = roomd (b, RIGHT);

    al_draw_filled_rectangle (0, ORIGINAL_HEIGHT - 8,
                              ORIGINAL_WIDTH, ORIGINAL_HEIGHT,
                              al_map_rgba (0, 0, 0, 192));
    sprintf (bottom_message, "S%i L%i R%i A%i B%i AL%i AR%i BL%i BR%i",
             s, l, r, a, b, al, ar, bl, br);
    draw_text (screen, bottom_message,
               ORIGINAL_WIDTH / 2.0, ORIGINAL_HEIGHT - 7,
               ALLEGRO_ALIGN_CENTRE);
  }

  i++;
}
