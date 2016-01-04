/*
  legacy-level.c -- legacy level module;

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
#include <time.h>
#include "kernel/video.h"
#include "kernel/audio.h"
#include "kernel/random.h"
#include "kernel/array.h"
#include "kernel/xerror.h"
#include "engine/level.h"
#include "engine/kid/kid.h"
#include "engine/door.h"
#include "engine/level-door.h"
#include "engine/physics.h"
#include "engine/anim.h"
#include "engine/mirror.h"
#include "legacy-level.h"

static struct level legacy_level;
static int level_3_checkpoint;
static int shadow_id;
static bool played_end_sample;

static struct con room_0[FLOORS][PLACES] =
  {{{WALL}, {WALL}, {WALL}, {WALL}, {WALL},
    {WALL}, {WALL}, {WALL}, {WALL}, {WALL}},
   {{WALL}, {WALL}, {WALL}, {WALL}, {WALL},
    {WALL}, {WALL}, {WALL}, {WALL}, {WALL}},
   {{WALL}, {WALL}, {WALL}, {WALL}, {WALL},
    {WALL}, {WALL}, {WALL}, {WALL}, {WALL}}};

static struct {
  uint8_t foretable[LROOMS][FLOORS][PLACES];
  uint8_t backtable[LROOMS][FLOORS][PLACES];
  uint8_t door_1[LEVENTS];
  uint8_t door_2[LEVENTS];
  uint8_t link[LROOMS][4];
  uint8_t unknown_1[64];
  uint8_t start_position[3];
  uint8_t unknown_2[3];
  uint8_t unknown_3;
  uint8_t guard_location[LROOMS];
  uint8_t guard_direction[LROOMS];
  uint8_t unknown_4a[LROOMS];
  uint8_t unknown_4b[LROOMS];
  uint8_t guard_skill[LROOMS];
  uint8_t unknown_4c[LROOMS];
  uint8_t guard_color[LROOMS];
  uint8_t unknown_4d[16];
  uint8_t signature[2];
} lv;

static enum ltile get_tile (struct pos *p);
static enum lgroup get_group (enum ltile t);

static void next_level (int lv, struct pos *exit_door_pos);
static void load_legacy_level (int number);
static void start (void);
static void special_events (void);
static void end (void);

void
play_legacy_level (void)
{
  next_level (1, NULL);
  play_level (&legacy_level);
}

static void
start (void)
{
  played_end_sample = false;
  stop_all_samples ();

  /* create kid */
  int id = create_kid (NULL);
  struct anim *k = &kid[id];

  /* make the kid turn as appropriate */
  switch (level.number) {
  case 1: k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT; break;
  case 13:
    k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT;
    k->i = -1; k->action = kid_stop_run; break;
  default: k->i = -1; k->action = kid_turn; break;
  }

  /* define camera's starting room */
  if (level.number == 7) {
    room_view = 1;
    camera_follow_kid = -1;
  } else if (level.number == 13) {
    room_view = 23;
    camera_follow_kid = -1;
  } else {
    room_view = k->f.c.room;
    camera_follow_kid = k->id;
  }

  /* define the enviroment mode based on the level */
  if (retry_level != level.number)
    switch (level.number) {
    case 4: case 5: case 6: case 10: case 11: case 14:
      em = PALACE; break;
    default: em = DUNGEON; break;
    }

  /* give the sword to kid if it's not in the starting level */
  if (level.number > 1) k->has_sword = true;

  /* in the first level */
  if (level.number == 1) {
    /* activate tile, usually to close the opened door in the starting
       room */
    struct pos p = {5,0,2};
    activate_con (&p);
    /* if it's the first try make kid wait before uncouching */
    if (retry_level != 1) k->uncouch_slowly = true;
  }

  /* in the third level */
  if (level.number == 3) {
    /* if it's the first time playing the checkpoint hasn't been
       reached yet */
    if (retry_level != 3) level_3_checkpoint = false;
    /* if the checkpoint has been reached, respawn there */
    struct pos p = {2,0,6};
    if (level_3_checkpoint) {
      struct pos plf = {7,0,4};
      con (&plf)->fg = NO_FLOOR;
      k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT;
      place_frame (&k->f, &k->f, kid_normal_00, &p,
                   k->f.dir == LEFT ? +22 : +31, +15);
      room_view = 2;
    }
  }

  /* in the fourth level */
  if (level.number == 4) {
   shadow_id = -1;

  /* struct pos p = {4,1,6}; */
  /* place_frame (&k->f, &k->f, kid_normal_00, &p, */
  /*              k->f.dir == LEFT ? +22 : +31, +15); */
  }
}

static void
special_events (void)
{
  struct pos np, p, pm;
  struct coord nc;
  struct anim *k = current_kid;

  /* in the first animation cycle */
  if (anim_cycle == 0) {
    /* close any level door in the starting room */
    struct pos p;
    p.room = current_kid->f.c.room;
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = -1; p.place < PLACES; p.place++)
        if (con (&p)->fg == LEVEL_DOOR) {
          struct level_door *ld = level_door_at_pos (&p);
          ld->i = 0;
          ld->action = CLOSE_LEVEL_DOOR;
        }
  }

  /* in the first level, first try, play the suspense sound */
  if (level.number == 1 && anim_cycle == 12 && retry_level != 1)
    play_sample (suspense_sample, k->f.c.room);

  /* level 3 checkpoint */
  if (level.number == 3) {
    p = (struct pos) {2,0,8};
    survey (_m, pos, &k->f, &nc, &pm, &np);
    if (peq (&pm, &p)) level_3_checkpoint = true;
  }

  /* in the fourth level */
  if (level.number == 4) {
      struct pos pld = {24,1,3};
      struct pos pmirror = {4,0,4};
      struct level_door *ld = level_door_at_pos (&pld);

      /* if the level door is open and the camera is on room 4, make
         the mirror appear */
      if (ld->i == 0
          && room_view == 4
          && con (&pmirror)->fg != MIRROR) {
        con (&pmirror)->fg = MIRROR;
        register_mirror (&pmirror);
        play_sample (suspense_sample, 4);
      }

      /* if the kid is crossing the mirror, make his shadow appear */
      if (con (&pmirror)->fg == MIRROR) {
        struct mirror *m = mirror_at_pos (&pmirror);
        if (m->kid_crossing == k->id) {
          k->current_lives = 1;
          int id = create_kid (k);
          kid[id].shadow = true;
          kid[id].f.dir = (kid[id].f.dir == LEFT) ? RIGHT : LEFT;
          kid[id].controllable = false;
          shadow_id = id;
        }
      }

      /* make the kid's shadow run to the right until it disappears
         from view */
      if (shadow_id != -1) {
        struct anim *ks = &kid[shadow_id];
        p = (struct pos) {11,0,2};
        survey (_m, pos, &ks->f, &nc, &pm, &np);
        if (! peq (&pm, &p)) ks->key.right = true;
        else {
          destroy_kid (ks);
          shadow_id = -1;
        }
      }
  }
}

static void
end (void)
{
  static ALLEGRO_SAMPLE_INSTANCE *si = NULL;

  if (! played_end_sample) {
    switch (level.number) {
    case 1: case 2: si = play_sample (success_sample, -1); break;
    default: break;
    }
    played_end_sample = true;
  }

  if (! is_playing_sample (si)) quit_anim = NEXT_LEVEL;
}

static void
next_level (int number, struct pos *exit_door_pos)
{
  if (number > 14) number = 1;
  load_legacy_level (number);
}

static void
load_legacy_level (int number)
{
  char *lvfn;
  xasprintf (&lvfn, "data/legacy-levels/%02d", number);

  FILE *lvf;
  lvf = fopen (lvfn, "r");
  if (! lvf) xerror (-1, 0, "cannot read legacy level file %s", lvfn);
  fread (&lv, sizeof (lv), 1, lvf);
  fclose (lvf);

  al_free (lvfn);

  struct pos p;

  memset (&legacy_level, 0, sizeof (legacy_level));
  legacy_level.number = number;
  if (number == 12 || number == 13)
    legacy_level.nominal_number = 12;
  else if (number == 14) legacy_level.nominal_number = -1;
  else legacy_level.nominal_number = number;
  legacy_level.start = start;
  legacy_level.special_events = special_events;
  legacy_level.end = end;
  legacy_level.next_level = next_level;
  memcpy (&legacy_level.con[0], &room_0, sizeof (room_0));

  /* LINKS: ok */
  for (p.room = 1; p.room <= LROOMS; p.room++) {
    legacy_level.link[p.room].l = lv.link[p.room - 1][LD_LEFT];
    legacy_level.link[p.room].r = lv.link[p.room - 1][LD_RIGHT];
    legacy_level.link[p.room].a = lv.link[p.room - 1][LD_ABOVE];
    legacy_level.link[p.room].b = lv.link[p.room - 1][LD_BELOW];
  }

  /* enable npos to be used from now on */
  memcpy (&level.link, &legacy_level.link, sizeof (level.link));

  for (p.room = 1; p.room <= LROOMS; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++) {
        struct con *c = &legacy_level.con[p.room][p.floor][p.place];

        uint8_t f = lv.foretable[p.room - 1][p.floor][p.place];
        uint8_t b = lv.backtable[p.room - 1][p.floor][p.place];

        bool mlf = f & 0x20 ? true : false;   /* loose floor modifier */
        enum ltile t = get_tile (&p);
        enum lgroup g = get_group (t);

        switch (t) {
        case LT_EMPTY: c->fg = NO_FLOOR; break;
        case LT_FLOOR: c->fg = FLOOR; break;
        case LT_SPIKES: c->fg = SPIKES_FLOOR; break;
        case LT_PILLAR: c->fg = PILLAR; break;
        case LT_GATE: c->fg = DOOR; c->bg = NO_BRICKS; break;
        case LT_STUCK_BUTTON: c->fg = STUCK_FLOOR; break;
        case LT_DROP_BUTTON: c->fg = CLOSER_FLOOR; break;
        case LT_TAPESTRY: c->fg = CARPET; break;
        case LT_BOTTOM_BIG_PILLAR: c->fg = BIG_PILLAR_BOTTOM; break;
        case LT_TOP_BIG_PILLAR: c->fg = BIG_PILLAR_TOP; c->bg = NO_BRICKS; break;
        case LT_POTION: c->fg = FLOOR; break;
        case LT_LOOSE_BOARD: c->fg = LOOSE_FLOOR; c->ext.cant_fall = mlf; break;
        case LT_TAPESTRY_TOP: c->fg = TCARPET; c->bg = NO_BRICKS; break;
        case LT_MIRROR: c->fg = MIRROR; break;
        case LT_DEBRIS: c->fg = BROKEN_FLOOR; break;
        case LT_RAISE_BUTTON: c->fg = OPENER_FLOOR; break;
        case LT_EXIT_LEFT: c->fg = FLOOR; c->bg = NO_BRICKS; break;
        case LT_EXIT_RIGHT: c->fg = LEVEL_DOOR; break;
        case LT_CHOPPER: c->fg = CHOPPER; break;
        case LT_TORCH: c->fg = FLOOR; c->bg = TORCH; break;
        case LT_WALL: c->fg = WALL; break;
        case LT_SKELETON: c->fg = SKELETON_FLOOR; break;
        case LT_SWORD: c->fg = FLOOR; c->ext.item = SWORD; break;
        case LT_BALCONY_LEFT: c->fg = FLOOR; c->bg = NO_BRICKS; break;
        case LT_BALCONY_RIGHT: c->fg = FLOOR; c->bg = BALCONY; break;
        case LT_LATTICE_PILLAR: c->fg = ARCH_BOTTOM; break;
        case LT_LATTICE_SUPPORT: c->fg = ARCH_TOP_MID; c->bg = NO_BRICKS; break;
        case LT_SMALL_LATTICE: c->fg = ARCH_TOP_SMALL; c->bg = NO_BRICKS; break;
        case LT_LATTICE_LEFT: c->fg = ARCH_TOP_LEFT; c->bg = NO_BRICKS; break;
        case LT_LATTICE_RIGHT: c->fg = ARCH_TOP_RIGHT; c->bg = NO_BRICKS; break;
        case LT_TORCH_WITH_DEBRIS: c->fg = BROKEN_FLOOR; c->bg = TORCH; break;
        case LT_NULL: break;    /* needless */
        default:
          xerror (-1, 0, "%s: unknown tile group (%i) at position (%i, %i, %i, %i)",
                 __func__, t, number, p.room, p.floor, p.place);
        }

        /* printf ("(%i, %i, %i): TILE!!!\n", */
        /*         p.room, p.floor, p.place); */

        struct pos pl; prel (&p, &pl, +0, -1);

        switch (g) {
        case LG_NONE: break;    /* ok */
        case LG_FREE:           /* ok */
          switch (b) {
          case LM_FREE_NOTHING_DUNGEON_BLUE_LINE_PALACE: /* ok */
            c->bg = (t == LT_EMPTY) ? NO_BRICKS : NO_BG; break;
          case LM_FREE_SPOT1_DUNGEON_NO_BLUE_LINE_PALACE: /* ok */
            c->bg = (t == LT_EMPTY) ? BRICKS_03 : BRICKS_01; break;
          case LM_FREE_SPOT2_DUNGEON_BLUE_LINE2_PALACE: /* ok */
            c->bg = (t == LT_EMPTY) ? BRICKS_04 : BRICKS_02; break;
          case LM_FREE_WINDOW: c->bg = WINDOW; break;
          case LM_FREE_SPOT3_DUNGEON_BLUE_LINE_PALACE: /* ok */
            c->bg = (t == LT_EMPTY) ? NO_BRICKS : NO_BG; break;
          }
          break;
        case LG_SPIKE:          /* ok */
          switch (b) {
          case LM_SPIKE_NORMAL: c->ext.step = 0; break;
          case LM_SPIKE_BARELY_OUT_1: c->ext.step = 1; break;
          case LM_SPIKE_HALF_OUT_1: c->ext.step = 2; break;
          case LM_SPIKE_FULLY_OUT_1: c->ext.step = 3; break;
          case LM_SPIKE_FULLY_OUT_2: c->ext.step = 4; break;
          case LM_SPIKE_OUT_1: c->ext.step = 5; break;
          case LM_SPIKE_OUT_2: c->ext.step = 6; break;
          case LM_SPIKE_HALF_OUT_2: c->ext.step = 7; break;
          case LM_SPIKE_BARELY_OUT_2: c->ext.step = 8; break;
          case LM_SPIKE_DISABLED: c->ext.step = 9; break;
            /* needless */
          case LM_SPIKE_WEIRD_1: break;
          case LM_SPIKE_WEIRD_2: break;
          case LM_SPIKE_WEIRD_3: break;
          }
          break;
        case LG_GATE:           /* ok */
          switch (b) {
          case LM_GATE_CLOSED: default: c->ext.step = DOOR_MAX_STEP; break;
          case LM_GATE_OPEN: c->ext.step = 0; break;
          }
          break;
        case LG_TAPEST:         /* ok */
          switch (b) {
          case LM_TAPEST_WITH_LATTICE: c->ext.design = ARCH_CARPET_LEFT; break;
          case LM_TAPEST_ALTERNATIVE_DESIGN: c->ext.design = CARPET_01; break;
          case LM_TAPEST_NORMAL: c->ext.design = CARPET_02; break;
          case LM_TAPEST_BLACK: break; /* needless */
            /* needless */
          case LM_TAPEST_WEIRD_1: break;
          case LM_TAPEST_WEIRD_2: break;
          case LM_TAPEST_WEIRD_3: break;
          case LM_TAPEST_WEIRD_4: break;
          case LM_TAPEST_WEIRD_5: break;
          case LM_TAPEST_WEIRD_6: break;
          case LM_TAPEST_WEIRD_7: break;
          }
          break;
        case LG_POTION:         /* ok */
          switch (b) {
          case LM_POTION_EMPTY: c->ext.item = EMPTY_POTION; break;
          case LM_POTION_HEALTH_POINT: c->ext.item = SMALL_LIFE_POTION; break;
          case LM_POTION_LIFE: c->ext.item = BIG_LIFE_POTION; break;
          case LM_POTION_FEATHER_FALL: c->ext.item = FLOAT_POTION; break;
          case LM_POTION_INVERT: c->ext.item = FLIP_POTION; break;
          case LM_POTION_POISON: c->ext.item = SMALL_POISON_POTION; break;
          case LM_POTION_OPEN: c->ext.item = ACTIVATION_POTION; break;
          }
          break;
        case LG_TTOP:           /* ok */
          switch (b) {
          case LM_TTOP_WITH_LATTICE: c->ext.design = ARCH_CARPET_LEFT; break;
          case LM_TTOP_ALTERNATIVE_DESIGN:
            c->ext.design = get_tile (&pl) == LT_LATTICE_SUPPORT ?
              ARCH_CARPET_RIGHT_01 : CARPET_01; break;
          case LM_TTOP_NORMAL:
            c->ext.design = get_tile (&pl) == LT_LATTICE_SUPPORT ?
              ARCH_CARPET_RIGHT_02 : CARPET_02; break;
            /* needless */
          case LM_TTOP_BLACK_01: break;
          case LM_TTOP_BLACK_02: break;
          case LM_TTOP_WITH_ALTERNATIVE_DESIGN_AND_BOTTOM: break;
          case LM_TTOP_WITH_BOTTOM: break;
          case LM_TTOP_WITH_WINDOW: break;
          case LM_TTOP_WEIRD_1: break;
          case LM_TTOP_WEIRD_2: break;
          case LM_TTOP_WEIRD_3: break;
          case LM_TTOP_WEIRD_4: break;
          case LM_TTOP_WEIRD_5: break;
          case LM_TTOP_WEIRD_6: break;
          case LM_TTOP_WEIRD_7: break;
          }
          break;
        case LG_CHOMP:          /* ok */
          switch (b & 0x7F) {
          case LM_CHOMP_NORMAL: c->ext.step = 0; break;
          case LM_CHOMP_HALF_OPEN: c->ext.step = 1; break;
          case LM_CHOMP_CLOSED: c->ext.step = 2; break;
          case LM_CHOMP_PARTIALLY_OPEN: c->ext.step = 3; break;
          case LM_CHOMP_EXTRA_OPEN: c->ext.step = 4; break;
          case LM_STUCK_OPEN: c->ext.step = 5; break;
          }
          c->ext.step |= (b & 0x80); /* bloody status */
          break;
        case LG_WALL:           /* ok */
          switch (b) {
          case LM_WALL_MARK: c->bg = NO_BG; break;
          case LM_WALL_NO_MARK: c->bg = NO_BRICKS; break;
          }
          break;
        case LG_EXIT:           /* ok */
          c->ext.step = LEVEL_DOOR_MAX_STEP;
          switch (b) {
          case LM_EXIT_HALF_OPEN: c->ext.step = 43; break;
            /* needless */
          case LM_EXIT_MORE_OPEN_1:
          case LM_EXIT_MORE_OPEN_2:
          case LM_EXIT_MORE_OPEN_3:
          case LM_EXIT_MORE_OPEN_4:
          case LM_EXIT_MORE_OPEN_5:
          case LM_EXIT_MORE_OPEN_6: break;
          case LM_EXIT_EVEN_MORE_OPEN: break;
          case LM_EXIT_MOST_OPEN: break;
          }
          break;
        case LG_EVENT: c->ext.event = b; break; /* ok */
        default:
          xerror (-1, 0, "%s: unknown tile group (%i) at position (%i, %i, %i, %i)",
                 __func__, g, number, p.room, p.floor, p.place);
        }
      }

  /* EVENTS: ok */
  int i;
  for (i = 0; i < LEVENTS; i++) {
    struct level_event *e = &legacy_level.event[i];
    e->p.room = (lv.door_2[i] >> 3) | ((lv.door_1[i] & 0x60) >> 5);
    int l = lv.door_1[i] & 0x1F;
    e->p.floor = l / PLACES;
    e->p.place = l % PLACES;
    if (get_tile (&e->p) == LT_EXIT_LEFT) e->p.place++;
    e->next = lv.door_1[i] & 0x80 ? false : true;
  }

  /* START POSITION: ok */
  struct pos *sp = &legacy_level.start_pos;
  sp->room = lv.start_position[0];
  sp->floor = lv.start_position[1] / PLACES;
  sp->place = lv.start_position[1] % PLACES;

  /* START DIRECTION: ok */
  enum dir *sd = &legacy_level.start_dir;
  *sd = lv.start_position[2] ? LEFT : RIGHT;
}

static enum ltile
get_tile (struct pos *p)
{
  struct pos np; npos (p, &np);
  return lv.foretable[np.room - 1][np.floor][np.place] & 0x1F;
}

static enum lgroup
get_group (enum ltile t)
{
  switch (t) {
  case LT_PILLAR: case LT_STUCK_BUTTON: case LT_BOTTOM_BIG_PILLAR:
  case LT_TOP_BIG_PILLAR: case LT_LOOSE_BOARD: case LT_MIRROR:
  case LT_DEBRIS: case LT_TORCH: case LT_SKELETON: case LT_SWORD:
  case LT_BALCONY_LEFT: case LT_BALCONY_RIGHT: case LT_LATTICE_PILLAR:
  case LT_LATTICE_SUPPORT: case LT_SMALL_LATTICE: case LT_LATTICE_LEFT:
  case LT_LATTICE_RIGHT: case LT_TORCH_WITH_DEBRIS: case LT_NULL:
    return LG_NONE;
  case LT_EMPTY: case LT_FLOOR: return LG_FREE;
  case LT_SPIKES: return LG_SPIKE;
  case LT_GATE: return LG_GATE;
  case LT_TAPESTRY: return LG_TAPEST;
  case LT_POTION: return LG_POTION;
  case LT_TAPESTRY_TOP: return LG_TTOP;
  case LT_CHOPPER: return LG_CHOMP;
  case LT_WALL: return LG_WALL;
  case LT_EXIT_LEFT: case LT_EXIT_RIGHT: return LG_EXIT;
  case LT_DROP_BUTTON: case LT_RAISE_BUTTON: return LG_EVENT;
  default:
    xerror (-1, 0, "%s: unknown tile (%i)", __func__, t);
  }
  return LG_NONE;
}
