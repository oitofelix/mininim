/*
  level.c -- level module;

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

/* functions */
static void process_death (void);
static void draw_hp (ALLEGRO_BITMAP *bitmap, struct actor *k);

/* variables */
struct level vanilla_level;
struct level global_level;

struct undo undo;

static int64_t last_auto_show_time;
static bool level_number_shown;

bool no_room_drawing;
bool game_paused;
int step_cycle = -1;
int retry_level = -1;
int camera_follow_kid;
int next_level_number = -1;
bool ignore_level_cutscene;
uint64_t death_timer;

struct level *
new_level (struct level *l, size_t room_nmemb, size_t event_nmemb,
           size_t guard_nmemb)
{
  l->room_nmemb = room_nmemb;
  l->tile = xcalloc (l->room_nmemb, sizeof (*l->tile));
  l->link = xcalloc (l->room_nmemb, sizeof (*l->link));

  l->event_nmemb = event_nmemb;
  l->event = xcalloc (l->event_nmemb, sizeof (*l->event));

  l->guard_nmemb = guard_nmemb;
  l->guard = xcalloc (l->guard_nmemb, sizeof (*l->guard));

  normalize_level (l);

  return l;
}

struct level *
copy_level (struct level *ld, struct level *ls)
{
  destroy_level (ld);

  *ld = *ls;

  /* tile */
  ld->tile = copy_array
    (ls->tile, ls->room_nmemb, &ld->room_nmemb, sizeof (*ld->tile));

  /* link */
  ld->link = copy_array
    (ls->link, ls->room_nmemb, &ld->room_nmemb, sizeof (*ld->link));

  /* events */
  ld->event = copy_array
    (ls->event, ls->event_nmemb, &ld->event_nmemb, sizeof (*ld->event));

  /* guards */
  ld->guard = copy_array
    (ls->guard, ls->guard_nmemb, &ld->guard_nmemb, sizeof (*ld->guard));

  normalize_level (ld);

  return ld;
}

void
destroy_level (struct level *l)
{
  destroy_array ((void **) &l->event, &l->event_nmemb);
  destroy_array ((void **) &l->guard, &l->guard_nmemb);
  destroy_array ((void **) &l->tile, &l->room_nmemb);
  destroy_array ((void **) &l->link, &l->room_nmemb);
}

struct level *
normalize_level (struct level *l)
{
  size_t i;

  fix_room_0 (l);
  fix_traversable_above_room_0 (l);

  if (l->start_dir != LEFT && l->start_dir != RIGHT)
    l->start_dir = LEFT;
  l->start_pos.l = l;
  npos (&l->start_pos, &l->start_pos);

  for (i = 0; i < l->event_nmemb; i++) {
    struct level_event *e = event (l, i);
    e->p.l = l;
    npos (&e->p, &e->p);
  }

  for (i = 0; i < l->guard_nmemb; i++) {
    struct guard *g = guard (l, i);
    g->p.l = l;
    if (g->dir != LEFT && g->dir != RIGHT)
      g->dir = LEFT;
    g->style = typed_int (g->style, 8, 1, NULL, NULL);
    npos (&g->p, &g->p);
  }
  return l;
}

bool
skill_eq (struct skill *s0, struct skill *s1)
{
  return s0->attack_prob == s1->attack_prob
    && s0->counter_attack_prob == s1->counter_attack_prob
    && s0->defense_prob == s1->defense_prob
    && s0->counter_defense_prob == s1->counter_defense_prob
    && s0->advance_prob == s1->advance_prob
    && s0->return_prob == s1->return_prob
    && s0->refraction == s1->refraction
    && s0->extra_hp == s1->extra_hp;
}

bool
room_linking_eq (struct room_linking *rl0, struct room_linking *rl1)
{
  return rl0->l == rl1->l
    && rl0->r == rl1->r
    && rl0->a == rl1->a
    && rl0->b == rl1->b;
}

bool
level_event_eq (struct level_event *le0, struct level_event *le1)
{
  return peq (&le0->p, &le1->p)
    && le0->next == le1->next;
}

bool
guard_eq (struct guard *g0, struct guard *g1)
{
  return g0->type == g1->type
    && peq (&g0->p, &g1->p)
    && g0->dir == g1->dir
    && skill_eq (&g0->skill, &g1->skill)
    && g0->total_hp == g1->total_hp
    && g0->style == g1->style;
}

bool
tile_eq (struct tile *c0, struct tile *c1)
{
  return fg_tile (c0) == fg_tile (c1)
    && bg_tile (c0) == bg_tile (c1)
    && ext_tile (c0) == ext_tile (c1)
    && fake_tile (c0) == fake_tile (c1);
}

bool
level_eq (struct level *l0, struct level *l1)
{
  if (l0->start != l1->start
      || l0->special_events != l1->special_events
      || l0->end != l1->end
      || l0->next_level != l1->next_level
      || l0->cutscene != l1->cutscene
      || ! peq (&l0->start_pos, &l1->start_pos)
      || l0->start_dir != l1->start_dir
      || l0->has_sword != l1->has_sword
      || l0->em != l1->em
      || l0->hue != l1->hue)
    return false;

  if (l0->room_nmemb != l1->room_nmemb) return false;
  for (size_t i = 0; i < l0->room_nmemb; i++)
    if (! room_linking_eq (llink (l0, i), llink (l1, i)))
      return false;

  if (l0->event_nmemb != l1->event_nmemb) return false;
  for (size_t i = 0; i < l0->event_nmemb; i++)
    if (! level_event_eq (event (l0, i), event (l1, i)))
      return false;

  if (l0->guard_nmemb != l1->guard_nmemb) return false;
  for (size_t i = 0; i < l0->guard_nmemb; i++)
    if (! guard_eq (guard (l0, i), guard (l1, i)))
      return false;

  struct pos p;
  for (p.room = 0; p.room < l0->room_nmemb; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++)
        if (! tile_eq (&l0->tile[p.room][p.floor][p.place],
                      &l1->tile[p.room][p.floor][p.place]))
          return false;

  return true;
}

void
replace_playing_level (struct level *l)
{
  destroy_tiles ();
  copy_level (&global_level, l);
  register_tiles ();
  em = global_level.em;
  hue = global_level.hue;
  mr.full_update = true;
}

void
play_level (struct level *lv)
{
 start:
  if (random_seed == 0) prandom (0);

  level_cleanup ();

  cutscene_mode (false);
  game_paused = false;
  ignore_level_cutscene = false;
  potion_flags = 0;
  copy_level (&global_level, lv);

  /* replay setup */
  replay_mode = level_start_replay_mode;

  struct replay *replay = get_replay ();

  if (! title_demo && replay_mode == PLAY_REPLAY) {
    HLINE;

    if (replay_index == 0) {
      printf ("REPLAY CHAIN BEGINNING\n");
      HLINE;
    }

    if (just_skipped_replay > 0
        || (just_skipped_replay == 0 && replay_index > 0
            && ! check_valid_replay_chain_pair (replay - 1, replay)))
      valid_replay_chain = false;

    print_replay_info (replay);
  }

  set_replay_mode_at_level_start (replay);

  play_time = start_level_time;

  if (mirror_level) mirror_level_h (&global_level);

  normalize_level (&global_level);

  apply_mr_fit_mode ();

  register_tiles ();
  register_actors ();

  stop_audio_instances ();
  play_time_stopped = false;
  death_timer = 0;

  if (global_level.start) global_level.start ();

  last_auto_show_time = 0;
  current_kid_id = 0;

  if (! force_em) em = global_level.em;
  if (! force_hue) hue = global_level.hue;

  last_edit = EDIT_MAIN;

  if (replay_mode != NO_REPLAY)
    exit_editor (NULL);

  level_number_shown = false;

  play_anim (draw_level, compute_level, cleanup_level);

  if (title_demo) {
    if (quit_anim != RESTART_LEVEL && quit_anim != NEXT_LEVEL)
      stop_replaying (0);
    level_cleanup ();
    return;
  }

  struct actor *k = get_actor_by_id (current_kid_id);

  switch (quit_anim) {
  default:
    level_cleanup ();
    break;
  case REPLAY_OUT_OF_TIME:
    replay->complete = false;
    replay->reason = REPLAY_INCOMPLETE_OUT_OF_TIME;
    replay->final_total_hp = k->total_hp;
    replay->final_kca = k->skill.counter_attack_prob + 1;
    replay->final_kcd = k->skill.counter_defense_prob + 1;
    break;
  case REPLAY_INCOMPLETE:
    replay->complete = false;
    replay->reason = k->current_hp > 0
      ? REPLAY_INCOMPLETE_STUCK : REPLAY_INCOMPLETE_DEAD;
    replay->final_total_hp = k->total_hp;
    replay->final_kca = k->skill.counter_attack_prob + 1;
    replay->final_kcd = k->skill.counter_defense_prob + 1;
    break;
  case REPLAY_COMPLETE:
    replay->complete = true;
    replay->reason = REPLAY_INCOMPLETE_NO_REASON;
    replay->final_total_hp = k->total_hp;
    replay->final_kca = k->skill.counter_attack_prob + 1;
    replay->final_kcd = k->skill.counter_defense_prob + 1;
    total_hp = k->total_hp;
    skill = k->skill;
    start_level_time = play_time;
    break;
  case REPLAY_RESTART_LEVEL:
    HLINE;
    printf ("RESTART REPLAY LEVEL\n");
    goto restart_level;
    break;
  case RESTART_LEVEL:
  restart_level:
    retry_level = global_level.n;
    level_cleanup ();
    ui_msg_clear (0);
   goto start;
   break;
  case NEXT_LEVEL:
  next_level:
    level_cleanup ();
    if (global_level.next_level)
      global_level.next_level (lv, next_level_number);
    ui_msg_clear (0);
    if (global_level.cutscene && ! ignore_level_cutscene
        && next_level_number >= global_level.n + 1) {
      cutscene_started = false;
      cutscene_mode (true);
      stop_video_effect ();
      stop_audio_instances ();
      play_anim (global_level.cutscene, NULL, NULL);
      stop_video_effect ();
      stop_audio_instances ();
      if (quit_anim == NEXT_LEVEL) goto next_level;
      else if (quit_anim == RESTART_LEVEL) goto restart_level;
      else if (quit_anim == RESTART_GAME) goto restart_game;
      else if (quit_anim == QUIT_GAME) goto quit;
    }
    goto start;
    break;
  case RESTART_GAME:
  restart_game:
    next_level_number = -1;
    retry_level = -1;
    level_cleanup ();
    ui_msg_clear (0);
    break;
  case OUT_OF_TIME:
    level_cleanup ();
    cutscene_started = false;
    cutscene_mode (true);
    stop_video_effect ();
    stop_audio_instances ();
    play_anim (cutscene_out_of_time_anim, NULL, NULL);
    stop_video_effect ();
    stop_audio_instances ();
    if (quit_anim == NEXT_LEVEL) goto next_level;
    else if (quit_anim == RESTART_LEVEL) goto restart_level;
    else if (quit_anim == RESTART_GAME) goto restart_game;
    else if (quit_anim == QUIT_GAME) goto quit;
    goto restart_game;
    break;
  }
 quit:

  if (! title_demo && replay_mode == PLAY_REPLAY) {
    if (! replay->complete) complete_replay_chain = false;
    if (quit_anim == REPLAY_NEXT) {
      HLINE;
      printf ("REPLAY SKIPPED\n");
      just_skipped_replay = replay_next_number < replay_index ? -1 : +1;
      replay_skipped = just_skipped_replay > 0;
    } else {
      print_replay_results (replay);
      just_skipped_replay = 0;
    }

    if ((quit_anim != REPLAY_NEXT
         || replay_next_number >= replay_index)
        && replay_index == replay_chain_nmemb - 1) {
      HLINE;
      printf ("REPLAY CHAIN END\n");
      HLINE;

      int status = ! replay_skipped && complete_replay_chain
        && valid_replay_chain ? 0 : 1;

      if (validate_replay_chain == WRITE_VALIDATE_REPLAY_CHAIN) {
        if (status == 0) {
          save_replay_chain ();
          fprintf (stderr, "MININIM: replay chain VALID and COMPLETE!  Replay chain HAS been saved.\n");
        } else fprintf (stderr, "MININIM: replay chain INVALID or INCOMPLETE!  Replay chain has NOT been saved.\n");
      }

      stop_replaying (1);

      if (command_line_replay) exit (status);

      switch (quit_anim) {
      case REPLAY_OUT_OF_TIME: goto restart_game;
      case REPLAY_INCOMPLETE: goto restart_level;
      case REPLAY_COMPLETE: goto next_level;
      default: assert (0); break;
      }
    } else {
      prepare_for_playing_replay
        (quit_anim == REPLAY_NEXT ? replay_next_number : replay_index + 1);
      goto next_level;
    }
  }
}

void *
tile_struct_at_pos (struct pos *p)
{
  switch (fg (p)) {
  case LOOSE_FLOOR: return loose_floor_at_pos (p);
  case OPENER_FLOOR: return opener_floor_at_pos (p);
  case CLOSER_FLOOR: return closer_floor_at_pos (p);
  case SPIKES_FLOOR: return spikes_floor_at_pos (p);;
  case DOOR: return door_at_pos (p);
  case LEVEL_DOOR: return level_door_at_pos (p);
  case CHOMPER: return chomper_at_pos (p);
  default: return NULL;
  }
}

bool
should_init (struct tile *c0, struct tile *c1)
{
  return fg_val (c0->fg) == fg_val (c1->fg)
    && ext_val (c0->fg, c0->ext) != ext_val (c1->fg, c1->ext);
}

void
copy_to_tile_state (union tile_state *to, struct pos *from_pos)
{
  void *from = tile_struct_at_pos (from_pos);
  switch (fg (from_pos)) {
  case LOOSE_FLOOR: copy_loose_floor (&to->loose_floor, from); break;
  case OPENER_FLOOR: copy_opener_floor (&to->opener_floor, from); break;
  case CLOSER_FLOOR: copy_closer_floor (&to->closer_floor, from); break;
  case SPIKES_FLOOR: copy_spikes_floor (&to->spikes_floor, from); break;
  case DOOR: copy_door (&to->door, from); break;
  case LEVEL_DOOR: copy_level_door (&to->level_door, from); break;
  case CHOMPER: copy_chomper (&to->chomper, from); break;
  default: break;
  }
}

void
copy_from_tile_state (struct pos *to_pos, union tile_state *from)
{
  void *to = tile_struct_at_pos (to_pos);
  switch (fg (to_pos)) {
  case LOOSE_FLOOR: copy_loose_floor (to, &from->loose_floor); break;
  case OPENER_FLOOR: copy_opener_floor (to, &from->opener_floor); break;
  case CLOSER_FLOOR: copy_closer_floor (to, &from->closer_floor); break;
  case SPIKES_FLOOR: copy_spikes_floor (to, &from->spikes_floor); break;
  case DOOR: copy_door (to, &from->door); break;
  case LEVEL_DOOR: copy_level_door (to, &from->level_door); break;
  case CHOMPER: copy_chomper (to, &from->chomper); break;
  default: break;
  }
}

void
init_tile_at_pos (struct pos *p)
{
  switch (fg (p)) {
  case LOOSE_FLOOR: init_loose_floor (p, loose_floor_at_pos (p)); break;
  case OPENER_FLOOR: init_opener_floor (p, opener_floor_at_pos (p)); break;
  case CLOSER_FLOOR: init_closer_floor (p, closer_floor_at_pos (p)); break;
  case SPIKES_FLOOR: init_spikes_floor (p, spikes_floor_at_pos (p)); break;
  case DOOR: init_door (p, door_at_pos (p)); break;
  case LEVEL_DOOR: init_level_door (p, level_door_at_pos (p)); break;
  case CHOMPER: init_chomper (p, chomper_at_pos (p)); break;
  default: break;
  }
}

void
destroy_tile_at_pos (struct pos *p)
{
  switch (fg (p)) {
  case LOOSE_FLOOR: remove_loose_floor (loose_floor_at_pos (p)); break;
  case OPENER_FLOOR: remove_opener_floor (opener_floor_at_pos (p)); break;
  case CLOSER_FLOOR: remove_closer_floor (closer_floor_at_pos (p)); break;
  case SPIKES_FLOOR: remove_spikes_floor (spikes_floor_at_pos (p)); break;
  case DOOR: remove_door (door_at_pos (p)); break;
  case LEVEL_DOOR: remove_level_door (level_door_at_pos (p)); break;
  case CHOMPER: remove_chomper (chomper_at_pos (p)); break;
  default: break;
  }
}

void
destroy_tiles (void)
{
  destroy_array ((void **) &loose_floor, &loose_floor_nmemb);
  destroy_array ((void **) &opener_floor, &opener_floor_nmemb);
  destroy_array ((void **) &closer_floor, &closer_floor_nmemb);
  destroy_array ((void **) &spikes_floor, &spikes_floor_nmemb);
  destroy_array ((void **) &door, &door_nmemb);
  destroy_array ((void **) &level_door, &level_door_nmemb);
  destroy_array ((void **) &chomper, &chomper_nmemb);
  destroy_array ((void **) &mirror, &mirror_nmemb);
}

void
register_tile_at_pos (struct pos *p)
{
  switch (fg (p)) {
  case LOOSE_FLOOR: register_loose_floor (p); break;
  case OPENER_FLOOR: register_opener_floor (p); break;
  case CLOSER_FLOOR: register_closer_floor (p); break;
  case SPIKES_FLOOR: register_spikes_floor (p); break;
  case DOOR: register_door (p); break;
  case LEVEL_DOOR: register_level_door (p); break;
  case CHOMPER: register_chomper (p); break;
  default: break;
  }
}

void
register_room (int room)
{
  struct pos p; new_pos (&p, &global_level, room, -1, -1);
  for (p.floor = 0; p.floor < FLOORS; p.floor++)
    for (p.place = 0; p.place < PLACES; p.place++)
      register_tile_at_pos (&p);
}

void
register_tiles (void)
{
  int room;
  for (room = 0; room < global_level.room_nmemb; room++)
    register_room (room);
}

void
register_actors (void)
{
  /* create kid */
  struct pos kid_start_pos; get_kid_start_pos (&kid_start_pos);
  int id = create_actor (NULL, KID, &kid_start_pos, global_level.start_dir);
  struct actor *k = &actor[id];
  k->total_hp = total_hp;
  k->skill = skill;
  k->current_hp = total_hp;
  k->controllable = true;
  k->immortal = immortal_mode;
  k->has_sword = global_level.has_sword;
  init_fellow_shadow_id ();

  /* create guards */
  int i;
  for (i = 0; i < global_level.guard_nmemb; i++) {
    struct guard *g = guard (&global_level, i);
    struct actor *a;
    int id;
    switch (g->type) {
    case NO_ACTOR: continue;
    case KID:
      id = create_actor (NULL, KID, &g->p, g->dir);
      actor[id].shadow = true;
      break;
    case GUARD: default:
      id = create_actor (NULL, GUARD, &g->p, g->dir);
      break;
    case FAT:
      id = create_actor (NULL, FAT, &g->p, g->dir);
      break;
    case VIZIER:
      id = create_actor (NULL, VIZIER, &g->p, g->dir);
      break;
    case SKELETON:
      id = create_actor (NULL, SKELETON, &g->p, g->dir);
      break;
    case SHADOW:
      id = create_actor (NULL, SHADOW, &g->p, g->dir);
      break;
    }
    a = &actor[id];
    apply_guard_mode (a, gm);
    a->level_id = i;
    a->has_sword = true;
    a->skill = g->skill;
    a->total_hp = g->total_hp + g->skill.extra_hp;
    a->current_hp = g->total_hp;
    if (global_level.n == 3 && semantics == LEGACY_SEMANTICS) {
      a->total_hp = a->current_hp = INT_MAX;
      a->dont_draw_hp = true;
    }
    a->style = g->style;
    if (a->total_hp == 0) a->glory_sample = true;
  }
}

void
level_cleanup (void)
{
  destroy_actors ();
  destroy_tiles ();
  free_undo (&undo);
}

void
load_level (void)
{
  load_mouse ();
}

void
unload_level (void)
{
  unload_mouse ();
}

void
compute_level (void)
{
  if (step_cycle > 0) {
    game_paused = false;
    step_cycle--;
  } else if (step_cycle == 0) {
    game_paused = true;
    step_cycle = -1;
  }

  level_hotkeys_cb (NULL, 0);

  process_death ();

  if (is_game_paused () && ! changing_vm) {
    keep_all_depressible_floors ();
    return;
  }

  struct actor *k = get_actor_by_id (current_kid_id);
  struct actor *ke = get_actor_by_id (k->enemy_id);

  struct replay *replay = get_replay ();
  replay_gamepad_update (k, replay, anim_cycle);

  if (! k->key.ctrl || ! k->key.left) k->ctrl_left = false;
  if (! k->key.ctrl || ! k->key.right) k->ctrl_right = false;
  if (! k->key.alt || ! k->key.up) k->alt_up = false;

  /* process fellow shadow gamepad commands */
  if (k->key.ctrl && k->key.left && ! k->ctrl_left) {
    k->ctrl_left = true;
    if (current_kid_id) next_fellow_shadow (-1);
    else create_fellow_shadow (! k->key.alt);
    k = get_actor_by_id (current_kid_id);
    ke = get_actor_by_id (k->enemy_id);
    k->ctrl_left = true;
  } else if (k->key.ctrl && k->key.right && ! k->ctrl_right) {
    k->ctrl_right = true;
    if (current_kid_id) next_fellow_shadow (+1);
    else create_fellow_shadow (! k->key.alt);
    k = get_actor_by_id (current_kid_id);
    ke = get_actor_by_id (k->enemy_id);
    k->ctrl_right = true;
  } else if (k->key.alt && k->key.up && ! k->alt_up) {
    k->alt_up = true;
    current_fellow_shadow ();
    k = get_actor_by_id (current_kid_id);
    ke = get_actor_by_id (k->enemy_id);
    k->alt_up = true;
  }

  /* if current controllable is a dead shadow, select its controllable
     source */
  if (k->shadow_of == 0 && k->current_hp <= 0) {
    if (k->death_timer++ > FELLOW_SHADOW_DEATH_WAIT_CYCLES) {
      k->death_timer = 0;
      k = get_actor_by_id (k->shadow_of);
      ke = get_actor_by_id (k->enemy_id);
      select_controllable_by_id (k->id);
    }
  }

  camera_follow_kid = (k->f.c.room == mr.room)
    ? k->id : -1;

  int prev_room = k->f.c.room;

  for (size_t i = 0; i < actor_nmemb; i++) {
    struct actor *a = &actor[i];
    a->splash = false;
    a->xf.b = NULL;
  }

  compute_loose_floors ();

  /* non-current controllables must defend themselves */
  for (size_t i = 0; i < actor_nmemb; i++) {
    struct actor *ks = &actor[i];

    if ((ks->id != 0 && ks->shadow_of != 0)
        || ks->id == current_kid_id
        || ! is_in_fight_mode (ks)) continue;

    struct actor *kse = get_reciprocal_enemy (ks);
    if (kse && kse->refraction > 3
        && ! is_in_range (ks, kse, FIGHT_RANGE)) {
      if (ks->f.dir == LEFT) ks->key.left = true;
      else ks->key.right = true;
    } else ks->key.up = ks->key.shift = true;
  }

  /* fight AI */
  for (size_t i = 0; i < actor_nmemb; i++) enter_fight_logic (&actor[i]);
  for (size_t i = 0; i < actor_nmemb; i++) leave_fight_logic (&actor[i]);
  for (size_t i = 0; i < actor_nmemb; i++) fight_ai (&actor[i]);

  /* actions */
  for (size_t i = 0; i < actor_nmemb; i++) {
    if (actor[i].next_action) {
      actor[i].next_action (&actor[i]);
      actor[i].next_action = NULL;
    } else actor[i].action (&actor[i]);
  }

  /* fight mechanics */
  for (size_t i = 0; i < actor_nmemb; i++) fight_mechanics (&actor[i]);

  /* timers */
  for (size_t i = 0; i < actor_nmemb; i++) {
    struct actor *a = &actor[i];
    if (a->float_timer && a->float_timer < FLOAT_TIMER_MAX) {
      request_gamepad_rumble (0.5 * (sin (a->float_timer * 0.17) + 1),
                              1.0 / DEFAULT_HZ);
      a->float_timer++;
    } else if (a->float_timer > 0) a->float_timer = 0;

    if (a->enemy_refraction > 0) a->enemy_refraction--;
    if (a->refraction > 0) a->refraction--;
    if (a->no_walkf_timer > 0) a->no_walkf_timer--;
  }

  /* appropriately turn controllables */
  for (size_t i = 0; i < actor_nmemb; i++) {
    struct actor *ks = &actor[i];
    if (ks->id == 0 || ks->shadow_of == 0)
      fight_turn_controllable (ks);
  }

  /* collision enforcement */
  for (size_t i = 0; i < actor_nmemb; i++) {
    struct actor *a = &actor[i];
    enforce_wall_collision (a);
  }

  clear_actors_keyboard_state ();

  if (k->f.c.room != prev_room
      && k->f.c.room != 0
      && camera_follow_kid == k->id)  {
    if (! is_room_visible (k->f.c.room)) {
      mr_coord (k->f.c.prev_room,
                k->f.c.xd, &mr.x, &mr.y);
      mr_set_origin (k->f.c.room, mr.x, mr.y);
    } else mr_focus_room (k->f.c.room);
    mr.select_cycles = 0;
  }

  if (mr.w > 1
      && k->current_hp > 0
      && k->f.c.room != 0
      && camera_follow_kid == k->id
      && (ke = get_reciprocal_enemy (k))
      && ! is_room_visible (ke->f.c.room)) {
    if (ke->f.c.room == roomd (&global_level, k->f.c.room, LEFT)) {
      mr_row_trans (LEFT);
      mr_focus_room (k->f.c.room);
      mr.room_select = ke->f.c.room;
    } else if (ke->f.c.room == roomd (&global_level, k->f.c.room, RIGHT)) {
      mr_row_trans (RIGHT);
      mr_focus_room (k->f.c.room);
      mr.room_select = ke->f.c.room;
    }
  } else if (mr.room_select > 0
             && (mr.select_cycles == 0
                 || mr.room != k->f.c.room))
    mr.room_select = -1;

  /* save individual multi-room origin */
  mr_save_origin (&k->mr_origin);

  if (global_level.special_events) global_level.special_events ();

  compute_closer_floors ();
  compute_opener_floors ();
  compute_spikes_floors ();
  compute_doors ();
  compute_level_doors ();
  compute_chompers ();

  register_changed_opener_floors ();
  register_changed_closer_floors ();

  if (! play_time_stopped) play_time++;
}

void
cleanup_level (void)
{
  unpress_opener_floors ();
  unpress_closer_floors ();
}

static void
process_death (void)
{
  struct actor *k = get_actor_by_id (0);

  /* Restart level after death */
  if (k->current_hp <= 0
      && ! is_game_paused ()) {
    death_timer++;

    if (death_timer == SEC2CYC (1)) {
      struct audio_source *as;
      switch (k->death_reason) {
      case SHADOW_FIGHT_DEATH: as = &success_suspense_audio; break;
      case FIGHT_DEATH: as = &fight_death_audio; break;
      default: as = &death_audio; break;
      }
      stop_audio_instance (&success_audio, NULL, k->id);
      play_audio (as, NULL, k->id);
    }

    if (death_timer >= SEC2CYC (5) && ! title_demo) {
      if ((death_timer < SEC2CYC (20)
           || death_timer % SEC2CYC (1) < (2 * SEC2CYC (1)) / 3)
          && ! active_menu) {
        if (death_timer >= SEC2CYC (21) && death_timer % SEC2CYC (1) == 0) {
          play_audio (&press_key_audio, NULL, -1);
          kid_haptic (k, KID_HAPTIC_PRESS_ANY_KEY);
        }
        ui_msg (-2, "Press Button to Continue");
      } else if (! active_menu) ui_msg (-2, "%s", "");

      if (was_any_key_pressed () && ! was_bmenu_key_pressed ())
        quit_anim = RESTART_LEVEL;
    }
  } else if (death_timer && ! is_game_paused ()) {
    death_timer = 0;
    ui_msg_clear (-2);
  }
}


void
draw_level (void)
{
  draw_multi_rooms ();

  draw_hp (uscreen, get_actor_by_id (current_kid_id));

  /* automatic level display */
  if (! title_demo && global_level.nominal_n >= 0
      && ! level_number_shown && anim_cycle <= SEC2CYC (10)
      && ui_msg (-1, "LEVEL %i", global_level.nominal_n))
    level_number_shown = true;

  /* automatic remaining time display */
  if (! title_demo) {
    int64_t rem_time = time_limit - play_time;
    int64_t rem_time_sec = precise_unit (rem_time, 1 * DEFAULT_HZ);
    int64_t rem_time_min = precise_unit (rem_time, 60 * DEFAULT_HZ);
    if ((rem_time_min % 5 == 0
         && labs (last_auto_show_time - rem_time_min) > 1)
        || rem_time_sec <= 60
        || (anim_cycle <= SEC2CYC (10)
            && labs (last_auto_show_time - rem_time_min) > 1)) {
      if (rem_time_sec <= 60
          && (rem_time + 1) % DEFAULT_HZ == 0
          && ! play_time_stopped)
        play_audio (&press_key_audio, NULL, -1);
      if (display_remaining_time (rem_time_sec <= 60 ? 0 : -2))
        last_auto_show_time = rem_time_min;
    }
    if (rem_time <= 0) quit_anim = OUT_OF_TIME;
  }

  if (is_game_paused () && ! active_menu) print_game_paused (-1);
}

static void
draw_hp (ALLEGRO_BITMAP *bitmap, struct actor *k)
{
  bool nrlc = no_recursive_links_continuity;
  no_recursive_links_continuity = true;

  if (is_room_visible (k->f.c.room)) {
    draw_kid_hp (bitmap, k);
    struct actor *ke = NULL;
    if (k->enemy_id != -1) {
      ke = get_actor_by_id (k->enemy_id);
      if (ke && ke->enemy_id == k->id)
        draw_guard_hp (bitmap, ke);
    }
  }

  no_recursive_links_continuity = nrlc;
}

void
pause_game (bool val)
{
  if (! val) ui_msg_clear (0);
  game_paused = val;
}

bool
is_game_paused (void)
{
  return anim_cycle > 0 && game_paused;
}

void
next_level (void)
{
  struct actor *k = get_actor_by_id (current_kid_id);

  total_hp = k->total_hp;
  current_hp = k->current_hp;
  skill = k->skill;
  start_level_time = play_time;
  next_level_number = global_level.n + 1;
  quit_anim = NEXT_LEVEL;
}
