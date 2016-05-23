/*
  anim.c -- animation module;

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

/* set to true to quit animation */
enum quit_anim quit_anim;
bool pause_anim;
bool cutscene;
bool next_frame_inv;
uint64_t anim_cycle;
ALLEGRO_TIMER *timer;
int anim_freq = SCRIPT_HZ_DEFAULT;

ALLEGRO_EVENT_QUEUE *event_queue;

struct anim *anima;
size_t anima_nmemb;

void
play_anim (void (*draw_callback) (void),
           void (*compute_callback) (void))
{
  if (cutscene) set_multi_room (1, 1);
  else apply_mr_fit_mode ();

  anim_cycle = 0;
  quit_anim = NO_QUIT;

  acknowledge_resize ();
  ALLEGRO_EVENT event;
  timer = create_timer (anim_freq > 0 ? 1.0 / anim_freq : -anim_freq + 2);
  if (! event_queue) event_queue = create_event_queue ();
  al_register_event_source (event_queue, get_display_event_source (display));
  al_register_event_source (event_queue, get_keyboard_event_source ());
  al_register_event_source (event_queue, get_joystick_event_source ());
  al_register_event_source (event_queue, get_mouse_event_source ());
  al_register_event_source (event_queue, get_timer_event_source (video_timer));
  al_register_event_source (event_queue, get_timer_event_source (timer));
  al_start_timer (timer);

  while (! quit_anim) {
    al_wait_for_event (event_queue, &event);
    switch (event.type) {
    case ALLEGRO_EVENT_TIMER:
      if (event.timer.source == timer) {
        /* update mouse pos */
        get_mouse_pos (&mouse_pos);

        /* load configuration */
        if (load_config_dialog_thread
            && al_get_thread_should_stop (load_config_dialog_thread)) {
          char *filename;
          al_join_thread (load_config_dialog_thread, (void *) &filename);
          al_destroy_thread (load_config_dialog_thread);
          load_config_dialog_thread = NULL;
          if (filename) {
            ALLEGRO_TEXTLOG *textlog = load_config (filename);
            al_free (filename);
            if (textlog)
              al_register_event_source
                (event_queue, get_native_text_log_event_source (textlog));
          }
        }

        /* save game */
        if (save_game_dialog_thread
            && al_get_thread_should_stop (save_game_dialog_thread)) {
          char *filename;
          al_join_thread (save_game_dialog_thread, (void *) &filename);
          al_destroy_thread (save_game_dialog_thread);
          save_game_dialog_thread = NULL;
          unpause_game ();
          if (filename) {
            save_game (filename);
            al_free (filename);
          }
        }

        if (was_key_pressed (ALLEGRO_KEY_ESCAPE, 0, ALLEGRO_KEYMOD_ALT, false))
          pause_anim = true;
        if (was_key_pressed (ALLEGRO_KEY_ESCAPE, 0, ALLEGRO_KEYMOD_CTRL, true))
          pause_anim = false;

        kid_debug ();

        if (anim_cycle > 0) {
          if (is_video_effect_started ()) {
            effect_counter--;
            show ();
          } else show ();
        }

        if (! pause_anim
            || was_key_pressed (ALLEGRO_KEY_ESCAPE, 0,
                                ALLEGRO_KEYMOD_ALT, true)) {
          if (compute_callback) compute_callback ();
          clear_bitmap (uscreen, TRANSPARENT_COLOR);
          draw_callback ();
          play_samples ();
          if (! game_paused) anim_cycle++;
        }
        if (! cutscene) editor ();
        if (bottom_text_timer) bottom_text_timer++;
        draw_bottom_text (uscreen, NULL, 0);
        drop_all_events_from_source
          (event_queue, get_timer_event_source (timer));
        al_set_timer_count (timer, 0);

      } else if (event.timer.source == video_timer) {
        show ();
        /* drop_all_events_from_source */
        /*   (event_queue, get_timer_event_source (video_timer)); */
        /* al_set_timer_count (video_timer, 0); */
      }
      memset (&key, 0, sizeof (key));
      break;
    case ALLEGRO_EVENT_DISPLAY_RESIZE:
      force_full_redraw = true;
      show ();
      acknowledge_resize ();
      break;
    case ALLEGRO_EVENT_DISPLAY_EXPOSE:
      show ();
      break;
    case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
      is_display_focused = false;
      break;
    case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
      is_display_focused = true;
      break;
    case ALLEGRO_EVENT_DISPLAY_CLOSE:
      quit_anim = QUIT_GAME;
      break;
    case ALLEGRO_EVENT_NATIVE_DIALOG_CLOSE:
      al_close_native_text_log ((ALLEGRO_TEXTLOG *) event.user.data1);
      break;
    case ALLEGRO_EVENT_JOYSTICK_CONFIGURATION:
      calibrate_joystick ();
      break;
    case ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN:
      button = event.joystick.button;
      break;
    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
      switch (event.mouse.button) {
      case 1: enter_exit_editor (); break;
      case 3:
        if (edit != EDIT_NONE)
          mr_center_room (mr.room);
        break;
      default: break;
      }

      /* struct coord c; get_mouse_coord (&c); */
      /* printf ("%i,%i,%i\n", c.room, c.x, c.y); */
      break;
    case ALLEGRO_EVENT_MOUSE_AXES:
      if (edit == EDIT_NONE) break;
      int dz = event.mouse.dz;  /* vertical */
      int dw = event.mouse.dw;  /* horizontal */

      ALLEGRO_KEYBOARD_STATE ks;

      al_get_keyboard_state (&ks);
      bool ctrl = al_key_down (&ks, ALLEGRO_KEY_LCTRL)
        || al_key_down (&ks, ALLEGRO_KEY_RCTRL);

      if (ctrl) {
        int dt;
        dt = dw;
        dw = dz;
        dz = dt;
      }

      if (flip_gamepad_vertical) dz *= -1;
      if (flip_gamepad_horizontal) dw *= -1;

      if (dz < 0) while (dz++ < 0) mr_view_trans (BELOW);
      else if (dz > 0) while (dz-- > 0) mr_view_trans (ABOVE);
      if (dw < 0) while (dw++ < 0) mr_view_trans (LEFT);
      else if (dw > 0) while (dw-- > 0) mr_view_trans (RIGHT);

      break;
    case ALLEGRO_EVENT_KEY_CHAR:
      key = event;

      char *text = NULL;

      /* (: decrease time frequency */
      if (was_key_pressed (0, '(', -1, true))
        ui_change_anim_freq (anim_freq - 1);

      /* ): increase time frenquency */
      if (was_key_pressed (0, ')', -1, true))
        ui_change_anim_freq (anim_freq + 1);

      /* F8: enable/disable level editor */
      if (was_key_pressed (ALLEGRO_KEY_F8, 0, 0, true))
        enter_exit_editor ();

      /* CTRL+R: restart game */
      if (was_key_pressed (ALLEGRO_KEY_R, 0, ALLEGRO_KEYMOD_CTRL, true))
        quit_anim = RESTART_GAME;

      /* CTRL+Q: quit game */
      if (was_key_pressed (ALLEGRO_KEY_Q, 0, ALLEGRO_KEYMOD_CTRL, true))
        quit_anim = QUIT_GAME;

      /* CTRL+V: show engine name and version */
      if (was_key_pressed (ALLEGRO_KEY_V, 0, ALLEGRO_KEYMOD_CTRL, true)) {
        xasprintf (&text, "MININIM %s", VERSION);
        draw_bottom_text (NULL, text, 0);
        al_free (text);
      }

      /* CTRL+S: enable/disable sound */
      if (was_key_pressed (ALLEGRO_KEY_S, 0, ALLEGRO_KEYMOD_CTRL, true)) {
        audio_enabled = ! audio_enabled;
        enable_audio (audio_enabled);
        xasprintf (&text, "SOUND %s", audio_enabled ? "ON" : "OFF");
        draw_bottom_text (NULL, text, 0);
        al_free (text);
      }

      /* D: change display mode */
      if (! active_menu
          && was_key_pressed (ALLEGRO_KEY_D, 0, 0, true)) {
        if (display_mode < 0) draw_bottom_text (NULL, "DISPLAY MODE: DESKTOP", 0);
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
              xasprintf (&text, "DISPLAY MODE: %ix%i", d.width, d.height);
              draw_bottom_text (NULL, text, 0);
              al_free (text);
            } else draw_bottom_text (NULL, "DISPLAY MODES QUERY FAILED", 0);
          } else draw_bottom_text (NULL, "NO DISPLAY MODE AVAILABLE", 0);
        }
      }

      /* F: enable/disable fullscreen mode */
      if (! active_menu
          && was_key_pressed (ALLEGRO_KEY_F, 0, 0, true)) {
        if (display_mode < 0) {
          force_full_redraw = true;
          char *boolean;
          if (is_fullscreen ()) {
            al_set_display_flag (display, ALLEGRO_FULLSCREEN_WINDOW, false);
            boolean = "OFF";
            show_mouse_cursor ();
          } else {
            al_set_display_flag (display, ALLEGRO_FULLSCREEN_WINDOW, true);
            boolean = "ON";
            hide_mouse_cursor ();
          }
          xasprintf (&text, "FULLSCREEN MODE %s", boolean);
          draw_bottom_text (NULL, text, 0);
          al_free (text);
        } else draw_bottom_text (NULL, "NON-DESKTOP MODE IS FULLSCREEN ONLY", 0);
      }

      /* SHIFT+I: flip screen */
      if (was_key_pressed (ALLEGRO_KEY_I, 0, ALLEGRO_KEYMOD_SHIFT, true)) {
        char *flip = "NONE";
        ALLEGRO_MOUSE_STATE m;
        al_get_mouse_state (&m);
        int w = al_get_display_width (display);
        int h = al_get_display_height (display);
        switch (screen_flags) {
        case 0:
          al_set_mouse_xy (display, m.x, h - m.y);
          screen_flags = ALLEGRO_FLIP_VERTICAL;
          flip = "VERTICAL"; break;
        case ALLEGRO_FLIP_VERTICAL:
          al_set_mouse_xy (display, w - m.x, h - m.y);
          screen_flags = ALLEGRO_FLIP_HORIZONTAL;
          flip = "HORIZONTAL"; break;
        case ALLEGRO_FLIP_HORIZONTAL:
          al_set_mouse_xy (display, m.x, h - m.y);
          screen_flags = ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL;
          flip = "VERTICAL + HORIZONTAL"; break;
        case ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL:
          al_set_mouse_xy (display, w - m.x, h - m.y);
          screen_flags = 0;
          break;
        }
        xasprintf (&text, "DISPLAY FLIP: %s", flip);
        draw_bottom_text (NULL, text, 0);
        al_free (text);
      }

      /* SHIFT+K: flip gamepad */
      if (was_key_pressed (ALLEGRO_KEY_K, 0, ALLEGRO_KEYMOD_SHIFT, true)) {
        char *flip = "NONE";
        if (! flip_gamepad_vertical && ! flip_gamepad_horizontal) {
          flip_gamepad_vertical = true;
          flip = "VERTICAL";
        } else if (flip_gamepad_vertical && ! flip_gamepad_horizontal) {
          flip_gamepad_vertical = false;
          flip_gamepad_horizontal = true;
          flip = "HORIZONTAL";
        } else if (! flip_gamepad_vertical && flip_gamepad_horizontal) {
          flip_gamepad_vertical = true;
          flip = "VERTICAL + HORIZONTAL";
        } else if (flip_gamepad_vertical && flip_gamepad_horizontal) {
          flip_gamepad_vertical = false;
          flip_gamepad_horizontal = false;
        }
        xasprintf (&text, "GAMEPAD FLIP: %s", flip);
        draw_bottom_text (NULL, text, 0);
        al_free (text);
      }

      /* CTRL+J: calibrate joystick */
      if (was_key_pressed (ALLEGRO_KEY_J, 0, ALLEGRO_KEYMOD_CTRL, true)) {
        calibrate_joystick ();
        xasprintf (&text, "JOYSTICK %s", joystick ? "CALIBRATED" : "NOT FOUND");
        draw_bottom_text (NULL, text, 0);
        al_free (text);
      }

      /* F9: change hue palette */
      if (was_key_pressed (ALLEGRO_KEY_F9, 0, 0, true)) {
        char *em_str = NULL;

        if (force_hue) {
          switch (hue) {
          case HUE_NONE: hue = HUE_GREEN; em_str = "GREEN"; break;
          case HUE_GREEN: hue = HUE_GRAY; em_str = "GRAY"; break;
          case HUE_GRAY: hue = HUE_YELLOW; em_str = "YELLOW"; break;
          case HUE_YELLOW: hue = HUE_BLUE; em_str = "BLUE"; break;
          case HUE_BLUE: hue = level.hue;
            em_str = "ORIGINAL"; force_hue = false; break;
          }
        } else {
          hue = HUE_NONE; em_str = "NONE"; force_hue = true;
        }

        xasprintf (&text, "HUE MODE: %s", em_str);
        draw_bottom_text (NULL, text, 0);
        al_free (text);
      }

      /* F11: change environment mode */
      if (was_key_pressed (ALLEGRO_KEY_F11, 0, 0, true)) {
        char *em_str = NULL;

        if (force_em) {
          switch (em) {
          case DUNGEON: em = PALACE; em_str = "PALACE"; break;
          case PALACE: em = level.em;
            em_str = "ORIGINAL"; force_em = false; break;
          }
        } else {
          em = DUNGEON; em_str = "DUNGEON"; force_em = true;
        }

        xasprintf (&text, "ENVIRONMENT MODE: %s", em_str);
        draw_bottom_text (NULL, text, 0);
        al_free (text);
      }

      /* F12: change video mode */
      if (was_key_pressed (ALLEGRO_KEY_F12, 0, 0, true)) {
        char *vm_str = NULL;

        switch (vm) {
        case CGA:
          if (hgc) {
            hgc = false; vm = VGA; vm_str = "VGA";
          } else {
            hgc = true; vm = CGA; vm_str = "HGC";
          }
          break;
        case EGA: vm = CGA; vm_str = "CGA"; break;
        case VGA: vm = EGA; vm_str = "EGA"; break;
        }

        xasprintf (&text, "VIDEO MODE: %s", vm_str);
        draw_bottom_text (NULL, text, 0);
        al_free (text);
      }

      /* CTRL+L: load configuration */
      if (was_key_pressed (ALLEGRO_KEY_L, 0, ALLEGRO_KEYMOD_CTRL, true)
          && ! load_config_dialog_thread) {
        load_config_dialog_thread =
          create_thread (load_config_dialog, NULL);
        al_start_thread (load_config_dialog_thread);
      }

      break;
    }
  }

  al_stop_timer (timer);
}

void
ui_change_anim_freq (int f)
{
  char *text;
  anim_freq = f;
  al_set_timer_speed (timer, f > 0 ? 1.0 / f : -f + 2);
  if (anim_freq > 0) xasprintf (&text, "TIME FREQ: %iHz", anim_freq);
  else xasprintf (&text, "TIME FREQ: 1/%iHz", -anim_freq + 2);
  draw_bottom_text (NULL, text, 0);
  al_free (text);
}




int
create_anim (struct anim *a0, enum anim_type t, struct pos *p, enum dir dir)
{
  struct anim a;
  int i = anima_nmemb;
  memset (&a, 0, sizeof (a));

  if (a0) a = *a0;
  else {
    a.type = t;
    a.original_type = t;
    a.f.dir = dir;
    a.f.c.room = p->room;
    a.controllable = false;
    a.enemy_pos = (struct pos) {-1,-1,-1};
  }

  a.id = i;
  a.level_id = -1;

  switch (a.type) {
  case NO_ANIM: default: break;
  case KID: create_kid (a0, &a, p, dir); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    create_guard (a0, &a, p, dir); break;
  case MOUSE: create_mouse (a0, &a, p, dir); break;
  }

  anima = add_to_array (&a, 1, anima, &anima_nmemb, i, sizeof (a));
  anima[i].f.id = &anima[i];
  return i;
}

void
destroy_anim (struct anim *a)
{
  forget_enemy (a);

  if (a->type == KID) destroy_kid (a);

  size_t i =  a - anima;
  anima = remove_from_array (anima, &anima_nmemb, i, 1, sizeof (*a));
}

void
destroy_anims (void)
{
  while (anima_nmemb) destroy_anim (&anima[0]);
  anima = NULL;
  anima_nmemb = 0;
}

struct anim *
get_anim_by_id (int id)
{
  int i;
  if (id < 0) return NULL;
  for (i = 0; i < anima_nmemb; i++)
    if (anima[i].id == id) return &anima[i];
  return NULL;
}

struct anim *
get_guard_anim_by_level_id (int id)
{
  int i;
  if (id < 0) return NULL;
  for (i = 0; i < anima_nmemb; i++)
    if (is_guard (&anima[i]) && anima[i].level_id == id)
      return &anima[i];
  return NULL;
}

void
draw_anim_frame (ALLEGRO_BITMAP *bitmap, struct anim *a, enum vm vm)
{
  switch (a->type) {
  case NO_ANIM: default: break;
  case KID: draw_kid_frame (bitmap, a, vm); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    draw_guard_frame (bitmap, a, vm); break;
  case MOUSE: draw_mouse_frame (bitmap, a, vm); break;
  }
}

void
draw_anims (ALLEGRO_BITMAP *bitmap, enum em em, enum vm vm)
{
  struct coord ml; struct pos pml, pmlr, pmlra;
  struct anim *a;

  /* coord_wa = true; */

  qsort (anima, anima_nmemb, sizeof (*a), compare_anims);

  size_t i;
  for (i = 0; i < anima_nmemb; i++) {
    a = &anima[i];
    a->f.id = a;
    if (a->invisible) continue;

    _ml (&a->f, &ml); pos (&ml, &pml);
    prel (&pml, &pmlr, 0, +1);
    prel (&pml, &pmlra, -1, +1);

    draw_anim_frame (bitmap, a, vm);

    draw_falling_loose_floor (bitmap, &pmlr, em, vm);
    draw_falling_loose_floor (bitmap, &pmlra, em, vm);
    draw_room_anim_fg (bitmap, em, vm, a);
  }

  /* coord_wa = false; */
}

int
compare_anims (const void *_a0, const void *_a1)
{
  struct coord tr0, tr1;

  struct anim *a0 = (struct anim *) _a0;
  struct anim *a1 = (struct anim *) _a1;

  if (is_kid_stairs (&a0->f) && ! is_kid_stairs (&a1->f))
    return -1;
  if (! is_kid_stairs (&a0->f) && is_kid_stairs (&a1->f))
    return 1;

  _tr (&a0->f, &tr0);
  _tr (&a1->f, &tr1);

  coord2room (&tr1, tr0.room, &tr1);

  if (tr0.room < tr1.room) return -1;
  if (tr0.room > tr1.room) return 1;

  if (is_near (a0, a1)) {
    if (tr0.x < tr1.x) return -1;
    if (tr0.x > tr1.x) return 1;
    if (a0->id < a1->id) return -1;
    if (a0->id > a1->id) return 1;
  } else {
    struct coord o = {tr0.room,0,ORIGINAL_HEIGHT};

    double d0 = dist_coord (&o, &tr0);
    double d1 = dist_coord (&o, &tr1);

    if (d0 < d1) return -1;
    if (d0 > d1) return 1;
  }

  return 0;
}

void
draw_anim_if_at_pos (ALLEGRO_BITMAP *bitmap, struct anim *a, struct pos *p,
                     enum vm vm)
{
  struct coord nc;
  struct pos np, pbl, pmbo, pbr, pml, pm, pmr, ptl, pmt, ptr;
  survey (_bl, pos, &a->f, &nc, &pbl, &np);
  survey (_mbo, pos, &a->f, &nc, &pmbo, &np);
  survey (_br, pos, &a->f, &nc, &pbr, &np);
  survey (_ml, pos, &a->f, &nc, &pml, &np);
  survey (_m, pos, &a->f, &nc, &pm, &np);
  survey (_mr, pos, &a->f, &nc, &pmr, &np);
  survey (_tl, pos, &a->f, &nc, &ptl, &np);
  survey (_mt, pos, &a->f, &nc, &pmt, &np);
  survey (_tr, pos, &a->f, &nc, &ptr, &np);

  if (! peq (&pbl, p)
      && ! peq (&pmbo, p)
      && ! peq (&pbr, p)
      && ! peq (&pml, p)
      && ! peq (&pm, p)
      && ! peq (&pmr, p)
      && ! peq (&ptl, p)
      && ! peq (&pmt, p)
      && ! peq (&ptr, p)) return;

  draw_anim_frame (bitmap, a, vm);
}

void
clear_anims_keyboard_state (void)
{
  int i;
  for (i = 0; i < anima_nmemb; i++)
    memset (&anima[i].key, 0, sizeof (anima[i].key));
}

bool
is_anim_dead (struct frame *f)
{
  return is_kid_dead (f)
    || is_guard_dead (f);
}

bool
is_anim_chopped (struct frame *f)
{
  return is_kid_chopped (f)
    || is_guard_chopped (f);
}

bool
is_anim_fall (struct frame *f)
{
  return is_kid_fall (f)
    || is_guard_fall (f);
}

bool
is_fightable_anim (struct anim *a)
{
  return a->type == KID || a->type == SHADOW
    || a->type == GUARD || a->type == FAT_GUARD
    || a->type == SKELETON || a->type == VIZIER;
}

void
anim_die_suddenly (struct anim *a)
{
  switch (a->type) {
  case NO_ANIM: default: break;
  case KID: kid_die_suddenly (a); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_die_suddenly (a); break;
  }
}

void
anim_die_spiked (struct anim *a)
{
  switch (a->type) {
  case NO_ANIM: default: break;
  case KID: kid_die_spiked (a); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_die_spiked (a); break;
  }
}

void
anim_die_chopped (struct anim *a)
{
  switch (a->type) {
  case NO_ANIM: default: break;
  case KID: kid_die_chopped (a); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_die_chopped (a); break;
  }
}

void
anim_die (struct anim *a)
{
  switch (a->type) {
  case NO_ANIM: default: break;
  case KID: kid_die (a); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_die (a); break;
  }
}

void
anim_sword_hit (struct anim *a)
{
  switch (a->type) {
  case NO_ANIM: default: break;
  case KID: kid_sword_hit (a); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_hit (a); break;
  }
}

void
anim_fall (struct anim *a)
{
  switch (a->type) {
  case NO_ANIM: default: break;
  case KID: kid_fall (a); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_fall (a); break;
  }
}

void
anim_walkf (struct anim *a)
{
  switch (a->type) {
  case NO_ANIM: default: break;
  case KID: kid_sword_walkf (a); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_walkf (a); break;
  }
}

void
anim_walkb (struct anim *a)
{
  switch (a->type) {
  case NO_ANIM: default: break;
  case KID: kid_sword_walkb (a); break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_walkb (a); break;
  }
}

struct coord *
splash_coord (struct frame *f, struct coord *c)
{
  int w = al_get_bitmap_width (v_kid_splash);
  int h = al_get_bitmap_height (v_kid_splash);
  int fw = al_get_bitmap_width (f->b);
  int fh = al_get_bitmap_height (f->b);
  c->x = f->c.x + (fw / 2) - (w / 2);
  c->y = f->c.y + (fh / 2) - (h / 2);
  c->room = f->c.room;
  return c;
}





void
draw_frame (ALLEGRO_BITMAP *bitmap, struct frame *f)
{
  if (! f->b) return;

  draw_bitmapc (f->b, bitmap, &f->c, f->flip);
}

void
draw_xframe (ALLEGRO_BITMAP *bitmap, struct frame *f, struct frame_offset *xf)
{
  struct coord c;
  if (! xf->b) return;
  draw_bitmapc (xf->b, bitmap, xframe_coord (f, xf, &c), f->flip);
}

struct coord *
xframe_coord (struct frame *f, struct frame_offset *xf, struct coord *c)
{
  int w = al_get_bitmap_width (xf->b);
  _tf (f, c);
  c->x += (f->dir == LEFT) ? xf->dx : -xf->dx - w + 1;
  c->y += xf->dy;
  c->room = f->c.room;
  return c;
}

struct frame *
xframe_frame (struct frame *f, struct frame_offset *xf, struct frame *nf)
{
  *nf = *f;
  nf->b = xf->b;
  xframe_coord (f, xf, &nf->c);
  return nf;
}

void
select_frame (struct anim *a, struct frameset *fs, int i)
{
  a->fo.b = fs[i].frame;
  a->fo.dx = fs[i].dx;
  a->fo.dy = fs[i].dy;
  a->i = i;
}

void
select_xframe (struct frame_offset *xf, struct frameset *fs, int j)
{
  xf->b = fs[j].frame;
  xf->dx = fs[j].dx;
  xf->dy = fs[j].dy;
  /* a->j = j; */
}

struct frame *
next_frame (struct frame *f, struct frame *nf, struct frame_offset *fo)
{
  *nf = *f;

  nf->oc = f->c;
  nf->ob = f->b;

  int ow = al_get_bitmap_width (nf->b);
  int oh = al_get_bitmap_height (nf->b);
  int w = al_get_bitmap_width (fo->b);
  int h = al_get_bitmap_height (fo->b);

  if (next_frame_inv) nf->c.x += (nf->dir == LEFT) ? ow - w - fo->dx : fo->dx;
  else nf->c.x += (nf->dir == LEFT) ? fo->dx : ow - w - fo->dx;
  nf->c.y += oh - h + fo->dy;

  nf->b = fo->b;
  if (! cutscene) nframe (nf, &nf->c);
  return nf;
}

struct frame *
prev_frame (struct frame *f, struct frame *pf)
{
  *pf = *f;
  pf->c = f->oc;
  pf->b = f->ob;
  return pf;
}

bool
frame_eq (struct frame *f0, struct frame *f1)
{
  return coord_eq (&f0->c, &f1->c)
    && f0->b == f1->b
    && f0->dir == f1->dir
    && f0->flip == f1->flip;
}

bool
wait_anim (int cycles)
{
  static int i = 0;
  if (i == 0) i = cycles + 1;
  return --i;
}

ALLEGRO_COLOR
start_anim_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;
  r = add_char (r, -64);
  g = add_char (g, -64);
  b = add_char (b, -64);
  return al_map_rgba (r, g, b, 100);
}
