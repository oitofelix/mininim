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

struct anim *anima;
size_t anima_nmemb;

void
play_anim (void (*draw_callback) (void),
           void (*compute_callback) (void),
           void (*cleanup_callback) (void))
{
  anim_cycle = 0;
  quit_anim = NO_QUIT;

  al_acknowledge_resize (display);

  ALLEGRO_EVENT event;
  anim_freq_real = anim_freq > 0 ? anim_freq : UNLIMITED_HZ;
  timer = al_create_timer
    (anim_freq > 0 ? 1.0 / anim_freq : 1.0 / UNLIMITED_HZ);
  al_register_event_source (event_queue, al_get_timer_event_source (timer));
  al_flush_event_queue (event_queue);
  al_start_timer (timer);

  double prev_time = al_get_time ();

  reset_haptic ();

  key.keycode = 0;
  joystick_button = -1;

  while (! quit_anim) {
    L_gc (main_L);
    unlock_thread ();
    al_wait_for_event (event_queue, &event);
    lock_thread ();

    switch (event.type) {
    case ALLEGRO_EVENT_TIMER:
      if (event.timer.source == timer) {
        /* ensures Lua stack is empty */
        if (DEBUG && lua_gettop (main_L)) {
          L_dump_stack (main_L);
          assert (false);
        }

        /* update main menu */
        main_menu ();

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
        struct anim *k;
        if (! title_demo
            && replay_mode == PLAY_REPLAY
            && (anim_cycle >= replay->packed_gamepad_state_nmemb
                + REPLAY_STUCK_THRESHOLD
                || ((k = get_anim_by_id (current_kid_id))
                    && k->current_lives <= 0
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

        /* update mouse pos */
        if (! cutscene) get_mouse_pos (&mouse_pos);

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
          draw_callback ();
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
      } else if (event.timer.source == video_timer) {
        if (rendering == BOTH_RENDERING || rendering == VIDEO_RENDERING)
          show ();
        else stop_video_effect ();
        /* drop_all_events_from_source */
        /*   (event_queue, get_timer_event_source (video_timer)); */
        /* al_set_timer_count (video_timer, 0); */
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
      case 3:
        if (edit != EDIT_NONE)
          ui_place_kid (get_anim_by_id (current_kid_id), &mouse_pos);
        break;
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
    case ALLEGRO_EVENT_MOUSE_AXES:
      if (pause_anim) break;
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



    case ALLEGRO_EVENT_MENU_CLICK:
      menu_mid (&event);
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

      anim_key_bindings ();
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
    new_coord (&a.f.c, p->l, p->room, -1, -1);
    a.controllable = false;
    invalid_pos (&a.enemy_pos);
  }

  a.id = i;
  a.f.parent_id = i;
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
get_next_controllable (struct anim *k)
{
  do {
    k = &anima[(k - anima + 1) % anima_nmemb];
  } while (k->type != KID || ! k->controllable);
  return k;
}

void
select_controllable_by_id (int id)
{
  struct anim *old_k = get_anim_by_id (current_kid_id);
  struct anim *k = get_anim_by_id (id);
  current_kid_id = id;
  if (id) {
    k->death_timer = 0;
    last_fellow_shadow_id = id;
  }
  if (! is_frame_visible (&k->f))
    mr_restore_origin (&k->mr_origin);
  if (k->f.c.room != old_k->f.c.room)
    mr_focus_room (k->f.c.room);
  k->selection_cycle = anim_cycle;
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
get_reciprocal_enemy (struct anim *k)
{
  if (! k) return NULL;
  else if (k->enemy_id == -1) return NULL;
  else if (k->enemy_id != -1 && k->enemy_refraction >= 0)
    return NULL;
  else if (k->enemy_id != -1) {
    struct anim *ke = get_anim_by_id (k->enemy_id);
    if (! ke) return NULL;
    else if (ke->enemy_id == -1) return NULL;
    else if (ke->enemy_id != -1 && ke->enemy_refraction >= 0)
      return NULL;
    else if (ke->enemy_id == k->id) return ke;
    else return NULL;
  } else return NULL;
}

struct anim *
get_anim_dead_at_pos (struct pos *p)
{
  int i;
  for (i = 0; i < anima_nmemb; i++)
    if (anima[i].current_lives <= 0
        && peq (&anima[i].p, p))
      return &anima[i];
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
  struct anim *a;

  /* coord_wa = true; */

  qsort (anima, anima_nmemb, sizeof (*a), compare_anims);

  size_t i;
  for (i = 0; i < anima_nmemb; i++) {
    a = &anima[i];
    if (a->invisible) continue;
    draw_anim_frame (bitmap, a, vm);
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
    struct coord o = {tr0.l, tr0.room, 0, ORIGINAL_HEIGHT};

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
  return
    new_coord (c, f->c.l, f->c.room,
               f->c.x + (fw / 2) - (w / 2),
               f->c.y + (fh / 2) - (h / 2));
}

bool
uncollide_static_neutral (struct anim *a)
{
  return uncollide_static (&a->f, &a->fo, _bf, 0, 0, _bb, 0, 0,
                           &a->fo);
}

bool
uncollide_static_kid_normal (struct anim *a)
{
  return uncollide_static (&a->f, &a->fo, _bf,
                           COLLISION_FRONT_LEFT_NORMAL,
                           COLLISION_FRONT_RIGHT_NORMAL,
                           _bb,
                           COLLISION_BACK_LEFT_NORMAL,
                           COLLISION_BACK_RIGHT_NORMAL,
                           &a->fo);
}

bool
uncollide_static_fight (struct anim *a)
{
  return uncollide_static (&a->f, &a->fo, _bf,
                           COLLISION_FRONT_LEFT_FIGHT,
                           COLLISION_FRONT_RIGHT_FIGHT,
                           _bb,
                           COLLISION_BACK_LEFT_FIGHT,
                           COLLISION_BACK_RIGHT_FIGHT,
                           &a->fo);
}

bool
is_colliding_front_fight (struct anim *a)
{
  return uncollide (&a->f, &a->fo, _bf,
                    COLLISION_FRONT_LEFT_FIGHT,
                    COLLISION_FRONT_RIGHT_FIGHT,
                    NULL, &a->ci);
}

bool
is_colliding_back_fight (struct anim *a)
{
  return uncollide (&a->f, &a->fo, _bb,
                    COLLISION_BACK_LEFT_FIGHT,
                    COLLISION_BACK_RIGHT_FIGHT,
                    NULL, &a->ci);
}

bool
uncollide_front_fight (struct anim *a)
{
  bool uf = uncollide (&a->f, &a->fo, _bf,
                       COLLISION_FRONT_LEFT_FIGHT,
                       COLLISION_FRONT_RIGHT_FIGHT,
                       &a->fo, NULL);

  bool ub = uncollide (&a->f, &a->fo, _bb,
                       COLLISION_BACK_LEFT_FIGHT,
                       COLLISION_BACK_RIGHT_FIGHT,
                       &a->fo, NULL);
  return uf || ub;
}

bool
uncollide_back_fight (struct anim *a)
{
  bool ub = uncollide (&a->f, &a->fo, _bb,
                       COLLISION_BACK_LEFT_FIGHT,
                       COLLISION_BACK_RIGHT_FIGHT,
                       &a->fo, NULL);

  bool uf = uncollide (&a->f, &a->fo, _bf,
                       COLLISION_FRONT_LEFT_FIGHT,
                       COLLISION_FRONT_RIGHT_FIGHT,
                       &a->fo, NULL);
  return ub || uf;
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
splash_frame (struct frame *f, struct frame *nf)
{
  *nf = *f;
  nf->b = v_kid_splash;
  splash_coord (f, &nf->c);
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
