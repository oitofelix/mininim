/*
  chopper.c -- chopper module;

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

#include <stdio.h>
#include "kernel/audio.h"
#include "kernel/array.h"
#include "kernel/random.h"
#include "anim.h"
#include "room.h"
#include "floor.h"
#include "loose-floor.h"
#include "kid/kid.h"
#include "level.h"
#include "chopper.h"

/* dungeon cga */
ALLEGRO_BITMAP *dc_chopper_01, *dc_chopper_fg_01,
  *dc_chopper_02, *dc_chopper_fg_02,
  *dc_chopper_03, *dc_chopper_fg_03,
  *dc_chopper_04, *dc_chopper_fg_04,
  *dc_chopper_05, *dc_chopper_fg_05;

/* palace cga */
ALLEGRO_BITMAP *pc_chopper_01, *pc_chopper_fg_01,
  *pc_chopper_02, *pc_chopper_fg_02,
  *pc_chopper_03, *pc_chopper_fg_03,
  *pc_chopper_04, *pc_chopper_fg_04,
  *pc_chopper_05, *pc_chopper_fg_05;

/* dungeon ega */
ALLEGRO_BITMAP *de_chopper_01, *de_chopper_fg_01,
  *de_chopper_02, *de_chopper_fg_02,
  *de_chopper_03, *de_chopper_fg_03,
  *de_chopper_04, *de_chopper_fg_04,
  *de_chopper_05, *de_chopper_fg_05;

/* palace ega */
ALLEGRO_BITMAP *pe_chopper_01, *pe_chopper_fg_01,
  *pe_chopper_02, *pe_chopper_fg_02,
  *pe_chopper_03, *pe_chopper_fg_03,
  *pe_chopper_04, *pe_chopper_fg_04,
  *pe_chopper_05, *pe_chopper_fg_05;

/* dungeon vga */
ALLEGRO_BITMAP *dv_chopper_01, *dv_chopper_fg_01,
  *dv_chopper_02, *dv_chopper_fg_02,
  *dv_chopper_03, *dv_chopper_fg_03,
  *dv_chopper_04, *dv_chopper_fg_04,
  *dv_chopper_05, *dv_chopper_fg_05;

/* palace vga */
ALLEGRO_BITMAP *pv_chopper_01, *pv_chopper_fg_01,
  *pv_chopper_02, *pv_chopper_fg_02,
  *pv_chopper_03, *pv_chopper_fg_03,
  *pv_chopper_04, *pv_chopper_fg_04,
  *pv_chopper_05, *pv_chopper_fg_05;

/* palettable */
ALLEGRO_BITMAP *chopper_blood_01, *chopper_blood_02,
  *chopper_blood_03, *chopper_blood_04, *chopper_blood_05;

ALLEGRO_SAMPLE *chopper_sample;

struct chopper *chopper = NULL;
size_t chopper_nmemb = 0;

static void
draw_left_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      struct chopper *ch, enum em em, enum vm vm);
static void
draw_left_02 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      struct chopper *ch, enum em em, enum vm vm);
static void
draw_left_03 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      struct chopper *ch, enum em em, enum vm vm);
static void
draw_left_04 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      struct chopper *ch, enum em em, enum vm vm);
static void
draw_left_05 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      struct chopper *ch, enum em em, enum vm vm);

static void
draw_fg_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                    struct chopper *ch, enum em em, enum vm vm);
static void
draw_fg_02 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                    struct chopper *ch, enum em em, enum vm vm);
static void
draw_fg_03 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                    struct chopper *ch, enum em em, enum vm vm);
static void
draw_fg_04 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                    struct chopper *ch, enum em em, enum vm vm);
static void
draw_fg_05 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                    struct chopper *ch, enum em em, enum vm vm);

static struct coord *chopper_coord (struct pos *p, struct coord *c);
static struct coord *blood_01_coord (struct pos *p, struct coord *c);
static struct coord *blood_02_coord (struct pos *p, struct coord *c);
static struct coord *blood_03_coord (struct pos *p, struct coord *c);
static struct coord *blood_04_coord (struct pos *p, struct coord *c);
static struct coord *blood_05_coord (struct pos *p, struct coord *c);

static ALLEGRO_COLOR c_blood_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR e_blood_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR v_blood_palette (ALLEGRO_COLOR c);

void
load_chopper (void)
{
  /* dungeon cga */
  dc_chopper_01 = load_bitmap (DC_CHOPPER_01);
  dc_chopper_fg_01 = load_bitmap (DC_CHOPPER_FG_01);
  dc_chopper_02 = load_bitmap (DC_CHOPPER_02);
  dc_chopper_fg_02 = load_bitmap (DC_CHOPPER_FG_02);
  dc_chopper_03 = load_bitmap (DC_CHOPPER_03);
  dc_chopper_fg_03 = load_bitmap (DC_CHOPPER_FG_03);
  dc_chopper_04 = load_bitmap (DC_CHOPPER_04);
  dc_chopper_fg_04 = load_bitmap (DC_CHOPPER_FG_04);
  dc_chopper_05 = load_bitmap (DC_CHOPPER_05);
  dc_chopper_fg_05 = load_bitmap (DC_CHOPPER_FG_05);

  /* palace cga */
  pc_chopper_01 = load_bitmap (PC_CHOPPER_01);
  pc_chopper_fg_01 = load_bitmap (PC_CHOPPER_FG_01);
  pc_chopper_02 = load_bitmap (PC_CHOPPER_02);
  pc_chopper_fg_02 = load_bitmap (PC_CHOPPER_FG_02);
  pc_chopper_03 = load_bitmap (PC_CHOPPER_03);
  pc_chopper_fg_03 = load_bitmap (PC_CHOPPER_FG_03);
  pc_chopper_04 = load_bitmap (PC_CHOPPER_04);
  pc_chopper_fg_04 = load_bitmap (PC_CHOPPER_FG_04);
  pc_chopper_05 = load_bitmap (PC_CHOPPER_05);
  pc_chopper_fg_05 = load_bitmap (PC_CHOPPER_FG_05);

  /* dungeon ega */
  de_chopper_01 = load_bitmap (DE_CHOPPER_01);
  de_chopper_fg_01 = load_bitmap (DE_CHOPPER_FG_01);
  de_chopper_02 = load_bitmap (DE_CHOPPER_02);
  de_chopper_fg_02 = load_bitmap (DE_CHOPPER_FG_02);
  de_chopper_03 = load_bitmap (DE_CHOPPER_03);
  de_chopper_fg_03 = load_bitmap (DE_CHOPPER_FG_03);
  de_chopper_04 = load_bitmap (DE_CHOPPER_04);
  de_chopper_fg_04 = load_bitmap (DE_CHOPPER_FG_04);
  de_chopper_05 = load_bitmap (DE_CHOPPER_05);
  de_chopper_fg_05 = load_bitmap (DE_CHOPPER_FG_05);

  /* palace ega */
  pe_chopper_01 = load_bitmap (PE_CHOPPER_01);
  pe_chopper_fg_01 = load_bitmap (PE_CHOPPER_FG_01);
  pe_chopper_02 = load_bitmap (PE_CHOPPER_02);
  pe_chopper_fg_02 = load_bitmap (PE_CHOPPER_FG_02);
  pe_chopper_03 = load_bitmap (PE_CHOPPER_03);
  pe_chopper_fg_03 = load_bitmap (PE_CHOPPER_FG_03);
  pe_chopper_04 = load_bitmap (PE_CHOPPER_04);
  pe_chopper_fg_04 = load_bitmap (PE_CHOPPER_FG_04);
  pe_chopper_05 = load_bitmap (PE_CHOPPER_05);
  pe_chopper_fg_05 = load_bitmap (PE_CHOPPER_FG_05);

  /* dungeon vga */
  dv_chopper_01 = load_bitmap (DV_CHOPPER_01);
  dv_chopper_fg_01 = load_bitmap (DV_CHOPPER_FG_01);
  dv_chopper_02 = load_bitmap (DV_CHOPPER_02);
  dv_chopper_fg_02 = load_bitmap (DV_CHOPPER_FG_02);
  dv_chopper_03 = load_bitmap (DV_CHOPPER_03);
  dv_chopper_fg_03 = load_bitmap (DV_CHOPPER_FG_03);
  dv_chopper_04 = load_bitmap (DV_CHOPPER_04);
  dv_chopper_fg_04 = load_bitmap (DV_CHOPPER_FG_04);
  dv_chopper_05 = load_bitmap (DV_CHOPPER_05);
  dv_chopper_fg_05 = load_bitmap (DV_CHOPPER_FG_05);

  /* palace vga */
  pv_chopper_01 = load_bitmap (PV_CHOPPER_01);
  pv_chopper_fg_01 = load_bitmap (PV_CHOPPER_FG_01);
  pv_chopper_02 = load_bitmap (PV_CHOPPER_02);
  pv_chopper_fg_02 = load_bitmap (PV_CHOPPER_FG_02);
  pv_chopper_03 = load_bitmap (PV_CHOPPER_03);
  pv_chopper_fg_03 = load_bitmap (PV_CHOPPER_FG_03);
  pv_chopper_04 = load_bitmap (PV_CHOPPER_04);
  pv_chopper_fg_04 = load_bitmap (PV_CHOPPER_FG_04);
  pv_chopper_05 = load_bitmap (PV_CHOPPER_05);
  pv_chopper_fg_05 = load_bitmap (PV_CHOPPER_FG_05);

  /* palettable */
  chopper_blood_01 = load_bitmap (CHOPPER_BLOOD_01);
  chopper_blood_02 = load_bitmap (CHOPPER_BLOOD_02);
  chopper_blood_03 = load_bitmap (CHOPPER_BLOOD_03);
  chopper_blood_04 = load_bitmap (CHOPPER_BLOOD_04);
  chopper_blood_05 = load_bitmap (CHOPPER_BLOOD_05);
}

void
unload_chopper (void)
{
  /* dungeon cga */
  al_destroy_bitmap (dc_chopper_01);
  al_destroy_bitmap (dc_chopper_fg_01);
  al_destroy_bitmap (dc_chopper_02);
  al_destroy_bitmap (dc_chopper_fg_02);
  al_destroy_bitmap (dc_chopper_03);
  al_destroy_bitmap (dc_chopper_fg_03);
  al_destroy_bitmap (dc_chopper_04);
  al_destroy_bitmap (dc_chopper_fg_04);
  al_destroy_bitmap (dc_chopper_05);
  al_destroy_bitmap (dc_chopper_fg_05);

  /* palace cga */
  al_destroy_bitmap (pc_chopper_01);
  al_destroy_bitmap (pc_chopper_fg_01);
  al_destroy_bitmap (pc_chopper_02);
  al_destroy_bitmap (pc_chopper_fg_02);
  al_destroy_bitmap (pc_chopper_03);
  al_destroy_bitmap (pc_chopper_fg_03);
  al_destroy_bitmap (pc_chopper_04);
  al_destroy_bitmap (pc_chopper_fg_04);
  al_destroy_bitmap (pc_chopper_05);
  al_destroy_bitmap (pc_chopper_fg_05);

  /* dungeon ega */
  al_destroy_bitmap (de_chopper_01);
  al_destroy_bitmap (de_chopper_fg_01);
  al_destroy_bitmap (de_chopper_02);
  al_destroy_bitmap (de_chopper_fg_02);
  al_destroy_bitmap (de_chopper_03);
  al_destroy_bitmap (de_chopper_fg_03);
  al_destroy_bitmap (de_chopper_04);
  al_destroy_bitmap (de_chopper_fg_04);
  al_destroy_bitmap (de_chopper_05);
  al_destroy_bitmap (de_chopper_fg_05);

  /* palace ega */
  al_destroy_bitmap (pe_chopper_01);
  al_destroy_bitmap (pe_chopper_fg_01);
  al_destroy_bitmap (pe_chopper_02);
  al_destroy_bitmap (pe_chopper_fg_02);
  al_destroy_bitmap (pe_chopper_03);
  al_destroy_bitmap (pe_chopper_fg_03);
  al_destroy_bitmap (pe_chopper_04);
  al_destroy_bitmap (pe_chopper_fg_04);
  al_destroy_bitmap (pe_chopper_05);
  al_destroy_bitmap (pe_chopper_fg_05);

  /* dungeon vga */
  al_destroy_bitmap (dv_chopper_01);
  al_destroy_bitmap (dv_chopper_fg_01);
  al_destroy_bitmap (dv_chopper_02);
  al_destroy_bitmap (dv_chopper_fg_02);
  al_destroy_bitmap (dv_chopper_03);
  al_destroy_bitmap (dv_chopper_fg_03);
  al_destroy_bitmap (dv_chopper_04);
  al_destroy_bitmap (dv_chopper_fg_04);
  al_destroy_bitmap (dv_chopper_05);
  al_destroy_bitmap (dv_chopper_fg_05);

  /* palace vga */
  al_destroy_bitmap (pv_chopper_01);
  al_destroy_bitmap (pv_chopper_fg_01);
  al_destroy_bitmap (pv_chopper_02);
  al_destroy_bitmap (pv_chopper_fg_02);
  al_destroy_bitmap (pv_chopper_03);
  al_destroy_bitmap (pv_chopper_fg_03);
  al_destroy_bitmap (pv_chopper_04);
  al_destroy_bitmap (pv_chopper_fg_04);
  al_destroy_bitmap (pv_chopper_05);
  al_destroy_bitmap (pv_chopper_fg_05);

  /* palettable */
  al_destroy_bitmap (chopper_blood_01);
  al_destroy_bitmap (chopper_blood_02);
  al_destroy_bitmap (chopper_blood_03);
  al_destroy_bitmap (chopper_blood_04);
  al_destroy_bitmap (chopper_blood_05);
}

void
load_chopper_samples (void)
{
  chopper_sample = load_sample (CHOPPER_SAMPLE);
}

void
unload_chopper_samples (void)
{
  al_destroy_sample (chopper_sample);
}

void
register_chopper (struct pos *p)
{
  struct chopper c;

  int step = con (p)->ext.step & 0x7F;

  c.p = *p;
  c.i = step < 5 ? step : 0;
  c.wait = CHOPPER_WAIT;
  c.blood = con (p)->ext.step & 0x80;
  c.inactive = (step >= 5);

  chopper =
    add_to_array (&c, 1, chopper, &chopper_nmemb, chopper_nmemb, sizeof (c));

  qsort (chopper, chopper_nmemb, sizeof (c), compare_choppers);
}

int
compare_choppers (const void *c0, const void *c1)
{
  return cpos (&((struct chopper *) c0)->p, &((struct chopper *) c1)->p);
}

struct chopper *
chopper_at_pos (struct pos *p)
{
  struct chopper c;
  c.p = *p;
  return bsearch (&c, chopper, chopper_nmemb, sizeof (c), compare_choppers);
}

bool
should_chomp (struct pos *p)
{
  int i;
  struct coord nc;
  struct pos np, pm, _p;

  for (i = 0; i < anima_nmemb; i++) {
    struct anim *a = &anima[i];
    if (is_anim_dead (a) && ! a->id == 0) continue;
    survey (_m, pos, &a->f, &nc, &pm, &np);
    int inc = p->place < pm.place ? +1 : -1;
    if (p->room == pm.room && p->floor == pm.floor) {
      for (_p = *p; _p.place != pm.place; _p.place += inc)
        if (con (&_p)->fg == WALL) return false;
      return true;
    }
  }

  return false;
}

void
compute_choppers (void)
{
  size_t i, j;

  for (i = 0; i < chopper_nmemb; i++) {
    struct chopper *c = &chopper[i];

    if (c->inactive) continue;

    if (c->p.room == -1) {
      /* remove_chopper (d); i--; */
      continue;
    }
    switch (c->i) {
    case 0:
      if ((c->wait-- <= 0 && should_chomp (&c->p)
           && (anim_cycle % CHOPPER_WAIT) ==
           prandom_pos (&c->p, CHOPPER_WAIT - 1))
          || c->activate) c->i++; break;
    case 1: c->i++; play_sample (chopper_sample, c->p.room); break;
    case 2: c->i++; break;
    case 3: c->i++; break;
    case 4: c->i = 0; c->wait = CHOPPER_WAIT; c->activate = false;
      break;
    }

    if (c->i != 1 && c->i != 2 ) continue;

    /* chomp kid */
    for (j = 0; j < anima_nmemb; j++) {
      struct anim *a = &anima[j];
      if (is_anim_dead (a)
          || is_anim_fall (a)
          || a->immortal
          || a->chopper_immune) continue;
      struct coord nc; struct pos np, pbf, pbb;
      survey (_bf, pos, &a->f, &nc, &pbf, &np);
      survey (_bb, pos, &a->f, &nc, &pbb, &np);
      pos2room (&pbf, c->p.room, &pbf);
      pos2room (&pbb, c->p.room, &pbb);
      if (((pbf.room == c->p.room
            && pbf.floor == c->p.floor)
           || (pbb.room == c->p.room
               && pbb.floor == c->p.floor))
          && ((a->f.dir == LEFT && pbf.place < c->p.place
               && pbb.place >= c->p.place)
              || (a->f.dir == RIGHT && pbf.place >= c->p.place
                  && pbb.place < c->p.place))) {
        c->blood = true;
        a->splash = true;
        a->current_lives = 0;
        a->p = c->p;
        video_effect.color = get_flicker_blood_color ();
        start_video_effect (VIDEO_FLICKERING, SECS_TO_VCYCLES (0.1));
        play_sample (chopped_sample, c->p.room);
        anim_die_chopped (a);
      }
    }
  }
}

void
draw_chopper_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   enum em em, enum vm vm)
{
  struct chopper *ch = chopper_at_pos (p);
  if (! ch) return;

  switch (ch->i) {
  case 0: draw_left_01 (bitmap, p, ch, em, vm); break;
  case 1: draw_left_02 (bitmap, p, ch, em, vm); break;
  case 2: draw_left_03 (bitmap, p, ch, em, vm); break;
  case 3: draw_left_04 (bitmap, p, ch, em, vm); break;
  case 4: draw_left_05 (bitmap, p, ch, em, vm); break;
  }
}

palette
get_blood_palette (enum vm vm)
{
  switch (vm) {
  case CGA: return c_blood_palette;
  case EGA: return e_blood_palette;
  case VGA: return v_blood_palette;
  }
  return NULL;
}

static void
draw_left_01 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
              enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *chopper = NULL;
  palette blood_palette = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: chopper = dc_chopper_01; break;
    case EGA: chopper = de_chopper_01; break;
    case VGA: chopper = dv_chopper_01; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: chopper = pc_chopper_01; break;
    case EGA: chopper = pe_chopper_01; break;
    case VGA: chopper = pv_chopper_01; break;
    }
    break;
  }

  if (hgc) chopper = apply_palette (chopper, hgc_palette);

  struct coord c;
  draw_bitmapc (chopper, bitmap, chopper_coord (p, &c), 0);
  if (ch->blood) {
    blood_palette = get_blood_palette (vm);
    ALLEGRO_BITMAP *blood = apply_palette (chopper_blood_01, blood_palette);
    if (hgc) blood = apply_palette (blood, hgc_palette);
    draw_bitmapc (blood, bitmap, blood_01_coord (p, &c), 0);
  }
}

static void
draw_left_02 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                      enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *chopper = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: chopper = dc_chopper_02; break;
    case EGA: chopper = de_chopper_02; break;
    case VGA: chopper = dv_chopper_02; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: chopper = pc_chopper_02; break;
    case EGA: chopper = pe_chopper_02; break;
    case VGA: chopper = pv_chopper_02; break;
    }
    break;
  }

  if (hgc) chopper = apply_palette (chopper, hgc_palette);

  struct coord c;
  draw_bitmapc (chopper, bitmap, chopper_coord (p, &c), 0);
  if (ch->blood) {
    palette blood_palette = get_blood_palette (vm);
    ALLEGRO_BITMAP *blood = apply_palette (chopper_blood_02, blood_palette);
    if (hgc) blood = apply_palette (blood, hgc_palette);
    draw_bitmapc (blood, bitmap, blood_02_coord (p, &c), 0);
  }
}

static void
draw_left_03 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                      enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *chopper = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: chopper = dc_chopper_03; break;
    case EGA: chopper = de_chopper_03; break;
    case VGA: chopper = dv_chopper_03; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: chopper = pc_chopper_03; break;
    case EGA: chopper = pe_chopper_03; break;
    case VGA: chopper = pv_chopper_03; break;
    }
    break;
  }

  if (hgc) chopper = apply_palette (chopper, hgc_palette);

  struct coord c;
  draw_bitmapc (chopper, bitmap, chopper_coord (p, &c), 0);
  if (ch->blood) {
    palette blood_palette = get_blood_palette (vm);
    ALLEGRO_BITMAP *blood = apply_palette (chopper_blood_03, blood_palette);
    if (hgc) blood = apply_palette (blood, hgc_palette);
    draw_bitmapc (blood, bitmap, blood_03_coord (p, &c), 0);
  }
}

static void
draw_left_04 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                      enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *chopper = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: chopper = dc_chopper_04; break;
    case EGA: chopper = de_chopper_04; break;
    case VGA: chopper = dv_chopper_04; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: chopper = pc_chopper_04; break;
    case EGA: chopper = pe_chopper_04; break;
    case VGA: chopper = pv_chopper_04; break;
    }
    break;
  }

  if (hgc) chopper = apply_palette (chopper, hgc_palette);

  struct coord c;
  draw_bitmapc (chopper, bitmap, chopper_coord (p, &c), 0);
  if (ch->blood) {
    palette blood_palette = get_blood_palette (vm);
    ALLEGRO_BITMAP *blood = apply_palette (chopper_blood_04, blood_palette);
    if (hgc) blood = apply_palette (blood, hgc_palette);
    draw_bitmapc (blood, bitmap, blood_04_coord (p, &c), 0);
  }
}

static void
draw_left_05 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                      enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *chopper = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: chopper = dc_chopper_05; break;
    case EGA: chopper = de_chopper_05; break;
    case VGA: chopper = dv_chopper_05; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: chopper = pc_chopper_05; break;
    case EGA: chopper = pe_chopper_05; break;
    case VGA: chopper = pv_chopper_05; break;
    }
    break;
  }

  if (hgc) chopper = apply_palette (chopper, hgc_palette);

  struct coord c;
  draw_bitmapc (chopper, bitmap, chopper_coord (p, &c), 0);
  if (ch->blood) {
    palette blood_palette = get_blood_palette (vm);
    ALLEGRO_BITMAP *blood = apply_palette (chopper_blood_05, blood_palette);
    if (hgc) blood = apply_palette (blood, hgc_palette);
    draw_bitmapc (blood, bitmap, blood_05_coord (p, &c), 0);
  }
}

void
draw_chopper_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm)
{
  struct chopper *ch = chopper_at_pos (p);
  if (! ch) return;

  switch (ch->i) {
  case 0: draw_fg_01 (bitmap, p, ch, em, vm); break;
  case 1: draw_fg_02 (bitmap, p, ch, em, vm); break;
  case 2: draw_fg_03 (bitmap, p, ch, em, vm); break;
  case 3: draw_fg_04 (bitmap, p, ch, em, vm); break;
  case 4: draw_fg_05 (bitmap, p, ch, em, vm); break;
  }
}

static void
draw_fg_01 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                    enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *fg = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: fg = dc_chopper_fg_01; break;
    case EGA: fg = de_chopper_fg_01; break;
    case VGA: fg = dv_chopper_fg_01; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: fg = pc_chopper_fg_01; break;
    case EGA: fg = pe_chopper_fg_01; break;
    case VGA: fg = pv_chopper_fg_01; break;
    }
    break;
  }

  if (hgc) fg = apply_palette (fg, hgc_palette);

  struct coord c;
  draw_bitmapc (fg, bitmap, chopper_coord (p, &c), 0);
  if (ch->blood) {
    int h = al_get_bitmap_height (chopper_blood_01);
    palette blood_palette = get_blood_palette (vm);
    ALLEGRO_BITMAP *blood = apply_palette (chopper_blood_01, blood_palette);
    if (hgc) blood = apply_palette (blood, hgc_palette);
    draw_bitmap_regionc (blood, bitmap, 0, 0, 2, h, blood_01_coord (p, &c), 0);
  }
}

static void
draw_fg_02 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                    enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *fg = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: fg = dc_chopper_fg_02; break;
    case EGA: fg = de_chopper_fg_02; break;
    case VGA: fg = dv_chopper_fg_02; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: fg = pc_chopper_fg_02; break;
    case EGA: fg = pe_chopper_fg_02; break;
    case VGA: fg = pv_chopper_fg_02; break;
    }
    break;
  }

  if (hgc) fg = apply_palette (fg, hgc_palette);

  struct coord c;
  draw_bitmapc (fg, bitmap, chopper_coord (p, &c), 0);
  if (ch->blood) {
    int h = al_get_bitmap_height (chopper_blood_02);
    palette blood_palette = get_blood_palette (vm);
    ALLEGRO_BITMAP *blood = apply_palette (chopper_blood_02, blood_palette);
    if (hgc) blood = apply_palette (blood, hgc_palette);
    draw_bitmap_regionc (blood, bitmap, 0, 0, 2, h, blood_02_coord (p, &c), 0);
  }
}

static void
draw_fg_03 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                    enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *fg = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: fg = dc_chopper_fg_03; break;
    case EGA: fg = de_chopper_fg_03; break;
    case VGA: fg = dv_chopper_fg_03; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: fg = pc_chopper_fg_03; break;
    case EGA: fg = pe_chopper_fg_03; break;
    case VGA: fg = pv_chopper_fg_03; break;
    }
    break;
  }

  if (hgc) fg = apply_palette (fg, hgc_palette);

  struct coord c;
  draw_bitmapc (fg, bitmap, chopper_coord (p, &c), 0);
  if (ch->blood) {
    int h = al_get_bitmap_height (chopper_blood_03);
    palette blood_palette = get_blood_palette (vm);
    ALLEGRO_BITMAP *blood = apply_palette (chopper_blood_03, blood_palette);
    if (hgc) blood = apply_palette (blood, hgc_palette);
    draw_bitmap_regionc (blood, bitmap, 0, 0, 2, h, blood_03_coord (p, &c), 0);
  }
}

static void
draw_fg_04 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                    enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *fg = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: fg = dc_chopper_fg_04; break;
    case EGA: fg = de_chopper_fg_04; break;
    case VGA: fg = dv_chopper_fg_04; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: fg = pc_chopper_fg_04; break;
    case EGA: fg = pe_chopper_fg_04; break;
    case VGA: fg = pv_chopper_fg_04; break;
    }
    break;
  }

  if (hgc) fg = apply_palette (fg, hgc_palette);

  struct coord c;
  draw_bitmapc (fg, bitmap, chopper_coord (p, &c), 0);
  if (ch->blood) {
    int h = al_get_bitmap_height (chopper_blood_04);
    palette blood_palette = get_blood_palette (vm);
    ALLEGRO_BITMAP *blood = apply_palette (chopper_blood_04, blood_palette);
    if (hgc) blood = apply_palette (blood, hgc_palette);
    draw_bitmap_regionc (blood, bitmap, 0, 0, 2, h, blood_04_coord (p, &c), 0);
  }
}

static void
draw_fg_05 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                    enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *fg = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: fg = dc_chopper_fg_05; break;
    case EGA: fg = de_chopper_fg_05; break;
    case VGA: fg = dv_chopper_fg_05; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: fg = pc_chopper_fg_05; break;
    case EGA: fg = pe_chopper_fg_05; break;
    case VGA: fg = pv_chopper_fg_05; break;
    }
    break;
  }

  if (hgc) fg = apply_palette (fg, hgc_palette);

  struct coord c;
  draw_bitmapc (fg, bitmap, chopper_coord (p, &c), 0);
  if (ch->blood) {
    int h = al_get_bitmap_height (chopper_blood_05);
    palette blood_palette = get_blood_palette (vm);
    ALLEGRO_BITMAP *blood = apply_palette (chopper_blood_05, blood_palette);
    if (hgc) blood = apply_palette (blood, hgc_palette);
    draw_bitmap_regionc (blood, bitmap, 0, 0, 2, h, blood_05_coord (p, &c), 0);
  }
}

struct coord *
chopper_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
blood_01_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + 12;
  c->y = PLACE_HEIGHT * p->floor + 48;
  c->room = p->room;
  return c;
}

struct coord *
blood_02_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + 12;
  c->y = PLACE_HEIGHT * p->floor + 39;
  c->room = p->room;
  return c;
}

struct coord *
blood_03_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + 12;
  c->y = PLACE_HEIGHT * p->floor + 28;
  c->room = p->room;
  return c;
}

struct coord *
blood_04_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + 12;
  c->y = PLACE_HEIGHT * p->floor + 32;
  c->room = p->room;
  return c;
}

struct coord *
blood_05_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + 12;
  c->y = PLACE_HEIGHT * p->floor + 52;
  c->room = p->room;
  return c;
}

ALLEGRO_COLOR
v_blood_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, WHITE)) return V_BLOOD_COLOR_01;
  else return c;
}

ALLEGRO_COLOR
e_blood_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, WHITE)) return E_BLOOD_COLOR_01;
  else return c;
}

ALLEGRO_COLOR
c_blood_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, WHITE)) return C_BLOOD_COLOR;
  else return c;
}
