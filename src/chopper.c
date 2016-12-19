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

#include "mininim.h"

/* dungeon cga */
ALLEGRO_BITMAP *dc_chopper_00, *dc_chopper_01,
  *dc_chopper_02, *dc_chopper_03, *dc_chopper_04;

/* palace cga */
ALLEGRO_BITMAP *pc_chopper_00, *pc_chopper_01,
  *pc_chopper_02, *pc_chopper_03, *pc_chopper_04;

/* dungeon ega */
ALLEGRO_BITMAP *de_chopper_00, *de_chopper_01,
  *de_chopper_02, *de_chopper_03, *de_chopper_04;

/* palace ega */
ALLEGRO_BITMAP *pe_chopper_00, *pe_chopper_01,
  *pe_chopper_02, *pe_chopper_03, *pe_chopper_04;

/* dungeon vga */
ALLEGRO_BITMAP *dv_chopper_00, *dv_chopper_01,
  *dv_chopper_02, *dv_chopper_03, *dv_chopper_04;

/* palace vga */
ALLEGRO_BITMAP *pv_chopper_00, *pv_chopper_01,
  *pv_chopper_02, *pv_chopper_03, *pv_chopper_04;

/* palettable */
ALLEGRO_BITMAP *chopper_blood_00, *chopper_blood_01,
  *chopper_blood_02, *chopper_blood_03, *chopper_blood_04;

struct chopper *chopper = NULL;
size_t chopper_nmemb = 0;

static void
draw_left_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
              struct chopper *ch, int w, enum em em, enum vm vm);
static void
draw_left_02 (ALLEGRO_BITMAP *bitmap, struct pos *p,
              struct chopper *ch, int w, enum em em, enum vm vm);
static void
draw_left_03 (ALLEGRO_BITMAP *bitmap, struct pos *p,
              struct chopper *ch, int w, enum em em, enum vm vm);
static void
draw_left_04 (ALLEGRO_BITMAP *bitmap, struct pos *p,
              struct chopper *ch, int w, enum em em, enum vm vm);
static void
draw_left_05 (ALLEGRO_BITMAP *bitmap, struct pos *p,
              struct chopper *ch, int w, enum em em, enum vm vm);

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
  dc_chopper_00 = load_bitmap (DC_CHOPPER_00);
  dc_chopper_01 = load_bitmap (DC_CHOPPER_01);
  dc_chopper_02 = load_bitmap (DC_CHOPPER_02);
  dc_chopper_03 = load_bitmap (DC_CHOPPER_03);
  dc_chopper_04 = load_bitmap (DC_CHOPPER_04);

  /* palace cga */
  pc_chopper_00 = load_bitmap (PC_CHOPPER_00);
  pc_chopper_01 = load_bitmap (PC_CHOPPER_01);
  pc_chopper_02 = load_bitmap (PC_CHOPPER_02);
  pc_chopper_03 = load_bitmap (PC_CHOPPER_03);
  pc_chopper_04 = load_bitmap (PC_CHOPPER_04);

  /* dungeon ega */
  de_chopper_00 = load_bitmap (DE_CHOPPER_00);
  de_chopper_01 = load_bitmap (DE_CHOPPER_01);
  de_chopper_02 = load_bitmap (DE_CHOPPER_02);
  de_chopper_03 = load_bitmap (DE_CHOPPER_03);
  de_chopper_04 = load_bitmap (DE_CHOPPER_04);

  /* palace ega */
  pe_chopper_00 = load_bitmap (PE_CHOPPER_00);
  pe_chopper_01 = load_bitmap (PE_CHOPPER_01);
  pe_chopper_02 = load_bitmap (PE_CHOPPER_02);
  pe_chopper_03 = load_bitmap (PE_CHOPPER_03);
  pe_chopper_04 = load_bitmap (PE_CHOPPER_04);

  /* dungeon vga */
  dv_chopper_00 = load_bitmap (DV_CHOPPER_00);
  dv_chopper_01 = load_bitmap (DV_CHOPPER_01);
  dv_chopper_02 = load_bitmap (DV_CHOPPER_02);
  dv_chopper_03 = load_bitmap (DV_CHOPPER_03);
  dv_chopper_04 = load_bitmap (DV_CHOPPER_04);

  /* palace vga */
  pv_chopper_00 = load_bitmap (PV_CHOPPER_00);
  pv_chopper_01 = load_bitmap (PV_CHOPPER_01);
  pv_chopper_02 = load_bitmap (PV_CHOPPER_02);
  pv_chopper_03 = load_bitmap (PV_CHOPPER_03);
  pv_chopper_04 = load_bitmap (PV_CHOPPER_04);

  /* palettable */
  chopper_blood_00 = load_bitmap (CHOPPER_BLOOD_00);
  chopper_blood_01 = load_bitmap (CHOPPER_BLOOD_01);
  chopper_blood_02 = load_bitmap (CHOPPER_BLOOD_02);
  chopper_blood_03 = load_bitmap (CHOPPER_BLOOD_03);
  chopper_blood_04 = load_bitmap (CHOPPER_BLOOD_04);
}

void
unload_chopper (void)
{
  /* dungeon cga */
  destroy_bitmap (dc_chopper_00);
  destroy_bitmap (dc_chopper_01);
  destroy_bitmap (dc_chopper_02);
  destroy_bitmap (dc_chopper_03);
  destroy_bitmap (dc_chopper_04);

  /* palace cga */
  destroy_bitmap (pc_chopper_00);
  destroy_bitmap (pc_chopper_01);
  destroy_bitmap (pc_chopper_02);
  destroy_bitmap (pc_chopper_03);
  destroy_bitmap (pc_chopper_04);

  /* dungeon ega */
  destroy_bitmap (de_chopper_00);
  destroy_bitmap (de_chopper_01);
  destroy_bitmap (de_chopper_02);
  destroy_bitmap (de_chopper_03);
  destroy_bitmap (de_chopper_04);

  /* palace ega */
  destroy_bitmap (pe_chopper_00);
  destroy_bitmap (pe_chopper_01);
  destroy_bitmap (pe_chopper_02);
  destroy_bitmap (pe_chopper_03);
  destroy_bitmap (pe_chopper_04);

  /* dungeon vga */
  destroy_bitmap (dv_chopper_00);
  destroy_bitmap (dv_chopper_01);
  destroy_bitmap (dv_chopper_02);
  destroy_bitmap (dv_chopper_03);
  destroy_bitmap (dv_chopper_04);

  /* palace vga */
  destroy_bitmap (pv_chopper_00);
  destroy_bitmap (pv_chopper_01);
  destroy_bitmap (pv_chopper_02);
  destroy_bitmap (pv_chopper_03);
  destroy_bitmap (pv_chopper_04);

  /* palettable */
  destroy_bitmap (chopper_blood_00);
  destroy_bitmap (chopper_blood_01);
  destroy_bitmap (chopper_blood_02);
  destroy_bitmap (chopper_blood_03);
  destroy_bitmap (chopper_blood_04);
}

void
register_chopper (struct pos *p)
{
  struct chopper c;

  npos (p, &c.p);

  int n, f;
  typed_int (ext (p), CHOPPER_STEPS, CHOPPER_FASES, &n, &f);

  c.i = n % (CHOPPER_STEPS - 1);
  c.blood = f;
  c.inactive = (n != 0);

  c.wait = CHOPPER_WAIT;
  c.alert = false;
  c.activate = false;

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
copy_chopper (struct chopper *to, struct chopper *from)
{
  struct pos p = to->p;
  *to = *from;
  to->p = p;
  return to;
}

struct chopper *
chopper_at_pos (struct pos *p)
{
  struct chopper c;
  c.p = *p;

  struct chopper *cc;

 search:
  cc = bsearch (&c, chopper, chopper_nmemb, sizeof (c),
                compare_choppers);

  if (cc && fg (p) != CHOPPER) {
    remove_chopper (cc);
    return NULL;
  } else if (! cc && fg (p) == CHOPPER) {
    register_chopper (p);
    goto search;
  }

  return cc;
}

void
remove_chopper (struct chopper *c)
{
  size_t i =  c - chopper;
  chopper =
    remove_from_array (chopper, &chopper_nmemb, i, 1, sizeof (*c));
}

bool
should_chomp (struct pos *p)
{
  int i;

  for (i = 0; i < anima_nmemb; i++) {
    struct anim *a = &anima[i];
    if (a->type != KID
        || (is_anim_dead (&a->f) && ! a->id == 0)) continue;
    if (is_pos_seeing (p, a, LEFT) || is_pos_seeing (p, a, RIGHT))
      return true;
  }

  return false;
}

void
compute_choppers (void)
{
  size_t i, j;

  for (i = 0; i < chopper_nmemb;) {
    struct chopper *c = &chopper[i];
    if (fg (&c->p) == CHOPPER) {
      i++; continue;
    }
    remove_chopper (c);
  }

  for (i = 0; i < chopper_nmemb; i++) {
    struct chopper *c = &chopper[i];

    if (c->inactive) continue;

    switch (c->i) {
    case 0:
      if (! c->alert) c->alert = ! should_chomp (&c->p);
      if ((c->wait-- <= 0 && should_chomp (&c->p)
           && (anim_cycle % CHOPPER_WAIT) ==
           prandom_pos (&c->p, CHOPPER_WAIT - 1))
          || c->activate) {
        c->i++;
        register_changed_pos (&c->p);
      }
      break;
    case 1:
      c->i++;
      if (c->alert) {
        alert_guards (&c->p);
        c->alert = false;
      }
      play_audio (&chopper_audio, &c->p, -1);
      register_changed_pos (&c->p);
      break;
    case 2:
      c->i++;
      register_changed_pos (&c->p);
      break;
    case 3:
      c->i++;
      register_changed_pos (&c->p);
      break;
    case 4:
      c->i = 0; c->wait = CHOPPER_WAIT; c->activate = false;
      register_changed_pos (&c->p);
      break;
    }

    if (c->i != 1 && c->i != 2 ) continue;

    /* chomp kid */
    for (j = 0; j < anima_nmemb; j++) {
      struct anim *a = &anima[j];
      struct pos pl; prel (&c->p, &pl, +0, -1);
      if (a->type == MOUSE
          || is_anim_fall (&a->f)
          || a->immortal
          || a->chopper_immune
          || (a->action == kid_walk && a->walk != -1)
          || is_collidable_at_right (&pl, &a->f))
        continue;
      struct pos pbf, pbb;
      survey (_bf, pos, &a->f, NULL, &pbf, NULL);
      survey (_bb, pos, &a->f, NULL, &pbb, NULL);
      pos2room (&pbf, c->p.room, &pbf);
      pos2room (&pbb, c->p.room, &pbb);
      if ((((pbf.room == c->p.room
             && pbf.floor == c->p.floor)
            || (pbb.room == c->p.room
                && pbb.floor == c->p.floor))
           && ((a->f.dir == LEFT && pbf.place < c->p.place
                && pbb.place >= c->p.place)
               || (a->f.dir == RIGHT && pbf.place >= c->p.place
                   && pbb.place < c->p.place)))
          && (! is_anim_dead (&a->f) || ! is_anim_chopped (&a->f))) {
        if (a->type != SKELETON) c->blood = true;
        a->splash = true;
        a->p = c->p;
        a->death_reason = CHOPPER_DEATH;
        if (a->id == current_kid_id) {
          mr.flicker = 2;
          mr.color = get_flicker_blood_color ();
        }
        if (a->type == SKELETON)
          play_audio (&skeleton_audio, &c->p, -1);
        else play_audio (&chopped_audio, &c->p, -1);
        a->next_action = anim_die_chopped;
      }
    }
  }
}

void
draw_chopper_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   enum em em, enum vm vm)
{
  if (is_fake (p)) {
    switch (abs (ext (p)) % 5) {
    case 0: draw_left_01 (bitmap, p, NULL, FULL_WIDTH, em, vm); break;
    case 1: draw_left_02 (bitmap, p, NULL, FULL_WIDTH, em, vm); break;
    case 2: draw_left_03 (bitmap, p, NULL, FULL_WIDTH, em, vm); break;
    case 3: draw_left_04 (bitmap, p, NULL, FULL_WIDTH, em, vm); break;
    case 4: draw_left_05 (bitmap, p, NULL, FULL_WIDTH, em, vm); break;
    }
    return;
  }

  struct chopper *ch = chopper_at_pos (p);
  if (! ch) return;

  switch (ch->i) {
  case 0: draw_left_01 (bitmap, p, ch, FULL_WIDTH, em, vm); break;
  case 1: draw_left_02 (bitmap, p, ch, FULL_WIDTH, em, vm); break;
  case 2: draw_left_03 (bitmap, p, ch, FULL_WIDTH, em, vm); break;
  case 3: draw_left_04 (bitmap, p, ch, FULL_WIDTH, em, vm); break;
  case 4: draw_left_05 (bitmap, p, ch, FULL_WIDTH, em, vm); break;
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
              int w, enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *chopper = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: chopper = dc_chopper_00; break;
    case EGA: chopper = de_chopper_00; break;
    case VGA: chopper = dv_chopper_00; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: chopper = pc_chopper_00; break;
    case EGA: chopper = pe_chopper_00; break;
    case VGA: chopper = pv_chopper_00; break;
    }
    break;
  }

  if (vm == VGA) chopper = apply_hue_palette (chopper);
  if (hgc) chopper = apply_palette (chopper, hgc_palette);
  if (peq (p, &mouse_pos)) chopper = apply_palette (chopper, selection_palette);

  struct coord c;
  draw_bitmap_regionc (chopper, bitmap, 0, 0,
                       w < 0 ? al_get_bitmap_width (chopper) : w,
                       al_get_bitmap_height (chopper),
                       chopper_coord (p, &c), 0);
  if (ch && ch->blood) {
    palette blood_palette = get_blood_palette (vm);
    ALLEGRO_BITMAP *blood = apply_palette (chopper_blood_00, blood_palette);
    if (hgc) blood = apply_palette (blood, hgc_palette);
    draw_bitmap_regionc (blood, bitmap, 0, 0,
                         w < 0 ? al_get_bitmap_width (blood) : w - 12,
                         al_get_bitmap_height (blood),
                         blood_01_coord (p, &c), 0);
  }
}

static void
draw_left_02 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
              int w, enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *chopper = NULL;

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

  if (vm == VGA) chopper = apply_hue_palette (chopper);
  if (hgc) chopper = apply_palette (chopper, hgc_palette);
  if (peq (p, &mouse_pos)) chopper = apply_palette (chopper, selection_palette);

  struct coord c;
  draw_bitmap_regionc (chopper, bitmap, 0, 0,
                       w < 0 ? al_get_bitmap_width (chopper) : w,
                       al_get_bitmap_height (chopper),
                       chopper_coord (p, &c), 0);
  if (ch && ch->blood) {
    palette blood_palette = get_blood_palette (vm);
    ALLEGRO_BITMAP *blood = apply_palette (chopper_blood_01, blood_palette);
    if (hgc) blood = apply_palette (blood, hgc_palette);
    draw_bitmap_regionc (blood, bitmap, 0, 0,
                         w < 0 ? al_get_bitmap_width (blood) : w - 12,
                         al_get_bitmap_height (blood),
                         blood_02_coord (p, &c), 0);
  }
}

static void
draw_left_03 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
              int w, enum em em, enum vm vm)
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

  if (vm == VGA) chopper = apply_hue_palette (chopper);
  if (hgc) chopper = apply_palette (chopper, hgc_palette);
  if (peq (p, &mouse_pos)) chopper = apply_palette (chopper, selection_palette);

  struct coord c;
  draw_bitmap_regionc (chopper, bitmap, 0, 0,
                       w < 0 ? al_get_bitmap_width (chopper) : w,
                       al_get_bitmap_height (chopper),
                       chopper_coord (p, &c), 0);
  if (ch && ch->blood) {
    palette blood_palette = get_blood_palette (vm);
    ALLEGRO_BITMAP *blood = apply_palette (chopper_blood_02, blood_palette);
    if (hgc) blood = apply_palette (blood, hgc_palette);
    draw_bitmap_regionc (blood, bitmap, 0, 0,
                         w < 0 ? al_get_bitmap_width (blood) : w - 12,
                         al_get_bitmap_height (blood),
                         blood_03_coord (p, &c), 0);
  }
}

static void
draw_left_04 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
              int w, enum em em, enum vm vm)
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

  if (vm == VGA) chopper = apply_hue_palette (chopper);
  if (hgc) chopper = apply_palette (chopper, hgc_palette);
  if (peq (p, &mouse_pos)) chopper = apply_palette (chopper, selection_palette);

  struct coord c;
  draw_bitmap_regionc (chopper, bitmap, 0, 0,
                       w < 0 ? al_get_bitmap_width (chopper) : w,
                       al_get_bitmap_height (chopper),
                       chopper_coord (p, &c), 0);
  if (ch && ch->blood) {
    palette blood_palette = get_blood_palette (vm);
    ALLEGRO_BITMAP *blood = apply_palette (chopper_blood_03, blood_palette);
    if (hgc) blood = apply_palette (blood, hgc_palette);
    draw_bitmap_regionc (blood, bitmap, 0, 0,
                         w < 0 ? al_get_bitmap_width (blood) : w - 12,
                         al_get_bitmap_height (blood),
                         blood_04_coord (p, &c), 0);
  }
}

static void
draw_left_05 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
              int w, enum em em, enum vm vm)
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

  if (vm == VGA) chopper = apply_hue_palette (chopper);
  if (hgc) chopper = apply_palette (chopper, hgc_palette);
  if (peq (p, &mouse_pos)) chopper = apply_palette (chopper, selection_palette);

  struct coord c;
  draw_bitmap_regionc (chopper, bitmap, 0, 0,
                       w < 0 ? al_get_bitmap_width (chopper) : w,
                       al_get_bitmap_height (chopper),
                       chopper_coord (p, &c), 0);
  if (ch && ch->blood) {
    palette blood_palette = get_blood_palette (vm);
    ALLEGRO_BITMAP *blood = apply_palette (chopper_blood_04, blood_palette);
    if (hgc) blood = apply_palette (blood, hgc_palette);
    draw_bitmap_regionc (blood, bitmap, 0, 0,
                         w < 0 ? al_get_bitmap_width (blood) : w - 12,
                         al_get_bitmap_height (blood),
                         blood_05_coord (p, &c), 0);
  }
}

void
draw_chopper_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm)
{
  if (is_fake (p)) {
    switch (abs (ext (p)) % 5) {
    case 0: draw_left_01 (bitmap, p, NULL, CHOPPER_FG_WIDTH, em, vm); break;
    case 1: draw_left_02 (bitmap, p, NULL, CHOPPER_FG_WIDTH, em, vm); break;
    case 2: draw_left_03 (bitmap, p, NULL, CHOPPER_FG_WIDTH, em, vm); break;
    case 3: draw_left_04 (bitmap, p, NULL, CHOPPER_FG_WIDTH, em, vm); break;
    case 4: draw_left_05 (bitmap, p, NULL, CHOPPER_FG_WIDTH, em, vm); break;
    }
    return;
  }

  struct chopper *ch = chopper_at_pos (p);
  if (! ch) return;

  switch (ch->i) {
  case 0: draw_left_01 (bitmap, p, ch, CHOPPER_FG_WIDTH, em, vm); break;
  case 1: draw_left_02 (bitmap, p, ch, CHOPPER_FG_WIDTH, em, vm); break;
  case 2: draw_left_03 (bitmap, p, ch, CHOPPER_FG_WIDTH, em, vm); break;
  case 3: draw_left_04 (bitmap, p, ch, CHOPPER_FG_WIDTH, em, vm); break;
  case 4: draw_left_05 (bitmap, p, ch, CHOPPER_FG_WIDTH, em, vm); break;
  }
}

struct coord *
chopper_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place,
               PLACE_HEIGHT * p->floor + 3);
}

struct coord *
blood_01_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place + 12,
               PLACE_HEIGHT * p->floor + 48);
}

struct coord *
blood_02_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place + 12,
               PLACE_HEIGHT * p->floor + 39);
}

struct coord *
blood_03_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place + 12,
               PLACE_HEIGHT * p->floor + 28);
}

struct coord *
blood_04_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place + 12,
               PLACE_HEIGHT * p->floor + 32);
}

struct coord *
blood_05_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place + 12,
               PLACE_HEIGHT * p->floor + 52);
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
