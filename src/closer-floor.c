/*
  closer-floor.c -- closer floor module;

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

/* dungeon cga */
ALLEGRO_BITMAP *dc_unpressed_closer_floor_base, *dc_pressed_closer_floor_base,
  *dc_pressed_closer_floor_right;

/* palace cga */
ALLEGRO_BITMAP *pc_unpressed_closer_floor_base, *pc_pressed_closer_floor_base,
  *pc_pressed_closer_floor_right;

/* dungeon ega */
ALLEGRO_BITMAP *de_unpressed_closer_floor_base, *de_pressed_closer_floor_base,
  *de_pressed_closer_floor_right;

/* palace ega */
ALLEGRO_BITMAP *pe_unpressed_closer_floor_base, *pe_pressed_closer_floor_base,
  *pe_pressed_closer_floor_right;

/* dungeon vga */
ALLEGRO_BITMAP *dv_unpressed_closer_floor_base, *dv_pressed_closer_floor_base,
  *dv_pressed_closer_floor_right;

/* palace vga */
ALLEGRO_BITMAP *pv_unpressed_closer_floor_base, *pv_pressed_closer_floor_base,
  *pv_pressed_closer_floor_right;

struct closer_floor *closer_floor = NULL;
size_t closer_floor_nmemb = 0;

void
load_closer_floor (void)
{
  /* dungeon cga */
  dc_unpressed_closer_floor_base = load_bitmap (DC_UNPRESSED_CLOSER_FLOOR_BASE);
  dc_pressed_closer_floor_base = load_bitmap (DC_PRESSED_CLOSER_FLOOR_BASE);
  dc_pressed_closer_floor_right = load_bitmap (DC_PRESSED_CLOSER_FLOOR_RIGHT);

  /* palace cga */
  pc_unpressed_closer_floor_base = load_bitmap (PC_UNPRESSED_CLOSER_FLOOR_BASE);
  pc_pressed_closer_floor_base = load_bitmap (PC_PRESSED_CLOSER_FLOOR_BASE);
  pc_pressed_closer_floor_right = load_bitmap (PC_PRESSED_CLOSER_FLOOR_RIGHT);

  /* dungeon ega */
  de_unpressed_closer_floor_base = load_bitmap (DE_UNPRESSED_CLOSER_FLOOR_BASE);
  de_pressed_closer_floor_base = load_bitmap (DE_PRESSED_CLOSER_FLOOR_BASE);
  de_pressed_closer_floor_right = load_bitmap (DE_PRESSED_CLOSER_FLOOR_RIGHT);

  /* palace ega */
  pe_unpressed_closer_floor_base = load_bitmap (PE_UNPRESSED_CLOSER_FLOOR_BASE);
  pe_pressed_closer_floor_base = load_bitmap (PE_PRESSED_CLOSER_FLOOR_BASE);
  pe_pressed_closer_floor_right = load_bitmap (PE_PRESSED_CLOSER_FLOOR_RIGHT);

  /* dungeon vga */
  dv_unpressed_closer_floor_base = load_bitmap (DV_UNPRESSED_CLOSER_FLOOR_BASE);
  dv_pressed_closer_floor_base = load_bitmap (DV_PRESSED_CLOSER_FLOOR_BASE);
  dv_pressed_closer_floor_right = load_bitmap (DV_PRESSED_CLOSER_FLOOR_RIGHT);

  /* palace vga */
  pv_unpressed_closer_floor_base = load_bitmap (PV_UNPRESSED_CLOSER_FLOOR_BASE);
  pv_pressed_closer_floor_base = load_bitmap (PV_PRESSED_CLOSER_FLOOR_BASE);
  pv_pressed_closer_floor_right = load_bitmap (PV_PRESSED_CLOSER_FLOOR_RIGHT);
}

void
unload_closer_floor (void)
{
  /* dungeon cga */
  destroy_bitmap (dc_unpressed_closer_floor_base);
  destroy_bitmap (dc_pressed_closer_floor_base);
  destroy_bitmap (dc_pressed_closer_floor_right);

  /* palace cga */
  destroy_bitmap (pc_unpressed_closer_floor_base);
  destroy_bitmap (pc_pressed_closer_floor_base);
  destroy_bitmap (pc_pressed_closer_floor_right);

  /* dungeon ega */
  destroy_bitmap (de_unpressed_closer_floor_base);
  destroy_bitmap (de_pressed_closer_floor_base);
  destroy_bitmap (de_pressed_closer_floor_right);

  /* palace ega */
  destroy_bitmap (pe_unpressed_closer_floor_base);
  destroy_bitmap (pe_pressed_closer_floor_base);
  destroy_bitmap (pe_pressed_closer_floor_right);

  /* dungeon vga */
  destroy_bitmap (dv_unpressed_closer_floor_base);
  destroy_bitmap (dv_pressed_closer_floor_base);
  destroy_bitmap (dv_pressed_closer_floor_right);

  /* palace vga */
  destroy_bitmap (pv_unpressed_closer_floor_base);
  destroy_bitmap (pv_pressed_closer_floor_base);
  destroy_bitmap (pv_pressed_closer_floor_right);
}

void
register_closer_floor (struct pos *p)
{
  assert (con (p)->fg == CLOSER_FLOOR
          && closer_floor_at_pos (p) == NULL);

  struct closer_floor c;

  int event = con (p)->ext.event;

  c.p = *p;
  c.event = (event < 0) ? -event - 1 : event;
  c.pressed = (event < 0);
  c.noise = (event < 0);
  c.broken = (event < 0);
  c.unresponsive = false;

  closer_floor =
    add_to_array (&c, 1, closer_floor, &closer_floor_nmemb,
                  closer_floor_nmemb, sizeof (c));

  qsort (closer_floor, closer_floor_nmemb, sizeof (c),
         compare_closer_floors);
}

int
compare_closer_floors (const void *c0, const void *c1)
{
  return cpos (&((struct closer_floor *) c0)->p,
               &((struct closer_floor *) c1)->p);
}

struct closer_floor *
closer_floor_at_pos (struct pos *p)
{
  struct closer_floor c;
  c.p = *p;

  return bsearch (&c, closer_floor, closer_floor_nmemb, sizeof (c),
                  compare_closer_floors);
}

struct closer_floor *
closer_floor_by_event (struct pos *p, int event, int dir)
{
  struct closer_floor *c;
  if (p) c = closer_floor_at_pos (p);
  else c = &closer_floor[0];

  if (! c) {
    c = &closer_floor[0];
    p = NULL;
  }

  int i;

  if (dir < 0)
    for (i = c - closer_floor - (p ? 1 : 0); i >= 0; i--) {
      if (closer_floor[i].event == event) return &closer_floor[i];
    }
  else
    for (i = c - closer_floor + (p ? 1 : 0);
         i < closer_floor_nmemb; i++) {
      if (closer_floor[i].event == event) return &closer_floor[i];
    }

  return NULL;
}

void
remove_closer_floor (struct closer_floor *c)
{
  size_t i =  c - closer_floor;
  closer_floor =
    remove_from_array (closer_floor, &closer_floor_nmemb, i, 1, sizeof (*c));
}

void
press_closer_floor (struct pos *p)
{
  struct closer_floor *c = closer_floor_at_pos (p);
  if (! c) return;
  if (c->broken) return;
  c->pressed = true;

  if (! c->prev_pressed) {
    register_changed_pos (p, CHPOS_PRESS_CLOSER_FLOOR);
    c->prev_pressed = true;
  }
}

void
break_closer_floor (struct pos *p)
{
  struct closer_floor *c = closer_floor_at_pos (p);
  if (! c) return;
  c->broken = true;
  c->pressed = true;
  register_con_undo
    (&undo, p,
     MIGNORE, MIGNORE, -abs (con (p)->ext.event) - 1,
     false, false, false, false,
     CHPOS_BREAK_CLOSER_FLOOR,
     "LOOSE FLOOR BREAKING");
}

void
unpress_closer_floors (void)
{
  size_t i;
  for (i = 0; i < closer_floor_nmemb; i++) {
    closer_floor[i].prev_pressed =
      closer_floor[i].pressed;
    closer_floor[i].pressed = false;
    closer_floor[i].unresponsive = false;
  }
}

void
register_changed_closer_floors (void)
{
  size_t i;
  for (i = 0; i < closer_floor_nmemb; i++)
    if (closer_floor[i].prev_pressed !=
        closer_floor[i].pressed)
      register_changed_pos (&closer_floor[i].p,
                            CHPOS_UNPRESS_CLOSER_FLOOR);
}

void
compute_closer_floors (void)
{
  size_t i;

  for (i = 0; i < closer_floor_nmemb; i++) {
    struct closer_floor *c = &closer_floor[i];
    if (c->pressed && ! c->unresponsive) {
      if (! c->noise) {
        alert_guards (&c->p);
        play_sample (closer_floor_sample, &c->p, -1);
        c->noise = true;
      }
      close_door (c->p.l, c->event);
    } else c->noise = false;
  }
}

void
draw_closer_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   enum em em, enum vm vm)
{
  struct closer_floor *c = closer_floor_at_pos (p);
  if (! c) return;

  if (c->broken) {
    draw_broken_floor (bitmap, p, em, vm);
    return;
  }

  if (c->pressed) draw_pressed_closer_floor (bitmap, p, em, vm);
  else draw_unpressed_closer_floor (bitmap, p, em, vm);
}

void
draw_closer_floor_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm)
{
  struct closer_floor *c = closer_floor_at_pos (p);
  if (! c) return;

  if (c->broken) {
    draw_floor_base (bitmap, p, em, vm);
    return;
  }

  if (c->pressed) draw_pressed_closer_floor_base (bitmap, p, em, vm);
  else draw_unpressed_closer_floor_base (bitmap, p, em, vm);
}

void
draw_closer_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm)
{
  struct closer_floor *c = closer_floor_at_pos (p);
  if (! c) return;

  if (c->broken) {
    draw_broken_floor_left (bitmap, p, em, vm);
    return;
  }

  if (c->pressed) draw_pressed_closer_floor_left (bitmap, p, em, vm);
  else draw_floor_left (bitmap, p, em, vm);
}

void
draw_closer_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                         enum em em, enum vm vm)
{
  struct closer_floor *c = closer_floor_at_pos (p);
  if (! c) return;

  if (c->broken) {
    draw_broken_floor_right (bitmap, p, em, vm);
    return;
  }

  if (c->pressed) draw_pressed_closer_floor_right (bitmap, p, em, vm);
  else draw_floor_right (bitmap, p, em, vm);
}

void
draw_closer_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm)
{
  struct closer_floor *c = closer_floor_at_pos (p);
  if (! c) return;

  if (c->broken) {
    draw_broken_floor_fg (bitmap, p, em, vm);
    return;
  }
}

void
draw_pressed_closer_floor_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *pressed_closer_floor_base = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: pressed_closer_floor_base = dc_pressed_closer_floor_base; break;
    case EGA: pressed_closer_floor_base = de_pressed_closer_floor_base; break;
    case VGA: pressed_closer_floor_base = dv_pressed_closer_floor_base; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: pressed_closer_floor_base = pc_pressed_closer_floor_base; break;
    case EGA: pressed_closer_floor_base = pe_pressed_closer_floor_base; break;
    case VGA: pressed_closer_floor_base = pv_pressed_closer_floor_base; break;
    }
    break;
  }

  if (vm == VGA) pressed_closer_floor_base = apply_hue_palette (pressed_closer_floor_base);
  if (hgc) pressed_closer_floor_base = apply_palette (pressed_closer_floor_base, hgc_palette);
  if (peq (p, &mouse_pos))
    pressed_closer_floor_base = apply_palette (pressed_closer_floor_base, selection_palette);

  struct coord c;
  draw_bitmapc (pressed_closer_floor_base, bitmap, floor_base_coord (p, &c), 0);
}

void
draw_pressed_closer_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm)
{
  draw_pressed_closer_floor_base (bitmap, p, em, vm);
  draw_pressed_closer_floor_left (bitmap, p, em, vm);
  draw_pressed_closer_floor_right (bitmap, p, em, vm);
}

void
draw_pressed_closer_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *pressed_closer_floor_left = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: pressed_closer_floor_left = dc_floor_left; break;
    case EGA: pressed_closer_floor_left = de_floor_left; break;
    case VGA: pressed_closer_floor_left = dv_floor_left; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: pressed_closer_floor_left = pc_floor_left; break;
    case EGA: pressed_closer_floor_left = pe_floor_left; break;
    case VGA: pressed_closer_floor_left = pv_floor_left; break;
    }
    break;
  }

  if (vm == VGA) pressed_closer_floor_left = apply_hue_palette (pressed_closer_floor_left);
  if (hgc) pressed_closer_floor_left = apply_palette (pressed_closer_floor_left, hgc_palette);
  if (peq (p, &mouse_pos))
    pressed_closer_floor_left = apply_palette (pressed_closer_floor_left, selection_palette);

  struct coord c;
  draw_bitmapc (pressed_closer_floor_left, bitmap,
                pressed_closer_floor_left_coord (p, &c), 0);
}

void
draw_pressed_closer_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                 enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *floor_right = NULL,
    *pressed_closer_floor_right = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA:
      floor_right = dc_floor_right;
      pressed_closer_floor_right = dc_pressed_closer_floor_right;
      break;
    case EGA:
      floor_right = de_floor_right;
      pressed_closer_floor_right = de_pressed_closer_floor_right;
      break;
    case VGA:
      floor_right = dv_floor_right;
      pressed_closer_floor_right = dv_pressed_closer_floor_right;
      break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA:
      floor_right = pc_floor_right;
      pressed_closer_floor_right = pc_pressed_closer_floor_right;
      break;
    case EGA:
      floor_right = pe_floor_right;
      pressed_closer_floor_right = pe_pressed_closer_floor_right;
      break;
    case VGA:
      floor_right = pv_floor_right;
      pressed_closer_floor_right = pv_pressed_closer_floor_right;
      break;
    }
    break;
  }

  if (vm == VGA) {
    floor_right = apply_hue_palette (floor_right);
    pressed_closer_floor_right = apply_hue_palette (pressed_closer_floor_right);
  }

  if (hgc) {
    floor_right = apply_palette (floor_right, hgc_palette);
    pressed_closer_floor_right = apply_palette (pressed_closer_floor_right, hgc_palette);
  }

  if (peq (p, &mouse_pos)) {
    floor_right = apply_palette (floor_right, selection_palette);
    pressed_closer_floor_right = apply_palette (pressed_closer_floor_right, selection_palette);
  }

  struct coord c; struct pos np;
  draw_bitmapc (floor_right, bitmap,
                pressed_closer_floor_right_coord (p, &c), 0);
  if (! is_strictly_traversable (prel (p, &np, 0, +1)))
    draw_bitmapc (pressed_closer_floor_right, bitmap,
                  floor_right_coord (p, &c), 0);
}

void
draw_unpressed_closer_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                             enum em em, enum vm vm)
{
  draw_unpressed_closer_floor_base (bitmap, p, em, vm);
  draw_floor_left (bitmap, p, em, vm);
  draw_floor_right (bitmap, p, em, vm);
}

void
draw_unpressed_closer_floor_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                  enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *unpressed_closer_floor_base = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: unpressed_closer_floor_base = dc_unpressed_closer_floor_base; break;
    case EGA: unpressed_closer_floor_base = de_unpressed_closer_floor_base; break;
    case VGA: unpressed_closer_floor_base = dv_unpressed_closer_floor_base; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: unpressed_closer_floor_base = pc_unpressed_closer_floor_base; break;
    case EGA: unpressed_closer_floor_base = pe_unpressed_closer_floor_base; break;
    case VGA: unpressed_closer_floor_base = pv_unpressed_closer_floor_base; break;
    }
    break;
  }

  if (vm == VGA) unpressed_closer_floor_base = apply_hue_palette (unpressed_closer_floor_base);
  if (hgc) unpressed_closer_floor_base = apply_palette (unpressed_closer_floor_base, hgc_palette);
  if (peq (p, &mouse_pos)) unpressed_closer_floor_base = apply_palette (unpressed_closer_floor_base, selection_palette);

  struct coord c;
  draw_bitmapc (unpressed_closer_floor_base, bitmap,
                floor_base_coord (p, &c), 0);
}

struct coord *
pressed_closer_floor_left_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place,
               PLACE_HEIGHT * p->floor + 51);
}

struct coord *
pressed_closer_floor_right_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * (p->place + 1),
               PLACE_HEIGHT * p->floor + 51);
}
