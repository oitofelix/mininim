/*
  ui-command.c -- user interface command module;

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

static void ui_change_prob_skill (int *skill, int new);
static void display_skill (struct actor *k);


void
ui_editor (void)
{
  if (edit == EDIT_NONE) {
    if (cutscene) toggle_menu_visibility ();
    else {
      if (replay_mode == NO_REPLAY ) {
        enter_editor (NULL);
        show_menu ();
      } else  {
        toggle_menu_visibility ();
        print_replay_mode (0);
      }
    }
  } else {
    exit_editor (NULL);
    hide_menu ();
  }
}

void
ui_load_game (void)
{
  load_config_dialog_thread =
    al_create_thread (dialog_thread, &load_config_dialog);
  al_start_thread (load_config_dialog_thread);
}

void
ui_load_config (void)
{
  load_config_dialog_thread =
    al_create_thread (dialog_thread, &load_config_dialog);
  al_start_thread (load_config_dialog_thread);
}

void
ui_save_game (void)
{
  save_game_dialog_thread =
    al_create_thread (dialog_thread, &save_game_dialog);
  al_start_thread (save_game_dialog_thread);
  pause_animation (true);
}

void
ui_audio_volume (float volume)
{
  char *key = "SOUND GAIN";
  char *value;
  value = xasprintf ("%.1f", volume);

  set_audio_volume (volume);

  char *status = NULL;
  if (volume == VOLUME_RANGE_MIN) status = "OFF";
  else if (volume < VOLUME_RANGE_LOW) status = "LOW";
  else if (volume < VOLUME_RANGE_MEDIUM) status = "MEDIUM";
  else if (volume <= VOLUME_RANGE_MAX) status = "HIGH";

  ui_msg (1, "SOUND %s", status);

  ui_save_setting (NULL, key, value);
  al_free (value);
}

void
ui_screenshot (void)
{
  save_picture_dialog_thread =
    al_create_thread (dialog_thread, &save_picture_dialog);
  al_start_thread (save_picture_dialog_thread);
  pause_animation (true);
}

void
ui_restart_game (void)
{
  exit_editor (NULL);
  quit_anim = RESTART_GAME;
}

void
ui_start_game (void)
{
  quit_anim = CUTSCENE_KEY_PRESS;
}

void
ui_quit_game (void)
{
  quit_anim = QUIT_GAME;
}

void
ui_full_screen (void)
{
  if (display_mode < 0) {
    char *key = "FULLSCREEN";
    char *value;
    force_full_redraw = true;
    if (is_fullscreen ()) {
      al_set_display_flag (display, ALLEGRO_FULLSCREEN_WINDOW, false);
      value = "OFF";
      al_show_mouse_cursor (display);
      show_menu ();
    } else {
      al_set_display_flag (display, ALLEGRO_FULLSCREEN_WINDOW, true);
      value = "ON";
      if (edit == EDIT_NONE) hide_menu ();
    }

    ui_msg (1, "%s: %s", key, value);

    ui_save_setting (NULL, key, value);
  } else ui_msg (1, "NON-DESKTOP MODE IS FULLSCREEN ONLY");
}

void
ui_zoom_fit (struct mr *mr, enum mr_fit_mode fit)
{
  char *key = "MULTI ROOM FIT MODE";
  char *value;

  switch (fit) {
  case MR_FIT_NONE:
    mr->fit_mode = MR_FIT_NONE;
    value = "NONE";
    break;
  case MR_FIT_STRETCH:
    mr->fit_mode = MR_FIT_STRETCH;
    value = "STRETCH";
    break;
  case MR_FIT_RATIO:
    mr->fit_mode = MR_FIT_RATIO;
    value = "RATIO";
    break;
  default:
    assert (false);
    return;
  }

  apply_mr_fit_mode (mr, global_level.rlink, global_level.room_nmemb);

  ui_msg (1, "ZOOM FIT: %s", value);

  ui_save_setting (NULL, key, value);
}

bool
ui_mr_set_dim (struct mr *mr, int w, int h, bool correct_mouse)
{
  char *key = "MULTI ROOM";
  char *value;

  if (w < 1 || h < 1) {
    ui_msg (1, "MULTI-ROOM %ix%i", mr->w, mr->h);
    return false;
  }

  struct mouse_coord m;
  get_mouse_coord (mr, &m);

  if (w != mr->w || h != mr->h) mr_set_dim (mr, w, h);

  mr_center_room (mr, mr->room, global_level.rlink,
                  global_level.room_nmemb);

  if (mr_coord (mr, m.c.room, -1, NULL, NULL) && correct_mouse)
    set_mouse_coord (mr, &m);

  ui_msg (1, "MULTI-ROOM %ix%i", mr->w, mr->h);

  value = xasprintf ("%ix%i", mr->w, mr->h);
  ui_save_setting (NULL, key, value);
  al_free (value);

  return true;
}

void
ui_show_coordinates (void)
{
  struct room_linking *rlink = global_level.rlink;
  size_t room_nmemb = global_level.room_nmemb;

  int s = global_mr.room;
  int l = roomd (rlink, room_nmemb, s, LEFT);
  int r = roomd (rlink, room_nmemb, s, RIGHT);
  int a = roomd (rlink, room_nmemb, s, ABOVE);
  int b = roomd (rlink, room_nmemb, s, BELOW);

  global_mr.select_cycles = SELECT_CYCLES;

  ui_msg (1, "S%i L%i R%i A%i B%i", s, l, r, a, b);
}

void
ui_show_indirect_coordinates (void)
{
  struct room_linking *rlink = global_level.rlink;
  size_t room_nmemb = global_level.room_nmemb;

  int a = roomd (rlink, room_nmemb, global_mr.room, ABOVE);
  int b = roomd (rlink, room_nmemb, global_mr.room, BELOW);
  int al = roomd (rlink, room_nmemb, a, LEFT);
  int ar = roomd (rlink, room_nmemb, a, RIGHT);
  int bl = roomd (rlink, room_nmemb, b, LEFT);
  int br = roomd (rlink, room_nmemb, b, RIGHT);

  global_mr.select_cycles = SELECT_CYCLES;

  ui_msg (1, "LV%i AL%i AR%i BL%i BR%i",
          global_level.n, al, ar, bl, br);
}

void
ui_hue_mode (enum hue new_hue)
{
  char *key = "HUE MODE";
  char *value = NULL;

  switch (new_hue) {
  default: /* HUE_ORIGINAL */
    force_hue = false;
    hue = global_level.hue;
    value = "ORIGINAL";
    break;
  case HUE_NONE:
    force_hue = true;
    hue = HUE_NONE;
    value = "NONE";
    break;
  case HUE_GREEN:
    force_hue = true;
    hue = HUE_GREEN;
    value = "GREEN";
    break;
  case HUE_GRAY:
    force_hue = true;
    hue = HUE_GRAY;
    value = "GRAY";
    break;
  case HUE_YELLOW:
    force_hue = true;
    hue = HUE_YELLOW;
    value = "YELLOW";
    break;
  case HUE_BLUE:
    force_hue = true;
    hue = HUE_BLUE;
    value = "BLUE";
    break;
  }

  set_string_var (&hue_mode, value);

  ui_msg (1, "%s: %s", key, value);

  ui_save_setting (NULL, key, value);
}

void
ui_gm (enum gm new_gm)
{
  if (replay_mode != NO_REPLAY) {
    print_replay_mode (0);
    return;
  }

  char *key = "GUARD MODE";
  char *value;

  switch (new_gm) {
  case ORIGINAL_GM:
    gm = ORIGINAL_GM;
    value = "ORIGINAL";
    break;
  case GUARD_GM:
    gm = GUARD_GM;
    value = "GUARD";
    break;
  case FAT_GM:
    gm = FAT_GM;
    value = "FAT";
    break;
  case VIZIER_GM:
    gm = VIZIER_GM;
    value = "VIZIER";
    break;
  case SKELETON_GM:
    gm = SKELETON_GM;
    value = "SKELETON";
    break;
  case SHADOW_GM:
    gm = SHADOW_GM;
    value = "SHADOW";
    break;
  }

  /* apply next guard mode */
  int i;
  for (i = 0; i < actor_nmemb; i++) apply_guard_mode (&actor[i], gm);

  ui_msg (1, "%s: %s", key, value);

  ui_save_setting (NULL, key, value);
}

void
ui_em (enum em new_em)
{
  char *key = "ENVIRONMENT MODE";
  char *value = NULL;

  switch (new_em) {
  default: /* ORIGINAL_EM */
    force_em = false;
    em = global_level.em;
    value = "ORIGINAL";
    break;
  case DUNGEON:
    force_em = true;
    em = DUNGEON;
    value = "DUNGEON";
    break;
  case PALACE:
    force_em = true;
    em = PALACE;
    value = "PALACE";
    break;
  }

  set_string_var (&env_mode, value);

  ui_msg (1, "%s: %s", key, value);

  ui_save_setting (NULL, key, value);
}

void
ui_vm (char *requested_vm)
{
  if (strcmp (video_mode, requested_vm))
    setup_video_mode (requested_vm);

  if (! video_mode) {
    ui_msg (1, "NO VIDEO MODE AVAILABLE");
    return;
  }

  char *key = "VIDEO MODE";
  ui_msg (1, "%s: %s", key, video_mode);
  ui_save_setting (NULL, key, video_mode);

  draw_bottom_text (uscreen, NULL, 0);
}

void
ui_flip_screen (int flags, bool correct_mouse, bool save_only)
{
  char *key = "DISPLAY FLIP MODE";
  char *value;

  switch (flags) {
  case 0: value = "NONE"; break;
  case ALLEGRO_FLIP_VERTICAL: value = "VERTICAL"; break;
  case ALLEGRO_FLIP_HORIZONTAL: value = "HORIZONTAL"; break;
  case ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL:
    value = "VERTICAL-HORIZONTAL"; break;
  default: assert (false); break;
  }

  ui_save_setting (NULL, key, value);

  if (save_only) return;

  if (correct_mouse) {
    int w = al_get_display_width (display);
    int h = al_get_display_height (display);

    ALLEGRO_MOUSE_STATE m;

    if ((screen_flags & ALLEGRO_FLIP_VERTICAL
         && ! (flags & ALLEGRO_FLIP_VERTICAL))
        || (! (screen_flags & ALLEGRO_FLIP_VERTICAL)
            && flags & ALLEGRO_FLIP_VERTICAL)) {
      al_get_mouse_state (&m);
      al_set_mouse_xy (display, m.x, h - m.y);
    }

    if ((screen_flags & ALLEGRO_FLIP_HORIZONTAL
         && ! (flags & ALLEGRO_FLIP_HORIZONTAL))
        || (! (screen_flags & ALLEGRO_FLIP_HORIZONTAL)
            && flags & ALLEGRO_FLIP_HORIZONTAL)) {
      al_get_mouse_state (&m);
      al_set_mouse_xy (display, w - m.x, m.y);
    }
  }

  potion_flags = 0;
  screen_flags = flags;

  ui_msg (1, "%s: %s", key, value);

  ui_flip_gamepad (flip_gamepad_vertical, flip_gamepad_horizontal, true);
  ui_save_setting (NULL, "MIRROR MODE", NULL);
}

void
ui_inhibit_screensaver (bool inhibit)
{
  char *key = "INHIBIT SCREENSAVER";
  char *value = inhibit ? "ON" : "OFF";

  inhibit_screensaver = inhibit;
  al_inhibit_screensaver (inhibit_screensaver);

  ui_msg (1, "%s: %s", key, value);

  ui_save_setting (NULL, key, value);
}

void
ui_room_drawing (bool draw)
{
  char *key = "ROOM DRAWING";
  char *value = draw ? "ON" : "OFF";

  no_room_drawing = ! draw;
  force_full_redraw = true;

  ui_msg (1, "%s: %s", key, value);
}

void
ui_gamepad_mode (enum gpm new_gpm)
{
  char *key = "GAMEPAD MODE";
  char *value;

  switch (new_gpm) {
  case KEYBOARD:
    gpm = KEYBOARD;
    ui_msg (1, "KEYBOARD MODE");
    value = "KEYBOARD";
    ui_save_setting (NULL, key, value);
    break;
  case JOYSTICK:
    if (calibrate_joystick ()) {
      enum gpm old_gpm = gpm;
      gpm = JOYSTICK;
      value = "JOYSTICK";
      ui_save_setting (NULL, key, value);
      gamepad_rumble (1.0, 0.6);
      ui_msg (1, "JOYSTICK %s",
              old_gpm == JOYSTICK ? "CALIBRATED" : "MODE");
    } else ui_msg (1, "JOYSTICK NOT FOUND");
    break;
  default: assert (false);
  }
}

void
ui_flip_gamepad (bool v, bool h, bool save_only)
{
  char *key = "GAMEPAD FLIP MODE";
  char *value = NULL;

  if (! v && ! h) value = "NONE";
  else if (v && ! h) value = "VERTICAL";
  else if (! v && h) value = "HORIZONTAL";
  else if (v && h) value = "VERTICAL-HORIZONTAL";

  ui_save_setting (NULL, key, value);

  if (save_only) return;

  flip_gamepad_vertical = v;
  flip_gamepad_horizontal = h;

  ui_msg (1, "%s: %s", key, value);

  ui_flip_screen (screen_flags, false, true);
  ui_save_setting (NULL, "MIRROR MODE", NULL);
}

void
ui_play_replay (void)
{
  if (! title_demo && replay_mode == PLAY_REPLAY) {
    print_replay_chain_aborted ();
    stop_replaying (2);
  } else create_load_replay_thread ();
}

void
ui_record_replay (void)
{
  if (replay_mode == RECORD_REPLAY) create_save_replay_thread ();
  else if (recording_replay_countdown < 0)
    prepare_for_recording_replay ();
  else {
    ui_msg (2, "RECORDING ABORTED");
    recording_replay_countdown = -1;
  }
}

void
ui_jump_to_level (int n)
{
  switch (replay_mode) {
  case NO_REPLAY:
    if (n == global_level.n) {
      quit_anim = RESTART_LEVEL;
    } else {
      ignore_level_cutscene = true;
      next_level_number = n;
      start_level_time = play_time;
      quit_anim = NEXT_LEVEL;
    }
    break;
  case PLAY_REPLAY:
    if (n == replay_index)
      quit_anim = REPLAY_RESTART_LEVEL;
    else if (0 <= n && n < replay_chain_nmemb) {
      replay_next_number = n;
      quit_anim = REPLAY_NEXT;
    } else ui_msg (1, "%s", n < (int) replay_index
                   ? "NO PREVIOUS REPLAY" : "NO NEXT REPLAY");
    break;
  default: print_replay_mode (0); break;
  }
}

void
ui_jump_to_level_rel (int d)
{
  ui_jump_to_level
    (replay_mode == PLAY_REPLAY ? replay_index + d : global_level.n + d);
}

void
ui_jump_to_level_menu (int i, int lower)
{
  if (title_demo) goto no_replay;

  int n;
  switch (replay_mode) {
  case NO_REPLAY: no_replay:
    n = i + lower;
    if (n == global_level.n) return;
    break;
  case PLAY_REPLAY:
    n = i - 1 + lower;
    if (n == replay_index) return;
    break;
  default: return;
  }
  ui_jump_to_level (n);
}

void
ui_time (void)
{
  display_remaining_time (0);
}

void
ui_skills (void)
{
  struct actor *k = get_actor_by_id (current_kid_id);
  display_skill (k);
}

void
ui_increase_time_frequency (void)
{
  if (anim_freq >= DEFAULT_HZ)
    ui_change_anim_freq (next_multiple (anim_freq, DEFAULT_HZ));
  else ui_change_anim_freq (next_multiple (anim_freq, 2));
}

void
ui_decrease_time_frequency (void)
{
  if (anim_freq > DEFAULT_HZ)
    ui_change_anim_freq (next_multiple (anim_freq, -DEFAULT_HZ));
  else if (anim_freq > 2)
    ui_change_anim_freq (next_multiple (anim_freq, -2));
}

void
ui_toggle_time_frequency_constraint (void)
{
  static int f = DEFAULT_HZ;

  if (anim_freq > 0) {
    f = anim_freq;
    ui_change_anim_freq (0);
  } else ui_change_anim_freq (f);
}

void
ui_change_anim_freq (int f)
{
  change_anim_freq (f);
  if (f > 0) ui_msg (1, "TIME FREQ: %iHz", f);
}

void
ui_toggle_pause_game (void)
{
  if (is_game_paused ()) pause_game (false);
  else pause_game (true);
}

void
ui_next_frame (void)
{
  step_cycle = 1;
}

void
print_game_paused (int priority)
{
  char *text;

  switch (replay_mode) {
  case RECORD_REPLAY: text = "RECORDING PAUSED"; break;
  case PLAY_REPLAY: text = "REPLAY PAUSED"; break;
  case NO_REPLAY: default: text = "GAME PAUSED"; break;
  }

  ui_msg (priority, "%s", text);
}




void
ui_resurrect (void)
{
  if (replay_mode == NO_REPLAY) {
    struct actor *k = get_actor_by_id (current_kid_id);
    kid_resurrect (k);
  } else print_replay_mode (0);
}

void
ui_kill_enemy (void)
{
  if (replay_mode == NO_REPLAY) {
    struct actor *k = get_actor_by_id (current_kid_id);
    struct actor *ke = get_actor_by_id (k->enemy_id);
    if (ke) {
      survey (_m, pos, &ke->f, NULL, &ke->p, NULL);
      actor_die (ke);
      play_audio (&guard_hit_audio, NULL, ke->id);
    }
  } else print_replay_mode (0);
}

void
ui_float (void)
{
  if (replay_mode == NO_REPLAY) {
    struct actor *k = get_actor_by_id (current_kid_id);
    float_kid (k);
  } else print_replay_mode (0);
}

void
ui_immortal (bool immortal)
{
  if (replay_mode == NO_REPLAY) {
    char *key = "IMMORTAL MODE";
    char *value = immortal ? "ON" : "OFF";
    struct actor *k = get_actor_by_id (current_kid_id);
    if (k->current_hp <= 0 && immortal) kid_resurrect (k);
    immortal_mode = immortal;
    k->immortal = immortal;
    ui_msg (1, "%s: %s", key, value);
    ui_save_setting (NULL, key, value);
  } else print_replay_mode (0);
}

void
ui_fill_hp (void)
{
  if (replay_mode == NO_REPLAY) {
    struct actor *k = get_actor_by_id (current_kid_id);
    increase_kid_current_hp (k);
  } else print_replay_mode (0);
}

void
ui_add_hp (void)
{
  if (replay_mode == NO_REPLAY) {
    struct actor *k = get_actor_by_id (current_kid_id);
    increase_kid_total_hp (k);
    total_hp = k->total_hp;
  } else print_replay_mode (0);
}

void
ui_change_time (int m)
{
  if (replay_mode == NO_REPLAY) {
    static int last_r_min = -1;

    if (last_r_min < 0)
      last_r_min = precise_unit (time_limit - play_time, 60 * DEFAULT_HZ);

    int64_t r = time_limit - play_time;
    if (m < 0 && r <= 60 * DEFAULT_HZ) return;
    else if (m > 0 && r <= 60 * DEFAULT_HZ)
      time_limit = play_time + max_int (2, m) * 60 * DEFAULT_HZ;
    else if (m < 0 && r <= -m * 60 * DEFAULT_HZ)
      time_limit = play_time + 60 * DEFAULT_HZ;
    else time_limit = play_time + next_multiple (r, m * 60 * DEFAULT_HZ);

    int r_min = precise_unit (time_limit - play_time, 60 * DEFAULT_HZ);
    if (r_min == last_r_min) {
      time_limit += m * 60 * DEFAULT_HZ;
      last_r_min = precise_unit (time_limit - play_time, 60 * DEFAULT_HZ);
    } else last_r_min = r_min;

    display_remaining_time (1);
  } else print_replay_mode (1);
}

void
ui_change_prob_skill (int *holder, int new)
{
  if (replay_mode == NO_REPLAY) {
    struct actor *k = get_actor_by_id (current_kid_id);
    new = new > -1 ? new : -1;
    new = new < 99 ? new : 99;
    *holder = new;
    display_skill (k);
    skill = k->skill;
  } else print_replay_mode (1);
}

void
ui_change_kca (int d)
{
  struct actor *k = get_actor_by_id (current_kid_id);
  ui_change_prob_skill
    (&k->skill.counter_attack_prob,
     next_multiple (k->skill.counter_attack_prob + 1, d) - 1);
}

void
ui_change_kcd (int d)
{
  struct actor *k = get_actor_by_id (current_kid_id);
  ui_change_prob_skill
    (&k->skill.counter_defense_prob,
     next_multiple (k->skill.counter_defense_prob + 1, d) - 1);
}

void
ui_home (void)
{
  mr_focus_room (&global_mr, get_actor_by_id (current_kid_id)->f.c.room,
                 global_level.rlink, global_level.room_nmemb);
}





void
ui_version (void)
{
  ui_msg (1, "MININIM %s", VERSION);
}

void
ui_about_screen (bool value)
{
  if (value) {
    main_menu_enabled = false;
    if (! message_box_thread_id) {
      message_box_thread_id =
        al_create_thread (message_box_thread, &about_dialog);
      al_start_thread (message_box_thread_id);
    }
    about_screen = true;
    pause_animation (true);
  } else if (! message_box_thread_id) {
    main_menu_enabled = true;
    about_screen = false;
    pause_animation (false);
 }
}

void
ui_next_display_mode (void)
{
  char *key = "DISPLAY MODE";
  char *value;

  if (display_mode < 0)
    ui_msg (1, "DISPLAY MODE: DESKTOP");
  else {
    int n = al_get_num_display_modes ();
    if (n) {
      int display_mode_bkp = display_mode;
    next_display_mode:
      display_mode = (display_mode + 1) % n;
      int w = al_get_display_width (display);
      int h = al_get_display_height (display);
      ALLEGRO_DISPLAY_MODE d;
      if (al_get_display_mode (display_mode, &d)) {
        if (d.width == w && d.height == h
            && display_mode != display_mode_bkp)
          goto next_display_mode;
        al_resize_display (display, d.width, d.height);
        ui_msg (1, "%s: %ix%i", key, d.width, d.height);
        value = xasprintf ("%i", display_mode);
        ui_save_setting (NULL, key, value);
        al_free (value);
      } else ui_msg (1, "DISPLAY MODES QUERYING FAILED");
    } else ui_msg (1, "NO DISPLAY MODE AVAILABLE");
  }
}





void
ui_save_replay_favorites (void)
{
  size_t i;
  int m = min_int (replay_favorite_nmemb, REPLAY_FAVORITE_MID_NMEMB);
  for (i = 0; i < m; i++) {
    char *section = xasprintf ("REPLAY FAVORITE %zu", i);

    char *key = "FILE";
    char *value = xasprintf ("%s", replay_favorite[i].filename);
    ui_save_setting (section, key, value);
    al_free (value);

    key = "CYCLE";
    value = xasprintf ("%" PRIu64, replay_favorite[i].cycle);
    ui_save_setting (section, key, value);
    al_free (value);

    al_free (section);
  }

  char *section = NULL;
  do {
    al_free (section);
    section = xasprintf ("REPLAY FAVORITE %zu", i++);
  } while (ui_save_setting (section, NULL, NULL));
  al_free (section);
}

void
ui_load_replay_favorites (void)
{
  free_replay_favorites ();
  size_t i = 0;

  while (i < REPLAY_FAVORITE_MID_NMEMB) {
    char *section = xasprintf ("REPLAY FAVORITE %zu", i);

    char *file_str = ui_get_setting (section, "FILE");
    char *cycle_str = ui_get_setting (section, "CYCLE");

    al_free (section);

    if (! file_str || ! cycle_str) {
      al_free (file_str);
      al_free (cycle_str);
      break;
    }

    uint64_t cycle;
    if (sscanf (cycle_str, "%" PRIu64, &cycle) != 1) break;
    add_replay_favorite (file_str, cycle);

    al_free (file_str);
    al_free (cycle_str);

    i++;
  }
}

void
ui_add_replay_favorite (void)
{
  assert (replay_mode == PLAY_REPLAY);
  size_t i = 0;
  for (i = 0; i < replay_favorite_nmemb; i++)
    if (replay_favorite[i].cycle == anim_cycle
        && ! strcmp (replay_favorite[i].filename,
                     replay_chain[replay_index].filename)) {
      ui_msg (1, "DUPLICATE REPLAY FAVORITE");
      return;
    }
  if (replay_favorite_nmemb < REPLAY_FAVORITE_MID_NMEMB) {
    add_current_replay_favorite ();
    ui_save_replay_favorites ();
    ui_msg (1, "REPLAY FAVORITE ADDED");
  } else ui_msg (1, "NO REPLAY FAVORITE SLOT FREE");
}

void
ui_go_to_replay_favorite (int i)
{
  if (replay_mode != NO_REPLAY) stop_replaying (0);
  free_replay_chain ();
  add_replay_file_to_replay_chain (replay_favorite[i].filename);
  replay_favorite_cycle = replay_favorite[i].cycle;
  anim_freq = 0;
  prepare_for_playing_replay (0);
}

void
ui_replace_replay_favorite (int i)
{
  assert (replay_mode == PLAY_REPLAY);
  al_free (replay_favorite[i].filename);
  replay_favorite[i].filename =
    xasprintf ("%s", replay_chain[replay_index].filename);
  replay_favorite[i].cycle = anim_cycle;
  ui_save_replay_favorites ();
  ui_msg (1, "REPLAY FAVORITE REPLACED");
}

void
ui_remove_replay_favorite (int i)
{
  remove_replay_favorite (i);
  ui_save_replay_favorites ();
  ui_msg (1, "REPLAY FAVORITE REMOVED");
}




bool
ui_save_setting (char *section, char *key, char *value)
{
  ALLEGRO_CONFIG *config = NULL;

  if (al_filename_exists (config_filename)) {
    config = al_load_config_file (config_filename);
    if (! config) goto error;
    const char *file_type_str =
      al_get_config_value (config, NULL, "FILE TYPE");
    if (! file_type_str
        || strcasecmp (file_type_str, "MININIM CONFIGURATION")) {
      al_destroy_config (config);
      config = al_create_config ();
    }
  } else if (value) {
    if (! al_make_directory (user_settings_dir)) goto error;
    config = al_create_config ();
  } else return false;

  if (! config) goto error;

  remove_config_entry (config, NULL, "FILE TYPE");
  al_set_config_value (config, NULL, "FILE TYPE", "MININIM CONFIGURATION");

  bool existing;
  if (key) {
    existing = remove_config_entry (config, section, key);
    if (value) al_set_config_value (config, section, key, value);
  } else existing = remove_config_section (config, section);

  if (! al_save_config_file (config_filename, config)) goto error;

  al_destroy_config (config);

  return existing;

 error:
  if (config) al_destroy_config (config);
  error (0, al_get_errno (), "can't save setting '[%s] %s=%s' to '%s'",
         section, key, value, config_filename);
  return false;
}

char *
ui_get_setting (char *section, char *key)
{
  ALLEGRO_CONFIG *config = NULL;

  config = al_load_config_file (config_filename);
  if (! config) goto error;
  const char *file_type_str =
    al_get_config_value (config, NULL, "FILE TYPE");
  if (! file_type_str
      || strcasecmp (file_type_str, "MININIM CONFIGURATION"))
    goto error;

  const char *value = al_get_config_value (config, section, key);
  if (value) value = xasprintf ("%s", value);

  al_destroy_config (config);

  return (char *) value;

 error:
  if (config) al_destroy_config (config);
  /* error (0, al_get_errno (), "can't get setting '[%s] %s' from '%s'", */
  /*        section, key, config_filename); */
  return NULL;
}

bool
ui_msg (int priority, const char *template, ...)
{
  va_list ap;
  va_start (ap, template);

  char *text;
  vasprintf (&text, template, ap);
  bool r = draw_bottom_text (NULL, text, priority);
  al_free (text);

  va_end (ap);
  return r;
}

bool
ui_msg_clear (int priority)
{
  return draw_bottom_text (NULL, NULL, priority);
}





bool
in_mirror_mode (void)
{
  return ! flip_gamepad_vertical && flip_gamepad_horizontal
    && screen_flags == ALLEGRO_FLIP_HORIZONTAL;
}

void
mirror_mode (bool mirror)
{
  if (mirror) {
    flip_gamepad_vertical = false;
    flip_gamepad_horizontal = true;
    screen_flags = ALLEGRO_FLIP_HORIZONTAL;
  } else {
    flip_gamepad_vertical = false;
    flip_gamepad_horizontal = false;
    screen_flags = 0;
  }
}

void
ui_mirror_mode (bool mirror)
{
  char *key = "MIRROR MODE";
  char *value = mirror ? "ON" : "OFF";

  mirror_mode (mirror);

  ui_msg (1, "%s: %s", key, value);

  ui_save_setting (NULL, "DISPLAY FLIP MODE", NULL);
  ui_save_setting (NULL, "GAMEPAD FLIP MODE", NULL);
  ui_save_setting (NULL, key, value);
}




bool
display_remaining_time (int priority)
{
  int t = time_limit - play_time;
  if (t < 0) t = 0;
  int tm = t > (60 * DEFAULT_HZ)
    ? precise_unit (t, 60 * DEFAULT_HZ)
    : precise_unit (t, 1 * DEFAULT_HZ);
  return ui_msg (priority, "%i %s", tm,
                 t > (60 * DEFAULT_HZ)
                 ? "MINUTES LEFT"
                 : "SECONDS LEFT");
}

void
display_skill (struct actor *k)
{
  struct actor *ke = get_actor_by_id (k->enemy_id);
  if (ke)
    ui_msg (1, "KCA%i KCD%i A%i CA%i D%i CD%i",
            k->skill.counter_attack_prob + 1,
            k->skill.counter_defense_prob + 1,
            ke->skill.attack_prob + 1,
            ke->skill.counter_attack_prob + 1,
            ke->skill.defense_prob + 1,
            ke->skill.counter_defense_prob + 1);
  else
    ui_msg (1, "KCA%i KCD%i",
            k->skill.counter_attack_prob + 1,
            k->skill.counter_defense_prob + 1);
}




void
ui_move_locked_place_selection (struct mr *mr, enum dir d)
{
  if (! selection_locked) return;

  struct pos p;

  switch (d) {
  case ABOVE:
    prel (&selection_pos, &p, -1, +0);
    break;
  case LEFT:
    prel (&selection_pos, &p, +0, -1);
    break;
  case RIGHT:
    prel (&selection_pos, &p, +0, +1);
    break;
  case BELOW:
    prel (&selection_pos, &p, +1, +0);
    break;
  default: assert (false);
  }

  select_pos (mr, &p);
}

void
unlock_relock_place_selection (void)
{
  if (! selection_locked && is_valid_pos (&last_selection_pos))
    selection_pos = last_selection_pos;
  selection_locked = ! selection_locked;
}

void
ui_add_rect_sel_to_sel_ring (enum rect_sel_type type, struct pos *dest_pos)
{
  struct pos *p =
    (selection_locked && is_valid_pos (&selection_pos))
    ? &selection_pos : dest_pos;

  bool success = add_rect_sel_to_sel_ring
    (&global_mr, &global_sel_ring, type, dest_pos, p);

  if (success && p == &selection_pos) selection_pos = *dest_pos;
  else if (! success) ui_msg (1, "INVALID START-END POINTS FOR MR ORIGIN");
}
