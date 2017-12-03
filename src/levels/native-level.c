/*
  native-level.c -- native level module;

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

struct level *
next_native_level (struct level *l, int n)
{
  n = validate_legacy_level_number (n);
  return load_native_level (l, n);
}

struct level *
load_native_level (struct level *l, int n)
{
  char *filename;
  filename = xasprintf ("data/levels/%02d.mim", n);

  ALLEGRO_CONFIG *c = (ALLEGRO_CONFIG *)
    load_resource (filename, (load_resource_f) al_load_config_file,
                   true);

  if (! c) {
    error (0, 0, "cannot read native level file %s", filename);
    return NULL;
  }

  al_free (filename);

  char *k;
  const char *v;
  int i;

  destroy_level (l);
  memset (l, 0, sizeof (*l));

  l->n = n;
  l->start = legacy_level_start;
  l->special_events = legacy_level_special_events;
  l->end = legacy_level_end;
  l->next_level = next_native_level;

  /* CUTSCENES: ok */
  switch (n) {
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
  l->nominal_n = n;
  v = al_get_config_value (c, NULL, "N");
  if (v) sscanf (v, "%i", &l->nominal_n);

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
    struct guard g;
    memset (&g, 0, sizeof (g));

    /* GUARD TYPE AND STYLE */
    /* GiT=t s */
    k = xasprintf ("G%iT", i);
    v = al_get_config_value (c, NULL, k);
    al_free (k);
    if (! v) break;
    g.type = NO_ACTOR, g.style = 0;
    sscanf (v, "%i %i", (int *) &g.type, (int *) &g.style);

    /* GUARD START POSITION AND DIRECTION */
    /* GiP=r f p d */
    k = xasprintf ("G%iP", i);
    v = al_get_config_value (c, NULL, k);
    al_free (k);
    if (! v) break;
    new_pos (&g.p, NULL, 0, 0, 0);
    g.dir = LEFT;
    sscanf (v, "%i %i %i %i", &g.p.room, &g.p.floor, &g.p.place,
            (int *) &g.dir);

    /* GUARD SKILLS AND TOTAL HP */
    /* GiK=a b d e a r f x l */
    k = xasprintf ("G%iK", i);
    v = al_get_config_value (c, NULL, k);
    al_free (k);
    if (! v) break;
    sscanf (v, "%i %i %i %i %i %i %i %i %i",
            &g.skill.attack_prob, &g.skill.counter_attack_prob,
            &g.skill.defense_prob, &g.skill.counter_defense_prob,
            &g.skill.advance_prob, &g.skill.return_prob,
            &g.skill.refraction, &g.skill.extra_hp,
            &g.total_hp);

    l->guard = add_to_array (&g, 1, l->guard, &l->guard_nmemb,
                             l->guard_nmemb, sizeof (*l->guard));
  }

  /* LINKS */

  /* Room 0  */
  l->rlink = xcalloc (1, sizeof (*l->rlink));
  memset (l->rlink, 0, 1 * sizeof (*l->rlink));
  l->room_nmemb = 1;

  for (i = 1;; i++) {
    /* Li=l r a b */
    struct room_linking r;
    memset (&r, 0, sizeof (r));

    k = xasprintf ("L%i", i);
    v = al_get_config_value (c, NULL, k);
    al_free (k);
    if (! v) break;
    sscanf (v, "%i %i %i %i", &r.l, &r.r, &r.a, &r.b);

    l->rlink = add_to_array (&r, 1, l->rlink, &l->room_nmemb,
                            l->room_nmemb, sizeof (*l->rlink));
  }

  /* EVENTS */
  for (i = 0;; i++) {
    /* Ei=r f p n*/
    struct level_event e;
    memset (&e, 0, sizeof (e));

    k = xasprintf ("E%i", i);
    v = al_get_config_value (c, NULL, k);
    al_free (k);
    if (! v) break;
    sscanf (v, "%i %i %i %i", &e.p.room, &e.p.floor, &e.p.place,
            (int *) &e.next);

    l->event = add_to_array (&e, 1, l->event, &l->event_nmemb,
                             l->event_nmemb, sizeof (*l->event));
  }

  /* TILES */
  l->tile = xcalloc (l->room_nmemb, sizeof (*l->tile));
  memset (l->tile, 0, l->room_nmemb * sizeof (*l->tile));

  struct pos p; new_pos (&p, l, -1, -1, -1);
  for (p.room = 1; p.room < l->room_nmemb; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++) {
        /* Tr f p=f b e ff */
        k = xasprintf ("T%i %i %i", p.room, p.floor, p.place);
        v = al_get_config_value (c, NULL, k);
        al_free (k);
        if (! v) goto end_tile_loop;
        int f = 0, b = 0, e = 0, ff = -1;
        sscanf (v, "%i %i %i %i", (int *) &f, &b, &e, &ff);
        set_tile (&p, f, b, e, ff);
      }
 end_tile_loop:

  al_destroy_config (c);

  return l;
}

bool
save_native_level (struct level *l, char *filename)
{
  ALLEGRO_CONFIG *c = al_create_config ();
  char *k, *v;
  int i;

  /* MININIM LEVEL FILE */
  al_add_config_comment (c, NULL, "MININIM LEVEL FILE");

  /* ENGINE VERSION */
  /* V=n */
  v = xasprintf ("%s", VERSION);
  al_set_config_value (c, NULL, "V", v);
  al_free (v);

  /* NOMINAL NUMBER */
  /* N=n */
  v = xasprintf ("%i", l->nominal_n);
  al_set_config_value (c, NULL, "N", v);
  al_free (v);

  /* START POSITION AND DIRECTION*/
  /* P=r f p d s */
  struct pos *sp = &l->start_pos;
  v = xasprintf ("%i %i %i %i %i", sp->room, sp->floor, sp->place,
             l->start_dir, l->has_sword);
  al_set_config_value (c, NULL, "P", v);
  al_free (v);

  /* ENVIRONMENT AND HUE */
  /* S=e h */
  v = xasprintf ("%i %i", l->em, l->hue);
  al_set_config_value (c, NULL, "S", v);
  al_free (v);

  /* GUARDS */
  for (i = 0; i < l->guard_nmemb; i++) {
    struct guard *g = guard (l, i);

    /* GUARD TYPE AND STYLE */
    /* GiT=t s */
    k = xasprintf ("G%iT", i);
    v = xasprintf ("%i %i", g->type, g->style);
    al_set_config_value (c, NULL, k, v);
    al_free (k);
    al_free (v);

    /* GUARD START POSITION AND DIRECTION */
    /* GiP=r f p d */
    k = xasprintf ("G%iP", i);
    v = xasprintf ("%i %i %i %i", g->p.room, g->p.floor, g->p.place, g->dir);
    al_set_config_value (c, NULL, k, v);
    al_free (k);
    al_free (v);

    /* GUARD SKILLS AND TOTAL HP */
    /* GiK=a b d e a r f x l */
    k = xasprintf ("G%iK", i);
    v = xasprintf ("%i %i %i %i %i %i %i %i %i",
               g->skill.attack_prob, g->skill.counter_attack_prob,
               g->skill.defense_prob, g->skill.counter_defense_prob,
               g->skill.advance_prob, g->skill.return_prob,
               g->skill.refraction, g->skill.extra_hp,
               g->total_hp);
    al_set_config_value (c, NULL, k, v);
    al_free (k);
    al_free (v);
  }

  /* LINKS */
  for (i = 1; i < l->room_nmemb; i++) {
    /* Li=l r a b */
    struct room_linking *r = llink (l->rlink, l->room_nmemb, i);
    k = xasprintf ("L%i", i);
    v = xasprintf ("%i %i %i %i", r->l, r->r, r->a, r->b);
    al_set_config_value (c, NULL, k, v);
    al_free (k);
    al_free (v);
  }

  /* EVENTS */
  for (size_t i = 0; i < l->event_nmemb; i++) {
    /* Ei=r f p n */
    struct level_event *e = event (l, i);
    k = xasprintf ("E%zu", i);
    v = xasprintf ("%i %i %i %i", e->p.room, e->p.floor, e->p.place, e->next);
    al_set_config_value (c, NULL, k, v);
    al_free (k);
    al_free (v);
  }

  /* TILES */
  struct pos p; new_pos (&p, l, -1, -1, -1);
  for (p.room = 1; p.room < l->room_nmemb; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++) {
        /* Tr f p=f b e ff */
        k = xasprintf ("T%i %i %i", p.room, p.floor, p.place);
        v = xasprintf ("%i %i %i %i", fg (&p), bg (&p),
                   ext (&p), tile (&p)->fake);
        al_set_config_value (c, NULL, k, v);
        al_free (k);
        al_free (v);
      }

  bool r = al_save_config_file (filename, c);
  al_destroy_config (c);
  return r;
}

char *tile_fg_str[] = {
  [NO_FLOOR] = "NO_FLOOR",
  [FLOOR] = "FLOOR",
  [BROKEN_FLOOR] = "BROKEN_FLOOR",
  [SKELETON_FLOOR] = "SKELETON_FLOOR",
  [LOOSE_FLOOR] = "LOOSE_FLOOR",
  [SPIKES_FLOOR] = "SPIKES_FLOOR",
  [OPENER_FLOOR] = "OPENER_FLOOR",
  [CLOSER_FLOOR] = "CLOSER_FLOOR",
  [STUCK_FLOOR] = "STUCK_FLOOR",
  [HIDDEN_FLOOR] = "HIDDEN_FLOOR",
  [PILLAR] = "PILLAR",
  [BIG_PILLAR_BOTTOM] = "BIG_PILLAR_BOTTOM",
  [BIG_PILLAR_TOP] = "BIG_PILLAR_TOP",
  [WALL] = "WALL",
  [DOOR] = "DOOR",
  [LEVEL_DOOR] = "LEVEL_DOOR",
  [CHOMPER] = "CHOMPER",
  [MIRROR] = "MIRROR",
  [CARPET] = "CARPET",
  [TCARPET] = "TCARPET",
  [ARCH_BOTTOM] = "ARCH_BOTTOM",
  [ARCH_TOP_LEFT] = "ARCH_TOP_LEFT",
  [ARCH_TOP_RIGHT] = "ARCH_TOP_RIGHT",
  [ARCH_TOP_MID] = "ARCH_TOP_MID",
  [ARCH_TOP_SMALL] = "ARCH_TOP_SMALL",
  NULL,
};

char *tile_bg_str[] = {
  [NO_BRICKS] = "NO_BRICKS",
  [BRICKS_1] = "BRICKS_1",
  [BRICKS_2] = "BRICKS_2",
  [BRICKS_3] = "BRICKS_3",
  [BRICKS_4] = "BRICKS_4",
  [BRICKS_5] = "BRICKS_5",
  [TORCH] = "TORCH",
  [WINDOW] = "WINDOW",
  [BALCONY] = "BALCONY",
  NULL
};

char *tile_item_str[] = {
  [NO_ITEM] = "NO_ITEM",
  [EMPTY_POTION] = "EMPTY_POTION",
  [SMALL_HP_POTION] = "SMALL_HP_POTION",
  [BIG_HP_POTION] = "BIG_HP_POTION",
  [SMALL_POISON_POTION] = "SMALL_POISON_POTION",
  [BIG_POISON_POTION] = "BIG_POISON_POTION",
  [FLOAT_POTION] = "FLOAT_POTION",
  [FLIP_POTION] = "FLIP_POTION",
  [ACTIVATION_POTION] = "ACTIVATION_POTION",
  [SWORD] = "SWORD",
  NULL
};

char *
get_tile_ext_str (struct pos *p)
{
  char *s = NULL;
  int e = ext (p);

  switch (fg (p)) {
  case FLOOR:
  case BROKEN_FLOOR:
  case SKELETON_FLOOR:
  case STUCK_FLOOR:
  case HIDDEN_FLOOR:
  case PILLAR:
  case BIG_PILLAR_BOTTOM:
  case ARCH_BOTTOM:
    s = xasprintf ("%s", tile_item_str[e]);
    break;
  case LOOSE_FLOOR:
    s = xasprintf ("%s", e ? "CANT_FALL" : "FALL");
    break;
  case SPIKES_FLOOR: case DOOR: case LEVEL_DOOR: case CHOMPER:
    s = xasprintf ("%i", e);
    break;
  case OPENER_FLOOR: case CLOSER_FLOOR:
    s = xasprintf ("%i", e);
    break;
  case CARPET:
  case TCARPET:
    switch (e) {
    case 0: s = "1"; break;
    case 1: s = "2"; break;
    case 2: s = "3"; break;
    case 3: s = "4"; break;
    case 4: s = "5"; break;
    case 5: s = "6"; break;
    default: assert (false); break;
    }
    if (s) s = xasprintf ("%s", s);
    break;
  default: break;
  }

  return s;
}
