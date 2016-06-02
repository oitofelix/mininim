/*
  level.c -- level module;

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

/* functions */
static void draw_level (void);
static void compute_level (void);
static void process_keys (void);
static void draw_lives (ALLEGRO_BITMAP *bitmap, struct anim *k, enum vm vm);

struct level *vanilla_level;
struct level level;

struct undo undo;

static char *undo_msg;
static int last_auto_show_time;
static uint64_t death_timer;
static bool ignore_level_cutscene;

bool no_room_drawing, game_paused, step_one_cycle;
int retry_level = -1;
int anti_camera_room;
int camera_follow_kid;
int auto_rem_time_1st_cycle = 24;

void
play_level (struct level *lv)
{
  char *text;
  vanilla_level = lv;

 start:
  free_undo (&undo);
  cutscene = false;
  game_paused = false;
  level = *lv;

  if (level_module == LEGACY_LEVEL_MODULE
      || level_module == PLV_LEVEL_MODULE
      || level_module == DAT_LEVEL_MODULE)
    fix_legacy_room_above_zero_with_traversable_at_bottom ();

  register_cons ();
  register_anims ();

  stop_all_samples ();
  play_time_stopped = false;
  death_timer = 0;

  if (level.start) level.start ();

  /* anim_cycle = 0; */
  last_auto_show_time = -1;
  current_kid_id = 0;

  if (retry_level != level.number)
    start_level_time = play_time;

  if (! force_em) em = level.em;
  if (! force_hue) hue = level.hue;

  edit = EDIT_MAIN;
  exit_editor ();

  if (level.nominal_number >= 0) {
    xasprintf (&text, "LEVEL %i", level.nominal_number);
    draw_bottom_text (NULL, text, 0);
    al_free (text);
  }

  play_anim (draw_level, compute_level);

  switch (quit_anim) {
  case NO_QUIT: break;
  case RESTART_LEVEL:
    retry_level = level.number;
    destroy_anims ();
    destroy_cons ();
    draw_bottom_text (NULL, NULL, 0);
   goto start;
  case PREVIOUS_LEVEL:
  case NEXT_LEVEL:
    destroy_anims ();
    destroy_cons ();
    int d = (quit_anim == PREVIOUS_LEVEL) ? -1 : +1;
    if (level.next_level) level.next_level (level.number + d);
    draw_bottom_text (NULL, NULL, 0);
    if (level.cutscene && ! ignore_level_cutscene) {
      cutscene_started = false;
      cutscene = true;
      stop_video_effect ();
      stop_all_samples ();
      play_anim (level.cutscene, NULL);
      stop_video_effect ();
      stop_all_samples ();

      if (quit_anim == RESTART_GAME) goto restart_game;
    }
    ignore_level_cutscene = false;
    goto start;
  case RESTART_GAME:
  restart_game:
    retry_level = -1;
    destroy_anims ();
    destroy_cons ();
    draw_bottom_text (NULL, NULL, 0);
    break;
  case QUIT_GAME: break;
  case OUT_OF_TIME:
    cutscene_started = false;
    cutscene = true;
    stop_video_effect ();
    stop_all_samples ();
    play_anim (cutscene_out_of_time_anim, NULL);
    stop_video_effect ();
    stop_all_samples ();
    goto restart_game;
    break;
  }
}

void
register_con_at_pos (struct pos *p)
{
  switch (con (p)->fg) {
  case LOOSE_FLOOR: register_loose_floor (p); break;
  case OPENER_FLOOR: register_opener_floor (p); break;
  case CLOSER_FLOOR: register_closer_floor (p); break;
  case SPIKES_FLOOR: register_spikes_floor (p); break;
  case DOOR: register_door (p); break;
  case LEVEL_DOOR: register_level_door (p); break;
  case CHOPPER: register_chopper (p); break;
  case MIRROR: register_mirror (p); break;
  default: break;
  }
}

void
register_room (int room)
{
  struct pos p;
  p.room = room;
  for (p.floor = 0; p.floor < FLOORS; p.floor++)
    for (p.place = 0; p.place < PLACES; p.place++)
      register_con_at_pos (&p);
}

void
register_cons (void)
{
  int room;
  for (room = 0; room < ROOMS; room++)
    register_room (room);
}

void
destroy_con_at_pos (struct pos *p)
{
  switch (con (p)->fg) {
  case LOOSE_FLOOR: remove_loose_floor (loose_floor_at_pos (p)); break;
  case OPENER_FLOOR: remove_opener_floor (opener_floor_at_pos (p)); break;
  case CLOSER_FLOOR: remove_closer_floor (closer_floor_at_pos (p)); break;
  case SPIKES_FLOOR: remove_spikes_floor (spikes_floor_at_pos (p)); break;
  case DOOR: remove_door (door_at_pos (p)); break;
  case LEVEL_DOOR: remove_level_door (level_door_at_pos (p)); break;
  case CHOPPER: remove_chopper (chopper_at_pos (p)); break;
  case MIRROR: remove_mirror (mirror_at_pos (p)); break;
  default: break;
  }
}

void
destroy_room (int room)
{
  struct pos p;
  p.room = room;
  for (p.floor = 0; p.floor < FLOORS; p.floor++)
    for (p.place = 0; p.place < PLACES; p.place++)
      destroy_con_at_pos (&p);
}

void
destroy_cons (void)
{
  destroy_array ((void **) &loose_floor, &loose_floor_nmemb);
  destroy_array ((void **) &opener_floor, &opener_floor_nmemb);
  destroy_array ((void **) &closer_floor, &closer_floor_nmemb);
  destroy_array ((void **) &spikes_floor, &spikes_floor_nmemb);
  destroy_array ((void **) &door, &door_nmemb);
  destroy_array ((void **) &level_door, &level_door_nmemb);
  destroy_array ((void **) &chopper, &chopper_nmemb);
  destroy_array ((void **) &mirror, &mirror_nmemb);
}

void
prepare_con_at_pos (struct pos *p)
{
  if (! is_pos_visible (p)) return;

  switch (con (p)->bg) {
  case BALCONY: generate_stars_for_pos (p); break;
  default: break;
  }
}

void
prepare_room (int room)
{
  if (! is_room_visible (room)) return;
  generate_stars_for_room (room);
}

void
prepare_view (void)
{
  generate_stars ();
}

void
register_anims (void)
{
  /* create kid */
  struct pos kid_start_pos;
  if (is_valid_pos (&start_pos))
    kid_start_pos = start_pos;
  else kid_start_pos = level.start_pos;
  mr_center_room (kid_start_pos.room);
  int id = create_anim (NULL, KID, &kid_start_pos, level.start_dir);
  struct anim *k = &anima[id];
  k->total_lives = total_lives;
  k->skill = skill;
  k->current_lives = total_lives;
  k->controllable = true;
  k->immortal = immortal_mode;
  k->has_sword = level.has_sword;

  /* create guards */
  int i;
  for (i = 0; i < GUARDS; i++) {
    struct guard *g = &level.guard[i];
    struct anim *a;
    int id;
    switch (g->type) {
    case NO_ANIM: default: continue;
    case KID:
      id = create_anim (NULL, KID, &g->p, g->dir);
      anima[id].shadow = true;
      break;
    case GUARD:
      id = create_anim (NULL, GUARD, &g->p, g->dir);
      break;
    case FAT_GUARD:
      id = create_anim (NULL, FAT_GUARD, &g->p, g->dir);
      break;
    case VIZIER:
      id = create_anim (NULL, VIZIER, &g->p, g->dir);
      break;
    case SKELETON:
      id = create_anim (NULL, SKELETON, &g->p, g->dir);
      break;
    case SHADOW:
      id = create_anim (NULL, SHADOW, &g->p, g->dir);
      break;
    }
    a = &anima[id];
    apply_guard_mode (a, gm);
    a->level_id = i;
    a->has_sword = true;
    a->skill = g->skill;
    a->total_lives = g->total_lives + g->skill.extra_life;
    a->current_lives = g->total_lives;
    a->style = g->style;
  }
}

void
load_level (void)
{
  load_room ();
  load_fire ();
  load_potion ();
  load_sword ();
  load_kid ();
  load_guard ();
  load_mouse ();
  load_box ();
}

void
unload_level (void)
{
  unload_room ();
  unload_fire ();
  unload_potion ();
  unload_sword ();
  unload_kid ();
  unload_guard ();
  unload_mouse ();
  unload_box ();
}

static void
compute_level (void)
{
  size_t i;

  process_keys ();

  if (game_paused) return;

  /* this condition is necessary to honor any floor press the start
     level function might have */
  if (anim_cycle > 0) {
    unpress_opener_floors ();
    unpress_closer_floors ();
    uncross_mirrors ();
  }

  struct anim *current_kid = get_anim_by_id (current_kid_id);

  int prev_room = current_kid->f.c.room;

  for (i = 0; i < anima_nmemb; i++) {
    struct anim *a = &anima[i];
    a->splash = false;
    a->xf.b = NULL;
  }

  compute_loose_floors ();

  get_gamepad_state (&current_kid->key);

  for (i = 0; i < anima_nmemb; i++) enter_fight_logic (&anima[i]);
  for (i = 0; i < anima_nmemb; i++) leave_fight_logic (&anima[i]);
  for (i = 0; i < anima_nmemb; i++) fight_ai (&anima[i]);
  for (i = 0; i < anima_nmemb; i++) anima[i].action (&anima[i]);
  for (i = 0; i < anima_nmemb; i++) fight_mechanics (&anima[i]);

  for (i = 0; i < anima_nmemb; i++)
    if (anima[i].float_timer) anima[i].float_timer++;

  fight_turn_controllable (current_kid);

  clear_anims_keyboard_state ();

  if (current_kid->f.c.room != prev_room
      && current_kid->f.c.room != 0
      && current_kid->f.c.room != anti_camera_room
      && camera_follow_kid == current_kid->id)  {
    if (! is_room_visible (current_kid->f.c.room)) {
      mr_coord (current_kid->f.c.prev_room,
                current_kid->f.c.xd, &mr.x, &mr.y);
      mr_set_origin (current_kid->f.c.room, mr.x, mr.y);
    } else mr_focus_room (current_kid->f.c.room);

    mr.select_cycles = 0;
  }

  if (level.special_events) level.special_events ();

  compute_closer_floors ();
  compute_opener_floors ();
  compute_spikes_floors ();
  compute_doors ();
  compute_level_doors ();
  compute_choppers ();

  register_changed_opener_floors ();
  register_changed_closer_floors ();

  if (! play_time_stopped) play_time++;
}

static void
process_keys (void)
{
  char *text = NULL;

  struct anim *current_kid = get_anim_by_id (current_kid_id);

  /* clear the keyboard buffer at the first cycle, so any key pressed
     on the title doesn't trigger any action */
  if (anim_cycle == 0) {
    memset (&key, 0, sizeof (key));
    button = -1;
  }

  /* M: change multi-room fit mode */
  if (! active_menu &&
      was_key_pressed (ALLEGRO_KEY_M, 0, 0, true)) {
    char *fit_str;

    switch (mr.fit_mode) {
    case MR_FIT_NONE:
      mr.fit_mode = MR_FIT_STRETCH;
      fit_str = "STRETCH";
      break;
    case MR_FIT_STRETCH:
      mr.fit_mode = MR_FIT_RATIO;
      fit_str = "RATIO";
      break;
    case VGA:
      mr.fit_mode = MR_FIT_NONE;
      fit_str = "NONE";
      break;
    }

    apply_mr_fit_mode ();

    xasprintf (&text, "MR FIT MODE: %s", fit_str);
    draw_bottom_text (NULL, text, 0);
    al_free (text);
  }

  /* CTRL+Z: undo */
  if (was_key_pressed (ALLEGRO_KEY_Z, 0, ALLEGRO_KEYMOD_CTRL, true))
    ui_undo_pass (&undo, -1, NULL);

  /* CTRL+Y: redo */
  if (was_key_pressed (ALLEGRO_KEY_Y, 0, ALLEGRO_KEYMOD_CTRL, true))
    ui_undo_pass (&undo, +1, NULL);

  /* [: decrease multi-room resolution */
  if (was_key_pressed (0, '[', 0, true)
      && ! cutscene)
    ui_set_multi_room (-1, -1);

  /* ]: increase multi-room resolution */
  if (was_key_pressed (0, ']', 0, true)
      && ! cutscene)
    ui_set_multi_room (+1, +1);

  /* CTRL+[: decrease multi-room width resolution */
  if (was_key_pressed (0, 0x1B, ALLEGRO_KEYMOD_CTRL, true)
      && ! cutscene)
    ui_set_multi_room (-1, +0);

  /* CTRL+]: increase multi-room width resolution */
  if ((was_key_pressed (0, 0x1D, ALLEGRO_KEYMOD_CTRL, true)
       || was_key_pressed (0, 0x1C, ALLEGRO_KEYMOD_CTRL, true))
      && ! cutscene)
    ui_set_multi_room (+1, +0);

  /* ALT+[: decrease multi-room height resolution */
  if (was_key_pressed (0, '[', ALLEGRO_KEYMOD_ALT, true)
      && ! cutscene)
    ui_set_multi_room (+0, -1);

  /* ALT+]: increase multi-room height resolution */
  if (was_key_pressed (0, ']', ALLEGRO_KEYMOD_ALT, true)
      && ! cutscene)
    ui_set_multi_room (+0, +1);

  /* SHIFT+B: enable/disable room drawing */
  if (was_key_pressed (ALLEGRO_KEY_B, 0, ALLEGRO_KEYMOD_SHIFT, true)) {
    no_room_drawing = ! no_room_drawing;
    force_full_redraw = true;
  }

  /* H: select room at left (J if flipped horizontally) */
  if (! active_menu
      && ((! flip_gamepad_horizontal
           && was_key_pressed (ALLEGRO_KEY_H, 0, 0, true))
          || (flip_gamepad_horizontal
              && was_key_pressed (ALLEGRO_KEY_J, 0, 0, true))))
    mr_select_trans (LEFT);

  /* J: select room at right (H if flipped horizontally) */
  if (! active_menu
      && ((! flip_gamepad_horizontal
           && was_key_pressed (ALLEGRO_KEY_J, 0, 0, true))
          || (flip_gamepad_horizontal
              && was_key_pressed (ALLEGRO_KEY_H, 0, 0, true))))
    mr_select_trans (RIGHT);

  /* U: select room above (N if flipped vertically) */
  if (! active_menu
      && ((! flip_gamepad_vertical
           && was_key_pressed (ALLEGRO_KEY_U, 0, 0, true))
          || (flip_gamepad_vertical
              && was_key_pressed (ALLEGRO_KEY_N, 0, 0, true))))
    mr_select_trans (ABOVE);

  /* N: select room below (U if flipped vertically) */
  if (! active_menu
      && ((! flip_gamepad_vertical
           && was_key_pressed (ALLEGRO_KEY_N, 0, 0, true))
          || (flip_gamepad_vertical
              && was_key_pressed (ALLEGRO_KEY_U, 0, 0, true))))
    mr_select_trans (BELOW);

  /* SHIFT+H: multi-room view to left (J if flipped horizontally) */
  if (! active_menu
      && ((! flip_gamepad_horizontal
           && was_key_pressed (ALLEGRO_KEY_H, 0, ALLEGRO_KEYMOD_SHIFT, true))
          || (flip_gamepad_horizontal
              && was_key_pressed (ALLEGRO_KEY_J, 0, ALLEGRO_KEYMOD_SHIFT, true))))
    mr_view_trans (LEFT);

  /* SHIFT+J: multi-room view to right (H if flipped horizontally) */
  if (! active_menu
      && ((! flip_gamepad_horizontal
           && was_key_pressed (ALLEGRO_KEY_J, 0, ALLEGRO_KEYMOD_SHIFT, true))
          || (flip_gamepad_horizontal
              && was_key_pressed (ALLEGRO_KEY_H, 0, ALLEGRO_KEYMOD_SHIFT, true))))
    mr_view_trans (RIGHT);

  /* SHIFT+U: multi-room view above (N if flipped vertically) */
  if (! active_menu
      && ((! flip_gamepad_vertical
           && was_key_pressed (ALLEGRO_KEY_U, 0, ALLEGRO_KEYMOD_SHIFT, true))
          || (flip_gamepad_vertical
              && was_key_pressed (ALLEGRO_KEY_N, 0, ALLEGRO_KEYMOD_SHIFT, true))))
    mr_view_trans (ABOVE);

  /* SHIFT+N: multi-room view below (U if flipped vertically) */
  if (! active_menu
      && ((! flip_gamepad_vertical
           && was_key_pressed (ALLEGRO_KEY_N, 0, ALLEGRO_KEYMOD_SHIFT, true))
          || (flip_gamepad_vertical
              && was_key_pressed (ALLEGRO_KEY_U, 0, ALLEGRO_KEYMOD_SHIFT, true))))
    mr_view_trans (BELOW);

  /* ALT+H: multi-room page view to left (J if flipped horizontally) */
  if (! active_menu
      && ((! flip_gamepad_horizontal
           && was_key_pressed (ALLEGRO_KEY_H, 0, ALLEGRO_KEYMOD_ALT, true))
          || (flip_gamepad_horizontal
              && was_key_pressed (ALLEGRO_KEY_J, 0, ALLEGRO_KEYMOD_ALT, true))))
    mr_view_page_trans (LEFT);

  /* ALT+J: multi-room page view to right (H if flipped horizontally) */
  if (! active_menu
      && ((! flip_gamepad_horizontal
           && was_key_pressed (ALLEGRO_KEY_J, 0, ALLEGRO_KEYMOD_ALT, true))
          || (flip_gamepad_horizontal
              && was_key_pressed (ALLEGRO_KEY_H, 0, ALLEGRO_KEYMOD_ALT, true))))
    mr_view_page_trans (RIGHT);

  /* ALT+U: multi-room page view above (N if flipped vertically) */
  if (! active_menu
      && ((! flip_gamepad_vertical
           && was_key_pressed (ALLEGRO_KEY_U, 0, ALLEGRO_KEYMOD_ALT, true))
          || (flip_gamepad_vertical
              && was_key_pressed (ALLEGRO_KEY_N, 0, ALLEGRO_KEYMOD_ALT, true))))
    mr_view_page_trans (ABOVE);

  /* ALT+N: multi-room page view below (U if flipped vertically) */
  if (! active_menu
      && ((! flip_gamepad_vertical
           && was_key_pressed (ALLEGRO_KEY_N, 0, ALLEGRO_KEYMOD_ALT, true))
          || (flip_gamepad_vertical
              && was_key_pressed (ALLEGRO_KEY_U, 0, ALLEGRO_KEYMOD_ALT, true))))
    mr_view_page_trans (BELOW);

  /* ESC: pause game */
  if (step_one_cycle) {
    step_one_cycle = false;
    game_paused = true;
  }

  if (was_key_pressed (ALLEGRO_KEY_ESCAPE, 0, 0, true)
      || was_button_pressed (joystick_pause_button, true)) {
    if (game_paused) {
      step_one_cycle = true;
      game_paused = false;
    } else pause_game ();
  } else if (game_paused
             && (! active_menu || ! was_menu_key_pressed ())
             && (key.keyboard.keycode || button != -1)
             && ! save_game_dialog_thread)
    unpause_game ();

  /* if (game_paused) anim_cycle--; */

  /* R: resurrect kid */
  if (! active_menu
      && was_key_pressed (ALLEGRO_KEY_R, 0, 0, true)
      && is_kid_dead (&current_kid->f))
    kid_resurrect (current_kid);

  /* HOME: focus multi-room view on kid */
  if (was_key_pressed (ALLEGRO_KEY_HOME, 0, 0, true))
    mr_focus_room (current_kid->f.c.room);

  /* SHIFT+HOME: center multi-room view */
  if (was_key_pressed (ALLEGRO_KEY_HOME, 0, ALLEGRO_KEYMOD_SHIFT, true))
    mr_center_room (mr.room);

  /* A: alternate between kid and its shadows */
  if (! active_menu
      && was_key_pressed (ALLEGRO_KEY_A, 0, 0, true)) {
    do {
      current_kid = &anima[(current_kid - anima + 1) % anima_nmemb];
    } while (current_kid->type != KID || ! current_kid->controllable);
    current_kid_id = current_kid->id;
    mr_focus_room (current_kid->f.c.room);
  }

  /* K: kill enemy */
  if (! active_menu
      && was_key_pressed (ALLEGRO_KEY_K, 0, 0, true)) {
    struct anim *ke = get_anim_by_id (current_kid->enemy_id);
    if (ke) {
      ke->current_lives = 0;
      play_sample (guard_hit_sample, ke->f.c.room);
    }
  }

  /* I: enable/disable immortal mode */
  if (! active_menu
      && was_key_pressed (ALLEGRO_KEY_I, 0, 0, true)) {
    immortal_mode = ! immortal_mode;
    current_kid->immortal = immortal_mode;
    xasprintf (&text, "%s MODE", immortal_mode
               ? "IMMORTAL" : "MORTAL");
    draw_bottom_text (NULL, text, 0);
    al_free (text);
  }

  /* SHIFT+S: incremet kid current lives */
  if (was_key_pressed (ALLEGRO_KEY_S, 0, ALLEGRO_KEYMOD_SHIFT, true))
    increase_kid_current_lives (current_kid);

  /* SHIFT+T: incremet kid total lives */
  if (was_key_pressed (ALLEGRO_KEY_T, 0, ALLEGRO_KEYMOD_SHIFT, true))
    increase_kid_total_lives (current_kid);

  /* SHIFT+W: float kid */
  if (was_key_pressed (ALLEGRO_KEY_W, 0, ALLEGRO_KEYMOD_SHIFT, true))
    float_kid (current_kid);

  /* CTRL+A: restart level */
  if (was_key_pressed (ALLEGRO_KEY_A, 0, ALLEGRO_KEYMOD_CTRL, true))
    quit_anim = RESTART_LEVEL;

  /* SHIFT+L: warp to next level */
  if (was_key_pressed (ALLEGRO_KEY_L, 0, ALLEGRO_KEYMOD_SHIFT, true)) {
    ignore_level_cutscene = true;
    quit_anim = NEXT_LEVEL;
  }

  /* SHIFT+M: warp to previous level */
  if (was_key_pressed (ALLEGRO_KEY_M, 0, ALLEGRO_KEYMOD_SHIFT, true)) {
    ignore_level_cutscene = true;
    quit_anim = PREVIOUS_LEVEL;
  }

  /* C: show direct coordinates */
  if (! active_menu
      && was_key_pressed (ALLEGRO_KEY_C, 0, 0, true)) {
    int s = mr.room;
    int l = roomd (s, LEFT);
    int r = roomd (s, RIGHT);
    int a = roomd (s, ABOVE);
    int b = roomd (s, BELOW);

    mr.select_cycles = SELECT_CYCLES;

    xasprintf (&text, "S%i L%i R%i A%i B%i", s, l, r, a, b);
    draw_bottom_text (NULL, text, 0);
    al_free (text);
  }

  /* SHIFT+C: show indirect coordinates */
  if (was_key_pressed (ALLEGRO_KEY_C, 0, ALLEGRO_KEYMOD_SHIFT, true)) {
    int a = roomd (mr.room, ABOVE);
    int b = roomd (mr.room, BELOW);
    int al = roomd (a, LEFT);
    int ar = roomd (a, RIGHT);
    int bl = roomd (b, LEFT);
    int br = roomd (b, RIGHT);

    mr.select_cycles = SELECT_CYCLES;

    xasprintf (&text, "LV%i AL%i AR%i BL%i BR%i",
               level.number, al, ar, bl, br);
    draw_bottom_text (NULL, text, 0);
    al_free (text);
  }

  /* SPACE: display remaining time */
  if (! active_menu
      && (was_key_pressed (ALLEGRO_KEY_SPACE, 0, 0, true)
          || was_button_pressed (joystick_time_button, true)))
    display_remaining_time ();

  /* +: increment and display remaining time */
  if (! active_menu
      && was_key_pressed (ALLEGRO_KEY_EQUALS, 0, ALLEGRO_KEYMOD_SHIFT, true)) {
    int t = time_limit - play_time;
    int d = t > SEC2CYC (60) ? SEC2CYC (+60) : SEC2CYC (+1);
    time_limit += d;
    display_remaining_time ();
  }

  /* -: decrement and display remaining time */
  if (! active_menu
      && was_key_pressed (ALLEGRO_KEY_MINUS, 0, 0, true)) {
    int t = time_limit - play_time;
    int d = t > SEC2CYC (60) ? SEC2CYC (-60) : SEC2CYC (-1);
    time_limit += d;
    display_remaining_time ();
  }

  /* TAB: display skill */
  if (was_key_pressed (ALLEGRO_KEY_TAB, 0, 0, true))
    display_skill (current_kid);

  /* CTRL+=: increment counter attack skill */
  if (was_key_pressed (ALLEGRO_KEY_EQUALS, 0, ALLEGRO_KEYMOD_CTRL, true)) {
    if (current_kid->skill.counter_attack_prob < 99)
      current_kid->skill.counter_attack_prob++;
    display_skill (current_kid);
  }

  /* CTRL+-: decrement counter attack skill */
  if (was_key_pressed (ALLEGRO_KEY_MINUS, 0, ALLEGRO_KEYMOD_CTRL, true)) {
    if (current_kid->skill.counter_attack_prob > -1)
      current_kid->skill.counter_attack_prob--;
    display_skill (current_kid);
  }

  /* ALT+=: increment counter defense skill */
  if (was_key_pressed (ALLEGRO_KEY_EQUALS, 0, ALLEGRO_KEYMOD_ALT, true)) {
    if (current_kid->skill.counter_defense_prob < 99)
      current_kid->skill.counter_defense_prob++;
    display_skill (current_kid);
  }

  /* ALT+-: decrement counter defense skill */
  if (was_key_pressed (ALLEGRO_KEY_MINUS, 0, ALLEGRO_KEYMOD_ALT, true)) {
    if (current_kid->skill.counter_defense_prob > -1)
      current_kid->skill.counter_defense_prob--;
    display_skill (current_kid);
  }

  /* F10: change guard mode */
  if (was_key_pressed (ALLEGRO_KEY_F10, 0, 0, true)) {
    char *gm_str = NULL;

    /* get next guard mode */
    switch (gm) {
    case ORIGINAL_GM: gm = GUARD_GM, gm_str = "GUARD"; break;
    case GUARD_GM: gm = FAT_GUARD_GM, gm_str = "FAT GUARD"; break;
    case FAT_GUARD_GM: gm = VIZIER_GM, gm_str = "VIZIER"; break;
    case VIZIER_GM: gm = SKELETON_GM, gm_str = "SKELETON"; break;
    case SKELETON_GM: gm = SHADOW_GM, gm_str = "SHADOW"; break;
    case SHADOW_GM: gm = ORIGINAL_GM, gm_str = "ORIGINAL"; break;
    }

    /* apply next guard mode */
    int i;
    for (i = 0; i < anima_nmemb; i++) apply_guard_mode (&anima[i], gm);

    xasprintf (&text, "GUARD MODE: %s", gm_str);
    draw_bottom_text (NULL, text, 0);
    al_free (text);
  }

  /* CTRL+G: save game */
  if (was_key_pressed (ALLEGRO_KEY_G, 0, ALLEGRO_KEYMOD_CTRL, true)
      && ! save_game_dialog_thread) {
    save_game_dialog_thread =
      create_thread (save_game_dialog, NULL);
    al_start_thread (save_game_dialog_thread);
    pause_game ();
  }

  /* Restart level after death */
  struct anim *k = get_anim_by_id (0);
  if (k->current_lives <= 0
      && ! game_paused) {
    death_timer++;
    if (death_timer < 12) k->sample = NULL;

    if (death_timer >= 12 && ! k->sample) {
      ALLEGRO_SAMPLE *sample;
      switch (k->death_reason) {
      case SHADOW_FIGHT_DEATH: sample = success_suspense_sample; break;
      case FIGHT_DEATH: sample = fight_death_sample; break;
      default: sample = death_sample; break;
      }
      k->sample = play_sample (sample, -1);
    }

    if (death_timer < 60 && ! active_menu) {
      key.keyboard.keycode = 0;
      button = -1;
    }

    if (death_timer >= 60) {
      if ((death_timer < 240 || death_timer % 12 < 8) && ! active_menu) {
        if (death_timer >= 252 && death_timer % 12 == 0)
          play_sample (press_key_sample, -1);
        xasprintf (&text, "Press Button to Continue");
        draw_bottom_text (NULL, text, -2);
        al_free (text);
      } else if (! active_menu) draw_bottom_text (NULL, "", -2);

      if ((key.keyboard.keycode || button != -1)
          && ! was_menu_key_pressed ())
        quit_anim = RESTART_LEVEL;
    }
  } else if (death_timer && ! game_paused) {
    death_timer = 0;
    draw_bottom_text (NULL, NULL, -2);
  }

  camera_follow_kid = (current_kid->f.c.room == mr.room)
    ? current_kid->id : -1;
}

static void
draw_level (void)
{
  draw_multi_rooms ();

  draw_lives (uscreen, get_anim_by_id (current_kid_id), vm);

  /* automatic remaining time display */
  int rem_time = time_limit - play_time;
  if ((rem_time % SEC2CYC (5 * 60) == 0
       && last_auto_show_time != rem_time
       && anim_cycle > SEC2CYC (720))
      || (auto_rem_time_1st_cycle >= 0
          && last_auto_show_time != rem_time
          && anim_cycle >= auto_rem_time_1st_cycle
          && anim_cycle <= auto_rem_time_1st_cycle + 6)
      || rem_time <= SEC2CYC (60)) {
    display_remaining_time ();
    if (rem_time <= SEC2CYC (60) && rem_time % SEC2CYC (1) == 0
        && ! play_time_stopped)
      play_sample (press_key_sample, -1);
    last_auto_show_time = rem_time;
  }
  if (rem_time <= 0) quit_anim = OUT_OF_TIME;

  if (game_paused && ! active_menu)
    draw_bottom_text (NULL, "GAME PAUSED", -1);
}

void
display_remaining_time (void)
{
  char *text;
  int t = time_limit - play_time;
  if (t < 0) t = 0;
  int m = t / SEC2CYC (60) + ((t % SEC2CYC (60)) ? 1 : 0);
  if (t > SEC2CYC (60)) xasprintf (&text, "%i MINUTES LEFT", m);
  else xasprintf (&text, "%i SECONDS LEFT", CYC2SEC (t));
  draw_bottom_text (NULL, text, 0);
  al_free (text);
}

void
display_skill (struct anim *k)
{
  char *text;
  struct anim *ke = get_anim_by_id (k->enemy_id);
  if (ke) xasprintf (&text, "KCA%i KCD%i A%i CA%i D%i CD%i",
                      k->skill.counter_attack_prob + 1,
                      k->skill.counter_defense_prob + 1,
                      ke->skill.attack_prob + 1,
                      ke->skill.counter_attack_prob + 1,
                      ke->skill.defense_prob + 1,
                      ke->skill.counter_defense_prob + 1);
  else xasprintf (&text, "KCA%i KCD%i",
                  k->skill.counter_attack_prob + 1,
                  k->skill.counter_defense_prob + 1);
  draw_bottom_text (NULL, text, 0);
  al_free (text);
}

static void
draw_lives (ALLEGRO_BITMAP *bitmap, struct anim *k, enum vm vm)
{
  bool nrlc = no_recursive_links_continuity;
  no_recursive_links_continuity = true;

  if (is_room_visible (k->f.c.room)) {
    draw_kid_lives (bitmap, k, vm);
    struct anim *ke = NULL;
    if (k->enemy_id != -1) {
      ke = get_anim_by_id (k->enemy_id);
      if (ke) draw_guard_lives (bitmap, ke, vm);
    }
  }

  no_recursive_links_continuity = nrlc;
}

void
pause_game (void)
{
  memset (&key, 0, sizeof (key));
  button = -1;
  game_paused = true;
}

void
unpause_game (void)
{
  game_paused = false;
  draw_bottom_text (NULL, NULL, 0);
  memset (&key, 0, sizeof (key));
  button = -1;
}

/************************************/
/* NOT USED ALTERNATIVE UNDO METHOD */
/************************************/

void
apply_to_diff_pos (struct diff *d, void (*func) (struct pos *p))
{
  size_t base = ((uint8_t *) &level.con[0][0][0]) - ((uint8_t *) &level);
  size_t end = ((uint8_t *) &level.con[ROOMS - 1][FLOORS - 1][PLACES - 1])
    - ((uint8_t *) &level);

  size_t i, j;
  for (i = 0; i < d->count; i++) {
    if (d->line[i].offset < base) continue;
    if (d->line[i].offset > end) continue;

    size_t base_con = d->line[i].offset - base;

    struct pos prev_p = {-1,-1,-1};
    for (j = 0; j < d->line[i].count; j++) {
      struct pos p;
      size_t qr = (base_con + j * d->unit_size)
        / (sizeof (struct con) * PLACES * FLOORS);
      size_t rr = (base_con + j * d->unit_size)
        % (sizeof (struct con) * PLACES * FLOORS);

      size_t qf = rr / (sizeof (struct con) * PLACES);
      size_t rf = rr % (sizeof (struct con) * PLACES);

      size_t qp = rf / sizeof (struct con);

      p.room = qr;
      p.floor = qf;
      p.place = qp;

      if (! peq (&p, &prev_p)) {
        /* printf ("%s: %i,%i,%i\n", */
        /*         (func == destroy_con_at_pos) ? "destroy" : "register", */
        /*         p.room, p.floor, p.place); */
        func (&p);
        prev_p = p;
      }
    }
  }
}

void
level_undo (struct diffset *diffset, int dir, char *prefix)
{
  char *text;
  char *dir_str = (dir >= 0) ? "REDO" : "UNDO";

  bool b = can_apply_diffset (diffset, dir);

  if (undo_msg) al_free (undo_msg);

  if (! b) {
    xasprintf (&undo_msg, "NO FURTHER %s %s", prefix, dir_str);
    editor_msg (undo_msg, 24);
    return;
  }

  size_t i = diffset->current;

  apply_to_diff_pos (&diffset->diff[(dir >= 0) ? i + 1 : i], destroy_con_at_pos);
  apply_diffset_diff (diffset, &level, sizeof (level), dir, &text);
  apply_to_diff_pos (&diffset->diff[(dir >= 0) ? i + 1 : i], register_con_at_pos);
  /* update_cache (em, vm); */
  /* generate_stars (); */

  xasprintf (&undo_msg, "%s %s: %s", prefix, dir_str, text);
  editor_msg (undo_msg, 24);
}
