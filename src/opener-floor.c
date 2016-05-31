/*
  opener-floor.c -- opener floor module;

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
ALLEGRO_BITMAP *dc_unpressed_opener_floor_base,
  *dc_unpressed_opener_floor_left, *dc_unpressed_opener_floor_right;

/* palace cga */
ALLEGRO_BITMAP *pc_unpressed_opener_floor_base,
  *pc_unpressed_opener_floor_left, *pc_unpressed_opener_floor_right;

/* dungeon ega */
ALLEGRO_BITMAP *de_unpressed_opener_floor_base,
  *de_unpressed_opener_floor_left, *de_unpressed_opener_floor_right;

/* palace ega */
ALLEGRO_BITMAP *pe_unpressed_opener_floor_base,
  *pe_unpressed_opener_floor_left, *pe_unpressed_opener_floor_right;

/* dungeon vga */
ALLEGRO_BITMAP *dv_unpressed_opener_floor_base,
  *dv_unpressed_opener_floor_left, *dv_unpressed_opener_floor_right;

/* palace vga */
ALLEGRO_BITMAP *pv_unpressed_opener_floor_base,
  *pv_unpressed_opener_floor_left, *pv_unpressed_opener_floor_right;

struct opener_floor *opener_floor = NULL;
size_t opener_floor_nmemb = 0;

void
load_opener_floor (void)
{
  /* dungeon cga */
  dc_unpressed_opener_floor_base = load_bitmap (DC_UNPRESSED_OPENER_FLOOR_BASE);
  dc_unpressed_opener_floor_left = load_bitmap (DC_UNPRESSED_OPENER_FLOOR_LEFT);
  dc_unpressed_opener_floor_right = load_bitmap (DC_UNPRESSED_OPENER_FLOOR_RIGHT);

  /* palace cga */
  pc_unpressed_opener_floor_base = load_bitmap (PC_UNPRESSED_OPENER_FLOOR_BASE);
  pc_unpressed_opener_floor_left = load_bitmap (PC_UNPRESSED_OPENER_FLOOR_LEFT);
  pc_unpressed_opener_floor_right = load_bitmap (PC_UNPRESSED_OPENER_FLOOR_RIGHT);

  /* dungeon ega */
  de_unpressed_opener_floor_base = load_bitmap (DE_UNPRESSED_OPENER_FLOOR_BASE);
  de_unpressed_opener_floor_left = load_bitmap (DE_UNPRESSED_OPENER_FLOOR_LEFT);
  de_unpressed_opener_floor_right = load_bitmap (DE_UNPRESSED_OPENER_FLOOR_RIGHT);

  /* palace ega */
  pe_unpressed_opener_floor_base = load_bitmap (PE_UNPRESSED_OPENER_FLOOR_BASE);
  pe_unpressed_opener_floor_left = load_bitmap (PE_UNPRESSED_OPENER_FLOOR_LEFT);
  pe_unpressed_opener_floor_right = load_bitmap (PE_UNPRESSED_OPENER_FLOOR_RIGHT);

  /* dungeon vga */
  dv_unpressed_opener_floor_base = load_bitmap (DV_UNPRESSED_OPENER_FLOOR_BASE);
  dv_unpressed_opener_floor_left = load_bitmap (DV_UNPRESSED_OPENER_FLOOR_LEFT);
  dv_unpressed_opener_floor_right = load_bitmap (DV_UNPRESSED_OPENER_FLOOR_RIGHT);

  /* palace vga */
  pv_unpressed_opener_floor_base = load_bitmap (PV_UNPRESSED_OPENER_FLOOR_BASE);
  pv_unpressed_opener_floor_left = load_bitmap (PV_UNPRESSED_OPENER_FLOOR_LEFT);
  pv_unpressed_opener_floor_right = load_bitmap (PV_UNPRESSED_OPENER_FLOOR_RIGHT);
}

void
unload_opener_floor (void)
{
  /* dungeon cga */
  destroy_bitmap (dc_unpressed_opener_floor_base);
  destroy_bitmap (dc_unpressed_opener_floor_left);
  destroy_bitmap (dc_unpressed_opener_floor_right);

  /* palace cga */
  destroy_bitmap (pc_unpressed_opener_floor_base);
  destroy_bitmap (pc_unpressed_opener_floor_left);
  destroy_bitmap (pc_unpressed_opener_floor_right);

  /* dungeon ega */
  destroy_bitmap (de_unpressed_opener_floor_base);
  destroy_bitmap (de_unpressed_opener_floor_left);
  destroy_bitmap (de_unpressed_opener_floor_right);

  /* palace ega */
  destroy_bitmap (pe_unpressed_opener_floor_base);
  destroy_bitmap (pe_unpressed_opener_floor_left);
  destroy_bitmap (pe_unpressed_opener_floor_right);

  /* dungeon vga */
  destroy_bitmap (dv_unpressed_opener_floor_base);
  destroy_bitmap (dv_unpressed_opener_floor_left);
  destroy_bitmap (dv_unpressed_opener_floor_right);

  /* palace vga */
  destroy_bitmap (pv_unpressed_opener_floor_base);
  destroy_bitmap (pv_unpressed_opener_floor_left);
  destroy_bitmap (pv_unpressed_opener_floor_right);
}

void
register_opener_floor (struct pos *p)
{
  assert (con (p)->fg == OPENER_FLOOR
          && opener_floor_at_pos (p) == NULL);

  struct opener_floor o;

  int event = con (p)->ext.event;

  o.p = *p;
  o.event = (event < 0) ? -event - 1 : event;
  o.pressed = (event < 0);
  o.noise = (event < 0);
  o.broken = (event < 0);

  opener_floor =
    add_to_array (&o, 1, opener_floor, &opener_floor_nmemb,
                  opener_floor_nmemb, sizeof (o));

  qsort (opener_floor, opener_floor_nmemb, sizeof (o),
         compare_opener_floors);
}

int
compare_opener_floors (const void *o0, const void *o1)
{
  return cpos (&((struct opener_floor *) o0)->p,
               &((struct opener_floor *) o1)->p);
}

struct opener_floor *
opener_floor_at_pos (struct pos *p)
{
  struct opener_floor o;
  o.p = *p;

  return bsearch (&o, opener_floor, opener_floor_nmemb, sizeof (o),
                  compare_opener_floors);
}

struct opener_floor *
opener_floor_by_event (struct pos *p, int event, int dir)
{
  struct opener_floor *o;
  if (p) o = opener_floor_at_pos (p);
  else o = &opener_floor[0];

  if (! o) {
    o = &opener_floor[0];
    p = NULL;
  }

  int i;

  if (dir < 0)
    for (i = o - opener_floor - (p ? 1 : 0); i >= 0; i--) {
      if (opener_floor[i].event == event) return &opener_floor[i];
    }
  else
    for (i = o - opener_floor + (p ? 1 : 0);
         i < opener_floor_nmemb; i++) {
      if (opener_floor[i].event == event) return &opener_floor[i];
    }

  return NULL;
}

void
remove_opener_floor (struct opener_floor *o)
{
  size_t i =  o - opener_floor;
  opener_floor =
    remove_from_array (opener_floor, &opener_floor_nmemb, i, 1, sizeof (*o));
}

void
press_opener_floor (struct pos *p)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;
  o->pressed = true;

  if (! o->prev_pressed) {
     register_changed_pos (p, CHPOS_PRESS_OPENER_FLOOR);
     o->prev_pressed = true;
  }
}

void
break_opener_floor (struct pos *p)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;
  o->broken = true;
  register_con_undo
    (&undo, p,
     MIGNORE, MIGNORE, -abs (con (p)->ext.event) - 1,
     false, false, false, false,
     CHPOS_BREAK_OPENER_FLOOR,
     "LOOSE FLOOR BREAKING");
}

void
unpress_opener_floors (void)
{
  size_t i;
  for (i = 0; i < opener_floor_nmemb; i++) {
    opener_floor[i].prev_pressed =
      opener_floor[i].pressed;
    opener_floor[i].pressed = false;
  }
}

void
register_changed_opener_floors (void)
{
  size_t i;
  for (i = 0; i < opener_floor_nmemb; i++)
    if (opener_floor[i].prev_pressed !=
        opener_floor[i].pressed)
      register_changed_pos (&opener_floor[i].p,
                            CHPOS_UNPRESS_OPENER_FLOOR);
}

void
compute_opener_floors (void)
{
  size_t i;

  for (i = 0; i < opener_floor_nmemb; i++) {
    struct opener_floor *o = &opener_floor[i];
    if (o->p.room == -1) {
      /* remove_opener_floor (o); i--; */
      continue;
    }
    if (o->pressed || o->broken) {
      if (! o->noise) {
        alert_guards (&o->p);
        play_sample (opener_floor_sample, o->p.room);
        o->noise = true;
      }
      open_door (o->event);
    } else o->noise = false;
  }
}

void
draw_opener_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   enum em em, enum vm vm)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;

  if (o->broken) {
    draw_broken_floor (bitmap, p, em, vm);
    return;
  }

  if (o->pressed) draw_floor (bitmap, p, em, vm);
  else draw_unpressed_opener_floor (bitmap, p, em, vm);
}

void
draw_opener_floor_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;

  if (o->broken) {
    draw_floor_base (bitmap, p, em, vm);
    return;
  }

  if (o->pressed) draw_floor_base (bitmap, p, em, vm);
  else draw_unpressed_opener_floor_base (bitmap, p, em, vm);
}

void
draw_opener_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;

  if (o->broken) {
    draw_broken_floor_left (bitmap, p, em, vm);
    return;
  }

  if (o->pressed) draw_floor_left (bitmap, p, em, vm);
  else draw_unpressed_opener_floor_left (bitmap, p, em, vm);
}

void
draw_opener_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                         enum em em, enum vm vm)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;

  if (o->broken) {
    draw_broken_floor_right (bitmap, p, em, vm);
    return;
  }

  if (o->pressed) draw_floor_right (bitmap, p, em, vm);
  else draw_unpressed_opener_floor_right (bitmap, p, em, vm);
}

void
draw_opener_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;

  if (o->broken) {
    draw_broken_floor_fg (bitmap, p, em, vm);
    return;
  }
}

void
draw_unpressed_opener_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                             enum em em, enum vm vm)
{
  draw_unpressed_opener_floor_base (bitmap, p, em, vm);
  draw_unpressed_opener_floor_left (bitmap, p, em, vm);
  draw_unpressed_opener_floor_right (bitmap, p, em, vm);
}

void
draw_unpressed_opener_floor_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                  enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *unpressed_opener_floor_base = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: unpressed_opener_floor_base = dc_unpressed_opener_floor_base; break;
    case EGA: unpressed_opener_floor_base = de_unpressed_opener_floor_base; break;
    case VGA: unpressed_opener_floor_base = dv_unpressed_opener_floor_base; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: unpressed_opener_floor_base = pc_unpressed_opener_floor_base; break;
    case EGA: unpressed_opener_floor_base = pe_unpressed_opener_floor_base; break;
    case VGA: unpressed_opener_floor_base = pv_unpressed_opener_floor_base; break;
    }
    break;
  }

  if (vm == VGA) unpressed_opener_floor_base = apply_hue_palette (unpressed_opener_floor_base);
  if (hgc) unpressed_opener_floor_base = apply_palette (unpressed_opener_floor_base, hgc_palette);
  if (peq (p, &mouse_pos))
    unpressed_opener_floor_base = apply_palette (unpressed_opener_floor_base, selection_palette);

  struct coord c;
  draw_bitmapc (unpressed_opener_floor_base, bitmap, floor_base_coord (p, &c), 0);
}

void
draw_unpressed_opener_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                  enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *unpressed_opener_floor_left = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: unpressed_opener_floor_left = dc_unpressed_opener_floor_left; break;
    case EGA: unpressed_opener_floor_left = de_unpressed_opener_floor_left; break;
    case VGA: unpressed_opener_floor_left = dv_unpressed_opener_floor_left; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: unpressed_opener_floor_left = pc_unpressed_opener_floor_left; break;
    case EGA: unpressed_opener_floor_left = pe_unpressed_opener_floor_left; break;
    case VGA: unpressed_opener_floor_left = pv_unpressed_opener_floor_left; break;
    }
    break;
  }

  if (vm == VGA) unpressed_opener_floor_left = apply_hue_palette (unpressed_opener_floor_left);
  if (hgc) unpressed_opener_floor_left = apply_palette (unpressed_opener_floor_left, hgc_palette);
  if (peq (p, &mouse_pos))
    unpressed_opener_floor_left = apply_palette (unpressed_opener_floor_left, selection_palette);

  struct coord c;
  draw_bitmapc (unpressed_opener_floor_left, bitmap,
                unpressed_opener_floor_left_coord (p, &c), 0);
}

void
draw_unpressed_opener_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                   enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *unpressed_opener_floor_right = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: unpressed_opener_floor_right = dc_unpressed_opener_floor_right; break;
    case EGA: unpressed_opener_floor_right = de_unpressed_opener_floor_right; break;
    case VGA: unpressed_opener_floor_right = dv_unpressed_opener_floor_right; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: unpressed_opener_floor_right = pc_unpressed_opener_floor_right; break;
    case EGA: unpressed_opener_floor_right = pe_unpressed_opener_floor_right; break;
    case VGA: unpressed_opener_floor_right = pv_unpressed_opener_floor_right; break;
    }
    break;
  }

  if (vm == VGA) unpressed_opener_floor_right = apply_hue_palette (unpressed_opener_floor_right);
  if (hgc) unpressed_opener_floor_right = apply_palette (unpressed_opener_floor_right, hgc_palette);
  if (peq (p, &mouse_pos))
    unpressed_opener_floor_right = apply_palette (unpressed_opener_floor_right, selection_palette);

  struct coord c;
  draw_bitmapc (unpressed_opener_floor_right, bitmap,
                unpressed_opener_floor_right_coord (p, &c), 0);
}

struct coord *
unpressed_opener_floor_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 50 - 1;
  c->room = p->room;
  return c;
}

struct coord *
unpressed_opener_floor_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 50 - 1;
  c->room = p->room;
  return c;
}
