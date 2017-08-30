/*
  hotkeys.c -- hotkeys module;

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



/*********************************************************************
 * animation_hotkeys_cb
 *********************************************************************/

int
animation_hotkeys_cb (Ihandle *ih, int c)
{
  /* CTRL+L: load game */
  if ((was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_L)
       || c == K_cL)
      && ! load_config_dialog_thread)
    ui_load_game ();

  /* CTRL+P: screenshot */
  else if ((was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_P)
            || c == K_cP)
           && ! save_picture_dialog_thread)
    ui_screenshot ();

  /* ALT+F7: start/stop replay recording */
  else if (! command_line_replay
           && (((title_demo || replay_mode != PLAY_REPLAY)
                && (was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_F7)
                    || c == K_mF7))
               || ((replay_mode == RECORD_REPLAY
                    || recording_replay_countdown > 0)
                   && (was_key_pressed (0, ALLEGRO_KEY_F7)
                       || c == K_F7))))
    ui_record_replay ();

  /* F7: load replay/stop replaying */
  else if (! command_line_replay
           && ((replay_mode != RECORD_REPLAY
                && (was_key_pressed (0, ALLEGRO_KEY_F7)
                    || c == K_F7))
               || (replay_mode == PLAY_REPLAY
                   && (was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_F7)
                       || c == K_mF7))))
    ui_play_replay ();

  /* CTRL+R: restart game */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_R)
           || c == K_cR)
    ui_restart_game ();

  /* CTRL+Q: quit game */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_Q)
           || c == K_cQ)
    ui_quit_game ();

  /* (: decrease time frequency */
  else if (was_char_pressed ('(')
           || c == K_parentleft)
    ui_change_anim_freq (anim_freq - 1);

  /* ): increase time frenquency */
  else if (was_char_pressed (')')
           || c == K_parentright)
    ui_change_anim_freq (anim_freq + 1);

  /* F8: enable/disable level editor */
  else if (was_key_pressed (0, ALLEGRO_KEY_F8)
           || c == K_F8)
    ui_editor ();

  /* CTRL+V: show engine name and version */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_V)
           || c == K_cV)
    ui_version ();

  /* CTRL+S: enable/disable sound */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_S)
           || c == K_cS) {
    if (audio_volume == VOLUME_RANGE_MIN)
      ui_audio_volume (VOLUME_LOW);
    else if (audio_volume < VOLUME_RANGE_LOW)
      ui_audio_volume (VOLUME_MEDIUM);
    else if (audio_volume < VOLUME_RANGE_MEDIUM)
      ui_audio_volume (VOLUME_HIGH);
    else if (audio_volume <= VOLUME_RANGE_MAX)
      ui_audio_volume (VOLUME_OFF);
  }

  /* F: enable/disable fullscreen mode */
  else if ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_F))
           || c == K_f
           || was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_ENTER)
           || c == K_mCR)
    ui_full_screen ();

  /* SHIFT+I: flip screen */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_I)
           || c == K_I) {
    switch (screen_flags) {
    case 0: ui_flip_screen (ALLEGRO_FLIP_VERTICAL, true, false); break;
    case ALLEGRO_FLIP_VERTICAL:
      ui_flip_screen (ALLEGRO_FLIP_HORIZONTAL, true, false); break;
    case ALLEGRO_FLIP_HORIZONTAL:
      ui_flip_screen (ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL,
                      true, false);
      break;
    case ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL:
    default: ui_flip_screen (0, true, false); break;
    }
  }

  /* SHIFT+K: flip gamepad */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_K)
           || c == K_K) {
    if (! flip_gamepad_vertical && ! flip_gamepad_horizontal)
      ui_flip_gamepad (true, false, false);
    else if (flip_gamepad_vertical && ! flip_gamepad_horizontal)
      ui_flip_gamepad (false, true, false);
    else if (! flip_gamepad_vertical && flip_gamepad_horizontal)
      ui_flip_gamepad (true, true, false);
    else if (flip_gamepad_vertical && flip_gamepad_horizontal)
      ui_flip_gamepad (false, false, false);
  }

  /* CTRL+K: keyboard mode (disables joystick) */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_K)
           || c == K_cK)
    ui_gamepad_mode (KEYBOARD);

  /* CTRL+J: joystick mode */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_J)
           || c == K_cJ)
    ui_gamepad_mode (JOYSTICK);

  /* ALT+F11: change hue palette */
  else if (was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_F11)
           || c == K_mF11) {
    if (force_hue)
      switch (hue) {
      case HUE_NONE: ui_hue_mode (HUE_GREEN); break;
      case HUE_GREEN: ui_hue_mode (HUE_GRAY); break;
      case HUE_GRAY: ui_hue_mode (HUE_YELLOW); break;
      case HUE_YELLOW: ui_hue_mode (HUE_BLUE); break;
      case HUE_BLUE: ui_hue_mode (HUE_ORIGINAL); break;
      }
    else ui_hue_mode (HUE_NONE);
  }

  /* F11: change environment mode */
  else if (was_key_pressed (0, ALLEGRO_KEY_F11)
           || c == K_F11) {
    if (force_em) {
      switch (em) {
      case DUNGEON: ui_em (PALACE); break;
      case PALACE: ui_em (ORIGINAL_EM); break;
      }
    } else ui_em (DUNGEON);
  }

  /* F12: change video mode */
  else if (was_key_pressed (0, ALLEGRO_KEY_F12)
           || c == K_F12) {
    char *next_vm = next_video_mode (video_mode);
    ui_vm (next_vm);
    al_free (next_vm);
  }

  /* D: change display mode */
  else if ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_D))
           || c == K_d)
    ui_next_display_mode ();

  else return IUP_CONTINUE;

  return IUP_IGNORE;
}




/*********************************************************************
 * level_hotkeys_cb
 *********************************************************************/

int
level_hotkeys_cb (Ihandle *ih, int c)
{
  if (title_demo) return IUP_CONTINUE;

  /* CTRL+A: restart level */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_A)
           || c == K_cA)
    ui_jump_to_level_rel (+0);

  /* SHIFT+L: warp to next level */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_L)
           || c == K_L)
    ui_jump_to_level_rel (+1);

  /* SHIFT+M: warp to previous level */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_M)
           || c == K_M)
    ui_jump_to_level_rel (-1);

  /* CTRL+G: save game */
  else if ((was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_G)
            || c == K_cG)
           && ! save_game_dialog_thread)
    ui_save_game ();

  /* SHIFT+F11: change guard mode */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_F11)
           || c == K_sF11)
      switch (gm) {
      case ORIGINAL_GM: ui_gm (GUARD_GM); break;
      case GUARD_GM: ui_gm (FAT_GM); break;
      case FAT_GM: ui_gm (VIZIER_GM); break;
      case VIZIER_GM: ui_gm (SKELETON_GM); break;
      case SKELETON_GM: ui_gm (SHADOW_GM); break;
      case SHADOW_GM: ui_gm (ORIGINAL_GM); break;
      }

  /* CTRL+Z: undo */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_Z)
           || c == K_cZ)
    ui_undo_pass (&undo, -1, NULL);

  /* CTRL+Y: redo */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_Y)
           || c == K_cY)
    ui_undo_pass (&undo, +1, NULL);

  /* M: change multi-room fit mode */
  else if ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_M))
           || c == K_m)
    switch (mr.fit_mode) {
    case MR_FIT_NONE: ui_zoom_fit (MR_FIT_STRETCH); break;
    case MR_FIT_STRETCH: ui_zoom_fit (MR_FIT_RATIO); break;
    case MR_FIT_RATIO: ui_zoom_fit (MR_FIT_NONE); break;
    default: assert (false); break;
    }

  /* [: decrease multi-room resolution */
  else if ((was_key_pressed (0, ALLEGRO_KEY_OPENBRACE)
            || c == K_bracketleft)
           && ! cutscene)
    ui_set_multi_room (-1, -1, true);

  /* ]: increase multi-room resolution */
  else if ((was_key_pressed (0, ALLEGRO_KEY_CLOSEBRACE)
            || c == K_bracketright)
           && ! cutscene)
    ui_set_multi_room (+1, +1, true);

  /* CTRL+[: decrease multi-room width resolution */
  else if ((was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_OPENBRACE)
            || c == K_cBracketleft)
           && ! cutscene)
    ui_set_multi_room (-1, +0, true);

  /* CTRL+]: increase multi-room width resolution */
  else if ((was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_CLOSEBRACE)
            || c == K_cBracketright)
           && ! cutscene)
    ui_set_multi_room (+1, +0, true);

  /* ALT+[: decrease multi-room height resolution */
  else if ((was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_OPENBRACE)
            || c == K_mBracketleft)
           && ! cutscene)
    ui_set_multi_room (+0, -1, true);

  /* ALT+]: increase multi-room height resolution */
  else if ((was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_CLOSEBRACE)
            || c == K_mBracketright)
           && ! cutscene)
    ui_set_multi_room (+0, +1, true);

  /* H: select room at left (J if flipped horizontally) */
  else if ((! flip_gamepad_horizontal
            && ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_H))
                || c == K_h))
           || (flip_gamepad_horizontal
               && ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_J))
                   || c == K_j)))
    mr_room_trans (LEFT);

  /* J: select room at right (H if flipped horizontally) */
  else if ((! flip_gamepad_horizontal
            && ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_J))
                || c == K_j))
           || (flip_gamepad_horizontal
               && ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_H))
                   || c == K_h)))
    mr_room_trans (RIGHT);

  /* U: select room above (N if flipped vertically) */
  else if ((! flip_gamepad_vertical
            && ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_U))
                || c == K_u))
           || (flip_gamepad_vertical
               && ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_N))
                   || c == K_n)))
    mr_room_trans (ABOVE);

  /* N: select room below (U if flipped vertically) */
  else if ((! flip_gamepad_vertical
            && ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_N))
                || c == K_n))
           || (flip_gamepad_vertical
               && ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_U))
                   || c == K_u)))
    mr_room_trans (BELOW);

  /* SHIFT+H: multi-room view to left (J if flipped horizontally) */
  else if ((! flip_gamepad_horizontal
            && ((! active_menu
                 && was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_H))
                || c == K_H))
           || (flip_gamepad_horizontal
               && ((! active_menu
                    && was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_J))
                   || c == K_J)))
    mr_row_trans (LEFT);

  /* SHIFT+J: multi-room view to right (H if flipped horizontally) */
  else if ((! flip_gamepad_horizontal
            && ((! active_menu
                 && was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_J))
                || c == K_J))
           || (flip_gamepad_horizontal
               && ((! active_menu
                    && was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_H))
                   || c == K_H)))
    mr_row_trans (RIGHT);

  /* SHIFT+U: multi-room view above (N if flipped vertically) */
  else if ((! flip_gamepad_vertical
            && ((! active_menu
                 && was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_U))
                || c == K_U))
           || (flip_gamepad_vertical
               && ((! active_menu
                    && was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_N))
                   || c == K_N)))
    mr_row_trans (ABOVE);

  /* SHIFT+N: multi-room view below (U if flipped vertically) */
  else if ((! flip_gamepad_vertical
            && ((! active_menu
                 && was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_N))
                || c == K_N))
           || (flip_gamepad_vertical
               && ((! active_menu
                    && was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_U))
                   || c == K_U)))
    mr_row_trans (BELOW);

  /* ALT+H: multi-room page view to left (J if flipped horizontally) */
  else if ((! flip_gamepad_horizontal
            && ((! active_menu
                 && was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_H))
                || c == K_mH))
           || (flip_gamepad_horizontal
               && ((! active_menu
                    && was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_J))
                   || c == K_mJ)))
    mr_page_trans (LEFT);

  /* ALT+J: multi-room page view to right (H if flipped horizontally) */
  else if ((! flip_gamepad_horizontal
            && ((! active_menu
                 && was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_J))
                || c == K_mJ))
           || (flip_gamepad_horizontal
               && ((! active_menu
                    && was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_H))
                   || c == K_mH)))
    mr_page_trans (RIGHT);

  /* ALT+U: multi-room page view above (N if flipped vertically) */
  else if ((! flip_gamepad_vertical
            && ((! active_menu
                 && was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_U))
                || c == K_mU))
           || (flip_gamepad_vertical
               && ((! active_menu
                    && was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_N))
                   || c == K_mN)))
    mr_page_trans (ABOVE);

  /* ALT+N: multi-room page view below (U if flipped vertically) */
  else if ((! flip_gamepad_vertical
            && ((! active_menu
                 && was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_N))
                || c == K_mN))
           || (flip_gamepad_vertical
               && ((! active_menu
                    && was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_U))
                   || c == K_mU)))
    mr_page_trans (BELOW);

  /* HOME: focus multi-room view on kid */
  else if (was_key_pressed (0, ALLEGRO_KEY_HOME)
           || c == K_HOME) {
    struct actor *k = get_actor_by_id (current_kid_id);
    mr_focus_room (k->f.c.room);
  }

  /* SHIFT+HOME: center multi-room view */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_HOME)
           || c == K_sHOME)
    mr_center_room (mr.room);

  /* C: show direct coordinates */
  else if ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_C))
           || c == K_c)
    ui_show_coordinates ();

  /* SHIFT+C: show indirect coordinates */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_C)
           || c == K_C)
    ui_show_indirect_coordinates ();

  /* SHIFT+B: enable/disable room drawing */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_B)
           || c == K_B)
    ui_room_drawing (no_room_drawing);

  /* K: kill enemy */
  else if ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_K))
           || c == K_k)
    ui_kill_enemy ();

  /* R: resurrect kid */
  else if ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_R))
           || c == K_r)
    ui_resurrect ();

  /* I: enable/disable immortal mode */
  else if ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_I))
           || c == K_i)
    ui_immortal (! immortal_mode);

  /* SHIFT+W: float kid */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_W)
           || c == K_W)
    ui_float ();

  /* SHIFT+S: incremet kid current hp */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_S)
           || c == K_S)
    ui_fill_hp ();

  /* SHIFT+T: incremet kid total hp */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_T)
           || c == K_T)
    ui_add_hp ();

  /* SPACE: display remaining time */
  else if ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_SPACE))
           || c == K_SP
           || was_joystick_button_pressed (joystick_time_button))
    ui_time ();

  /* TAB: display skill */
  else if (was_key_pressed (0, ALLEGRO_KEY_TAB)
           || c == K_TAB)
    ui_skills ();

  /* +: increase time limit and display remaining time */
  else if ((! active_menu
            && (was_key_pressed (0, ALLEGRO_KEY_EQUALS)
                || was_key_pressed (ALLEGRO_KEYMOD_SHIFT,
                                    ALLEGRO_KEY_EQUALS)))
           || c == K_plus
           || c == K_equal)
    ui_change_time (+1);

  /* -: decrease time limit and display remaining time */
  else if ((! active_menu
           && (was_key_pressed (0, ALLEGRO_KEY_MINUS)
               || was_char_pressed ('_')))
           || c == K_minus)
    ui_change_time (-1);

  /* CTRL+=: increase counter attack skill */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_EQUALS)
           || c == K_cEqual)
    ui_change_kca (+1);

  /* CTRL+-: decrease counter attack skill */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_MINUS)
           || c == K_cMinus)
    ui_change_kca (-1);

  /* ALT+=: increase counter defense skill */
  else if (was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_EQUALS)
           || c == K_mEqual)
    ui_change_kcd (+1);

  /* ALT+-: decrease counter defense skill */
  else if (was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_MINUS)
           || c == K_mMinus)
    ui_change_kcd (-1);

  /* ESC: pause game */
  else if (was_key_pressed (0, ALLEGRO_KEY_ESCAPE)
      || was_joystick_button_pressed (joystick_pause_button)) {
    if (is_game_paused ()) {
      step_cycle = 0;
      game_paused = false;
    } else pause_game (true);
  } else if (is_game_paused ()
             && (! active_menu || ! was_bmenu_key_pressed ())
             && was_any_key_pressed ())
    pause_game (false);

  else return IUP_CONTINUE;

  return IUP_IGNORE;
}
