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

char *tile_ext_control_name (struct pos *p);
static int show_cb (Ihandle *ih, int state);
static int _update_cb (Ihandle *ih);
static int k_any (Ihandle *ih, int c);
static int move_selection_hotkeys_cb (Ihandle *ih, int c);
static int tabchange_cb (Ihandle *ih, Ihandle *new_tab, Ihandle *old_tab);


Ihandle *
gui_create_editor_dialog (void)
{
  Ihandle *ih = IupSetCallbacks
    (IupSetAttributes
     (IupDialog
      (IupVbox
       (
        IupFill (),

        IupSetAttributes
        (IupHbox
         (
          IupSetAttributes
          (gui_create_undo_control (&undo, NULL),
           "NAME = UNDO_CONTROL"),

          IupFill (),

          IupSetAttributes
          (gui_create_coordinates_control (&global_level, NULL),
           "NAME = COORDINATES_CONTROL"),

          NULL),
         "ALIGNMENT = ACENTER"),

        IupFill (),

        IupSetAttributes
        (IupFrame
         (IupSetAttributes
          (IupVbox
           (
            IupFill (),

            IupSetAttributes
            (IupHbox
             (
              IupSetAttributes
              (gui_create_zoom_control (NULL),
               "NAME = ZOOM_CONTROL"),

              IupFill (),

              IupSetAttributes
              (gui_create_place_control (&selection_pos, NULL),
               "NAME = PLACE_CONTROL"),

              IupFill (),

              IupSetAttributes
              (gui_create_room_control (NULL),
               "NAME = ROOM_CONTROL"),

              IupFill (),

              IupSetAttributes
              (gui_create_row_control (NULL),
               "NAME = ROW_CONTROL"),

              IupFill (),

              IupSetAttributes
              (gui_create_page_control (NULL),
               "NAME = PAGE_CONTROL"),

              NULL),
             "ALIGNMENT = ACENTER"),

            IupFill (),

            NULL),
           "ALIGNMENT = ACENTER")),
         "SUNKEN = YES,"),

        IupFill (),

        IupSetCallbacks
        (IupSetAttributes
         (IupTabs
          (

           /* Tiles */
           IupSetAttributes
           (IupGridBox
            (
             IupSetAttributes
             (gui_create_tile_part_control
              (&selection_pos, TILE_FG, "TILE_PART_NORM"),
              "NAME = TILE_FG_CONTROL"),

             IupSetAttributes
             (gui_create_tile_part_control
              (&selection_pos, TILE_BG, "TILE_PART_NORM"),
              "NAME = TILE_BG_CONTROL"),

             IupSetAttributes
             (IupZbox
              (IupSetAttributes
               (IupFrame
                (IupSetAttributes
                 (IupVbox
                  (IupFill (),
                   IupLabel ("NO EXTENSION"),
                   IupFill (),
                   NULL),
                  "ALIGNMENT = ACENTER,"
                  "NORMALIZERGROUP = TILE_PART_NORM")),
                "TITLE = Extension,"
                "NAME = TILE_EXT_NONE_CONTROL,"
                "ACTIVE = NO"),
               IupSetAttributes
               (gui_create_tile_part_control
                (&selection_pos, TILE_EXT_ITEM, "TILE_PART_NORM"),
                "NAME = TILE_EXT_ITEM_CONTROL"),
               IupSetAttributes
               (gui_create_tile_part_control
                (&selection_pos, TILE_EXT_DESIGN, "TILE_PART_NORM"),
                "NAME = TILE_EXT_DESIGN_CONTROL"),
               IupSetAttributes
               (gui_create_tile_part_control
                (&selection_pos, TILE_EXT_STEP_SPIKES_FLOOR, "TILE_PART_NORM"),
                "NAME = TILE_EXT_STEP_SPIKES_FLOOR_CONTROL"),
               IupSetAttributes
               (gui_create_tile_part_control
                (&selection_pos, TILE_EXT_STEP_CHOMPER, "TILE_PART_NORM"),
                "NAME = TILE_EXT_STEP_CHOMPER_CONTROL"),
               IupSetAttributes
               (gui_create_tile_part_control
                (&selection_pos, TILE_EXT_STEP_DOOR, "TILE_PART_NORM"),
                "NAME = TILE_EXT_STEP_DOOR_CONTROL"),
               IupSetAttributes
               (gui_create_tile_part_control
                (&selection_pos, TILE_EXT_STEP_LEVEL_DOOR, "TILE_PART_NORM"),
                "NAME = TILE_EXT_STEP_LEVEL_DOOR_CONTROL"),
               IupSetAttributes
               (gui_create_tile_part_control
                (&selection_pos, TILE_EXT_EVENT, "TILE_PART_NORM"),
                "NAME = TILE_EXT_EVENT_CONTROL"),
               IupSetAttributes
               (gui_create_tile_part_control
                (&selection_pos, TILE_EXT_FALL, "TILE_PART_NORM"),
                "NAME = TILE_EXT_FALL_CONTROL"),
               NULL),
              "NAME = TILE_EXT,"
              "ALIGNMENT = ACENTER"),

             IupSetAttributes
             (gui_create_tile_part_control
              (&selection_pos, TILE_FAKE, "TILE_PART_NORM"),
              "NAME = TILE_FAKE_CONTROL"),

             IupSetAttributes
             (gui_create_tile_clipboard_control
              (&selection_pos, &global_sel_ring,
               "TILE_PART_NORM"),
              "NAME = TILE_CLIPBOARD_CONTROL"),

             IupSetAttributes
             (gui_create_tile_transform_control
              (&selection_pos, &global_sel_ring,
               "TILE_PART_NORM"),
              "NAME = TILE_TRANSFORM_CONTROL"),

             IupSetAttributes
             (gui_create_tile_mirror_control
              (&selection_pos, &global_sel_ring,
               "TILE_PART_NORM"),
              "NAME = TILE_MIRROR_CONTROL"),

             IupSetAttributes
             (gui_create_tile_move_control
              (&selection_pos, "TILE_PART_NORM"),
              "NAME = TILE_MOVE_CONTROL"),

             NULL),
            "ORIENTATION = HORIZONTAL,"
            "NUMDIV = 4,"
            "SIZECOL = -1,"
            "SIZELIN = -1,"
            "NGAPCOL = 2,"
            "NGAPLIN = 2,"
            "NORMALIZERGROUP = EDITOR_TAB_NORM"),

           /* Events */
           IupSetAttributes
           (gui_create_editor_events_control
            ("EDITOR_TAB_NORM", &global_level),
            "NAME = EVENTS_CONTROL"),

           /* Room */
           IupSetAttributes
           (gui_create_editor_room_control
            ("EDITOR_TAB_NORM", &global_level),
            "NAME = ROOM_CONTROL"),

           IupVbox (NULL),
           IupVbox (NULL),
           IupVbox (NULL),
           NULL),
          "NAME = TABS_CONTROL,"
          "TABTITLE0 = Tile,"
          "TABTITLE1 = Event,"
          "TABTITLE2 = Room,"
          "TABTITLE3 = Kid,"
          "TABTITLE4 = Guard,"
          "TABTITLE5 = Level,"),
         "TABCHANGE_CB", (Icallback) tabchange_cb,
         NULL),

        IupSetAttributes
        (IupHbox
         (IupSetAttributes
          (IupFrame
           (IupHbox
            (IupSetAttributes
             (IupToggle ("&Selection", NULL),
              "NAME = SELECTION_HOTKEYS_TOGGLE,"
              "TIP = \"Arrow keys move the locked place selection\","),
             IupSetAttributes
             (IupToggle ("&Game", NULL),
              "NAME = GAME_HOTKEYS_TOGGLE,"
              "TIP = \"Usual game key bindings work anywhere\""),
             NULL)),
           "TITLE = Hotkeys"),

          IupSetAttributes
          (gui_create_position_control
           (&selection_pos, NULL),
           "NAME = POSITION_CONTROL"),

          IupSetAttributes
          (gui_create_editor_toolbar_control
           (NULL),
           "NAME = TOOLBAR_CONTROL"),

          IupFill (),

          IupSetCallbacks
          (IupSetAttributes
           (IupButton ("&Close", NULL),
            "PADDING = 16,"),
           "ACTION", exit_editor,
           NULL),
          NULL),
         "ALIGNMENT = ACENTER,"),
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

  gui_run_callback_IFn ("_UPDATE_CB", ih);

  Ihandle *norm = IupGetHandle ("TILE_PART_NORM");
  IupSetAttribute (norm, "NORMALIZE", "BOTH");

  Ihandle *tab_norm = IupGetHandle ("EDITOR_TAB_NORM");
  IupSetAttribute (tab_norm, "NORMALIZE", "BOTH");

  dialog_fit_natural_size (ih);

  return ih;
}

char *
tile_ext_control_name (struct pos *p)
{
  if (is_item_fg (p)) return "TILE_EXT_ITEM_CONTROL";
  else if (is_fall_fg (p)) return "TILE_EXT_FALL_CONTROL";
  else if (is_event_fg (p)) return "TILE_EXT_EVENT_CONTROL";
  else if (fg (p) == SPIKES_FLOOR)
    return "TILE_EXT_STEP_SPIKES_FLOOR_CONTROL";
  else if (fg (p) == DOOR)
    return "TILE_EXT_STEP_DOOR_CONTROL";
  else if (fg (p) == LEVEL_DOOR)
    return "TILE_EXT_STEP_LEVEL_DOOR_CONTROL";
  else if (fg (p) == CHOMPER)
    return "TILE_EXT_STEP_CHOMPER_CONTROL";
  else if (is_design_fg (p)) return "TILE_EXT_DESIGN_CONTROL";
  else return "TILE_EXT_NONE_CONTROL";
}

int
show_cb (Ihandle *ih, int state)
{
  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "UNDO_CONTROL"), state);

  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "COORDINATES_CONTROL"), state);

  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "ZOOM_CONTROL"), state);

  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "PLACE_CONTROL"), state);

  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "ROOM_CONTROL"), state);

  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "ROW_CONTROL"), state);

  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "PAGE_CONTROL"), state);

  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "TILE_FG_CONTROL"), state);
  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "TILE_BG_CONTROL"), state);

  Ihandle *c = IupGetDialogChild (ih, "TILE_EXT");
  char *name = tile_ext_control_name (&selection_pos);
  Ihandle *nsc = IupGetDialogChild (ih, name);
  IupSetAttribute (c, "VALUE_HANDLE", (void *) nsc);
  gui_run_callback_IFni ("SHOW_CB", nsc, state);

  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "TILE_FAKE_CONTROL"), state);

  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "TILE_CLIPBOARD_CONTROL"), state);

  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "TILE_TRANSFORM_CONTROL"), state);

  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "TILE_MIRROR_CONTROL"), state);

  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "TILE_MOVE_CONTROL"), state);

  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "EVENTS_CONTROL"), state);

  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "ROOM_CONTROL"), state);

  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "POSITION_CONTROL"), state);

  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "TOOLBAR_CONTROL"), state);

  return IUP_DEFAULT;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  gui_run_callback_IFn
    ("_UPDATE_CB", IupGetDialogChild (ih, "UNDO_CONTROL"));

  gui_run_callback_IFn
    ("_UPDATE_CB", IupGetDialogChild (ih, "COORDINATES_CONTROL"));

  gui_run_callback_IFn
    ("_UPDATE_CB", IupGetDialogChild (ih, "ZOOM_CONTROL"));

  gui_run_callback_IFn
    ("_UPDATE_CB", IupGetDialogChild (ih, "PLACE_CONTROL"));

  gui_run_callback_IFn
    ("_UPDATE_CB", IupGetDialogChild (ih, "ROOM_CONTROL"));

  gui_run_callback_IFn
    ("_UPDATE_CB", IupGetDialogChild (ih, "ROW_CONTROL"));

  gui_run_callback_IFn
    ("_UPDATE_CB", IupGetDialogChild (ih, "PAGE_CONTROL"));

  gui_run_callback_IFn
    ("_UPDATE_CB", IupGetDialogChild (ih, "TILE_FG_CONTROL"));

  gui_run_callback_IFn
    ("_UPDATE_CB", IupGetDialogChild (ih, "TILE_BG_CONTROL"));

  Ihandle *c = IupGetDialogChild (ih, "TILE_EXT");
  Ihandle *csc = (void *) IupGetAttribute (c, "VALUE_HANDLE");
  if (is_valid_pos (&selection_pos)) {
    char *name = tile_ext_control_name (&selection_pos);
    Ihandle *nsc = IupGetDialogChild (ih, name);
    if (csc != nsc) {
      Ihandle *tile_part_dialog =
        (void *) IupGetAttribute (csc, "_TILE_PART_DIALOG");
      if (tile_part_dialog) {
        int dialog_visible = IupGetInt (tile_part_dialog, "VISIBLE");
        IupSetInt (c, "_DIALOG_VISIBLE", dialog_visible);
        IupHide (tile_part_dialog);
      }
      IupSetAttribute (c, "VALUE_HANDLE", (void *) nsc);
      tile_part_dialog = (void *)
        IupGetAttribute (nsc, "_TILE_PART_DIALOG");
      if (tile_part_dialog) {
        int dialog_visible = IupGetInt (c, "_DIALOG_VISIBLE");
        if (dialog_visible) IupShow (tile_part_dialog);
      }
    }
    gui_run_callback_IFn ("_UPDATE_CB", nsc);
  } else gui_run_callback_IFn ("_UPDATE_CB", csc);

  gui_run_callback_IFn
    ("_UPDATE_CB", IupGetDialogChild (ih, "TILE_FAKE_CONTROL"));

  gui_run_callback_IFn
    ("_UPDATE_CB", IupGetDialogChild (ih, "TILE_CLIPBOARD_CONTROL"));

  gui_run_callback_IFn
    ("_UPDATE_CB", IupGetDialogChild (ih, "TILE_TRANSFORM_CONTROL"));

  gui_run_callback_IFn
    ("_UPDATE_CB", IupGetDialogChild (ih, "TILE_MIRROR_CONTROL"));

  gui_run_callback_IFn
    ("_UPDATE_CB", IupGetDialogChild (ih, "TILE_MOVE_CONTROL"));

  gui_run_callback_IFn
    ("_UPDATE_CB", IupGetDialogChild (ih, "EVENTS_CONTROL"));

  gui_run_callback_IFn
    ("_UPDATE_CB", IupGetDialogChild (ih, "ROOM_CONTROL"));

  gui_run_callback_IFn
    ("_UPDATE_CB", IupGetDialogChild (ih, "POSITION_CONTROL"));

  gui_run_callback_IFn
    ("_UPDATE_CB", IupGetDialogChild (ih, "TOOLBAR_CONTROL"));

  return IUP_DEFAULT;
}

int
k_any (Ihandle *ih, int c)
{
  Ihandle *selection_hotkeys_toggle =
    IupGetDialogChild (ih, "SELECTION_HOTKEYS_TOGGLE");

  bool selection_hotkeys = IupGetInt (selection_hotkeys_toggle, "VALUE");

  Ihandle *game_hotkeys_toggle =
    IupGetDialogChild (ih, "GAME_HOTKEYS_TOGGLE");

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

int
tabchange_cb (Ihandle *ih, Ihandle *new_tab, Ihandle *old_tab)
{
  return IUP_DEFAULT;
}
