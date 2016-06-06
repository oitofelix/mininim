/*
  spikes-floor.c -- spikes floor module;

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
ALLEGRO_BITMAP *dc_spikes_floor_left, *dc_spikes_floor_right,
  *dc_spikes_left_00, *dc_spikes_right_00, *dc_spikes_fg_00,
  *dc_spikes_left_01, *dc_spikes_right_01, *dc_spikes_fg_01,
  *dc_spikes_left_02, *dc_spikes_right_02, *dc_spikes_fg_02,
  *dc_spikes_left_03, *dc_spikes_right_03, *dc_spikes_fg_03,
  *dc_spikes_left_04, *dc_spikes_right_04, *dc_spikes_fg_04;

/* palace cga */
ALLEGRO_BITMAP *pc_spikes_floor_left, *pc_spikes_floor_right,
  *pc_spikes_left_00, *pc_spikes_right_00, *pc_spikes_fg_00,
  *pc_spikes_left_01, *pc_spikes_right_01, *pc_spikes_fg_01,
  *pc_spikes_left_02, *pc_spikes_right_02, *pc_spikes_fg_02,
  *pc_spikes_left_03, *pc_spikes_right_03, *pc_spikes_fg_03,
  *pc_spikes_left_04, *pc_spikes_right_04, *pc_spikes_fg_04;

/* dungeon ega */
ALLEGRO_BITMAP *de_spikes_floor_left, *de_spikes_floor_right,
  *de_spikes_left_00, *de_spikes_right_00, *de_spikes_fg_00,
  *de_spikes_left_01, *de_spikes_right_01, *de_spikes_fg_01,
  *de_spikes_left_02, *de_spikes_right_02, *de_spikes_fg_02,
  *de_spikes_left_03, *de_spikes_right_03, *de_spikes_fg_03,
  *de_spikes_left_04, *de_spikes_right_04, *de_spikes_fg_04;

/* palace ega */
ALLEGRO_BITMAP *pe_spikes_floor_left, *pe_spikes_floor_right,
  *pe_spikes_left_00, *pe_spikes_right_00, *pe_spikes_fg_00,
  *pe_spikes_left_01, *pe_spikes_right_01, *pe_spikes_fg_01,
  *pe_spikes_left_02, *pe_spikes_right_02, *pe_spikes_fg_02,
  *pe_spikes_left_03, *pe_spikes_right_03, *pe_spikes_fg_03,
  *pe_spikes_left_04, *pe_spikes_right_04, *pe_spikes_fg_04;

/* dungeon vga */
ALLEGRO_BITMAP *dv_spikes_floor_left, *dv_spikes_floor_right,
  *dv_spikes_left_00, *dv_spikes_right_00, *dv_spikes_fg_00,
  *dv_spikes_left_01, *dv_spikes_right_01, *dv_spikes_fg_01,
  *dv_spikes_left_02, *dv_spikes_right_02, *dv_spikes_fg_02,
  *dv_spikes_left_03, *dv_spikes_right_03, *dv_spikes_fg_03,
  *dv_spikes_left_04, *dv_spikes_right_04, *dv_spikes_fg_04;

/* palace vga */
ALLEGRO_BITMAP *pv_spikes_floor_left, *pv_spikes_floor_right,
  *pv_spikes_left_00, *pv_spikes_right_00, *pv_spikes_fg_00,
  *pv_spikes_left_01, *pv_spikes_right_01, *pv_spikes_fg_01,
  *pv_spikes_left_02, *pv_spikes_right_02, *pv_spikes_fg_02,
  *pv_spikes_left_03, *pv_spikes_right_03, *pv_spikes_fg_03,
  *pv_spikes_left_04, *pv_spikes_right_04, *pv_spikes_fg_04;

struct spikes_floor *spikes_floor = NULL;
size_t spikes_floor_nmemb = 0;

void
load_spikes_floor (void)
{
  /* dungeon cga */
  dc_spikes_floor_left = load_bitmap (DC_SPIKES_FLOOR_LEFT);
  dc_spikes_floor_right = load_bitmap (DC_SPIKES_FLOOR_RIGHT);
  dc_spikes_left_00 = load_bitmap (DC_SPIKES_LEFT_00);
  dc_spikes_right_00 = load_bitmap (DC_SPIKES_RIGHT_00);
  dc_spikes_fg_00 = load_bitmap (DC_SPIKES_FG_00);
  dc_spikes_left_01 = load_bitmap (DC_SPIKES_LEFT_01);
  dc_spikes_right_01 = load_bitmap (DC_SPIKES_RIGHT_01);
  dc_spikes_fg_01 = load_bitmap (DC_SPIKES_FG_01);
  dc_spikes_left_02 = load_bitmap (DC_SPIKES_LEFT_02);
  dc_spikes_right_02 = load_bitmap (DC_SPIKES_RIGHT_02);
  dc_spikes_fg_02 = load_bitmap (DC_SPIKES_FG_02);
  dc_spikes_left_03 = load_bitmap (DC_SPIKES_LEFT_03);
  dc_spikes_right_03 = load_bitmap (DC_SPIKES_RIGHT_03);
  dc_spikes_fg_03 = load_bitmap (DC_SPIKES_FG_03);
  dc_spikes_left_04 = load_bitmap (DC_SPIKES_LEFT_04);
  dc_spikes_right_04 = load_bitmap (DC_SPIKES_RIGHT_04);
  dc_spikes_fg_04 = load_bitmap (DC_SPIKES_FG_04);

  /* palace cga */
  pc_spikes_floor_left = load_bitmap (PC_SPIKES_FLOOR_LEFT);
  pc_spikes_floor_right = load_bitmap (PC_SPIKES_FLOOR_RIGHT);
  pc_spikes_left_00 = load_bitmap (PC_SPIKES_LEFT_00);
  pc_spikes_right_00 = load_bitmap (PC_SPIKES_RIGHT_00);
  pc_spikes_fg_00 = load_bitmap (PC_SPIKES_FG_00);
  pc_spikes_left_01 = load_bitmap (PC_SPIKES_LEFT_01);
  pc_spikes_right_01 = load_bitmap (PC_SPIKES_RIGHT_01);
  pc_spikes_fg_01 = load_bitmap (PC_SPIKES_FG_01);
  pc_spikes_left_02 = load_bitmap (PC_SPIKES_LEFT_02);
  pc_spikes_right_02 = load_bitmap (PC_SPIKES_RIGHT_02);
  pc_spikes_fg_02 = load_bitmap (PC_SPIKES_FG_02);
  pc_spikes_left_03 = load_bitmap (PC_SPIKES_LEFT_03);
  pc_spikes_right_03 = load_bitmap (PC_SPIKES_RIGHT_03);
  pc_spikes_fg_03 = load_bitmap (PC_SPIKES_FG_03);
  pc_spikes_left_04 = load_bitmap (PC_SPIKES_LEFT_04);
  pc_spikes_right_04 = load_bitmap (PC_SPIKES_RIGHT_04);
  pc_spikes_fg_04 = load_bitmap (PC_SPIKES_FG_04);

  /* dungeon ega */
  de_spikes_floor_left = load_bitmap (DE_SPIKES_FLOOR_LEFT);
  de_spikes_floor_right = load_bitmap (DE_SPIKES_FLOOR_RIGHT);
  de_spikes_left_00 = load_bitmap (DE_SPIKES_LEFT_00);
  de_spikes_right_00 = load_bitmap (DE_SPIKES_RIGHT_00);
  de_spikes_fg_00 = load_bitmap (DE_SPIKES_FG_00);
  de_spikes_left_01 = load_bitmap (DE_SPIKES_LEFT_01);
  de_spikes_right_01 = load_bitmap (DE_SPIKES_RIGHT_01);
  de_spikes_fg_01 = load_bitmap (DE_SPIKES_FG_01);
  de_spikes_left_02 = load_bitmap (DE_SPIKES_LEFT_02);
  de_spikes_right_02 = load_bitmap (DE_SPIKES_RIGHT_02);
  de_spikes_fg_02 = load_bitmap (DE_SPIKES_FG_02);
  de_spikes_left_03 = load_bitmap (DE_SPIKES_LEFT_03);
  de_spikes_right_03 = load_bitmap (DE_SPIKES_RIGHT_03);
  de_spikes_fg_03 = load_bitmap (DE_SPIKES_FG_03);
  de_spikes_left_04 = load_bitmap (DE_SPIKES_LEFT_04);
  de_spikes_right_04 = load_bitmap (DE_SPIKES_RIGHT_04);
  de_spikes_fg_04 = load_bitmap (DE_SPIKES_FG_04);

  /* palace ega */
  pe_spikes_floor_left = load_bitmap (PE_SPIKES_FLOOR_LEFT);
  pe_spikes_floor_right = load_bitmap (PE_SPIKES_FLOOR_RIGHT);
  pe_spikes_left_00 = load_bitmap (PE_SPIKES_LEFT_00);
  pe_spikes_right_00 = load_bitmap (PE_SPIKES_RIGHT_00);
  pe_spikes_fg_00 = load_bitmap (PE_SPIKES_FG_00);
  pe_spikes_left_01 = load_bitmap (PE_SPIKES_LEFT_01);
  pe_spikes_right_01 = load_bitmap (PE_SPIKES_RIGHT_01);
  pe_spikes_fg_01 = load_bitmap (PE_SPIKES_FG_01);
  pe_spikes_left_02 = load_bitmap (PE_SPIKES_LEFT_02);
  pe_spikes_right_02 = load_bitmap (PE_SPIKES_RIGHT_02);
  pe_spikes_fg_02 = load_bitmap (PE_SPIKES_FG_02);
  pe_spikes_left_03 = load_bitmap (PE_SPIKES_LEFT_03);
  pe_spikes_right_03 = load_bitmap (PE_SPIKES_RIGHT_03);
  pe_spikes_fg_03 = load_bitmap (PE_SPIKES_FG_03);
  pe_spikes_left_04 = load_bitmap (PE_SPIKES_LEFT_04);
  pe_spikes_right_04 = load_bitmap (PE_SPIKES_RIGHT_04);
  pe_spikes_fg_04 = load_bitmap (PE_SPIKES_FG_04);

  /* dungeon vga */
  dv_spikes_floor_left = load_bitmap (DV_SPIKES_FLOOR_LEFT);
  dv_spikes_floor_right = load_bitmap (DV_SPIKES_FLOOR_RIGHT);
  dv_spikes_left_00 = load_bitmap (DV_SPIKES_LEFT_00);
  dv_spikes_right_00 = load_bitmap (DV_SPIKES_RIGHT_00);
  dv_spikes_fg_00 = load_bitmap (DV_SPIKES_FG_00);
  dv_spikes_left_01 = load_bitmap (DV_SPIKES_LEFT_01);
  dv_spikes_right_01 = load_bitmap (DV_SPIKES_RIGHT_01);
  dv_spikes_fg_01 = load_bitmap (DV_SPIKES_FG_01);
  dv_spikes_left_02 = load_bitmap (DV_SPIKES_LEFT_02);
  dv_spikes_right_02 = load_bitmap (DV_SPIKES_RIGHT_02);
  dv_spikes_fg_02 = load_bitmap (DV_SPIKES_FG_02);
  dv_spikes_left_03 = load_bitmap (DV_SPIKES_LEFT_03);
  dv_spikes_right_03 = load_bitmap (DV_SPIKES_RIGHT_03);
  dv_spikes_fg_03 = load_bitmap (DV_SPIKES_FG_03);
  dv_spikes_left_04 = load_bitmap (DV_SPIKES_LEFT_04);
  dv_spikes_right_04 = load_bitmap (DV_SPIKES_RIGHT_04);
  dv_spikes_fg_04 = load_bitmap (DV_SPIKES_FG_04);

  /* palace vga */
  pv_spikes_floor_left = load_bitmap (PV_SPIKES_FLOOR_LEFT);
  pv_spikes_floor_right = load_bitmap (PV_SPIKES_FLOOR_RIGHT);
  pv_spikes_left_00 = load_bitmap (PV_SPIKES_LEFT_00);
  pv_spikes_right_00 = load_bitmap (PV_SPIKES_RIGHT_00);
  pv_spikes_fg_00 = load_bitmap (PV_SPIKES_FG_00);
  pv_spikes_left_01 = load_bitmap (PV_SPIKES_LEFT_01);
  pv_spikes_right_01 = load_bitmap (PV_SPIKES_RIGHT_01);
  pv_spikes_fg_01 = load_bitmap (PV_SPIKES_FG_01);
  pv_spikes_left_02 = load_bitmap (PV_SPIKES_LEFT_02);
  pv_spikes_right_02 = load_bitmap (PV_SPIKES_RIGHT_02);
  pv_spikes_fg_02 = load_bitmap (PV_SPIKES_FG_02);
  pv_spikes_left_03 = load_bitmap (PV_SPIKES_LEFT_03);
  pv_spikes_right_03 = load_bitmap (PV_SPIKES_RIGHT_03);
  pv_spikes_fg_03 = load_bitmap (PV_SPIKES_FG_03);
  pv_spikes_left_04 = load_bitmap (PV_SPIKES_LEFT_04);
  pv_spikes_right_04 = load_bitmap (PV_SPIKES_RIGHT_04);
  pv_spikes_fg_04 = load_bitmap (PV_SPIKES_FG_04);
}

void
unload_spikes_floor (void)
{
  /* dungeon cga */
  destroy_bitmap (dc_spikes_floor_left);
  destroy_bitmap (dc_spikes_floor_right);
  destroy_bitmap (dc_spikes_left_00);
  destroy_bitmap (dc_spikes_right_00);
  destroy_bitmap (dc_spikes_fg_00);
  destroy_bitmap (dc_spikes_left_01);
  destroy_bitmap (dc_spikes_right_01);
  destroy_bitmap (dc_spikes_fg_01);
  destroy_bitmap (dc_spikes_left_02);
  destroy_bitmap (dc_spikes_right_02);
  destroy_bitmap (dc_spikes_fg_02);
  destroy_bitmap (dc_spikes_left_03);
  destroy_bitmap (dc_spikes_right_03);
  destroy_bitmap (dc_spikes_fg_03);
  destroy_bitmap (dc_spikes_left_04);
  destroy_bitmap (dc_spikes_right_04);
  destroy_bitmap (dc_spikes_fg_04);

  /* palace cga */
  destroy_bitmap (pc_spikes_floor_left);
  destroy_bitmap (pc_spikes_floor_right);
  destroy_bitmap (pc_spikes_left_00);
  destroy_bitmap (pc_spikes_right_00);
  destroy_bitmap (pc_spikes_fg_00);
  destroy_bitmap (pc_spikes_left_01);
  destroy_bitmap (pc_spikes_right_01);
  destroy_bitmap (pc_spikes_fg_01);
  destroy_bitmap (pc_spikes_left_02);
  destroy_bitmap (pc_spikes_right_02);
  destroy_bitmap (pc_spikes_fg_02);
  destroy_bitmap (pc_spikes_left_03);
  destroy_bitmap (pc_spikes_right_03);
  destroy_bitmap (pc_spikes_fg_03);
  destroy_bitmap (pc_spikes_left_04);
  destroy_bitmap (pc_spikes_right_04);
  destroy_bitmap (pc_spikes_fg_04);

  /* dungeon ega */
  destroy_bitmap (de_spikes_floor_left);
  destroy_bitmap (de_spikes_floor_right);
  destroy_bitmap (de_spikes_left_00);
  destroy_bitmap (de_spikes_right_00);
  destroy_bitmap (de_spikes_fg_00);
  destroy_bitmap (de_spikes_left_01);
  destroy_bitmap (de_spikes_right_01);
  destroy_bitmap (de_spikes_fg_01);
  destroy_bitmap (de_spikes_left_02);
  destroy_bitmap (de_spikes_right_02);
  destroy_bitmap (de_spikes_fg_02);
  destroy_bitmap (de_spikes_left_03);
  destroy_bitmap (de_spikes_right_03);
  destroy_bitmap (de_spikes_fg_03);
  destroy_bitmap (de_spikes_left_04);
  destroy_bitmap (de_spikes_right_04);
  destroy_bitmap (de_spikes_fg_04);

  /* palace ega */
  destroy_bitmap (pe_spikes_floor_left);
  destroy_bitmap (pe_spikes_floor_right);
  destroy_bitmap (pe_spikes_left_00);
  destroy_bitmap (pe_spikes_right_00);
  destroy_bitmap (pe_spikes_fg_00);
  destroy_bitmap (pe_spikes_left_01);
  destroy_bitmap (pe_spikes_right_01);
  destroy_bitmap (pe_spikes_fg_01);
  destroy_bitmap (pe_spikes_left_02);
  destroy_bitmap (pe_spikes_right_02);
  destroy_bitmap (pe_spikes_fg_02);
  destroy_bitmap (pe_spikes_left_03);
  destroy_bitmap (pe_spikes_right_03);
  destroy_bitmap (pe_spikes_fg_03);
  destroy_bitmap (pe_spikes_left_04);
  destroy_bitmap (pe_spikes_right_04);
  destroy_bitmap (pe_spikes_fg_04);

  /* dungeon vga */
  destroy_bitmap (dv_spikes_floor_left);
  destroy_bitmap (dv_spikes_floor_right);
  destroy_bitmap (dv_spikes_left_00);
  destroy_bitmap (dv_spikes_right_00);
  destroy_bitmap (dv_spikes_fg_00);
  destroy_bitmap (dv_spikes_left_01);
  destroy_bitmap (dv_spikes_right_01);
  destroy_bitmap (dv_spikes_fg_01);
  destroy_bitmap (dv_spikes_left_02);
  destroy_bitmap (dv_spikes_right_02);
  destroy_bitmap (dv_spikes_fg_02);
  destroy_bitmap (dv_spikes_left_03);
  destroy_bitmap (dv_spikes_right_03);
  destroy_bitmap (dv_spikes_fg_03);
  destroy_bitmap (dv_spikes_left_04);
  destroy_bitmap (dv_spikes_right_04);
  destroy_bitmap (dv_spikes_fg_04);

  /* palace vga */
  destroy_bitmap (pv_spikes_floor_left);
  destroy_bitmap (pv_spikes_floor_right);
  destroy_bitmap (pv_spikes_left_00);
  destroy_bitmap (pv_spikes_right_00);
  destroy_bitmap (pv_spikes_fg_00);
  destroy_bitmap (pv_spikes_left_01);
  destroy_bitmap (pv_spikes_right_01);
  destroy_bitmap (pv_spikes_fg_01);
  destroy_bitmap (pv_spikes_left_02);
  destroy_bitmap (pv_spikes_right_02);
  destroy_bitmap (pv_spikes_fg_02);
  destroy_bitmap (pv_spikes_left_03);
  destroy_bitmap (pv_spikes_right_03);
  destroy_bitmap (pv_spikes_fg_03);
  destroy_bitmap (pv_spikes_left_04);
  destroy_bitmap (pv_spikes_right_04);
  destroy_bitmap (pv_spikes_fg_04);
}

void
register_spikes_floor (struct pos *p)
{
  assert (con (p)->fg == SPIKES_FLOOR
          && spikes_floor_at_pos (p) == NULL);

  struct spikes_floor s;

  s.p = *p;
  s.wait = SPIKES_WAIT;
  s.murdered_anim = -1;
  s.activate = false;

  switch (con (p)->ext.step) {
  case 0: s.i = 0; s.state = 0; break;
  case 1: s.i = 1; s.state = 1; break;
  case 2: s.i = 2; s.state = 2; break;
  case 3: s.i = 3; s.state = 3; break;
  case 4: s.i = 4; s.state = 4; break;
  case 5: s.i = 4; s.state = 5; break;
  case 6: s.i = 5; s.state = 3; break;
  case 7: s.i = 6; s.state = 2; break;
  case 8: s.i = 0; s.state = 1; break;
  case 9: s.i = 0; s.state = 0; break;
  }

  s.inactive = (con (p)->ext.step != 0);

  spikes_floor =
    add_to_array (&s, 1, spikes_floor, &spikes_floor_nmemb,
                  spikes_floor_nmemb, sizeof (s));

  sort_spikes_floors ();
}

void
sort_spikes_floors (void)
{
  qsort (spikes_floor, spikes_floor_nmemb,
         sizeof (struct spikes_floor), compare_spikes_floors);
}

int
compare_spikes_floors (const void *s0, const void *s1)
{
  return cpos (&((struct spikes_floor *) s0)->p,
               &((struct spikes_floor *) s1)->p);
}

struct spikes_floor *
spikes_floor_at_pos (struct pos *p)
{
  struct spikes_floor s;
  s.p = *p;

  return bsearch (&s, spikes_floor, spikes_floor_nmemb, sizeof (s),
                  compare_spikes_floors);
}

void
remove_spikes_floor (struct spikes_floor *s)
{
  size_t i =  s - spikes_floor;
  spikes_floor =
    remove_from_array (spikes_floor, &spikes_floor_nmemb, i, 1, sizeof (*s));
}

void
break_spikes_floor (struct pos *p)
{
  struct spikes_floor *s = spikes_floor_at_pos (p);
  if (! s) return;
  if (s->murdered_anim != -1)
    anim_die_suddenly (get_anim_by_id (s->murdered_anim));
  remove_spikes_floor (s);
}

void
reset_murder_spikes_floor (int id) {
  int i = 0;
  for (i = 0; i < spikes_floor_nmemb; i++) {
    struct spikes_floor *s = &spikes_floor[i];
    if (s->murdered_anim == id) {
      s->murdered_anim = -1;
      s->inactive = false;
    }
  }
}

void
compute_spikes_floors (void)
{
  size_t i, j;
  struct coord nc; struct pos pm, np;

  for (i = 0; i < spikes_floor_nmemb; i++) {
    struct spikes_floor *s = &spikes_floor[i];

    if (s->inactive) continue;

    if (s->p.room == -1) {
      /* remove_spikes_floor (s); i--; */
      continue;
    }

    int state = s->state;

    switch (s->i) {
    case 0: if (should_spikes_raise (&s->p) || s->activate) {
        alert_guards (&s->p);
        play_sample (spikes_sample, &s->p, -1);
        s->i++;
        s->wait = 12;
        s->state = 1;
      } else if (s->state != 0) s->state = 0;
      break;
    case 1:
      s->i++;
      s->state = 2;
      break;
    case 2:
      s->i++;
      s->state = 3;
      break;
    case 3:
      s->i++;
      s->state = 4;
      break;
    case 4: if (! should_spikes_raise (&s->p)) {
        if (s->wait-- == 0) {
          s->i++;
          s->state = 3;
        } else s->state = 5;
      } else {
        s->state = 5;
      }
      break;
    case 5:
      s->i++;
      s->state = 2;
      break;
    case 6:
      s->i = 0;
      s->state = 1;
      s->activate = false;
      break;
    }

    if (state != s->state)
      register_changed_pos (&s->p, CHPOS_SPIKES);

    /* spike kid */
    for (j = 0; j < anima_nmemb; j++) {
      struct anim *a = &anima[j];
      if (is_kid_dead (&a->f)
          || a->immortal
          || a->spikes_immune) continue;
      survey (_m, pos, &a->f, &nc, &pm, &np);
      if (peq (&pm, &s->p)
          && (((s->state >= 2 && s->state <= 4)
               && (is_kid_start_run (&a->f)
                   || is_kid_run (&a->f)
                   || is_kid_stop_run (&a->f)
                   || is_kid_run_jump_running (&a->f)))
              || (is_kid_couch (&a->f) && a->fall && a->i < 3
                  && ! a->float_timer)
              || (is_kid_jump_landing (&a->f) && a->i <= 13)
              || is_kid_run_jump_landing (&a->f))) {
        a->p = s->p;
        anim_die_spiked (a);
      }
    }
  }
}

bool
should_spikes_raise_for_pos (struct pos *p, struct pos *pk)
{
  struct pos pb1, pb2, np;

  return peq (pk, p)
    || (peq (pk, prel (p, &pb1, -1, 0))
        && is_strictly_traversable (prel (p, &np, -1, +0)))
    || (peq (pk, prel (p, &pb2, -2, 0))
        && is_strictly_traversable (prel (p, &np, -1, +0))
        && is_strictly_traversable (prel (p, &np, -2, +0)));
}

bool
should_spikes_raise (struct pos *p)
{
  int i;
  struct coord nc;
  struct pos np, pmf, pm, pmba;

  for (i = 0; i < anima_nmemb; i++) {
    struct anim *a = &anima[i];
    if (is_anim_dead (&a->f)) continue;
    survey (_mf, pos, &a->f, &nc, &pmf, &np);
    survey (_m, pos, &a->f, &nc, &pm, &np);
    survey (_mba, pos, &a->f, &nc, &pmba, &np);

    if (should_spikes_raise_for_pos (p, &pmf)
        || should_spikes_raise_for_pos (p, &pm)
        || should_spikes_raise_for_pos (p, &pmba))
      return true;
  }

  return false;
}

void
draw_spikes_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   enum em em, enum vm vm)
{
  struct spikes_floor *s = spikes_floor_at_pos (p);
  if (! s) return;

  draw_spikes_floor_floor (bitmap, p, em, vm);
  draw_spikes (bitmap, p, s, em, vm);
}

void
draw_spikes_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm)
{
  struct spikes_floor *s = spikes_floor_at_pos (p);
  if (! s) return;

  draw_spikes_floor_floor_left (bitmap, p, em, vm);
  draw_spikes_left (bitmap, p, s, em, vm);
}

void
draw_spikes_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                         enum em em, enum vm vm)
{
  struct spikes_floor *s = spikes_floor_at_pos (p);
  if (! s) return;

  draw_spikes_floor_floor_right (bitmap, p, em, vm);
  draw_spikes_right (bitmap, p, s, em, vm);
}

void
draw_spikes_floor_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                         enum em em, enum vm vm)
{
  draw_floor_base (bitmap, p, em, vm);
  draw_spikes_floor_floor_left (bitmap, p, em, vm);
  draw_spikes_floor_floor_right (bitmap, p, em, vm);
}

void
draw_spikes_floor_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                              enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *spikes_floor_left = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: spikes_floor_left = dc_spikes_floor_left; break;
    case EGA: spikes_floor_left = de_spikes_floor_left; break;
    case VGA: spikes_floor_left = dv_spikes_floor_left; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: spikes_floor_left = pc_spikes_floor_left; break;
    case EGA: spikes_floor_left = pe_spikes_floor_left; break;
    case VGA: spikes_floor_left = pv_spikes_floor_left; break;
    }
    break;
  }

  if (vm == VGA) spikes_floor_left = apply_hue_palette (spikes_floor_left);
  if (hgc) spikes_floor_left = apply_palette (spikes_floor_left, hgc_palette);
  if (peq (p, &mouse_pos))
    spikes_floor_left = apply_palette (spikes_floor_left, selection_palette);

  struct coord c;
  draw_bitmapc (spikes_floor_left, bitmap, floor_left_coord (p, &c), 0);
}

void
draw_spikes_floor_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                               enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *spikes_floor_right = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: spikes_floor_right = dc_spikes_floor_right; break;
    case EGA: spikes_floor_right = de_spikes_floor_right; break;
    case VGA: spikes_floor_right = dv_spikes_floor_right; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: spikes_floor_right = pc_spikes_floor_right; break;
    case EGA: spikes_floor_right = pe_spikes_floor_right; break;
    case VGA: spikes_floor_right = pv_spikes_floor_right; break;
    }
    break;
  }

  if (vm == VGA) spikes_floor_right = apply_hue_palette (spikes_floor_right);
  if (hgc) spikes_floor_right = apply_palette (spikes_floor_right, hgc_palette);
  if (peq (p, &mouse_pos))
    spikes_floor_right = apply_palette (spikes_floor_right, selection_palette);

  struct coord c;
  draw_bitmapc (spikes_floor_right, bitmap, floor_right_coord (p, &c), 0);
}

void
draw_spikes (ALLEGRO_BITMAP *bitmap, struct pos *p,
             struct spikes_floor *s, enum em em, enum vm vm)
{
  draw_spikes_left (bitmap, p, s, em, vm);
  draw_spikes_right (bitmap, p, s, em, vm);
}

void
draw_spikes_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                  struct spikes_floor *s, enum em em, enum vm vm)
{
  switch (s->state) {
  case 0: break;
  case 1: draw_spikes_left_00 (bitmap, p, em, vm); break;
  case 2: draw_spikes_left_01 (bitmap, p, em, vm); break;
  case 3: draw_spikes_left_02 (bitmap, p, em, vm); break;
  case 4: draw_spikes_left_03 (bitmap, p, em, vm); break;
  case 5: draw_spikes_left_04 (bitmap, p, em, vm); break;
  }
}

void
draw_spikes_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   struct spikes_floor *s, enum em em, enum vm vm)
{
  switch (s->state) {
  case 0: break;
  case 1: draw_spikes_right_00 (bitmap, p, em, vm); break;
  case 2: draw_spikes_right_01 (bitmap, p, em, vm); break;
  case 3: draw_spikes_right_02 (bitmap, p, em, vm); break;
  case 4: draw_spikes_right_03 (bitmap, p, em, vm); break;
  case 5: draw_spikes_right_04 (bitmap, p, em, vm); break;
  }
}

void
draw_spikes_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  struct spikes_floor *s = spikes_floor_at_pos (p);
  if (! s) return;

  switch (s->state) {
  case 0: break;
  case 1: draw_spikes_fg_00 (bitmap, p, em, vm); break;
  case 2: draw_spikes_fg_01 (bitmap, p, em, vm); break;
  case 3: draw_spikes_fg_02 (bitmap, p, em, vm); break;
  case 4: draw_spikes_fg_03 (bitmap, p, em, vm); break;
  case 5: draw_spikes_fg_04 (bitmap, p, em, vm); break;
  }
}

void
draw_spikes_00 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  draw_spikes_left_00 (bitmap, p, em, vm);
  draw_spikes_right_00 (bitmap, p, em, vm);
}

void
draw_spikes_left_00 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *spikes_left_00 = NULL;
  pos2coord_f spikes_left_00_coord = NULL;

  switch (em) {
  case DUNGEON:
    spikes_left_00_coord = d_spikes_left_00_coord;
    switch (vm) {
    case CGA: spikes_left_00 = dc_spikes_left_00; break;
    case EGA: spikes_left_00 = de_spikes_left_00; break;
    case VGA: spikes_left_00 = dv_spikes_left_00; break;
    }
    break;
  case PALACE:
    spikes_left_00_coord = p_spikes_left_00_coord;
    switch (vm) {
    case CGA: spikes_left_00 = pc_spikes_left_00; break;
    case EGA: spikes_left_00 = pe_spikes_left_00; break;
    case VGA: spikes_left_00 = pv_spikes_left_00; break;
    }
    break;
  }

  if (vm == VGA) spikes_left_00 = apply_hue_palette (spikes_left_00);
  if (hgc) spikes_left_00 = apply_palette (spikes_left_00, hgc_palette);
  if (peq (p, &mouse_pos))
    spikes_left_00 = apply_palette (spikes_left_00, selection_palette);

  struct coord c;
  draw_bitmapc (spikes_left_00, bitmap, spikes_left_00_coord (p, &c), 0);
}

void
draw_spikes_right_00 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *spikes_right_00 = NULL;
  pos2coord_f spikes_right_00_coord = NULL;

  switch (em) {
  case DUNGEON:
    spikes_right_00_coord = d_spikes_right_00_coord;
    switch (vm) {
    case CGA: spikes_right_00 = dc_spikes_right_00; break;
    case EGA: spikes_right_00 = de_spikes_right_00; break;
    case VGA: spikes_right_00 = dv_spikes_right_00; break;
    }
    break;
  case PALACE:
    spikes_right_00_coord = p_spikes_right_00_coord;
    switch (vm) {
    case CGA: spikes_right_00 = pc_spikes_right_00; break;
    case EGA: spikes_right_00 = pe_spikes_right_00; break;
    case VGA: spikes_right_00 = pv_spikes_right_00; break;
    }
    break;
  }

  if (vm == VGA) spikes_right_00 = apply_hue_palette (spikes_right_00);
  if (hgc) spikes_right_00 = apply_palette (spikes_right_00, hgc_palette);
  if (peq (p, &mouse_pos))
    spikes_right_00 = apply_palette (spikes_right_00, selection_palette);

  struct coord c;
  draw_bitmapc (spikes_right_00, bitmap, spikes_right_00_coord (p, &c), 0);
}

void
draw_spikes_fg_00 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *spikes_fg_00 = NULL;
  pos2coord_f spikes_fg_00_coord = NULL;

  switch (em) {
  case DUNGEON:
    spikes_fg_00_coord = d_spikes_fg_00_coord;
    switch (vm) {
    case CGA: spikes_fg_00 = dc_spikes_fg_00; break;
    case EGA: spikes_fg_00 = de_spikes_fg_00; break;
    case VGA: spikes_fg_00 = dv_spikes_fg_00; break;
    }
    break;
  case PALACE:
    spikes_fg_00_coord = p_spikes_fg_00_coord;
    switch (vm) {
    case CGA: spikes_fg_00 = pc_spikes_fg_00; break;
    case EGA: spikes_fg_00 = pe_spikes_fg_00; break;
    case VGA: spikes_fg_00 = pv_spikes_fg_00; break;
    }
    break;
  }

  if (vm == VGA) spikes_fg_00 = apply_hue_palette (spikes_fg_00);
  if (hgc) spikes_fg_00 = apply_palette (spikes_fg_00, hgc_palette);
  if (peq (p, &mouse_pos))
    spikes_fg_00 = apply_palette (spikes_fg_00, selection_palette);

  struct coord c;
  draw_bitmapc (spikes_fg_00, bitmap, spikes_fg_00_coord (p, &c), 0);
}

struct coord *
d_spikes_left_00_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 51;
  c->room = p->room;
  return c;
}

struct coord *
p_spikes_left_00_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 49;
  c->room = p->room;
  return c;
}

struct coord *
d_spikes_right_00_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 53;
  c->room = p->room;
  return c;
}

struct coord *
p_spikes_right_00_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 49;
  c->room = p->room;
  return c;
}

struct coord *
d_spikes_fg_00_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 56;
  c->room = p->room;
  return c;
}

struct coord *
p_spikes_fg_00_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 54;
  c->room = p->room;
  return c;
}

void
draw_spikes_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  draw_spikes_left_01 (bitmap, p, em, vm);
  draw_spikes_right_01 (bitmap, p, em, vm);
}

void
draw_spikes_left_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *spikes_left_01 = NULL;
  pos2coord_f spikes_left_01_coord = NULL;

  switch (em) {
  case DUNGEON:
    spikes_left_01_coord = d_spikes_left_01_coord;
    switch (vm) {
    case CGA: spikes_left_01 = dc_spikes_left_01; break;
    case EGA: spikes_left_01 = de_spikes_left_01; break;
    case VGA: spikes_left_01 = dv_spikes_left_01; break;
    }
    break;
  case PALACE:
    spikes_left_01_coord = p_spikes_left_01_coord;
    switch (vm) {
    case CGA: spikes_left_01 = pc_spikes_left_01; break;
    case EGA: spikes_left_01 = pe_spikes_left_01; break;
    case VGA: spikes_left_01 = pv_spikes_left_01; break;
    }
    break;
  }

  if (vm == VGA) spikes_left_01 = apply_hue_palette (spikes_left_01);
  if (hgc) spikes_left_01 = apply_palette (spikes_left_01, hgc_palette);
  if (peq (p, &mouse_pos))
    spikes_left_01 = apply_palette (spikes_left_01, selection_palette);

  struct coord c;
  draw_bitmapc (spikes_left_01, bitmap, spikes_left_01_coord (p, &c), 0);
}

void
draw_spikes_right_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *spikes_right_01 = NULL;
  pos2coord_f spikes_right_01_coord = NULL;

  switch (em) {
  case DUNGEON:
    spikes_right_01_coord = d_spikes_right_01_coord;
    switch (vm) {
    case CGA: spikes_right_01 = dc_spikes_right_01; break;
    case EGA: spikes_right_01 = de_spikes_right_01; break;
    case VGA: spikes_right_01 = dv_spikes_right_01; break;
    }
    break;
  case PALACE:
    spikes_right_01_coord = p_spikes_right_01_coord;
    switch (vm) {
    case CGA: spikes_right_01 = pc_spikes_right_01; break;
    case EGA: spikes_right_01 = pe_spikes_right_01; break;
    case VGA: spikes_right_01 = pv_spikes_right_01; break;
    }
    break;
  }

  if (vm == VGA) spikes_right_01 = apply_hue_palette (spikes_right_01);
  if (hgc) spikes_right_01 = apply_palette (spikes_right_01, hgc_palette);
  if (peq (p, &mouse_pos))
    spikes_right_01 = apply_palette (spikes_right_01, selection_palette);

  struct coord c;
  draw_bitmapc (spikes_right_01, bitmap, spikes_right_01_coord (p, &c), 0);
}

void
draw_spikes_fg_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *spikes_fg_01 = NULL;
  pos2coord_f spikes_fg_01_coord = NULL;

  switch (em) {
  case DUNGEON:
    spikes_fg_01_coord = d_spikes_fg_01_coord;
    switch (vm) {
    case CGA: spikes_fg_01 = dc_spikes_fg_01; break;
    case EGA: spikes_fg_01 = de_spikes_fg_01; break;
    case VGA: spikes_fg_01 = dv_spikes_fg_01; break;
    }
    break;
  case PALACE:
    spikes_fg_01_coord = p_spikes_fg_01_coord;
    switch (vm) {
    case CGA: spikes_fg_01 = pc_spikes_fg_01; break;
    case EGA: spikes_fg_01 = pe_spikes_fg_01; break;
    case VGA: spikes_fg_01 = pv_spikes_fg_01; break;
    }
    break;
  }

  if (vm == VGA) spikes_fg_01 = apply_hue_palette (spikes_fg_01);
  if (hgc) spikes_fg_01 = apply_palette (spikes_fg_01, hgc_palette);
  if (peq (p, &mouse_pos))
    spikes_fg_01 = apply_palette (spikes_fg_01, selection_palette);

  struct coord c;
  draw_bitmapc (spikes_fg_01, bitmap, spikes_fg_01_coord (p, &c), 0);
}

struct coord *
d_spikes_left_01_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 41;
  c->room = p->room;
  return c;
}

struct coord *
p_spikes_left_01_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 42;
  c->room = p->room;
  return c;
}

struct coord *
d_spikes_right_01_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 41;
  c->room = p->room;
  return c;
}

struct coord *
p_spikes_right_01_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 44;
  c->room = p->room;
  return c;
}

struct coord *
d_spikes_fg_01_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 48;
  c->room = p->room;
  return c;
}

struct coord *
p_spikes_fg_01_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 48;
  c->room = p->room;
  return c;
}

void
draw_spikes_02 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  draw_spikes_left_02 (bitmap, p, em, vm);
  draw_spikes_right_02 (bitmap, p, em, vm);
}

void
draw_spikes_left_02 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *spikes_left_02 = NULL;
  pos2coord_f spikes_left_02_coord = NULL;

  switch (em) {
  case DUNGEON:
    spikes_left_02_coord = d_spikes_left_02_coord;
    switch (vm) {
    case CGA: spikes_left_02 = dc_spikes_left_02; break;
    case EGA: spikes_left_02 = de_spikes_left_02; break;
    case VGA: spikes_left_02 = dv_spikes_left_02; break;
    }
    break;
  case PALACE:
    spikes_left_02_coord = p_spikes_left_02_coord;
    switch (vm) {
    case CGA: spikes_left_02 = pc_spikes_left_02; break;
    case EGA: spikes_left_02 = pe_spikes_left_02; break;
    case VGA: spikes_left_02 = pv_spikes_left_02; break;
    }
    break;
  }

  if (vm == VGA) spikes_left_02 = apply_hue_palette (spikes_left_02);
  if (hgc) spikes_left_02 = apply_palette (spikes_left_02, hgc_palette);
  if (peq (p, &mouse_pos))
    spikes_left_02 = apply_palette (spikes_left_02, selection_palette);

  struct coord c;
  draw_bitmapc (spikes_left_02, bitmap, spikes_left_02_coord (p, &c), 0);
}

void
draw_spikes_right_02 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *spikes_right_02 = NULL;
  pos2coord_f spikes_right_02_coord = NULL;

  switch (em) {
  case DUNGEON:
    spikes_right_02_coord = d_spikes_right_02_coord;
    switch (vm) {
    case CGA: spikes_right_02 = dc_spikes_right_02; break;
    case EGA: spikes_right_02 = de_spikes_right_02; break;
    case VGA: spikes_right_02 = dv_spikes_right_02; break;
    }
    break;
  case PALACE:
    spikes_right_02_coord = p_spikes_right_02_coord;
    switch (vm) {
    case CGA: spikes_right_02 = pc_spikes_right_02; break;
    case EGA: spikes_right_02 = pe_spikes_right_02; break;
    case VGA: spikes_right_02 = pv_spikes_right_02; break;
    }
    break;
  }

  if (vm == VGA) spikes_right_02 = apply_hue_palette (spikes_right_02);
  if (hgc) spikes_right_02 = apply_palette (spikes_right_02, hgc_palette);
  if (peq (p, &mouse_pos))
    spikes_right_02 = apply_palette (spikes_right_02, selection_palette);

  struct coord c;
  draw_bitmapc (spikes_right_02, bitmap, spikes_right_02_coord (p, &c), 0);
}

void
draw_spikes_fg_02 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *spikes_fg_02 = NULL;
  pos2coord_f spikes_fg_02_coord = NULL;

  switch (em) {
  case DUNGEON:
    spikes_fg_02_coord = d_spikes_fg_02_coord;
    switch (vm) {
    case CGA: spikes_fg_02 = dc_spikes_fg_02; break;
    case EGA: spikes_fg_02 = de_spikes_fg_02; break;
    case VGA: spikes_fg_02 = dv_spikes_fg_02; break;
    }
    break;
  case PALACE:
    spikes_fg_02_coord = p_spikes_fg_02_coord;
    switch (vm) {
    case CGA: spikes_fg_02 = pc_spikes_fg_02; break;
    case EGA: spikes_fg_02 = pe_spikes_fg_02; break;
    case VGA: spikes_fg_02 = pv_spikes_fg_02; break;
    }
    break;
  }

  if (vm == VGA) spikes_fg_02 = apply_hue_palette (spikes_fg_02);
  if (hgc) spikes_fg_02 = apply_palette (spikes_fg_02, hgc_palette);
  if (peq (p, &mouse_pos))
    spikes_fg_02 = apply_palette (spikes_fg_02, selection_palette);

  struct coord c;
  draw_bitmapc (spikes_fg_02, bitmap, spikes_fg_02_coord (p, &c), 0);
}

struct coord *
d_spikes_left_02_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 34;
  c->room = p->room;
  return c;
}

struct coord *
p_spikes_left_02_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 34;
  c->room = p->room;
  return c;
}

struct coord *
d_spikes_right_02_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 41;
  c->room = p->room;
  return c;
}

struct coord *
p_spikes_right_02_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 41;
  c->room = p->room;
  return c;
}

struct coord *
d_spikes_fg_02_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 37;
  c->room = p->room;
  return c;
}

struct coord *
p_spikes_fg_02_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 37;
  c->room = p->room;
  return c;
}

void
draw_spikes_03 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  draw_spikes_left_03 (bitmap, p, em, vm);
  draw_spikes_right_03 (bitmap, p, em, vm);
}

void
draw_spikes_left_03 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *spikes_left_03 = NULL;
  pos2coord_f spikes_left_03_coord = NULL;

  switch (em) {
  case DUNGEON:
    spikes_left_03_coord = d_spikes_left_03_coord;
    switch (vm) {
    case CGA: spikes_left_03 = dc_spikes_left_03; break;
    case EGA: spikes_left_03 = de_spikes_left_03; break;
    case VGA: spikes_left_03 = dv_spikes_left_03; break;
    }
    break;
  case PALACE:
    spikes_left_03_coord = p_spikes_left_03_coord;
    switch (vm) {
    case CGA: spikes_left_03 = pc_spikes_left_03; break;
    case EGA: spikes_left_03 = pe_spikes_left_03; break;
    case VGA: spikes_left_03 = pv_spikes_left_03; break;
    }
    break;
  }

  if (vm == VGA) spikes_left_03 = apply_hue_palette (spikes_left_03);
  if (hgc) spikes_left_03 = apply_palette (spikes_left_03, hgc_palette);
  if (peq (p, &mouse_pos))
    spikes_left_03 = apply_palette (spikes_left_03, selection_palette);

  struct coord c;
  draw_bitmapc (spikes_left_03, bitmap, spikes_left_03_coord (p, &c), 0);
}

void
draw_spikes_right_03 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *spikes_right_03 = NULL;
  pos2coord_f spikes_right_03_coord = NULL;

  switch (em) {
  case DUNGEON:
    spikes_right_03_coord = d_spikes_right_03_coord;
    switch (vm) {
    case CGA: spikes_right_03 = dc_spikes_right_03; break;
    case EGA: spikes_right_03 = de_spikes_right_03; break;
    case VGA: spikes_right_03 = dv_spikes_right_03; break;
    }
    break;
  case PALACE:
    spikes_right_03_coord = p_spikes_right_03_coord;
    switch (vm) {
    case CGA: spikes_right_03 = pc_spikes_right_03; break;
    case EGA: spikes_right_03 = pe_spikes_right_03; break;
    case VGA: spikes_right_03 = pv_spikes_right_03; break;
    }
    break;
  }

  if (vm == VGA) spikes_right_03 = apply_hue_palette (spikes_right_03);
  if (hgc) spikes_right_03 = apply_palette (spikes_right_03, hgc_palette);
  if (peq (p, &mouse_pos))
    spikes_right_03 = apply_palette (spikes_right_03, selection_palette);

  struct coord c;
  draw_bitmapc (spikes_right_03, bitmap, spikes_right_03_coord (p, &c), 0);
}

void
draw_spikes_fg_03 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *spikes_fg_03 = NULL;
  pos2coord_f spikes_fg_03_coord = NULL;

  switch (em) {
  case DUNGEON:
    spikes_fg_03_coord = d_spikes_fg_03_coord;
    switch (vm) {
    case CGA: spikes_fg_03 = dc_spikes_fg_03; break;
    case EGA: spikes_fg_03 = de_spikes_fg_03; break;
    case VGA: spikes_fg_03 = dv_spikes_fg_03; break;
    }
    break;
  case PALACE:
    spikes_fg_03_coord = p_spikes_fg_03_coord;
    switch (vm) {
    case CGA: spikes_fg_03 = pc_spikes_fg_03; break;
    case EGA: spikes_fg_03 = pe_spikes_fg_03; break;
    case VGA: spikes_fg_03 = pv_spikes_fg_03; break;
    }
    break;
  }

  if (vm == VGA) spikes_fg_03 = apply_hue_palette (spikes_fg_03);
  if (hgc) spikes_fg_03 = apply_palette (spikes_fg_03, hgc_palette);
  if (peq (p, &mouse_pos))
    spikes_fg_03 = apply_palette (spikes_fg_03, selection_palette);

  struct coord c;
  draw_bitmapc (spikes_fg_03, bitmap, spikes_fg_03_coord (p, &c), 0);
}

struct coord *
d_spikes_left_03_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 35;
  c->room = p->room;
  return c;
}

struct coord *
p_spikes_left_03_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 35;
  c->room = p->room;
  return c;
}

struct coord *
d_spikes_right_03_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 36;
  c->room = p->room;
  return c;
}

struct coord *
p_spikes_right_03_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 36;
  c->room = p->room;
  return c;
}

struct coord *
d_spikes_fg_03_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 38;
  c->room = p->room;
  return c;
}

struct coord *
p_spikes_fg_03_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 38;
  c->room = p->room;
  return c;
}

void
draw_spikes_04 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  draw_spikes_left_04 (bitmap, p, em, vm);
  draw_spikes_right_04 (bitmap, p, em, vm);
}

void
draw_spikes_left_04 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *spikes_left_04 = NULL;
  pos2coord_f spikes_left_04_coord = NULL;

  switch (em) {
  case DUNGEON:
    spikes_left_04_coord = d_spikes_left_04_coord;
    switch (vm) {
    case CGA: spikes_left_04 = dc_spikes_left_04; break;
    case EGA: spikes_left_04 = de_spikes_left_04; break;
    case VGA: spikes_left_04 = dv_spikes_left_04; break;
    }
    break;
  case PALACE:
    spikes_left_04_coord = p_spikes_left_04_coord;
    switch (vm) {
    case CGA: spikes_left_04 = pc_spikes_left_04; break;
    case EGA: spikes_left_04 = pe_spikes_left_04; break;
    case VGA: spikes_left_04 = pv_spikes_left_04; break;
    }
    break;
  }

  if (vm == VGA) spikes_left_04 = apply_hue_palette (spikes_left_04);
  if (hgc) spikes_left_04 = apply_palette (spikes_left_04, hgc_palette);
  if (peq (p, &mouse_pos))
    spikes_left_04 = apply_palette (spikes_left_04, selection_palette);

  struct coord c;
  draw_bitmapc (spikes_left_04, bitmap, spikes_left_04_coord (p, &c), 0);
}

void
draw_spikes_right_04 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *spikes_right_04 = NULL;
  pos2coord_f spikes_right_04_coord = NULL;

  switch (em) {
  case DUNGEON:
    spikes_right_04_coord = d_spikes_right_04_coord;
    switch (vm) {
    case CGA: spikes_right_04 = dc_spikes_right_04; break;
    case EGA: spikes_right_04 = de_spikes_right_04; break;
    case VGA: spikes_right_04 = dv_spikes_right_04; break;
    }
    break;
  case PALACE:
    spikes_right_04_coord = p_spikes_right_04_coord;
    switch (vm) {
    case CGA: spikes_right_04 = pc_spikes_right_04; break;
    case EGA: spikes_right_04 = pe_spikes_right_04; break;
    case VGA: spikes_right_04 = pv_spikes_right_04; break;
    }
    break;
  }

  if (vm == VGA) spikes_right_04 = apply_hue_palette (spikes_right_04);
  if (hgc) spikes_right_04 = apply_palette (spikes_right_04, hgc_palette);
  if (peq (p, &mouse_pos))
    spikes_right_04 = apply_palette (spikes_right_04, selection_palette);

  struct coord c;
  draw_bitmapc (spikes_right_04, bitmap, spikes_right_04_coord (p, &c), 0);
}

void
draw_spikes_fg_04 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *spikes_fg_04 = NULL;
  pos2coord_f spikes_fg_04_coord = NULL;

  switch (em) {
  case DUNGEON:
    spikes_fg_04_coord = d_spikes_fg_04_coord;
    switch (vm) {
    case CGA: spikes_fg_04 = dc_spikes_fg_04; break;
    case EGA: spikes_fg_04 = de_spikes_fg_04; break;
    case VGA: spikes_fg_04 = dv_spikes_fg_04; break;
    }
    break;
  case PALACE:
    spikes_fg_04_coord = p_spikes_fg_04_coord;
    switch (vm) {
    case CGA: spikes_fg_04 = pc_spikes_fg_04; break;
    case EGA: spikes_fg_04 = pe_spikes_fg_04; break;
    case VGA: spikes_fg_04 = pv_spikes_fg_04; break;
    }
    break;
  }

  if (vm == VGA) spikes_fg_04 = apply_hue_palette (spikes_fg_04);
  if (hgc) spikes_fg_04 = apply_palette (spikes_fg_04, hgc_palette);
  if (peq (p, &mouse_pos))
    spikes_fg_04 = apply_palette (spikes_fg_04, selection_palette);

  struct coord c;
  draw_bitmapc (spikes_fg_04, bitmap, spikes_fg_04_coord (p, &c), 0);
}

struct coord *
d_spikes_left_04_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 35;
  c->room = p->room;
  return c;
}

struct coord *
p_spikes_left_04_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 35;
  c->room = p->room;
  return c;
}

struct coord *
d_spikes_right_04_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 37;
  c->room = p->room;
  return c;
}

struct coord *
p_spikes_right_04_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 37;
  c->room = p->room;
  return c;
}

struct coord *
d_spikes_fg_04_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 40;
  c->room = p->room;
  return c;
}

struct coord *
p_spikes_fg_04_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 39;
  c->room = p->room;
  return c;
}
