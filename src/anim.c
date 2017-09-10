/*
  anim.c -- animation module;

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

/* set to true to quit animation */
enum quit_anim quit_anim;
bool pause_anim;
bool cutscene;
bool next_frame_inv;
uint64_t anim_cycle;
ALLEGRO_TIMER *timer;
int anim_freq = DEFAULT_HZ;
int anim_freq_real;

ALLEGRO_EVENT_QUEUE *event_queue;

anim_callback_t anim_draw_callback, anim_compute_callback,
  anim_cleanup_callback;

void
play_anim (anim_callback_t draw_callback,
           anim_callback_t compute_callback,
           anim_callback_t cleanup_callback)
{
  assert (! anim_cycle
          && ! anim_draw_callback
          && ! anim_compute_callback
          && ! anim_cleanup_callback);

  anim_cycle = 0;
  quit_anim = NO_QUIT;
  anim_draw_callback = draw_callback;
  anim_compute_callback = compute_callback;
  anim_cleanup_callback = cleanup_callback;

  al_acknowledge_resize (display);

  ALLEGRO_EVENT event;
  anim_freq_real = anim_freq > 0 ? anim_freq : UNLIMITED_HZ;

  timer = al_create_timer
    (anim_freq > 0 ? 1.0 / anim_freq : 1.0 / UNLIMITED_HZ);
  al_register_event_source (event_queue, al_get_timer_event_source (timer));
  menu_timer = al_create_timer (MENU_PERIOD);
  al_register_event_source
    (event_queue,al_get_timer_event_source (menu_timer));
  iup_timer = al_create_timer (IUP_PERIOD);
  al_register_event_source
    (event_queue,al_get_timer_event_source (iup_timer));

  /* register menu event sources */
  al_register_event_source
    (event_queue, al_get_default_menu_event_source ());
  al_register_event_source (event_queue, main_menu_event_source ());
  aux_menu ();
  al_register_event_source (event_queue, aux_menu_event_source ());

  al_flush_event_queue (event_queue);
  al_start_timer (timer);
  al_start_timer (menu_timer);
  al_start_timer (iup_timer);

  double prev_time = al_get_time ();

  reset_haptic ();

  key.keycode = 0;
  joystick_button = -1;

  invalid_pos (&mouse_pos);
  invalid_pos (&selection_pos);
  selection_locked = false;

  while (! quit_anim) {
    L_gc (main_L);

    if (al_is_event_queue_empty (event_queue)) {
      unlock_lua ();
      al_wait_for_event (event_queue, &event);
      lock_lua ();
    } else al_get_next_event (event_queue, &event);

    switch (event.type) {
    case ALLEGRO_EVENT_TIMER:
      if (event.timer.source == iup_timer) {
        /* IUP events */
#if WINDOWS_PORT
        IupFlush ();
#else
        for (int i = 1; i <= 100; i++) IupLoopStep ();
        IupFlush ();
#endif
        drop_all_events_from_source
          (event_queue, al_get_timer_event_source (iup_timer));
        al_set_timer_count (iup_timer, 0);
      } else if (event.timer.source == video_timer) {
        if (rendering == BOTH_RENDERING || rendering == VIDEO_RENDERING)
          show ();
        else stop_video_effect ();
        /* drop_all_events_from_source */
        /*   (event_queue, get_timer_event_source (video_timer)); */
        /* al_set_timer_count (video_timer, 0); */
      } else if (event.timer.source == timer) {
        /* ensures Lua stack is empty */
        if (DEBUG && lua_gettop (main_L)) {
          L_dump_stack (main_L);
          assert (false);
        }

        /* check for replay favorite cycle */
        if (replay_favorite_cycle > 0
            && anim_cycle == replay_favorite_cycle) {
          replay_favorite_cycle = 0;
          change_anim_freq (DEFAULT_HZ);
          pause_game (true);
          ui_msg (2, "REPLAY FAVORITE REACHED");
        }

        struct replay *replay = get_replay ();

        /* detect incomplete replays */
        struct actor *k;
        if (! title_demo
            && replay_mode == PLAY_REPLAY
            && (anim_cycle >= replay->packed_gamepad_state_nmemb
                + REPLAY_STUCK_THRESHOLD
                || ((k = get_actor_by_id (current_kid_id))
                    && k->current_hp <= 0
                    && death_timer >= SEC2CYC (8))))
          quit_anim = REPLAY_INCOMPLETE;

        /* /\* ---- *\/ */
        /* while (anim_cycle == 1039) { */
        /*   show (); */
        /*   al_rest (0.1); */
        /* } */
        /* /\* ---- *\/ */

        /* compute actual time frequency */
        anim_freq_real = 1.0 / (al_get_time () - prev_time);
        prev_time = al_get_time ();

        /* replay handler */
        start_recording_replay (2);

        /* update selection position */
        if (! cutscene) {
          get_mouse_pos (&mouse_pos);
          if (! selection_locked)
            get_mouse_pos (&selection_pos);
        }

        /* message box */
        handle_message_box_thread ();

        /* load configuration */
        handle_load_config_thread (2);

        /* save game */
        handle_save_game_thread (2);

        /* save picture */
        handle_save_picture_thread (2);

        /* save replay */
        handle_save_replay_thread (2);

        /* load replay */
        handle_load_replay_thread (2);

        kid_debug ();

        if (anim_cycle > 0 && ! is_video_effect_started ()
            && (rendering == BOTH_RENDERING
                || rendering == VIDEO_RENDERING
                || update_replay_progress (NULL)))
          show ();

        if (! pause_anim) {
          if (compute_callback) compute_callback ();
          clear_bitmap (uscreen, TRANSPARENT_COLOR);
          uint32_t random_seed_before_draw;
          if (replay_mode != NO_REPLAY)
            random_seed_before_draw = random_seed;
          if (draw_callback) draw_callback ();
          if (replay_mode != NO_REPLAY)
            random_seed = random_seed_before_draw;
          play_audio_instances ();
          if (! title_demo && replay_mode != PLAY_REPLAY)
            execute_haptic ();

          if (cleanup_callback) cleanup_callback ();

          if (! is_game_paused ())
            anim_cycle++;

          if (! title_demo
              && replay_mode == PLAY_REPLAY
              && (rendering == BOTH_RENDERING
                  || rendering == VIDEO_RENDERING)) {
            int progress;
            if (update_replay_progress (&progress))
              fprintf (stderr, "%3i%%\r", progress);
          }

          /* if (replay_mode == PLAY_REPLAY) debug_random_seed (); */
          if (! cutscene) editor ();
          if (bottom_text_timer) bottom_text_timer++;
          draw_bottom_text (uscreen, NULL, 0);
        }
        drop_all_events_from_source
          (event_queue, al_get_timer_event_source (timer));
        al_set_timer_count (timer, 0);

      /* fprintf (stderr, "KEY DOWN: %i, %s, %c\n", key.modifiers, */
      /*          al_keycode_to_name (key.keycode), */
      /*          toupper (key2char (&key))); */

        if (about_screen
            && (was_any_key_pressed () || ! message_box_thread_id))
          ui_about_screen (false);

        key.keycode = 0;
        joystick_button = -1;
      } else if (event.timer.source == menu_timer) {
        /* update main menu to reflect programatic changes */
        main_menu ();
        drop_all_events_from_source
          (event_queue, al_get_timer_event_source (menu_timer));
        al_set_timer_count (menu_timer, 0);
      }
      break;
    case ALLEGRO_EVENT_DISPLAY_RESIZE:
      force_full_redraw = true;
      show ();
      al_acknowledge_resize (display);
      break;
    case ALLEGRO_EVENT_DISPLAY_EXPOSE:
      show ();
      break;
    case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
      al_clear_keyboard_state (event.display.source);
      break;
    case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
      break;
    case ALLEGRO_EVENT_DISPLAY_CLOSE:
      quit_anim = QUIT_GAME;
      break;
    case ALLEGRO_EVENT_NATIVE_DIALOG_CLOSE:
      close_text_log (&event);
      break;
    case ALLEGRO_EVENT_JOYSTICK_CONFIGURATION:
      {
        enum gpm gpm_old = gpm;
        bool c = calibrate_joystick ();
        if (gpm_old == JOYSTICK && ! c)
          ui_gamepad_mode (KEYBOARD);
      }
      break;
    case ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN:
      joystick_button = event.joystick.button;
      break;
    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
      if (about_screen) {
        ui_about_screen (false);
        break;
      }

      if (pause_anim) break;

      switch (event.mouse.button) {
      case 1: ui_editor (); break;
      case 3: {
        struct pos p; get_mouse_pos (&p);
        ui_place_kid (get_actor_by_id (current_kid_id), &p);
        break;
      }
      default: break;
      }

      /* int w = al_get_display_width (display); */
      /* int h = al_get_display_height (display); */
      /* ALLEGRO_MOUSE_STATE mouse_state; */
      /* al_get_mouse_state (&mouse_state); */
      /* printf ("display: %i, %i; mouse: %i, %i\n", */
      /*         w, h, mouse_state.x, mouse_state.y); */

      /* struct mouse_coord mc; get_mouse_coord (&mc); */
      /* printf ("%i,%i,%i\n", mc.c.room, mc.c.x, mc.c.y); */
      break;
    case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
      /* for some reason popup menus don't show properly in response
         to ALLEGRO_EVENT_MOUSE_BUTTON_DOWN */
      if (event.mouse.button == 2) show_aux_menu ();
      break;
    case ALLEGRO_EVENT_MOUSE_AXES:
      if (pause_anim) break;
      if (edit == EDIT_NONE) break;

      keep_all_depressible_floors ();

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

      if (dz < 0) while (dz++ < 0) mr_row_trans (BELOW);
      else if (dz > 0) while (dz-- > 0) mr_row_trans (ABOVE);
      if (dw < 0) while (dw++ < 0) mr_row_trans (LEFT);
      else if (dw > 0) while (dw-- > 0) mr_row_trans (RIGHT);

      break;



    case ALLEGRO_EVENT_MENU_CLICK:
      dispatch_menu_event (&event);
      break;



    case ALLEGRO_EVENT_KEY_DOWN:
      if (! is_mod_key (event.keyboard.keycode))
        key.keycode = event.keyboard.keycode;
      key.modifiers = get_key_modifiers ();

      /* fprintf (stderr, "KEY DOWN: %i, %s, %c\n", key.modifiers, */
      /*          al_keycode_to_name (key.keycode), */
      /*          toupper (key2char (&key))); */

      if (key.keycode == 0) break;

      if (pause_anim) break;

      animation_hotkeys_cb (NULL, 0);
      break;
    }
  }

  if (replay_mode == NO_REPLAY && recording_replay_countdown >= 0) {
    main_menu_enabled = false;
    main_menu ();
    while (recording_replay_countdown >= 0) {
      al_rest (1.0 / DEFAULT_HZ);
      start_recording_replay (2);
      draw_bottom_text (uscreen, NULL, 0);
      show ();
      process_display_events ();
    }
    main_menu_enabled = true;
  }

  if (replay_mode == RECORD_REPLAY) {
    create_save_replay_thread ();
    while (! al_get_thread_should_stop (save_replay_dialog_thread)) {
      process_display_events ();
      al_rest (0.01);
    }
    handle_save_replay_thread (1);
  }

  if (! title_demo
      && replay_mode == PLAY_REPLAY) {
    switch (quit_anim) {
    case OUT_OF_TIME: quit_anim = REPLAY_OUT_OF_TIME; break;
    case NEXT_LEVEL: quit_anim = REPLAY_COMPLETE; break;
    case REPLAY_INCOMPLETE: break;
    case REPLAY_NEXT: break;
    case REPLAY_RESTART_LEVEL: break;
    default:
      print_replay_chain_aborted ();
      stop_replaying (1);
      if (command_line_replay) exit (-1);
      break;
    }
  }

  al_stop_timer (timer);
  anim_cycle = 0;
  anim_draw_callback = NULL;
  anim_compute_callback = NULL;
  anim_cleanup_callback = NULL;
}

void
pause_animation (bool val)
{
  pause_anim = val;
  pause_audio_instances (val);
}

void
cutscene_mode (bool val)
{
  cutscene = val;
}

void
change_anim_freq (int f)
{
  f = f < 0 ? 0 : f;
  f = f > UNLIMITED_HZ ? UNLIMITED_HZ : f;
  anim_freq = f;
  al_set_timer_speed (timer, f > 0 ? 1.0 / f : 1.0 / UNLIMITED_HZ);
}



void
draw_frame (ALLEGRO_BITMAP *bitmap, struct frame *f)
{
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
  lua_Number w = IW (get_bitmap_width (xf->b));
  _tf (f, c);
  return
    new_coord (c, f->c.l, f->c.room,
               c->x + ((f->dir == LEFT) ? xf->dx : -xf->dx - w + 1),
               c->y + xf->dy);
}

struct frame *
xframe_frame (struct frame *f, struct frame_offset *xf, struct frame *nf)
{
  *nf = *f;
  nf->b = xf->b;
  xframe_coord (f, xf, &nf->c);
  return nf;
}

struct frame *
splash_frame (ALLEGRO_BITMAP *splash, struct frame *f, struct frame *nf)
{
  *nf = *f;
  nf->b = splash;
  splash_coord (splash, f, &nf->c);
  return nf;
}

void
select_frame (struct actor *a, struct frameset *fs, int i)
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

  if (! f->b) nf->b = fo->b;

  lua_Number ow = IW (get_bitmap_width (nf->b));
  lua_Number oh = IH (get_bitmap_height (nf->b));
  lua_Number w = IW (get_bitmap_width (fo->b));
  lua_Number h = IH (get_bitmap_height (fo->b));

  lua_Number dx = fo->dx;
  lua_Number dy = fo->dy;

  if (next_frame_inv) nf->c.x += (nf->dir == LEFT) ? ow - w - dx : dx;
  else nf->c.x += (nf->dir == LEFT) ? dx : ow - w - dx;
  nf->c.y += oh - h + dy;

  nf->b = fo->b;
  if (! cutscene) nframe (nf, &nf->c);
  return nf;
}

bool
frame_eq (struct frame *f0, struct frame *f1)
{
  return coord_eq (&f0->c, &f1->c)
    && f0->b == f1->b
    && f0->dir == f1->dir
    && f0->flip == f1->flip;
}

struct frame *
invert_frame_dir (struct frame *f0, struct frame *f1)
{
  *f1 = *f0;
  enum dir d = f1->dir;
  f1->dir = opposite_dir (d);
  if (d == LEFT || d == RIGHT)
    f1->flip ^= ALLEGRO_FLIP_HORIZONTAL;
  else if (d == ABOVE || d == BELOW)
    f1->flip ^= ALLEGRO_FLIP_VERTICAL;
  else assert (false);
  return f1;
}

bool
wait_anim (int cycles)
{
  static int i = 0;
  if (i == 0) i = cycles + 1;
  return --i;
}
