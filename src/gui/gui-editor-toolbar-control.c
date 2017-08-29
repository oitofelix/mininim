/*
  gui-editor-toolbar-control.c -- editor GUI toolbar control module;

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

static int _update_cb (Ihandle *ih);
static int button_action_cb (Ihandle *ih);


Ihandle *
gui_create_editor_toolbar_control (char *norm_group)
{
  Ihandle *ih;

  Ihandle *lock_button, *pause_button, *full_screen_button, *home_button;

  ih = IupSetCallbacks
    (IupSetAttributes
     (IupGridBox
      (
       lock_button = IupSetCallbacks
       (IupSetAttributes
        (IupButton (NULL, NULL),
         "IMAGE = UNLOCK_ICON,"
         "FLAT = YES,"
         "CANFOCUS = NO,"),
        "ACTION", button_action_cb,
        NULL),

       pause_button = IupSetCallbacks
       (IupSetAttributes
        (IupButton (NULL, NULL),
         "IMAGE = PAUSE_ICON,"
         "FLAT = YES,"
         "CANFOCUS = NO,"),
        "ACTION", button_action_cb,
        NULL),

       full_screen_button = IupSetCallbacks
       (IupSetAttributes
        (IupButton (NULL, NULL),
         "IMAGE = FULL_SCREEN_ICON,"
         "FLAT = YES,"
         "CANFOCUS = NO,"),
        "ACTION", button_action_cb,
        NULL),

       home_button = IupSetCallbacks
       (IupSetAttributes
        (IupButton (NULL, NULL),
         "IMAGE = HOME_ICON,"
         "FLAT = YES,"
         "CANFOCUS = NO,"),
        "ACTION", button_action_cb,
        NULL),

       NULL),
      "ORIENTATION = HORIZONTAL,"
      "NUMDIV = 4,"
      "SIZECOL = -1,"
      "SIZELIN = -1,"
      "NORMALIZESIZE = BOTH,"),
     "_UPDATE_CB", _update_cb,
     NULL);

  IupSetAttribute (ih, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_LOCK_BUTTON", (void *) lock_button);
  IupSetAttribute (ih, "_PAUSE_BUTTON", (void *) pause_button);
  IupSetAttribute (ih, "_FULL_SCREEN_BUTTON", (void *) full_screen_button);
  IupSetAttribute (ih, "_HOME_BUTTON", (void *) home_button);

  return ih;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  Ihandle *lock_button = (void *) IupGetAttribute (ih, "_LOCK_BUTTON");
  Ihandle *pause_button = (void *) IupGetAttribute (ih, "_PAUSE_BUTTON");
  Ihandle *full_screen_button =
    (void *) IupGetAttribute (ih, "_FULL_SCREEN_BUTTON");
  Ihandle *home_button = (void *) IupGetAttribute (ih, "_HOME_BUTTON");

  /* Lock */
  gui_set_stock_image (lock_button, selection_locked
                       ? "LOCK_ICON" : "UNLOCK_ICON");

  gui_control_attribute (lock_button, "TIP", selection_locked
                         ? "Unlock place selection"
                         : "Lock place selection");

  /* Pause */
  gui_set_stock_image (pause_button, is_game_paused ()
                       ? "PLAY_ICON" : "PAUSE_ICON");

  gui_control_attribute (pause_button, "TIP", is_game_paused ()
                         ? "Continue game" : "Pause game");

  /* Full screen */
  gui_set_stock_image (full_screen_button, is_fullscreen ()
                       ? "WINDOWS_ICON" : "FULL_SCREEN_ICON");

  gui_control_attribute (full_screen_button, "TIP", is_fullscreen ()
                         ? "Windowed (F)" : "Fullscreen (F)");

  /* Home */
  gui_set_stock_image (home_button, "HOME_ICON");

  gui_control_attribute (home_button, "TIP", "Home (Home)");

  return IUP_DEFAULT;
}

int
button_action_cb (Ihandle *ih)
{
  Ihandle *lock_button = (void *) IupGetAttribute (ih, "_LOCK_BUTTON");
  Ihandle *pause_button = (void *) IupGetAttribute (ih, "_PAUSE_BUTTON");
  Ihandle *full_screen_button =
    (void *) IupGetAttribute (ih, "_FULL_SCREEN_BUTTON");
  Ihandle *home_button = (void *) IupGetAttribute (ih, "_HOME_BUTTON");

  if (ih == lock_button) {
    if (! selection_locked)
      new_pos (&selection_pos, &global_level, mr.room, 1,  4);
    selection_locked = ! selection_locked;
  } else if (ih == pause_button) ui_toggle_pause_game ();
  else if (ih == full_screen_button) ui_full_screen ();
  else if (ih == home_button) ui_home ();

  return IUP_DEFAULT;
}
