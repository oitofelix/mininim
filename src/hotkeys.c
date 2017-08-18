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
       || (c && c == K_cL))
      && ! load_config_dialog_thread)
    ui_load_game ();

  /* CTRL+P: screenshot */
  else if ((was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_P)
            || (c && c == K_cP))
           && ! save_picture_dialog_thread)
    ui_screenshot ();

  /* ALT+F7: start/stop replay recording */
  else if (! command_line_replay
           && (((title_demo || replay_mode != PLAY_REPLAY)
                && (was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_F7)
                    || (c && c == K_mF7)))
               || ((replay_mode == RECORD_REPLAY
                    || recording_replay_countdown > 0)
                   && (was_key_pressed (0, ALLEGRO_KEY_F7)
                       || (c && c == K_F7)))))
    ui_record_replay ();

  /* F7: load replay/stop replaying */
  else if (! command_line_replay
           && ((replay_mode != RECORD_REPLAY
                && (was_key_pressed (0, ALLEGRO_KEY_F7)
                    || (c && c == K_F7)))
               || (replay_mode == PLAY_REPLAY
                   && (was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_F7)
                       || (c && c == K_mF7)))))
    ui_play_replay ();

  /* CTRL+R: restart game */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_R)
           || (c && c == K_cR))
    ui_restart_game ();

  /* CTRL+Q: quit game */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_Q)
           || (c && c == K_cQ))
    ui_quit_game ();

  /* (: decrease time frequency */
  else if (was_char_pressed ('(')
           || (c && c == K_parentleft))
    ui_change_anim_freq (anim_freq - 1);

  /* ): increase time frenquency */
  else if (was_char_pressed (')')
           || (c && c == K_parentright))
    ui_change_anim_freq (anim_freq + 1);

  /* F8: enable/disable level editor */
  else if (was_key_pressed (0, ALLEGRO_KEY_F8)
           || (c && c == K_F8))
    ui_editor ();

  /* CTRL+V: show engine name and version */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_V)
           || (c && c == K_cV))
    ui_version ();

  /* CTRL+S: enable/disable sound */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_S)
           || (c && c == K_cS)) {
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
           || (c && (c == K_F || c == K_f))
           || was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_ENTER)
           || (c && c == K_mCR))
    ui_full_screen ();

  /* SHIFT+I: flip screen */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_I)
           || (c && c == K_I)) {
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
           || (c && c == K_K)) {
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
           || (c && c == K_cK))
    ui_gamepad_mode (KEYBOARD);

  /* CTRL+J: joystick mode */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_J)
           || (c && c == K_cJ))
    ui_gamepad_mode (JOYSTICK);

  /* ALT+F11: change hue palette */
  else if (was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_F11)
           || (c && c == K_mF11)) {
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
           || (c && c == K_F11)) {
    if (force_em) {
      switch (em) {
      case DUNGEON: ui_em (PALACE); break;
      case PALACE: ui_em (ORIGINAL_EM); break;
      }
    } else ui_em (DUNGEON);
  }

  /* F12: change video mode */
  else if (was_key_pressed (0, ALLEGRO_KEY_F12)
           || (c && c == K_F12)) {
    char *next_vm = next_video_mode (video_mode);
    ui_vm (next_vm);
    al_free (next_vm);
  }

  /* D: change display mode */
  else if ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_D))
           || (c && (c == K_D || c == K_d)))
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
           || (c && c == K_cA))
    ui_jump_to_level_rel (+0);

  /* SHIFT+L: warp to next level */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_L)
           || (c && c == K_L))
    ui_jump_to_level_rel (+1);

  /* SHIFT+M: warp to previous level */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_M)
           || (c && c == K_M))
    ui_jump_to_level_rel (-1);

  /* CTRL+G: save game */
  if ((was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_G)
       || (c && c == K_cG))
      && ! save_game_dialog_thread)
    ui_save_game ();

 else return IUP_CONTINUE;

  return IUP_IGNORE;
}
