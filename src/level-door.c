/*
  level-door.c -- level door module;

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
ALLEGRO_BITMAP *dc_level_door_top_left, *dc_level_door_top_right,
  *dc_level_door_bottom, *dc_level_door_floor, *dc_level_door_stairs,
  *dc_level_door_front;
ALLEGRO_BITMAP *dc_level_door_front_cache[LEVEL_DOOR_STEPS];

/* palace cga */
ALLEGRO_BITMAP *pc_level_door_top_left, *pc_level_door_top_right,
  *pc_level_door_bottom, *pc_level_door_floor, *pc_level_door_stairs,
  *pc_level_door_front;
ALLEGRO_BITMAP *pc_level_door_front_cache[LEVEL_DOOR_STEPS];

/* dungeon ega */
ALLEGRO_BITMAP *de_level_door_top_left, *de_level_door_top_right,
  *de_level_door_bottom, *de_level_door_floor, *de_level_door_stairs,
  *de_level_door_front;
ALLEGRO_BITMAP *de_level_door_front_cache[LEVEL_DOOR_STEPS];

/* palace ega */
ALLEGRO_BITMAP *pe_level_door_top_left, *pe_level_door_top_right,
  *pe_level_door_bottom, *pe_level_door_floor, *pe_level_door_stairs,
  *pe_level_door_front;
ALLEGRO_BITMAP *pe_level_door_front_cache[LEVEL_DOOR_STEPS];

/* dungeon vga */
ALLEGRO_BITMAP *dv_level_door_top_left, *dv_level_door_top_right,
  *dv_level_door_bottom, *dv_level_door_floor, *dv_level_door_stairs,
  *dv_level_door_front;
ALLEGRO_BITMAP *dv_level_door_front_cache[LEVEL_DOOR_STEPS];

/* palace vga */
ALLEGRO_BITMAP *pv_level_door_top_left, *pv_level_door_top_right,
  *pv_level_door_bottom, *pv_level_door_floor, *pv_level_door_stairs,
  *pv_level_door_front;
ALLEGRO_BITMAP *pv_level_door_front_cache[LEVEL_DOOR_STEPS];

struct level_door *level_door = NULL;
size_t level_door_nmemb = 0;

void
load_level_door (void)
{
  /* dungeon cga */
  dc_level_door_top_left = load_bitmap (DC_LEVEL_DOOR_TOP_LEFT);
  dc_level_door_top_right = load_bitmap (DC_LEVEL_DOOR_TOP_RIGHT);
  dc_level_door_bottom = load_bitmap (DC_LEVEL_DOOR_BOTTOM);
  dc_level_door_floor = load_bitmap (DC_LEVEL_DOOR_FLOOR);
  dc_level_door_stairs = load_bitmap (DC_LEVEL_DOOR_STAIRS);
  dc_level_door_front = load_bitmap (DC_LEVEL_DOOR_FRONT);
  generate_level_door_front_cache (dc_level_door_front_cache, DUNGEON, CGA);

  /* palace cga */
  pc_level_door_top_left = load_bitmap (PC_LEVEL_DOOR_TOP_LEFT);
  pc_level_door_top_right = load_bitmap (PC_LEVEL_DOOR_TOP_RIGHT);
  pc_level_door_bottom = load_bitmap (PC_LEVEL_DOOR_BOTTOM);
  pc_level_door_floor = load_bitmap (PC_LEVEL_DOOR_FLOOR);
  pc_level_door_stairs = load_bitmap (PC_LEVEL_DOOR_STAIRS);
  pc_level_door_front = load_bitmap (PC_LEVEL_DOOR_FRONT);
  generate_level_door_front_cache (pc_level_door_front_cache, PALACE, CGA);

  /* dungeon ega */
  de_level_door_top_left = load_bitmap (DE_LEVEL_DOOR_TOP_LEFT);
  de_level_door_top_right = load_bitmap (DE_LEVEL_DOOR_TOP_RIGHT);
  de_level_door_bottom = load_bitmap (DE_LEVEL_DOOR_BOTTOM);
  de_level_door_floor = load_bitmap (DE_LEVEL_DOOR_FLOOR);
  de_level_door_stairs = load_bitmap (DE_LEVEL_DOOR_STAIRS);
  de_level_door_front = load_bitmap (DE_LEVEL_DOOR_FRONT);
  generate_level_door_front_cache (de_level_door_front_cache, DUNGEON, EGA);

  /* palace ega */
  pe_level_door_top_left = load_bitmap (PE_LEVEL_DOOR_TOP_LEFT);
  pe_level_door_top_right = load_bitmap (PE_LEVEL_DOOR_TOP_RIGHT);
  pe_level_door_bottom = load_bitmap (PE_LEVEL_DOOR_BOTTOM);
  pe_level_door_floor = load_bitmap (PE_LEVEL_DOOR_FLOOR);
  pe_level_door_stairs = load_bitmap (PE_LEVEL_DOOR_STAIRS);
  pe_level_door_front = load_bitmap (PE_LEVEL_DOOR_FRONT);
  generate_level_door_front_cache (pe_level_door_front_cache, PALACE, EGA);

  /* dungeon vga */
  dv_level_door_top_left = load_bitmap (DV_LEVEL_DOOR_TOP_LEFT);
  dv_level_door_top_right = load_bitmap (DV_LEVEL_DOOR_TOP_RIGHT);
  dv_level_door_bottom = load_bitmap (DV_LEVEL_DOOR_BOTTOM);
  dv_level_door_floor = load_bitmap (DV_LEVEL_DOOR_FLOOR);
  dv_level_door_stairs = load_bitmap (DV_LEVEL_DOOR_STAIRS);
  dv_level_door_front = load_bitmap (DV_LEVEL_DOOR_FRONT);
  generate_level_door_front_cache (dv_level_door_front_cache, DUNGEON, VGA);

  /* palace vga */
  pv_level_door_top_left = load_bitmap (PV_LEVEL_DOOR_TOP_LEFT);
  pv_level_door_top_right = load_bitmap (PV_LEVEL_DOOR_TOP_RIGHT);
  pv_level_door_bottom = load_bitmap (PV_LEVEL_DOOR_BOTTOM);
  pv_level_door_floor = load_bitmap (PV_LEVEL_DOOR_FLOOR);
  pv_level_door_stairs = load_bitmap (PV_LEVEL_DOOR_STAIRS);
  pv_level_door_front = load_bitmap (PV_LEVEL_DOOR_FRONT);
  generate_level_door_front_cache (pv_level_door_front_cache, PALACE, VGA);
}

void
unload_level_door (void)
{
  /* dungeon cga */
  destroy_bitmap (dc_level_door_top_left);
  destroy_bitmap (dc_level_door_top_right);
  destroy_bitmap (dc_level_door_bottom);
  destroy_bitmap (dc_level_door_floor);
  destroy_bitmap (dc_level_door_stairs);
  destroy_bitmap (dc_level_door_front);
  destroy_level_door_front_cache (dc_level_door_front_cache);

  /* palace cga */
  destroy_bitmap (pc_level_door_top_left);
  destroy_bitmap (pc_level_door_top_right);
  destroy_bitmap (pc_level_door_bottom);
  destroy_bitmap (pc_level_door_floor);
  destroy_bitmap (pc_level_door_stairs);
  destroy_bitmap (pc_level_door_front);
  destroy_level_door_front_cache (pc_level_door_front_cache);

  /* dungeon ega */
  destroy_bitmap (de_level_door_top_left);
  destroy_bitmap (de_level_door_top_right);
  destroy_bitmap (de_level_door_bottom);
  destroy_bitmap (de_level_door_floor);
  destroy_bitmap (de_level_door_stairs);
  destroy_bitmap (de_level_door_front);
  destroy_level_door_front_cache (de_level_door_front_cache);

  /* palace ega */
  destroy_bitmap (pe_level_door_top_left);
  destroy_bitmap (pe_level_door_top_right);
  destroy_bitmap (pe_level_door_bottom);
  destroy_bitmap (pe_level_door_floor);
  destroy_bitmap (pe_level_door_stairs);
  destroy_bitmap (pe_level_door_front);
  destroy_level_door_front_cache (pe_level_door_front_cache);

  /* dungeon vga */
  destroy_bitmap (dv_level_door_top_left);
  destroy_bitmap (dv_level_door_top_right);
  destroy_bitmap (dv_level_door_bottom);
  destroy_bitmap (dv_level_door_floor);
  destroy_bitmap (dv_level_door_stairs);
  destroy_bitmap (dv_level_door_front);
  destroy_level_door_front_cache (dv_level_door_front_cache);

  /* palace vga */
  destroy_bitmap (pv_level_door_top_left);
  destroy_bitmap (pv_level_door_top_right);
  destroy_bitmap (pv_level_door_bottom);
  destroy_bitmap (pv_level_door_floor);
  destroy_bitmap (pv_level_door_stairs);
  destroy_bitmap (pv_level_door_front);
  destroy_level_door_front_cache (pv_level_door_front_cache);
}

void
generate_level_door_front_cache (ALLEGRO_BITMAP *cache[LEVEL_DOOR_STEPS],
                                 enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *level_door_front = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: level_door_front = dc_level_door_front; break;
    case EGA: level_door_front = de_level_door_front; break;
    case VGA: level_door_front = dv_level_door_front; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: level_door_front = pc_level_door_front; break;
    case EGA: level_door_front = pe_level_door_front; break;
    case VGA: level_door_front = pv_level_door_front; break;
    }
    break;
  }

  int i;
  for (i = 0; i < LEVEL_DOOR_STEPS; i++) {
    int q = (i + 1) / 4;
    int r = (i + 1) % 4;
    int w = al_get_bitmap_width (level_door_front);

    cache[i] = create_bitmap (w, r + 1 + (q + 2) * 4);
    clear_bitmap (cache[i], TRANSPARENT_COLOR);

    draw_bitmap_region (level_door_front, cache[i], 0, 3 - r, w, r + 1,
                        0, 0, 0);
    int j;
    for (j = 0; j <= q; j++)
      draw_bitmap (level_door_front, cache[i], 0, j * 4 + r + 1, 0);

    validate_bitmap_for_mingw (cache[i]);
  }
}

void
destroy_level_door_front_cache (ALLEGRO_BITMAP *cache[LEVEL_DOOR_STEPS])
{
  size_t i;
  for (i = 0; i < LEVEL_DOOR_STEPS; i++) destroy_bitmap (cache[i]);
}

void
register_level_door (struct pos *p)
{
  struct level_door d;

  int n, f;
  typed_int (ext (p), LEVEL_DOOR_STEPS, LEVEL_DOOR_FASES, &n, &f);

  npos (p, &d.p);
  d.broken = f;
  d.i = n;
  d.action = NO_LEVEL_DOOR_ACTION;
  d.no_stairs = peq (p, &p->l->start_pos);
  d.priority = 0;

  level_door =
    add_to_array (&d, 1, level_door, &level_door_nmemb, level_door_nmemb, sizeof (d));

  qsort (level_door, level_door_nmemb, sizeof (d), compare_level_doors);
}

int
compare_level_doors (const void *d0, const void *d1)
{
  return cpos (&((struct level_door *) d0)->p, &((struct level_door *) d1)->p);
}

struct level_door *
copy_level_door (struct level_door *to, struct level_door *from)
{
  struct pos p = to->p;
  *to = *from;
  to->p = p;
  return to;
}

struct level_door *
level_door_at_pos (struct pos *p)
{
  struct level_door d;
  d.p = *p;

  struct level_door *dd;

 search:
  dd = bsearch (&d, level_door, level_door_nmemb, sizeof (d),
                compare_level_doors);

  if (dd && fg (p) != LEVEL_DOOR) {
    remove_level_door (dd);
    return NULL;
  } else if (! dd && fg (p) == LEVEL_DOOR) {
    register_level_door (p);
    goto search;
  }

  return dd;
}

void
remove_level_door (struct level_door *d)
{
  size_t i =  d - level_door;
  level_door =
    remove_from_array (level_door, &level_door_nmemb, i, 1, sizeof (*d));
}

int
fake_level_door_step (struct pos *p)
{
  return is_fake (p) ? fake_ext (p) : level_door_at_pos (p)->i;
}

bool
fake_level_door_stairs (struct pos *p)
{
  return is_fake (p) ? abs (ext (p)) % 2 : ! level_door_at_pos (p)->no_stairs;
}

bool
fake_level_door_broken (struct pos *p)
{
  return is_fake (p) ? abs (ext (p)) % 2 : level_door_at_pos (p)->broken;
}

void
break_level_door (struct pos *p)
{
  struct level_door *d = level_door_at_pos (p);
  if (! d) return;
  register_con_undo
    (&undo, p,
     MIGNORE, MIGNORE, d->i + LEVEL_DOOR_STEPS, MIGNORE,
     NULL, false, "LOOSE FLOOR BREAKING");
  d->broken = true;
}

void
compute_level_doors (void)
{
  size_t i;

  for (i = 0; i < level_door_nmemb;) {
    struct level_door *d = &level_door[i];
    if (fg (&d->p) == LEVEL_DOOR) {
      i++; continue;
    }
    remove_level_door (d);
  }

  for (i = 0; i < level_door_nmemb; i++) {
    struct level_door *d = &level_door[i];
    switch (d->action) {
    case OPEN_LEVEL_DOOR:
      if (d->i % 5 == 2 && d->i > 2) {
        if (d->i == LEVEL_DOOR_STEPS - 2) alert_guards (&d->p);
        play_audio (&level_door_open_audio, &d->p, -1);
      }
      if (d->i > 0) {
        d->i--;
        register_changed_pos (&d->p);
      } else d->action = NO_LEVEL_DOOR_ACTION;
      break;
    case CLOSE_LEVEL_DOOR:
      if (d->i == 0) {
        alert_guards (&d->p);
        play_audio (&level_door_open_audio, &d->p, -1);
      }
      if (d->i < LEVEL_DOOR_STEPS - 1) {
        int r = 14 - (d->i % 15);
        d->i += r ? r : 15;
        register_changed_pos (&d->p);
        if (d->i >= LEVEL_DOOR_STEPS - 1) {
          d->i = LEVEL_DOOR_STEPS - 1;
          d->action = NO_LEVEL_DOOR_ACTION;
          play_audio (&level_door_close_audio, &d->p, -1);
        }
      }
      break;
    default:
      break;
    }
  }
}

struct level_door *
get_exit_level_door (struct level *l, int n)
{
  int i;
  for (i = 0; i < level_door_nmemb; i++) {
    struct level_door *d = &level_door[i];
    if (! peq (&d->p, &l->start_pos)
        && d->i == 0
        && n-- == 0) return d;
  }
  return NULL;
}

void
draw_level_door (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm)
{
  struct coord c;

  pos2coord_f level_door_top_left_coord = NULL,
    level_door_top_right_coord = NULL,
    level_door_bottom_left_coord = NULL,
    level_door_bottom_right_coord = NULL;

  ALLEGRO_BITMAP *level_door_bottom = NULL,
    *level_door_top_left = NULL,
    *level_door_floor = NULL,
    *level_door_stairs = NULL,
    *level_door_top_right = NULL;

  switch (em) {
  case DUNGEON:
    level_door_top_left_coord = d_level_door_top_left_coord;
    level_door_top_right_coord = d_level_door_top_right_coord;
    level_door_bottom_left_coord = d_level_door_bottom_left_coord;
    level_door_bottom_right_coord = d_level_door_bottom_right_coord;
    switch (vm) {
    case CGA:
      level_door_bottom = dc_level_door_bottom;
      level_door_top_left = dc_level_door_top_left;
      level_door_floor = dc_level_door_floor;
      level_door_stairs = dc_level_door_stairs;
      level_door_top_right = dc_level_door_top_right;
      break;
    case EGA:
      level_door_bottom = de_level_door_bottom;
      level_door_top_left = de_level_door_top_left;
      level_door_floor = de_level_door_floor;
      level_door_stairs = de_level_door_stairs;
      level_door_top_right = de_level_door_top_right;
      break;
    case VGA:
      level_door_bottom = dv_level_door_bottom;
      level_door_top_left = dv_level_door_top_left;
      level_door_floor = dv_level_door_floor;
      level_door_stairs =  dv_level_door_stairs;
      level_door_top_right = dv_level_door_top_right;
      break;
    }
    break;
  case PALACE:
    level_door_top_left_coord = p_level_door_top_left_coord;
    level_door_top_right_coord = p_level_door_top_right_coord;
    level_door_bottom_left_coord = p_level_door_bottom_left_coord;
    level_door_bottom_right_coord = p_level_door_bottom_right_coord;
    switch (vm) {
    case CGA:
      level_door_bottom = pc_level_door_bottom;
      level_door_top_left = pc_level_door_top_left;
      level_door_floor = pc_level_door_floor;
      level_door_stairs = pc_level_door_stairs;
      level_door_top_right = pc_level_door_top_right;
      break;
    case EGA:
      level_door_bottom = pe_level_door_bottom;
      level_door_top_left = pe_level_door_top_left;
      level_door_floor = pe_level_door_floor;
      level_door_stairs = pe_level_door_stairs;
      level_door_top_right = pe_level_door_top_right;
      break;
    case VGA:
      level_door_bottom = pv_level_door_bottom;
      level_door_top_left = pv_level_door_top_left;
      level_door_floor = pv_level_door_floor;
      level_door_stairs = pv_level_door_stairs;
      level_door_top_right = pv_level_door_top_right;
      break;
    }
    break;
  }

  if (vm == VGA) {
    level_door_bottom = apply_hue_palette (level_door_bottom);
    level_door_top_left = apply_hue_palette (level_door_top_left);
    level_door_floor = apply_hue_palette (level_door_floor);
    level_door_stairs = apply_hue_palette (level_door_stairs);
    level_door_top_right = apply_hue_palette (level_door_top_right);
  }

  if (hgc) {
    level_door_bottom = apply_palette (level_door_bottom, hgc_palette);
    level_door_top_left = apply_palette (level_door_top_left, hgc_palette);
    level_door_floor = apply_palette (level_door_floor, hgc_palette);
    level_door_stairs = apply_palette (level_door_stairs, hgc_palette);
    level_door_top_right = apply_palette (level_door_top_right, hgc_palette);
  }

  if (peq (p, &mouse_pos)) {
    level_door_bottom = apply_palette (level_door_bottom, selection_palette);
    level_door_top_left = apply_palette (level_door_top_left, selection_palette);
    level_door_floor = apply_palette (level_door_floor, selection_palette);
    level_door_stairs = apply_palette (level_door_stairs, selection_palette);
    level_door_top_right = apply_palette (level_door_top_right, selection_palette);
  }

  draw_bitmapc (level_door_bottom, bitmap,
                level_door_bottom_left_coord (p, &c), 0);
  draw_bitmapc (level_door_top_left, bitmap,
                level_door_top_left_coord (p, &c), 0);
  draw_bitmapc (level_door_floor, bitmap,
                level_door_floor_coord (p, &c), 0);
  if (fake_level_door_stairs (p))
    draw_bitmapc (level_door_stairs, bitmap,
                  level_door_stairs_coord (p, &c), 0);
  draw_bitmapc (level_door_bottom, bitmap,
                level_door_bottom_right_coord (p, &c), 0);
  draw_bitmapc (level_door_top_right, bitmap,
                level_door_top_right_coord (p, &c), 0);
  draw_level_door_front (bitmap, p, fake_level_door_step (p), em, vm);
}

void
draw_level_door_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f,
                    enum em em, enum vm vm)
{
  pos2coord_f level_door_bottom_right_coord = NULL;
  ALLEGRO_BITMAP *level_door_bottom = NULL;

  if (f && ! is_kid_stairs (f)) return;

  switch (em) {
  case DUNGEON:
    level_door_bottom_right_coord = d_level_door_bottom_right_coord;
    switch (vm) {
    case CGA: level_door_bottom = dc_level_door_bottom; break;
    case EGA: level_door_bottom = de_level_door_bottom; break;
    case VGA: level_door_bottom = dv_level_door_bottom; break;
    }
    break;
  case PALACE:
    level_door_bottom_right_coord = p_level_door_bottom_right_coord;
    switch (vm) {
    case CGA: level_door_bottom = pc_level_door_bottom; break;
    case EGA: level_door_bottom = pe_level_door_bottom; break;
    case VGA: level_door_bottom = pv_level_door_bottom; break;
    }
    break;
  }

  if (vm == VGA) level_door_bottom = apply_hue_palette (level_door_bottom);
  if (hgc) level_door_bottom = apply_palette (level_door_bottom, hgc_palette);
  if (peq (p, &mouse_pos))
    level_door_bottom = apply_palette (level_door_bottom, selection_palette);

  struct coord c;
  push_drawn_rectangle (bitmap);
  draw_bitmapc (level_door_bottom, bitmap,
                level_door_bottom_right_coord (p, &c), 0);
  draw_level_door_front (bitmap, p, fake_level_door_step (p), em, vm);
  draw_confg (bitmap, p, em, vm);
  redraw_drawn_rectangle (pop_drawn_rectangle (), p, em, vm);
}

ALLEGRO_BITMAP *
level_door_front_bitmap (int i, enum em em, enum vm vm)
{
  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: return dc_level_door_front_cache[i];
    case EGA: return de_level_door_front_cache[i];
    case VGA: return dv_level_door_front_cache[i];
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: return pc_level_door_front_cache[i];
    case EGA: return pe_level_door_front_cache[i];
    case VGA: return pv_level_door_front_cache[i];
    }
    break;
  }
  assert (false);
  return NULL;
}

void
draw_level_door_front (ALLEGRO_BITMAP *bitmap, struct pos *p, int i,
                       enum em em, enum vm vm)
{
  i = i < 0 ? 0 : i;
  i = i > LEVEL_DOOR_STEPS - 1 ? LEVEL_DOOR_STEPS - 1 : i;

  ALLEGRO_BITMAP *level_door_front = level_door_front_bitmap (i, em, vm);

  if (vm == VGA) level_door_front = apply_hue_palette (level_door_front);
  if (hgc) level_door_front = apply_palette (level_door_front, hgc_palette);
  if (peq (p, &mouse_pos))
    level_door_front = apply_palette (level_door_front, selection_palette);

  struct coord c;
  draw_bitmapc (level_door_front, bitmap,
                level_door_front_coord_base (p, &c), 0);
}

void
xdraw_level_door_front (ALLEGRO_BITMAP *bitmap, struct pos *p, int i,
                        enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *level_door_front = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: level_door_front = dc_level_door_front; break;
    case EGA: level_door_front = de_level_door_front; break;
    case VGA: level_door_front = dv_level_door_front; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: level_door_front = pc_level_door_front; break;
    case EGA: level_door_front = pe_level_door_front; break;
    case VGA: level_door_front = pv_level_door_front; break;
    }
    break;
  }

  if (vm == VGA) level_door_front = apply_hue_palette (level_door_front);
  if (hgc) level_door_front = apply_palette (level_door_front, hgc_palette);
  if (peq (p, &mouse_pos))
    level_door_front = apply_palette (level_door_front, selection_palette);

  i = i < 0 ? 0 : i;
  i = i > LEVEL_DOOR_STEPS - 1 ? LEVEL_DOOR_STEPS - 1 : i;

  int q = (i + 1) / 4;
  int r = (i + 1) % 4;
  int w = al_get_bitmap_width (level_door_front);
  int j;

  struct coord c;
  draw_bitmap_regionc (level_door_front, bitmap, 0, 3 - r, w, r + 1,
                       level_door_front_coord_base (p, &c), 0);
  for (j = 0; j <= q; j++)
    draw_bitmapc (level_door_front, bitmap,
                  level_door_front_coord (p, &c, j, i), 0);
}

void
draw_level_door_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm)
{
  if (fake_level_door_broken (p)) draw_broken_floor_left (bitmap, p, em, vm);
  else draw_floor_left (bitmap, p, em, vm);
}

void
draw_level_door_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm)
{
  if (fake_level_door_broken (p)) draw_broken_floor_right (bitmap, p, em, vm);
  else draw_floor_right (bitmap, p, em, vm);
}

struct coord *
level_door_front_coord_base (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place + 8,
               PLACE_HEIGHT * p->floor - 1);
}

struct coord *
level_door_front_coord (struct pos *p, struct coord *c, int j, int i)
{
  int r = (i + 1) % 4;
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place + 8,
               PLACE_HEIGHT * p->floor - 1 + j * 4 + r + 1);
}

struct coord *
d_level_door_top_left_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place,
               PLACE_HEIGHT * p->floor - 13);
}

struct coord *
p_level_door_top_left_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place,
               PLACE_HEIGHT * p->floor - 10);
}

struct coord *
d_level_door_top_right_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * (p->place + 1),
               PLACE_HEIGHT * p->floor - 13);
}

struct coord *
p_level_door_top_right_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * (p->place + 1),
               PLACE_HEIGHT * p->floor - 10);
}

struct coord *
d_level_door_bottom_left_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place + 1,
               PLACE_HEIGHT * p->floor + 3);
}

struct coord *
p_level_door_bottom_left_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place - 1,
               PLACE_HEIGHT * p->floor + 3);
}

struct coord *
d_level_door_bottom_right_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * (p->place + 1) + 17,
               PLACE_HEIGHT * p->floor + 3);
}

struct coord *
p_level_door_bottom_right_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * (p->place + 1) + 23,
               PLACE_HEIGHT * p->floor + 3);
}

struct coord *
level_door_floor_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place + 8,
               PLACE_HEIGHT * p->floor + 46);
}

struct coord *
level_door_stairs_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place + 8,
               PLACE_HEIGHT * p->floor + 1);
}
