/*
  gui-editor-dialog.c -- GUI editor dialog module;

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

static int show_cb (Ihandle *ih, int state);
static int _update_cb (Ihandle *ih);
static int k_any (Ihandle *ih, int c);
static int move_selection_hotkeys_cb (Ihandle *ih, int c);


Ihandle *
gui_create_editor_dialog (void)
{
  Ihandle *undo_ctrl = gui_create_undo_control (&undo, NULL);

  Ihandle *coordinates_ctrl = gui_create_coordinates_control
    (&global_level, NULL);

  Ihandle *undo_coord_hbox = IupSetAttributes
    (IupHbox
     (undo_ctrl,
      IupFill (),
      coordinates_ctrl,
      NULL),
     "ALIGNMENT = ACENTER");

  Ihandle *view_nav_ctrl = gui_create_editor_view_nav_control (NULL);

  Ihandle *editor_tabs = gui_create_editor_tabs (NULL, &global_level);

  Ihandle *selection_hotkeys_toggle = IupSetAttributes
    (IupToggle ("&Selection", NULL),
     "TIP = \"Arrow keys move the locked place selection\",");

  Ihandle *game_hotkeys_toggle = IupSetAttributes
    (IupToggle ("&Game", NULL),
     "TIP = \"Usual game key bindings work anywhere\"");

  Ihandle *hotkeys_ctrl = IupSetAttributes
    (IupFrame
     (IupHbox
      (selection_hotkeys_toggle,
       game_hotkeys_toggle,
       NULL)),
     "TITLE = Hotkeys");

  Ihandle *position_ctrl = gui_create_position_control
    (&selection_pos, NULL);

  Ihandle *toolbar_ctrl = gui_create_editor_toolbar_control
    (NULL);

  Ihandle *close_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton ("&Close", NULL),
      "PADDING = 16,"),
     "ACTION", exit_editor,
     NULL);

  Ihandle *bottom_ctrl = IupSetAttributes
    (IupHbox
     (hotkeys_ctrl,
      position_ctrl,
      toolbar_ctrl,
      IupFill (),
      close_button,
      NULL),
     "ALIGNMENT = ACENTER,");

  Ihandle *ih = IupSetCallbacks
    (IupSetAttributes
     (IupDialog
      (IupVbox
       (IupFill (),
        undo_coord_hbox,
        IupFill (),
        view_nav_ctrl,
        IupFill (),
        editor_tabs,
        IupFill (),
        bottom_ctrl,
        IupFill (),
        NULL)),
      "TITLE = \"MININIM: Editor\","
      "ICON = LOGO_ICON,"
      "MARGIN = 2,"
      "GAP = 2,"),
     "K_ANY", (Icallback) k_any,
     "SHOW_CB", (Icallback) show_cb,
     "CLOSE_CB", exit_editor,
     "RESIZE_CB", gui_resize_cb,
     "_UPDATE_CB", _update_cb,
     NULL);

  IupSetAttribute (ih, "_UNDO_CTRL", (void *) undo_ctrl);
  IupSetAttribute (ih, "_COORDINATES_CTRL", (void *) coordinates_ctrl);
  IupSetAttribute (ih, "_UNDO_COORD_HBOX", (void *) undo_coord_hbox);

  IupSetAttribute (ih, "_VIEW_NAV_CTRL", (void *) view_nav_ctrl);

  IupSetAttribute (ih, "_EDITOR_TABS", (void *) editor_tabs);

  IupSetAttribute (ih, "_HOTKEYS_CTRL", (void *) hotkeys_ctrl);
  IupSetAttribute (ih, "_SELECTION_HOTKEYS_TOGGLE",
                   (void *) selection_hotkeys_toggle);
  IupSetAttribute (ih, "_GAME_HOTKEYS_TOGGLE",
                   (void *) game_hotkeys_toggle);

  IupSetAttribute (ih, "_POSITION_CTRL", (void *) position_ctrl);
  IupSetAttribute (ih, "_TOOLBAR_CTRL", (void *) toolbar_ctrl);
  IupSetAttribute (ih, "_BOTTOM_CTRL", (void *) bottom_ctrl);

  gui_run_callback_IFn ("_UPDATE_CB", ih);

  dialog_fit_natural_size (ih);

  return ih;
}

int
show_cb (Ihandle *ih, int state)
{
  Ihandle *undo_ctrl = (void *) IupGetAttribute (ih, "_UNDO_CTRL");
  gui_run_callback_IFni ("SHOW_CB", undo_ctrl, state);

  Ihandle *coordinates_ctrl =
    (void *) IupGetAttribute (ih, "_COORDINATES_CTRL");
  gui_run_callback_IFni ("SHOW_CB", coordinates_ctrl, state);

  Ihandle *view_nav_ctrl = (void *) IupGetAttribute (ih, "_VIEW_NAV_CTRL");
  gui_run_callback_IFni ("SHOW_CB", view_nav_ctrl, state);

  Ihandle *editor_tabs = (void *) IupGetAttribute (ih, "_EDITOR_TABS");
  gui_run_callback_IFni ("SHOW_CB", editor_tabs, state);

  Ihandle *position_ctrl = (void *) IupGetAttribute (ih, "_POSITION_CTRL");
  gui_run_callback_IFni ("SHOW_CB", position_ctrl, state);

  Ihandle *toolbar_ctrl = (void *) IupGetAttribute (ih, "_TOOLBAR_CTRL");
  gui_run_callback_IFni ("SHOW_CB", toolbar_ctrl, state);

  return IUP_DEFAULT;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  Ihandle *undo_ctrl = (void *) IupGetAttribute (ih, "_UNDO_CTRL");
  gui_run_callback_IFn ("_UPDATE_CB", undo_ctrl);

  Ihandle *coordinates_ctrl =
    (void *) IupGetAttribute (ih, "_COORDINATES_CTRL");
  gui_run_callback_IFn ("_UPDATE_CB", coordinates_ctrl);

  Ihandle *view_nav_ctrl = (void *) IupGetAttribute (ih, "_VIEW_NAV_CTRL");
  gui_run_callback_IFn ("_UPDATE_CB", view_nav_ctrl);

  Ihandle *editor_tabs = (void *) IupGetAttribute (ih, "_EDITOR_TABS");
  gui_run_callback_IFn ("_UPDATE_CB", editor_tabs);

  Ihandle *position_ctrl = (void *) IupGetAttribute (ih, "_POSITION_CTRL");
  gui_run_callback_IFn ("_UPDATE_CB", position_ctrl);

  Ihandle *toolbar_ctrl = (void *) IupGetAttribute (ih, "_TOOLBAR_CTRL");
  gui_run_callback_IFn ("_UPDATE_CB", toolbar_ctrl);

  return IUP_DEFAULT;
}

int
k_any (Ihandle *ih, int c)
{
  Ihandle *selection_hotkeys_toggle =
    (void *) IupGetAttribute (ih, "_SELECTION_HOTKEYS_TOGGLE");

  bool selection_hotkeys = IupGetInt (selection_hotkeys_toggle, "VALUE");

  Ihandle *game_hotkeys_toggle =
    (void *) IupGetAttribute (ih, "_GAME_HOTKEYS_TOGGLE");

  bool game_hotkeys = IupGetInt (game_hotkeys_toggle, "VALUE");

  if (selection_hotkeys && move_selection_hotkeys_cb (ih, c) == IUP_IGNORE)
    return IUP_IGNORE;
  else if (game_hotkeys && animation_hotkeys_cb (ih, c) == IUP_IGNORE)
    return IUP_IGNORE;
  else if (game_hotkeys && level_hotkeys_cb (ih, c) == IUP_IGNORE)
    return IUP_IGNORE;
  else return IUP_CONTINUE;
}

int
move_selection_hotkeys_cb (Ihandle *ih, int c)
{
  if (! selection_locked) return IUP_CONTINUE;

  enum dir d;

  switch (c) {
  case K_UP: d = ABOVE; break;
  case K_LEFT: d = LEFT; break;
  case K_RIGHT: d = RIGHT; break;
  case K_DOWN: d = BELOW; break;
  default: return IUP_CONTINUE;
  }

  ui_move_locked_place_selection (&global_mr, d);

  return IUP_IGNORE;
}
