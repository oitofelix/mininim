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

#include "mininim.h"

static int level_3_checkpoint;
static int shadow_id;
static int skeleton_id;
static bool played_sample;
static uint64_t mouse_timer;
static int mouse_id;
static bool coming_from_12;
static bool shadow_merged;
static bool met_jaffar;
static bool played_vizier_death_sample;

static int life_table[] = {4, 3, 3, 3, 3, 4, 5, 4, 4, 5, 5, 5, 4, 6, 0, 0};

struct legacy_level lv;

static enum ltile get_tile (struct pos *p);
static enum lgroup get_group (enum ltile t);

static struct legacy_level *load_legacy_level_file (int n);

void
legacy_level_start (void)
{
  /* initialize some state */
  played_sample = false;
  shadow_id = mouse_id = skeleton_id = -1;
  stop_audio_instances ();
  mouse_timer = 0;
  shadow_merged = false;
  met_jaffar = false;
  played_vizier_death_sample = false;

  if (coming_from_12) auto_rem_time_1st_cycle = -1;
  else auto_rem_time_1st_cycle = 24;

  /* get kid */
  struct anim *k = get_anim_by_id (0);

  if (coming_from_12) k->current_lives = current_lives;
  else k->current_lives = total_lives;

  /* make the kid turn as appropriate */
  if (global_level.n == 13) {
    k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT;
    k->next_action = kid_run;
    frame2room (&k->f, roomd (k->f.c.l, 23, RIGHT), &k->f.c);
  } else if (global_level.n == 1)
    k->next_action = kid_turn;
  else {
    k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT;
    k->next_action = kid_turn;
  }

  /* define camera's starting room */
  if (global_level.n == 7) {
    mr_center_room (1);
    camera_follow_kid = -1;
  } else if (global_level.n == 13) {
    mr_center_room (23);
    camera_follow_kid = -1;
  } else {
    mr_center_room (k->f.c.room);
    camera_follow_kid = k->id;
  }

  /* if in level 14 stop the timer */
  if (global_level.n == 14) play_time_stopped = true;

  /* in the first level */
  if (global_level.n == 1) {
    /* activate tile, usually to close the opened door in the starting
       room */
    struct pos p; new_pos (&p, &global_level, 5, 0, 2);
    activate_con (&p);
    /* if it's the first try make kid wait before uncouching */
    if (retry_level != 1) k->uncouch_slowly = true;
  }

  /* in the third level */
  if (global_level.n == 3) {
    /* if it's the first time playing the checkpoint hasn't been
       reached yet */
    if (retry_level != 3) level_3_checkpoint = false;
    /* if the checkpoint has been reached, respawn there */
    struct pos p; new_pos (&p, &global_level, 2, 0, 6);
    if (level_3_checkpoint) {
      struct pos plf; new_pos (&plf, &global_level, 7, 0, 4);
      register_con_undo (&undo, &plf,
                         NO_FLOOR, MIGNORE, MIGNORE,
                         true, true, false, true,
                         CHPOS_LOOSE_FLOOR_FALL,
                         "NO FLOOR");
      k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT;
      place_frame (&k->f, &k->f, kid_normal_00, &p,
                   k->f.dir == LEFT ? +22 : +31, +15);
      mr_center_room (2);
    }
  }

  /* in the tenth level unflip the screen vertically, (helpful if the
     kid has not drank the potion that would do it on its own) */
  if (global_level.n == 10) screen_flags &= ~ ALLEGRO_FLIP_VERTICAL;

  /* level 13 adjustements */
  coming_from_12 = false;
  if (global_level.n == 13 && retry_level == 13)
    global_level.nominal_n = 12;

  if (global_level.n == 13) {
    struct anim *v = get_anim_by_id (1);
    v->fight = false;
  }
}

void
legacy_level_special_events (void)
{
  struct pos p, pm, pms;
  struct coord m, ms;
  struct anim *k = get_anim_by_id (current_kid_id);

  /* in the first animation cycle */
  if (anim_cycle == 0 || anim_cycle == 1) {
    /* close level door the kid came from */
    if (fg (&global_level.start_pos) == LEVEL_DOOR) {
      struct level_door *ld = level_door_at_pos (&global_level.start_pos);
      if (anim_cycle == 0) ld->i = 0;
      if (anim_cycle == 1) ld->action = CLOSE_LEVEL_DOOR;
    }
  }

  /* in the first level, first try, play the suspense sound */
  if (global_level.n == 1 && anim_cycle == 12 && retry_level != 1)
    play_audio (&suspense_audio, NULL, -1);

  /* in the third level */
  if (global_level.n == 3) {

    /* level 3 checkpoint */
    new_pos (&p, &global_level, 2, 0, 8);
    survey (_m, pos, &k->f, NULL, &pm, NULL);
    if (peq (&pm, &p)) {
      level_3_checkpoint = true;
      total_lives = k->total_lives;
      current_lives = k->current_lives;
      skill = k->skill;
    }

    struct anim *s = NULL;

    /* raise the skeleton as soon as the exit door is open and the
       kid reaches the second place of the room 1 */
    struct pos skeleton_floor_pos;
    new_pos (&skeleton_floor_pos, &global_level, 1, 1, 5);
    survey (_m, pos, &k->f, NULL, &pm, NULL);
    if (pm.room == 1
        && (pm.place == 2 || pm.place == 3)
        && fg (&skeleton_floor_pos) == SKELETON_FLOOR
        && get_exit_level_door (&global_level, 0)) {
      register_con_undo (&undo, &skeleton_floor_pos,
                         FLOOR, MIGNORE, MIGNORE,
                         true, true, false, true,
                         -1, "FLOOR");
      skeleton_id = create_anim (NULL, SKELETON, &skeleton_floor_pos, LEFT);
      s = &anima[skeleton_id];
      get_legacy_skill (2, &s->skill);
      s->has_sword = true;
      s->total_lives = INT_MAX;
      s->current_lives = INT_MAX;
      s->dont_draw_lives = true;
      s->p = skeleton_floor_pos;
      s->refraction = 16;
      raise_skeleton (s);
    }

    /* when the skeleton falls into room 3, place him at the center of
       the screen waiting for the kid */
    s = get_anim_by_id (skeleton_id);
    if (s) {
      survey (_m, pos, &s->f, NULL, &pm, NULL);
      new_pos (&p, &global_level, 3, 1, 4);
      if (s->f.c.room == 3 && pm.floor == 0
          && is_guard_fall (&s->f)) {
        s->f.dir = RIGHT;
        s->action = guard_normal;
        place_frame (&s->f, &s->f, skeleton_normal_00, &p, +16, +20);
      }
    }
  }

  /* in the fourth level */
  if (global_level.n == 4) {
    struct pos mirror_pos; new_pos (&mirror_pos, &global_level, 4, 0, 4);

    /* if the level door is open and the camera is on room 4, make
       the mirror appear */
    if (is_pos_visible (&mirror_pos)
        && fg (&mirror_pos) != MIRROR
        && get_exit_level_door (&global_level, 0)) {
      register_con_undo (&undo, &mirror_pos,
                         MIRROR, MIGNORE, MIGNORE,
                         true, true, true, true,
                         -1, "MIRROR");
      play_audio (&suspense_audio, &mirror_pos, -1);
    }

    /* if the kid is crossing the mirror, make his shadow appear */
    struct mirror *m;
    if (fg (&mirror_pos) == MIRROR
        && (m = mirror_at_pos (&mirror_pos))
        && is_valid_pos (&k->cross_mirror_p)
        && peq (&k->ci.con_p, &mirror_pos)
        && shadow_id == -1) {
      k->current_lives = 1;
      int id = create_anim (k, 0, NULL, 0);
      struct anim *ks = &anima[id];
      ks->fight = false;
      invert_frame_dir (&ks->f, &ks->f);
      ks->controllable = false;
      ks->dont_draw_lives = true;
      ks->immortal = true;
      ks->shadow = true;
      shadow_id = id;
    } else m = NULL;

    /* make the kid's shadow run to the right until he disappears
       from view */
    if (shadow_id != -1) {
      struct anim *ks = get_anim_by_id (shadow_id);
      if (is_frame_visible_at_room (&ks->f, mirror_pos.room))
        ks->key.right = true;
      else {
        destroy_anim (ks);
        shadow_id = -1;
      }
    }
  }

  /* in the fifth level */
  if (global_level.n == 5) {
    struct pos door_pos; new_pos (&door_pos, &global_level, 24, 0, 1);
    struct pos potion_pos; new_pos (&potion_pos, &global_level, 24, 0, 3);
    struct pos shadow_pos; new_pos (&shadow_pos, &global_level, 24, 0, -1);

    /* if there is a door sufficiently open, and a potion in room 24,
       and the camera is there, create a kid's shadow to drink the
       potion */
    if (is_pos_visible (&potion_pos)
        && shadow_id == -1
        && fg (&door_pos) == DOOR
        && is_potion (&potion_pos)
        && door_at_pos (&door_pos)->i <= 25) {
      int id = create_anim (k, 0, NULL, 0);
      struct anim *ks = &anima[id];
      ks->shadow = true;
      ks->fight = false;
      ks->f.dir = RIGHT;
      ks->controllable = false;
      ks->action = kid_run;
      ks->i = -1;
      ks->immortal = true;
      ks->dont_draw_lives = true;
      place_frame (&ks->f, &ks->f, kid_normal_00, &shadow_pos,
                   +0, +15);
      shadow_id = id;
    }

    /* if the kid's shadow has been created, make him run until he
       reaches the potion, and then drink it.  Make him turn back
       running until he gets out of view */
    if (shadow_id != -1) {
      struct anim *ks = get_anim_by_id (shadow_id);
      if (is_potion (&potion_pos)) {
        survey (_m, pos, &ks->f, NULL, &pm, NULL);
        pos2room (&pm, 24, &pm);
        if (pm.place < potion_pos.place - 1) ks->key.right = true;
        else ks->key.shift = true;
      } else if (is_frame_visible_at_room (&ks->f, potion_pos.room))
        ks->key.left = true;
      else {
        destroy_anim (ks);
        shadow_id = -1;
      }
    }
  }

  /* in the sixth level */
  if (global_level.n == 6) {
    struct anim *ks;

    /* create kid's shadow to wait for kid at room 1 */
    if (shadow_id == -1) {
      struct pos shadow_pos; new_pos (&shadow_pos, &global_level, 1, 1, 1);
      int id = create_anim (k, 0, NULL, 0);
      ks = &anima[id];
      ks->fight = false;
      ks->shadow = true;
      ks->f.dir = RIGHT;
      ks->controllable = false;
      ks->action = kid_normal;
      ks->dont_draw_lives = true;
      ks->immortal = true;
      place_frame (&ks->f, &ks->f, kid_normal_00, &shadow_pos,
                   +9, +15);
      shadow_id = id;
    } else ks = get_anim_by_id (shadow_id);

    /* when kid enters room 1, play the suspense sound */
    if (k->f.c.room == 1
        && ! played_sample) {
      play_audio (&suspense_audio, NULL, k->id);
      played_sample = true;
    }

    /* if kid opens the door and jumps to reach the other side of room
       1, make his shadow step over the closer floor */
    struct pos door_pos; new_pos (&door_pos, &global_level, 1, 1, 2);
    if (k->f.c.room == 1
        && k->action == kid_run_jump
        && k->i == 7
        && fg (&door_pos) == DOOR
        && door_at_pos (&door_pos)->i < DOOR_STEPS - 1) {
      ks->key.right = true;
      ks->key.shift = true;
    }

    /* when kid falls from room 1 to the room below it, quit to the
       next level */
    if (k->f.c.room == roomd (&global_level, 1, BELOW)
        && k->f.c.prev_room == 1
        && k->f.c.xd == BELOW) {
      total_lives = k->total_lives;
      current_lives = k->current_lives;
      skill = k->skill;
      next_level = global_level.n + 1;
      quit_anim = NEXT_LEVEL;
    }
  }

  /* in the eighth level */
  if (global_level.n == 8) {
    struct pos mouse_pos; new_pos (&mouse_pos, &global_level, 16, 0, 12);
    struct anim *m = NULL;

    if (mouse_id != -1) m = get_anim_by_id (mouse_id);

    /* if the exit level door is open and the kid is at room 16,
       start counting (or continue if started already) for the mouse
       arrival */
    if (k->f.c.room == 16 && mouse_timer <= 138
        && get_exit_level_door (&global_level, 0)) mouse_timer++;

    /* if enough cycles have passed since the start of the countdown
       and the camera is at room 16, make the mouse appear */
    if (mouse_timer == 138 && is_room_visible (mouse_pos.room)) {
      mouse_id = create_anim (NULL, MOUSE, &mouse_pos, RIGHT);
      m = &anima[mouse_id];
      m->f.flip = ALLEGRO_FLIP_HORIZONTAL;
    }

    /* make the mouse disapear as soon as it goes out of view */
    if (m && m->action == mouse_run && m->f.dir == RIGHT
        && ! is_frame_visible_at_room (&m->f, mouse_pos.room)) {
        destroy_anim (m);
        mouse_id = -1;
    }
  }

  /* in the twelfth level */
  if (global_level.n == 12) {
    struct pos sword_pos; new_pos (&sword_pos, &global_level, 15, 0, 1);
    struct pos first_hidden_floor_pos;
    new_pos (&first_hidden_floor_pos, &global_level, 2, 0, 7);
    struct anim *ks = NULL;

    /* make the sword in room 15 disappear (kid's shadow has it) when
       the kid leaves room 18 to the right */
    if (k->f.c.room == roomd (&global_level, 18, RIGHT)
        && ext (&sword_pos) == SWORD)
      set_ext (&sword_pos, NO_ITEM);

    /* make shadow fall in kid's front */
    survey (_m, pos, &k->f, NULL, &pm, NULL);
    if (shadow_id == -1
        && ext (&sword_pos) != SWORD
        && pm.room == 15 && pm.floor == 0 && pm.place < 6
        && ! shadow_merged) {
      struct pos shadow_pos;
      new_pos (&shadow_pos, &global_level, roomd (&global_level, 15, ABOVE), 2, 1);
      shadow_id = create_anim (NULL, SHADOW, &shadow_pos, RIGHT);
      ks = &anima[shadow_id];
      ks->fight = true;
      ks->controllable = false;
      ks->refraction = 12;
      ks->current_lives = k->current_lives;
      ks->total_lives = k->total_lives;
      ks->action = guard_normal;
      ks->f.dir = RIGHT;
      ks->glory_sample = true;
      get_legacy_skill (3, &ks->skill);
      guard_fall (ks);
      struct frameset *frameset = get_guard_fall_frameset (ks->type);
      place_frame (&ks->f, &ks->f, frameset[0].frame, &shadow_pos,
                   +9, +15);
    } else if (shadow_id != -1) ks = get_anim_by_id (shadow_id);

    /* if shadow has appeared but not merged yet */
    struct audio_instance *ai;
    if (ks) {
      k->death_reason = ks->death_reason = SHADOW_FIGHT_DEATH;
      survey (_m, pos, &ks->f, &ms, &pms, NULL);
      survey (_m, pos, &k->f, &m, &pm, NULL);
      ks->p = pms;
      k->p = pm;

      ks->keep_sword_fast = k->keep_sword_fast = false;

      /* any harm caused to the shadow reflects to the kid */
      if (ks->action == guard_hit && ks->i == 0
          && is_attacking (k)) {
        mr.flicker = 2;
        mr.color = get_flicker_blood_color ();
        play_audio (&harm_audio, NULL, k->id);
        k->splash = true;
        k->current_lives--;
        kid_sword_hit (k);
      }

      /* any harm caused to the kid reflects to the shadow */
      if (k->action == kid_sword_hit && k->i == 0
          && is_attacking (ks)) {
        mr.flicker = 2;
        mr.color = get_flicker_blood_color ();
        play_audio (&guard_hit_audio, NULL, ks->id);
        ks->splash = true;
        ks->current_lives--;
        guard_hit (ks);
      }

      /* if the shadow dies, the kid dies */
      if (ks->current_lives <= 0 && k->current_lives > 0) {
        k->splash = true;
        kid_die (k);
      }

      /* if the kid dies, the shadow dies */
      if (k->current_lives <= 0 && ks->current_lives > 0) {
        ks->splash = true;
        guard_die (ks);
      }

      /* make the shadow disappear when the kid is dead */
      struct audio_instance *ai;
      if (k->current_lives <= 0
          && (ai = search_audio_instance (&success_suspense_audio,
                                          0, NULL, k->id))
          && get_audio_instance_position (ai->data) >= 3.3
          && ! ks->invisible) {
        mr.flicker = 8;
        mr.color = WHITE;
        ks->invisible = true;
      }

      /* if the kid keep his sword, the shadow does the same */
      if (k->action == kid_keep_sword && ks->type == SHADOW) {
        ks->type = KID;
        ks->controllable = true;
        kid_keep_sword (ks);
        place_on_the_ground (&ks->f, &ks->f.c);
        ks->f.c.x += (ks->f.dir == LEFT) ? +8 : -8;
      }

      /* if the kid change his mind and take the sword again, the
         shadow becomes offensive again too */
      if (k->action == kid_take_sword && ks->type == KID) {
        ks->type = SHADOW;
        ks->controllable = false;
        guard_normal (ks);
        place_on_the_ground (&ks->f, &ks->f.c);
      }

      /* if both, the kid and the shadow, are not in fight mode and
         get close enough, a merge happens */
      if (ks->type == KID && pms.room == pm.room && pms.floor == pm.floor
          && abs (ms.x - m.x) <= 8) {
        destroy_anim (ks);
        shadow_id = -1; ks = NULL;
        shadow_merged = true;
        k = get_anim_by_id (current_kid_id);
        play_audio (&success_audio, NULL, k->id);
        k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT;
        place_on_the_ground (&k->f, &k->f.c);
        kid_turn_run (k);
        k->current_lives = ++k->total_lives;
        mr.flicker = 8;
        mr.color = WHITE;
      }
      /* while the merge doesn't happen and neither the shadow nor the
         kid are in fight mode, the shadow's movements mirror the
         kid's */
      else if (ks->type == KID) {
        get_gamepad_state (&ks->key);
        bool l = ks->key.left;
        bool r = ks->key.right;
        ks->key.left = r;
        ks->key.right = l;
      }
      /* if the shadow and the kid has merged, one overlaps each other
         making them glow intermittently */
    } else if (shadow_merged
               && (ai = search_audio_instance (&success_audio, 0,
                                               NULL, k->id))
               && is_audio_instance_playing (ai->data))
      k->shadow = (anim_cycle % 2);
    /* after the success music has finished to play, the kid goes
       normal again */
    else if (shadow_merged) k->shadow = false;

    /* transform the empty space in hidden floors after the shadow and
       the kid has merged */
    if (shadow_merged
        && k->f.c.room == 2
        && fg (&first_hidden_floor_pos) == NO_FLOOR)
      for (new_pos (&p, &global_level, 2, 0, -4); p.place < PLACES;
           prel (&p, &p, +0, +1))
        if (fg (&p) == NO_FLOOR) set_fg (&p, HIDDEN_FLOOR);

    /* when the kid enters room 23, go to the next level */
    if (k->f.c.room == 23) {
      coming_from_12 = true;
      total_lives = k->total_lives;
      current_lives = k->current_lives;
      skill = k->skill;
      next_level = global_level.n + 1;
      quit_anim = NEXT_LEVEL;
    }
  }

  /* in the thirteenth level */
  if (global_level.n == 13) {

    /* make the top loose floors fall spontaneously */
    if (k->f.c.room == 16 || k->f.c.room == 23) {
      struct pos p; new_pos (&p, &global_level, k->f.c.room, -1, 0);
      p.place = prandom (9);
      activate_con (&p);
    }

    struct anim *v = get_anim_by_id (1);
    if (v) {
      /* play a special tune when meeting the vizier for the first
         time */
      if (k->f.c.room == v->f.c.room
          && v->action == guard_normal
          && ! met_jaffar) {
        play_audio (&meet_vizier_audio, NULL, k->id);
        met_jaffar = true;
      }

      /* put the vizier in vigilance */
      struct audio_instance *ai;
      if ((ai = search_audio_instance (&meet_vizier_audio, 0,
                                       NULL, k->id))
          && get_audio_instance_position (ai->data) >= 2.2)
        v->fight = true;

      /* when the vizier dies do some video effect, play a tune, stop
         the play timer and display the remaining time */
      if (v->current_lives <= 0
          && ! played_vizier_death_sample) {
        mr.flicker = 12;
        mr.color = WHITE;
        stop_audio_instance (&meet_vizier_audio, NULL, k->id);
        play_audio (&vizier_death_audio, NULL, k->id);
        played_vizier_death_sample = true;
        play_time_stopped = true;
        display_remaining_time ();
      }

      /* after vizier's death activate certain tile in order to open
         the exit level door */
      struct pos p; new_pos (&p, &global_level, 24, 0, 0);
      if (v->current_lives <= 0
          && k->f.c.room != v->f.c.room)
        activate_con (&p);
    }
  }

  /* in the fourteenth level */
  if (global_level.n == 14) {
    /* when the kid enters room 5, go to the next level */
    if (k->f.c.room == 5) {
      total_lives = k->total_lives;
      current_lives = k->current_lives;
      skill = k->skill;
      next_level = global_level.n + 1;
      quit_anim = NEXT_LEVEL;
    }
  }
}

void
legacy_level_end (struct pos *p)
{
  struct anim *k = get_anim_by_id (current_kid_id);
  static union audio_instance_data ai_data
    = (union audio_instance_data) {.stream = NULL};

  /* end music samples to play per level */
  if (! played_sample) {
    stop_audio_instance (&floating_audio, NULL, k->id);
    switch (global_level.n) {
    case 1: case 2: case 3: case 5: case 6: case 7:
    case 8: case 9: case 10: case 11: case 12:
      ai_data = play_audio (&success_audio, NULL, k->id); break;
    case 4:
      ai_data = play_audio (&success_suspense_audio, NULL, k->id);
      break;
    case 13: break;
    }
    played_sample = true;
  }

  if (! is_audio_instance_playing (ai_data)) {
    next_level = global_level.n + 1;
    quit_anim = NEXT_LEVEL;
  }
}

struct level *
next_legacy_level (struct level *l, int n)
{
  if (n < 1) n = 14;
  else if (n > 14) n = 1;
  if (! load_legacy_level_file (n)) return NULL;
  return interpret_legacy_level (l, n);
}

static struct legacy_level *
load_legacy_level_file (int n)
{
  char *filename;
  xasprintf (&filename, "data/legacy-levels/%02d", n);

  ALLEGRO_FILE *lvf =
    load_resource (filename, (load_resource_f) xfopen_r);

  if (! lvf) {
    error (0, 0, "cannot read legacy level file %s", filename);
    return NULL;
  }

  al_fread (lvf, &lv, sizeof (lv));
  al_fclose (lvf);
  al_free (filename);
  return &lv;
}

struct level *
interpret_legacy_level (struct level *l, int n)
{
  struct pos p;
  new_pos (&p, l, -1, -1, -1);

  memset (l, 0, sizeof (*l));
  l->n = n;
  if (n == 12 || n == 13) {
    if (coming_from_12) l->nominal_n = -1;
    else l->nominal_n = 12;
  } else if (n == 14) l->nominal_n = -1;
  else l->nominal_n = n;
  l->start = legacy_level_start;
  l->special_events = legacy_level_special_events;
  l->end = legacy_level_end;
  l->next_level = next_legacy_level;

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

  /* SWORD: ok */
  l->has_sword = (n != 1);

  /* LINKS: ok */
  for (p.room = 1; p.room <= LROOMS; p.room++) {
    link_room (l, p.room, lv.link[p.room - 1][LD_LEFT], LEFT);
    link_room (l, p.room, lv.link[p.room - 1][LD_RIGHT], RIGHT);
    link_room (l, p.room, lv.link[p.room - 1][LD_ABOVE], ABOVE);
    link_room (l, p.room, lv.link[p.room - 1][LD_BELOW], BELOW);
  }

  /* FORETABLE and BACKTABLE: ok */
  for (p.room = 1; p.room <= LROOMS; p.room++)
    for (p.floor = 0; p.floor < LFLOORS; p.floor++)
      for (p.place = 0; p.place < LPLACES; p.place++) {
        uint8_t f = lv.foretable[p.room - 1][p.floor][p.place];
        uint8_t b = lv.backtable[p.room - 1][p.floor][p.place];

        bool mlf = f & 0x20 ? true : false; /* loose floor modifier */
        enum ltile t = get_tile (&p);
        enum lgroup g = get_group (t);

        switch (t) {
        case LT_EMPTY: set_fg (&p, NO_FLOOR); break;
        case LT_FLOOR: set_fg (&p, FLOOR); break;
        case LT_SPIKES: set_fg (&p, SPIKES_FLOOR); break;
        case LT_PILLAR: set_fg (&p, PILLAR); break;
        case LT_GATE: set_fg (&p, DOOR);
          set_bg (&p, NO_BRICKS); break;
        case LT_STUCK_BUTTON: set_fg (&p, STUCK_FLOOR); break;
        case LT_DROP_BUTTON: set_fg (&p, CLOSER_FLOOR); break;
        case LT_TAPESTRY: set_fg (&p, CARPET); break;
        case LT_BOTTOM_BIG_PILLAR: set_fg (&p, BIG_PILLAR_BOTTOM);
          break;
        case LT_TOP_BIG_PILLAR: set_fg (&p, BIG_PILLAR_TOP);
          set_bg (&p, NO_BRICKS); break;
        case LT_POTION: set_fg (&p, FLOOR); break;
        case LT_LOOSE_BOARD:
          set_fg (&p, LOOSE_FLOOR);
          set_ext (&p, mlf); break;
        case LT_TAPESTRY_TOP: set_fg (&p, TCARPET);
          set_bg (&p, NO_BRICKS); break;
        case LT_MIRROR: set_fg (&p, MIRROR); break;
        case LT_DEBRIS: set_fg (&p, BROKEN_FLOOR); break;
        case LT_RAISE_BUTTON: set_fg (&p, OPENER_FLOOR); break;
        case LT_EXIT_LEFT: set_fg (&p, FLOOR);
          set_bg (&p, NO_BRICKS); break;
        case LT_EXIT_RIGHT: set_fg (&p, LEVEL_DOOR);
          set_bg (&p, NO_BRICKS); break;
        case LT_CHOPPER: set_fg (&p, CHOPPER); break;
        case LT_TORCH: set_fg (&p, FLOOR);
          set_bg (&p, TORCH); break;
        case LT_WALL: set_fg (&p, WALL); break;
        case LT_SKELETON: set_fg (&p, SKELETON_FLOOR); break;
        case LT_SWORD:
          set_fg (&p, FLOOR);
          set_ext (&p, SWORD); break;
        case LT_BALCONY_LEFT:
          set_fg (&p, FLOOR);
          set_bg (&p, NO_BRICKS); break;
        case LT_BALCONY_RIGHT: set_fg (&p, FLOOR);
          set_bg (&p, BALCONY); break;
        case LT_LATTICE_PILLAR: set_fg (&p, ARCH_BOTTOM); break;
        case LT_LATTICE_SUPPORT: set_fg (&p, ARCH_TOP_MID);
          set_bg (&p, NO_BRICKS); break;
        case LT_SMALL_LATTICE: set_fg (&p, ARCH_TOP_SMALL);
          set_bg (&p, NO_BRICKS); break;
        case LT_LATTICE_LEFT: set_fg (&p, ARCH_TOP_LEFT);
          set_bg (&p, NO_BRICKS); break;
        case LT_LATTICE_RIGHT: set_fg (&p, ARCH_TOP_RIGHT);
          set_bg (&p, NO_BRICKS); break;
        case LT_TORCH_WITH_DEBRIS: set_fg (&p, BROKEN_FLOOR);
          set_bg (&p, TORCH); break;
        case LT_NULL: break;    /* needless */
        default:
          error (0, 0, "%s: unknown tile group (%i) at position (%i, %i, %i, %i)",
                 __func__, t, n, p.room, p.floor, p.place);
        }

        /* printf ("(%i, %i, %i): TILE!!!\n", */
        /*         p.room, p.floor, p.place); */

        struct pos pl; prel (&p, &pl, +0, -1);
        int step = 0;

        switch (g) {
        case LG_NONE: break;    /* ok */
        case LG_FREE:           /* ok */
          switch (b) {
          case LM_FREE_NOTHING_DUNGEON_BLUE_LINE_PALACE: /* ok */
            set_bg (&p, (t == LT_EMPTY) ? NO_BRICKS : NO_BG); break;
          case LM_FREE_SPOT1_DUNGEON_NO_BLUE_LINE_PALACE: /* ok */
            set_bg (&p, (t == LT_EMPTY) ? BRICKS_02 : BRICKS_00);
            break;
          case LM_FREE_SPOT2_DUNGEON_BLUE_LINE2_PALACE: /* ok */
            set_bg (&p, (t == LT_EMPTY) ? BRICKS_03 : BRICKS_01);
            break;
          case LM_FREE_WINDOW:
            set_bg (&p, WINDOW); break;
          case LM_FREE_SPOT3_DUNGEON_BLUE_LINE_PALACE: /* ok */
            set_bg (&p, (t == LT_EMPTY) ? NO_BRICKS : NO_BG); break;
          }
          break;
        case LG_SPIKE:          /* ok */
          switch (b) {
          case LM_SPIKE_NORMAL: set_ext (&p, 0); break;
          case LM_SPIKE_BARELY_OUT_1: set_ext (&p, 1); break;
          case LM_SPIKE_HALF_OUT_1: set_ext (&p, 2); break;
          case LM_SPIKE_FULLY_OUT_1: set_ext (&p, 3); break;
          case LM_SPIKE_FULLY_OUT_2: set_ext (&p, 4); break;
          case LM_SPIKE_OUT_1: set_ext (&p, 5); break;
          case LM_SPIKE_OUT_2: set_ext (&p, 6); break;
          case LM_SPIKE_HALF_OUT_2: set_ext (&p, 7); break;
          case LM_SPIKE_BARELY_OUT_2: set_ext (&p, 8); break;
          case LM_SPIKE_DISABLED: set_ext (&p, 9); break;
            /* needless */
          case LM_SPIKE_WEIRD_1: break;
          case LM_SPIKE_WEIRD_2: break;
          case LM_SPIKE_WEIRD_3: break;
          }
          break;
        case LG_GATE:           /* ok */
          switch (b) {
          case LM_GATE_CLOSED: default:
            set_ext (&p, DOOR_STEPS - 1); break;
          case LM_GATE_OPEN:
            set_ext (&p, 0); break;
          }
          break;
        case LG_TAPEST:         /* ok */
          switch (b) {
          case LM_TAPEST_WITH_LATTICE:
            set_ext (&p, ARCH_CARPET_LEFT_00); break;
          case LM_TAPEST_ALTERNATIVE_DESIGN:
            set_ext (&p, CARPET_00); break;
          case LM_TAPEST_NORMAL:
            set_ext (&p, CARPET_01); break;
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
          case LM_POTION_EMPTY: set_ext (&p, EMPTY_POTION); break;
          case LM_POTION_HEALTH_POINT: set_ext (&p, SMALL_LIFE_POTION); break;
          case LM_POTION_LIFE: set_ext (&p, BIG_LIFE_POTION); break;
          case LM_POTION_FEATHER_FALL: set_ext (&p, FLOAT_POTION); break;
          case LM_POTION_INVERT: set_ext (&p, FLIP_POTION); break;
          case LM_POTION_POISON: set_ext (&p, SMALL_POISON_POTION); break;
          case LM_POTION_OPEN: set_ext (&p, ACTIVATION_POTION); break;
          }
          break;
        case LG_TTOP:           /* ok */
          switch (b) {
          case LM_TTOP_WITH_LATTICE:
            set_ext (&p, ARCH_CARPET_LEFT_00); break;
          case LM_TTOP_ALTERNATIVE_DESIGN:
            set_ext (&p, get_tile (&pl) == LT_LATTICE_SUPPORT ?
                     ARCH_CARPET_RIGHT_00 : CARPET_00); break;
          case LM_TTOP_NORMAL:
            set_ext (&p, get_tile (&pl) == LT_LATTICE_SUPPORT ?
                     ARCH_CARPET_RIGHT_01 : CARPET_01); break;
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
          case LM_CHOMP_NORMAL: step = 0; break;
          case LM_CHOMP_HALF_OPEN: step = 1; break;
          case LM_CHOMP_CLOSED: step = 2; break;
          case LM_CHOMP_PARTIALLY_OPEN: step = 3; break;
          case LM_CHOMP_EXTRA_OPEN: step = 4; break;
          case LM_STUCK_OPEN: step = 5; break;
          }
          if (b & 0x80)
            set_ext (&p, step + CHOPPER_STEPS); /* bloody status */
          break;
        case LG_WALL:           /* ok */
          switch (b) {
          case LM_WALL_MARK:
            set_bg (&p, NO_BG); break;
          case LM_WALL_NO_MARK:
            set_bg (&p, NO_BRICKS); break;
          }
          break;
        case LG_EXIT:           /* ok */
          if (fg (&p) != LEVEL_DOOR) break;
          set_ext (&p, LEVEL_DOOR_STEPS - 1);
          switch (b) {
          case LM_EXIT_HALF_OPEN:
            set_ext (&p, 20); break;
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
        case LG_EVENT:
          set_ext (&p, b); break; /* ok */
        default:
          error (0, 0, "%s: unknown tile group (%i) at position (%i, %i, %i, %i)",
                 __func__, g, n, p.room, p.floor, p.place);
        }
      }

  /* EVENTS: ok */
  int i;
  for (i = 0; i < LEVENTS; i++) {
    struct level_event *e = event (l, i);
    int ld = lv.door_1[i] & 0x1F;
    new_pos (&e->p, l,
             (lv.door_2[i] >> 3) | ((lv.door_1[i] & 0x60) >> 5),
             ld / LPLACES, ld % LPLACES);
    if (get_tile (&e->p) == LT_EXIT_LEFT) e->p.place++;
    e->next = lv.door_1[i] & 0x80 ? false : true;
  }

  /* START POSITION: ok */
  struct pos *sp = &l->start_pos;
  sp->l = l;
  sp->room = lv.start_position[0];
  sp->floor = lv.start_position[1] / LPLACES;
  sp->place = lv.start_position[1] % LPLACES;

  /* START DIRECTION: ok */
  enum dir *sd = &l->start_dir;
  *sd = lv.start_position[2] ? LEFT : RIGHT;

  /* GUARD LOCATION, DIRECTION, SKILL and COLOR: ok */
  for (i = 0; i < LROOMS; i++) {
    struct guard *g = guard (l, i + 1);

    if (lv.guard_location[i] > 29) {
      g->type = NO_ANIM;
      continue;
    }

    /* TYPE AND STYLE: ok */
    switch (n) {
    case 3: g->type = SKELETON; g->style = 0; break;
    case 6: g->type = FAT_GUARD; g->style = 1; break;
    case 12: g->type = SHADOW; g->style = 0; break;
    case 13: g->type = VIZIER; g->style = 0; break;
    default: g->type = GUARD;
      g->style = lv.guard_color[i]; break;
    }

    /* LOCATION: ok */
    new_pos (&g->p, l, (i + 1),
             lv.guard_location[i] / LPLACES,
             lv.guard_location[i] % LPLACES);

    /* DIRECTION: ok */
    g->dir = lv.guard_direction[i] ? LEFT : RIGHT;

    /* SKILL: ok */
    get_legacy_skill (lv.guard_skill[i], &g->skill);
    g->total_lives = life_table[n];

    /* printf ("(%i, %i, %i), style: %i\n", */
    /*         g->p.room, g->p.floor, g->p.place, g->style); */
  }

  /* define the enviroment mode based on the level */
  switch (l->n) {
  case 4: case 5: case 6: case 10: case 11: case 14:
    l->em = PALACE; break;
  default: l->em = DUNGEON; break;
  }

  /* define hue palettes based on the level */
  switch (n) {
  default: l->hue = HUE_NONE; break;
  case 3: case 7: l->hue = HUE_GREEN; break;
  case 8: case 9: l->hue = HUE_GRAY; break;
  case 12: case 13: l->hue = HUE_YELLOW; break;
  case 14: l->hue = HUE_BLUE; break;
  }

  return l;
}

struct skill *
get_legacy_skill (int i, struct skill *skill)
{
  switch (i) {
  case 0: default:
    skill->attack_prob = 23;
    skill->counter_attack_prob = -1;
    skill->defense_prob = -1;
    skill->counter_defense_prob = -1;
    skill->advance_prob = 99;
    skill->return_prob = -1;
    skill->refraction = 16;
    skill->extra_life = 0;
    break;
  case 1:
    skill->attack_prob = 38;
    skill->counter_attack_prob = -1;
    skill->defense_prob = 58;
    skill->counter_defense_prob = 23;
    skill->advance_prob = 77;
    skill->return_prob = 21;
    skill->refraction = 16;
    skill->extra_life = 0;
    break;
  case 2:
    skill->attack_prob = 23;
    skill->counter_attack_prob = -1;
    skill->defense_prob = 58;
    skill->counter_defense_prob = 23;
    skill->advance_prob = 77;
    skill->return_prob = 21;
    skill->refraction = 16;
    skill->extra_life = 0;
    break;
  case 3:
    skill->attack_prob = 23;
    skill->counter_attack_prob = 1;
    skill->defense_prob = 77;
    skill->counter_defense_prob = 38;
    skill->advance_prob = 77;
    skill->return_prob = 21;
    skill->refraction = 16;
    skill->extra_life = 0;
    break;
  case 4:
    skill->attack_prob = 23;
    skill->counter_attack_prob = 1;
    skill->defense_prob = 77;
    skill->counter_defense_prob = 38;
    skill->advance_prob = 99;
    skill->return_prob = -1;
    skill->refraction = 8;
    skill->extra_life = 1;
    break;
  case 5:
    skill->attack_prob = 15;
    skill->counter_attack_prob = 68;
    skill->defense_prob = 99;
    skill->counter_defense_prob = 56;
    skill->advance_prob = 99;
    skill->return_prob = -1;
    skill->refraction = 8;
    skill->extra_life = 0;
    break;
  case 6:
    skill->attack_prob = 38;
    skill->counter_attack_prob = 5;
    skill->defense_prob = 77;
    skill->counter_defense_prob = 38;
    skill->advance_prob = 77;
    skill->return_prob = 21;
    skill->refraction = 8;
    skill->extra_life = 0;
    break;
  case 7:
    skill->attack_prob = 85;
    skill->counter_attack_prob = 2;
    skill->defense_prob = 97;
    skill->counter_defense_prob = 97;
    skill->advance_prob = -1;
    skill->return_prob = 99;
    skill->refraction = 8;
    skill->extra_life = 0;
    break;
  case 8:
    skill->attack_prob = -1;
    skill->counter_attack_prob = -1;
    skill->defense_prob = -1;
    skill->counter_defense_prob = -1;
    skill->advance_prob = -1;
    skill->return_prob = 99;
    skill->refraction = 0;
    skill->extra_life = 0;
    break;
  case 9:
    skill->attack_prob = 18;
    skill->counter_attack_prob = 99;
    skill->defense_prob = 99;
    skill->counter_defense_prob = 56;
    skill->advance_prob = 99;
    skill->return_prob = -1;
    skill->refraction = 8;
    skill->extra_life = 0;
    break;
  case 10:
    skill->attack_prob = 12;
    skill->counter_attack_prob = 99;
    skill->defense_prob = 99;
    skill->counter_defense_prob = 99;
    skill->advance_prob = 38;
    skill->return_prob = 60;
    skill->refraction = 0;
    skill->extra_life = 0;
    break;
  case 11:
    skill->attack_prob = 18;
    skill->counter_attack_prob = 58;
    skill->defense_prob = 99;
    skill->counter_defense_prob = 68;
    skill->advance_prob = 38;
    skill->return_prob = 60;
    skill->refraction = 0;
    skill->extra_life = 0;
  }

  return skill;
}

static enum ltile
get_tile (struct pos *p)
{
  if (! is_valid_pos (p)) return LT_NULL;
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
    error (0, 0, "%s: unknown tile (%i)", __func__, t);
  }
  return LG_NONE;
}
