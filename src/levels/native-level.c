/*
  native-level.c -- native level module;

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

struct level native_level;

void
play_native_level (int number)
{
  next_native_level (number);
  play_level (&native_level);
}

void
next_native_level (int number)
{
  if (number < 1) number = 14;
  else if (number > 14) number = 1;
  load_native_level (number, &native_level);
}

void
load_native_level (int number, struct level *l)
{
  char *filename;
  xasprintf (&filename, "data/levels/%02d.mim", number);

  ALLEGRO_CONFIG *c =
    load_resource (filename, (load_resource_f) al_load_config_file);

  if (! c)
    error (-1, 0, "cannot read native level file %s", filename);

  al_free (filename);

  char *k;
  const char *v;
  int i;

  memset (l, 0, sizeof (*l));

  l->number = number;
  l->start = legacy_level_start;
  l->special_events = legacy_level_special_events;
  l->end = legacy_level_end;
  l->next_level = next_native_level;

  /* CUTSCENES: ok */
  switch (number) {
  default: break;
  case 1: l->cutscene = cutscene_01_05_11_anim; break;
  case 3: l->cutscene = cutscene_03_anim; break;
  case 5: l->cutscene = cutscene_01_05_11_anim; break;
  case 7: l->cutscene = cutscene_07_anim; break;
  case 8: l->cutscene = cutscene_08_anim; break;
  case 11: l->cutscene = cutscene_11_anim; break;
  case 14: l->cutscene = cutscene_14_anim; break;
  }

  /* NOMINAL NUMBER */
  /* N=n */
  l->nominal_number = number;
  v = al_get_config_value (c, NULL, "N");
  if (v) sscanf (v, "%i", &l->nominal_number);

  /* START POSITION AND DIRECTION */
  /* P=r f p d s */
  struct pos *sp = &l->start_pos;
  sp->room = 1, sp->floor = 0, sp->place = 0,
    l->start_dir = LEFT, l->has_sword = false;
  v = al_get_config_value (c, NULL, "P");
  if (v) sscanf (v, "%i %i %i %i %i", &sp->room, &sp->floor, &sp->place,
                 (int *) &l->start_dir, (int *) &l->has_sword);

  /* ENVIRONMENT AND HUE */
  /* S=e h */
  l->em = DUNGEON, l->hue = HUE_NONE;
  v = al_get_config_value (c, NULL, "S");
  if (v) sscanf (v, "%i %i", (int *) &l->em, (int *) &l->hue);

  /* GUARDS */
  for (i = 0;; i++) {
    struct guard *g = &l->guard[i];

    /* GUARD TYPE AND STYLE */
    /* GiT=t s */
    xasprintf (&k, "G%iT", i);
    v = al_get_config_value (c, NULL, k);
    al_free (k);
    if (! v) break;
    g->type = NO_ANIM, g->style = 0;
    sscanf (v, "%i %i", (int *) &g->type, (int *) &g->style);

    /* GUARD START POSITION AND DIRECTION */
    /* GiP=r f p d */
    xasprintf (&k, "G%iP", i);
    v = al_get_config_value (c, NULL, k);
    al_free (k);
    if (! v) break;
    new_pos (&g->p, NULL, 0, 0, 0);
    g->dir = LEFT;
    sscanf (v, "%i %i %i %i", &g->p.room, &g->p.floor, &g->p.place,
            (int *) &g->dir);

    /* GUARD SKILLS AND TOTAL LIVES */
    /* GiK=a b d e a r f x l */
    xasprintf (&k, "G%iK", i);
    v = al_get_config_value (c, NULL, k);
    al_free (k);
    if (! v) break;
    sscanf (v, "%i %i %i %i %i %i %i %i %i",
            &g->skill.attack_prob, &g->skill.counter_attack_prob,
            &g->skill.defense_prob, &g->skill.counter_defense_prob,
            &g->skill.advance_prob, &g->skill.return_prob,
            &g->skill.refraction, &g->skill.extra_life,
            &g->total_lives);
  }

  /* LINKS */
  for (i = 0;; i++) {
    /* Li=l r a b */
    struct room_linking *r = &l->link[i];
    xasprintf (&k, "L%i", i);
    v = al_get_config_value (c, NULL, k);
    al_free (k);
    if (! v) break;
    sscanf (v, "%i %i %i %i", &r->l, &r->r, &r->a, &r->b);
  }

  /* EVENTS */
  for (i = 0;; i++) {
    /* Ei=r f p n*/
    struct level_event *e = &l->event[i];
    xasprintf (&k, "E%i", i);
    v = al_get_config_value (c, NULL, k);
    al_free (k);
    if (! v) break;
    sscanf (v, "%i %i %i %i", &e->p.room, &e->p.floor, &e->p.place,
            (int *) &e->next);
  }

  /* CONSTRUCTIONS */
  struct pos p; new_pos (&p, l, -1, -1, -1);
  for (p.room = 0;; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++) {
        /* Cr f p=f b e */
        xasprintf (&k, "C%i %i %i", p.room, p.floor, p.place);
        v = al_get_config_value (c, NULL, k);
        al_free (k);
        if (! v) goto end_con_loop;
        sscanf (v, "%i %i %i", (int *) &con(&p)->fg,
                (int *) &con (&p)->bg,
                (int *) &con (&p)->ext);
      }
 end_con_loop:

  al_destroy_config (c);
}

bool
save_native_level (struct level *l, char *filename)
{
  ALLEGRO_CONFIG *c = create_config ();
  char *k, *v;
  int i;

  /* MININIM LEVEL FILE */
  al_add_config_comment (c, NULL, "MININIM LEVEL FILE");

  /* NOMINAL NUMBER */
  /* N=n */
  xasprintf (&v, "%i", l->nominal_number);
  al_set_config_value (c, NULL, "N", v);
  al_free (v);

  /* START POSITION AND DIRECTION*/
  /* P=r f p d s */
  struct pos *sp = &l->start_pos;
  xasprintf (&v, "%i %i %i %i %i", sp->room, sp->floor, sp->place,
             l->start_dir, l->has_sword);
  al_set_config_value (c, NULL, "P", v);
  al_free (v);

  /* ENVIRONMENT AND HUE */
  /* S=e h */
  xasprintf (&v, "%i %i", l->em, l->hue);
  al_set_config_value (c, NULL, "S", v);
  al_free (v);

  /* GUARDS */
  for (i = 0; i < GUARDS; i++) {
    struct guard *g = &l->guard[i];

    /* GUARD TYPE AND STYLE */
    /* GiT=t s */
    xasprintf (&k, "G%iT", i);
    xasprintf (&v, "%i %i", g->type, g->style);
    al_set_config_value (c, NULL, k, v);
    al_free (k);
    al_free (v);

    /* GUARD START POSITION AND DIRECTION */
    /* GiP=r f p d */
    xasprintf (&k, "G%iP", i);
    xasprintf (&v, "%i %i %i %i", g->p.room, g->p.floor, g->p.place, g->dir);
    al_set_config_value (c, NULL, k, v);
    al_free (k);
    al_free (v);

    /* GUARD SKILLS AND TOTAL LIVES */
    /* GiK=a b d e a r f x l */
    xasprintf (&k, "G%iK", i);
    xasprintf (&v, "%i %i %i %i %i %i %i %i %i",
               g->skill.attack_prob, g->skill.counter_attack_prob,
               g->skill.defense_prob, g->skill.counter_defense_prob,
               g->skill.advance_prob, g->skill.return_prob,
               g->skill.refraction, g->skill.extra_life,
               g->total_lives);
    al_set_config_value (c, NULL, k, v);
    al_free (k);
    al_free (v);
  }

  /* LINKS */
  for (i = 0; i < ROOMS; i++) {
    /* Li=l r a b */
    struct room_linking *r = &l->link[i];
    xasprintf (&k, "L%i", i);
    xasprintf (&v, "%i %i %i %i", r->l, r->r, r->a, r->b);
    al_set_config_value (c, NULL, k, v);
    al_free (k);
    al_free (v);
  }

  /* EVENTS */
  for (i = 0; i < EVENTS; i++) {
    /* Ei=r f p n */
    struct level_event *e = &l->event[i];
    xasprintf (&k, "E%i", i);
    xasprintf (&v, "%i %i %i %i", e->p.room, e->p.floor, e->p.place, e->next);
    al_set_config_value (c, NULL, k, v);
    al_free (k);
    al_free (v);
  }

  /* CONSTRUCTIONS */
  struct pos p; new_pos (&p, l, -1, -1, -1);
  for (p.room = 0; p.room < ROOMS; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++) {
        /* Cr f p=f b e */
        xasprintf (&k, "C%i %i %i", p.room, p.floor, p.place);
        xasprintf (&v, "%i %i %i", con (&p)->fg,
                   con (&p)->bg, con (&p)->ext);
        al_set_config_value (c, NULL, k, v);
        al_free (k);
        al_free (v);
      }

  bool r = al_save_config_file (filename, c);
  al_destroy_config (c);
  return r;
}

char *
get_confg_str (struct pos *p)
{
  switch (con (p)->fg) {
  case NO_FLOOR: return "NO_FLOOR";
  case FLOOR: return "FLOOR";
  case BROKEN_FLOOR: return "BROKEN_FLOOR";
  case SKELETON_FLOOR: return "SKELETON_FLOOR";
  case LOOSE_FLOOR: return "LOOSE_FLOOR";
  case SPIKES_FLOOR: return "SPIKES_FLOOR";
  case OPENER_FLOOR: return "OPENER_FLOOR";
  case CLOSER_FLOOR: return "CLOSER_FLOOR";
  case STUCK_FLOOR: return "STUCK_FLOOR";
  case HIDDEN_FLOOR: return "HIDDEN_FLOOR";
  case PILLAR: return "PILLAR";
  case BIG_PILLAR_BOTTOM: return "BIG_PILLAR_BOTTOM";
  case BIG_PILLAR_TOP: return "BIG_PILLAR_TOP";
  case WALL: return "WALL";
  case DOOR: return "DOOR";
  case LEVEL_DOOR: return "LEVEL_DOOR";
  case CHOPPER: return "CHOPPER";
  case MIRROR: return "MIRROR";
  case CARPET: return "CARPET";
  case TCARPET: return "TCARPET";
  case ARCH_BOTTOM: return "ARCH_BOTTOM";
  case ARCH_TOP_LEFT: return "ARCH_TOP_LEFT";
  case ARCH_TOP_RIGHT: return "ARCH_TOP_RIGHT";
  case ARCH_TOP_MID: return "ARCH_TOP_MID";
  case ARCH_TOP_SMALL: return "ARCH_TOP_SMALL";
  default: return NULL;
  }
}

char *
get_conbg_str (struct pos *p)
{
  switch (con (p)->bg) {
  case NO_BG: return "NO_BG";
  case BRICKS_00: return "BRICKS_00";
  case BRICKS_01: return "BRICKS_01";
  case BRICKS_02: return "BRICKS_02";
  case BRICKS_03: return "BRICKS_03";
  case NO_BRICKS: return "NO_BRICKS";
  case TORCH: return "TORCH";
  case WINDOW: return "WINDOW";
  case BALCONY: return "BALCONY";
  default: return NULL;
  }
}

char *
get_conext_str (struct pos *p)
{
  char *s = NULL;

  switch (con (p)->fg) {
  case FLOOR:
    switch (con (p)->ext.item) {
    case NO_ITEM: xasprintf (&s, "NO_ITEM"); break;
    case EMPTY_POTION: xasprintf (&s, "EMPTY_POTION"); break;
    case SMALL_LIFE_POTION: xasprintf (&s, "SMALL_LIFE_POTION"); break;
    case BIG_LIFE_POTION: xasprintf (&s, "BIG_LIFE_POTION"); break;
    case SMALL_POISON_POTION: xasprintf (&s, "SMALL_POISON_POTION"); break;
    case BIG_POISON_POTION: xasprintf (&s, "BIG_POISON_POTION"); break;
    case FLOAT_POTION: xasprintf (&s, "FLOAT_POTION"); break;
    case FLIP_POTION: xasprintf (&s, "FLIP_POTION"); break;
    case ACTIVATION_POTION: xasprintf (&s, "ACTIVATION_POTION"); break;
    case SWORD: xasprintf (&s, "SWORD"); break;
    }
    break;
  case LOOSE_FLOOR:
    xasprintf (&s, "%s", con (p)->ext.cant_fall ? "CANT_FALL" : "FALL");
    break;
  case SPIKES_FLOOR: case DOOR: case LEVEL_DOOR: case CHOPPER:
    xasprintf (&s, "%i", con (p)->ext.step);
    break;
  case OPENER_FLOOR: case CLOSER_FLOOR:
    xasprintf (&s, "%i", con (p)->ext.event);
    break;
  case CARPET:
    switch (con (p)->ext.design) {
    case CARPET_00: s = "CARPET_00"; break;
    case CARPET_01: s = "CARPET_01"; break;
    case ARCH_CARPET_LEFT: s = "ARCH_CARPET_LEFT"; break;
    }
    if (s) xasprintf (&s, "%s", s);
    break;
  case TCARPET:
    switch (con (p)->ext.design) {
    case CARPET_00: s = "CARPET_00"; break;
    case CARPET_01: s = "CARPET_01"; break;
    case ARCH_CARPET_RIGHT_00: s = "ARCH_CARPET_RIGHT_00"; break;
    case ARCH_CARPET_RIGHT_01: s = "ARCH_CARPET_RIGHT_01"; break;
    case ARCH_CARPET_LEFT: s = "ARCH_CARPET_LEFT"; break;
    }
    if (s) xasprintf (&s, "%s", s);
    break;
  default: break;
  }

  return s;
}
