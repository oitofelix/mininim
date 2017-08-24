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


Ihandle *
gui_create_editor_dialog (void)
{
  Ihandle *zoom_label,*zoom_none_button, *zoom_stretch_button,
    *zoom_ratio_button;

  Ihandle *zoom_in_label, *zoom_in_vh_button, *zoom_in_v_button,
    *zoom_in_h_button;

  Ihandle *zoom_out_label, *zoom_out_vh_button, *zoom_out_v_button,
    *zoom_out_h_button;

  Ihandle *nav_select_label, *nav_select_left_button,
    *nav_select_right_button, *nav_select_above_button,
    *nav_select_below_button;

  Ihandle *nav_row_label, *nav_row_left_button,
    *nav_row_right_button, *nav_row_above_button,
    *nav_row_below_button;

  Ihandle *nav_page_label, *nav_page_left_button,
    *nav_page_right_button, *nav_page_above_button,
    *nav_page_below_button;

  Ihandle *zbox;

  Ihandle *ih = IupSetCallbacks
    (IupSetAttributes
     (IupDialog
      (IupVbox
       (IupFill (),
        IupSetAttributes
        (IupHbox
         (IupSetAttributes
          (IupFrame
           (IupSetAttributes
            (IupGridBox
             (zoom_label = IupLabel (NULL),
              zoom_none_button = IupButton (NULL, NULL),
              zoom_stretch_button = IupButton (NULL, NULL),
              zoom_ratio_button = IupButton (NULL, NULL),
              zoom_in_label = IupLabel (NULL),
              zoom_in_v_button = IupButton (NULL, NULL),
              zoom_in_vh_button = IupButton (NULL, NULL),
              zoom_in_h_button = IupButton (NULL, NULL),
              zoom_out_label = IupLabel (NULL),
              zoom_out_v_button = IupButton (NULL, NULL),
              zoom_out_vh_button = IupButton (NULL, NULL),
              zoom_out_h_button = IupButton (NULL, NULL),
              NULL),
             "ORIENTATION = HORIZONTAL,"
             "NUMDIV = 4,"
             "SIZECOL = -1,"
             "SIZELIN = -1,"
             "NORMALIZESIZE = BOTH")),
           "TITLE = Zoom"),
          IupFill (),

          IupSetAttributes
          (IupFrame
           (IupSetAttributes
            (IupGridBox
             (IupFill (),
              nav_select_above_button = IupButton (NULL, NULL),
              IupFill (),
              nav_select_left_button = IupButton (NULL, NULL),
              nav_select_label =
              IupSetAttributes
              (IupLabel (NULL),
               "ALIGNMENT = ACENTER:ACENTER"),
              nav_select_right_button = IupButton (NULL, NULL),
              IupFill (),
              nav_select_below_button = IupButton (NULL, NULL),
              IupFill (),
              NULL),
             "ORIENTATION = HORIZONTAL,"
             "NUMDIV = 3,"
             "SIZECOL = -1,"
             "SIZELIN = -1,"
             "NORMALIZESIZE = BOTH,")),
           "TITLE = Room"),
          IupFill (),

          IupSetAttributes
          (IupFrame
           (IupSetAttributes
            (IupGridBox
             (IupFill (),
              nav_row_above_button = IupButton (NULL, NULL),
              IupFill (),
              nav_row_left_button = IupButton (NULL, NULL),
              nav_row_label =
              IupSetAttributes
              (IupLabel (NULL),
               "ALIGNMENT = ACENTER:ACENTER"),
              nav_row_right_button = IupButton (NULL, NULL),
              IupFill (),
              nav_row_below_button = IupButton (NULL, NULL),
              IupFill (),
              NULL),
             "ORIENTATION = HORIZONTAL,"
             "NUMDIV = 3,"
             "SIZECOL = -1,"
             "SIZELIN = -1,"
             "NORMALIZESIZE = BOTH,")),
           "TITLE = Row"),

          IupSetAttributes
          (IupFrame
           (IupSetAttributes
            (IupGridBox
             (IupFill (),
              nav_page_above_button = IupButton (NULL, NULL),
              IupFill (),
              nav_page_left_button = IupButton (NULL, NULL),
              nav_page_label =
              IupSetAttributes
              (IupLabel (NULL),
               "ALIGNMENT = ACENTER:ACENTER"),
              nav_page_right_button = IupButton (NULL, NULL),
              IupFill (),
              nav_page_below_button = IupButton (NULL, NULL),
              IupFill (),
              NULL),
             "ORIENTATION = HORIZONTAL,"
             "NUMDIV = 3,"
             "SIZECOL = -1,"
             "SIZELIN = -1,"
             "NORMALIZESIZE = BOTH,")),
           "TITLE = Page"),

          NULL),
         "ALIGNMENT = ACENTER,"
         "GAP = 0"),
        IupSetAttributes
        (IupTabs
         (IupHbox
          (IupSetAttributes
           (gui_create_tile_part_control (&selection_pos, TILE_FG),
            "NAME = TILE_FG_CONTROL"),
           IupSetAttributes
           (gui_create_tile_part_control (&selection_pos, TILE_BG),
            "NAME = TILE_BG_CONTROL"),

           IupSetAttributes
           (zbox = IupZbox
            (IupSetAttributes
             (IupFrame
              (IupSetAttributes
               (IupVbox
                (IupFill (),
                 IupLabel ("NO EXTENSION"),
                 IupFill (),
                 NULL),
                "ALIGNMENT = ACENTER,"
                "NORMALIZERGROUP = TILE_EXT_NORM")),
              "TITLE = Extension,"
              "NAME = TILE_EXT_NONE_CONTROL,"
              "ACTIVE = NO"),
             IupSetAttributes
             (gui_create_tile_part_control (&selection_pos, TILE_EXT_ITEM),
              "NAME = TILE_EXT_ITEM_CONTROL"),
             IupSetAttributes
             (gui_create_tile_part_control (&selection_pos, TILE_EXT_DESIGN),
              "NAME = TILE_EXT_DESIGN_CONTROL"),
             IupSetAttributes
             (gui_create_tile_part_control
              (&selection_pos, TILE_EXT_STEP_SPIKES_FLOOR),
              "NAME = TILE_EXT_STEP_SPIKES_FLOOR_CONTROL"),
             IupSetAttributes
             (gui_create_tile_part_control
              (&selection_pos, TILE_EXT_STEP_CHOMPER),
              "NAME = TILE_EXT_STEP_CHOMPER_CONTROL"),
             IupSetAttributes
             (gui_create_tile_part_control
              (&selection_pos, TILE_EXT_STEP_DOOR),
              "NAME = TILE_EXT_STEP_DOOR_CONTROL"),
             IupSetAttributes
             (gui_create_tile_part_control
              (&selection_pos, TILE_EXT_STEP_LEVEL_DOOR),
              "NAME = TILE_EXT_STEP_LEVEL_DOOR_CONTROL"),
             IupSetAttributes
             (gui_create_tile_part_control
              (&selection_pos, TILE_EXT_EVENT),
              "NAME = TILE_EXT_EVENT_CONTROL"),
             IupSetAttributes
             (gui_create_tile_part_control
              (&selection_pos, TILE_EXT_FALL),
              "NAME = TILE_EXT_FALL_CONTROL"),
             NULL),
            "NAME = TILE_EXT,"
            "ALIGNMENT = ACENTER"),

           NULL),
          IupVbox (NULL),
          IupVbox (NULL),
          IupVbox (NULL),
          IupVbox (NULL),
          IupVbox (NULL),
          NULL),
         "TABTITLE0 = Tile,"
         "TABTITLE1 = Event,"
         "TABTITLE2 = Room,"
         "TABTITLE3 = Kid,"
         "TABTITLE4 = Guard,"
         "TABTITLE5 = Level"),
        IupSetAttributes
        (IupHbox
         (IupSetAttributes
          (IupFrame
           (IupVbox
            (IupSetAttributes
             (IupToggle ("&Selection", NULL),
              "NAME = SELECTION_HOTKEYS_TOGGLE,"
              "TIP = \"Arrow keys move the locked tile selection\","),
             IupSetAttributes
             (IupToggle ("&Game", NULL),
              "NAME = GAME_HOTKEYS_TOGGLE,"
              "TIP = \"Usual game key bindings work anywhere\""),
             NULL)),
           "TITLE = Hotkeys"),
          IupFill (),
          IupSetCallbacks
          (IupButton ("&Close", NULL),
           "ACTION", exit_editor,
           NULL),
          NULL),
         "ALIGNMENT = ACENTER,"
         "PADDING = 16"),
        IupFill (),
        NULL)),
      "TITLE = \"MININIM: Editor\","
      "ICON = LOGO_ICON,"
      "MARGIN = 5,"
      "GAP = 5"),
     "K_ANY", (Icallback) k_any,
     "SHOW_CB", (Icallback) show_cb,
     "CLOSE_CB", exit_editor,
     "_UPDATE_CB", _update_cb,
     NULL);

  ALLEGRO_BITMAP *zoom_icon =
    load_memory_bitmap (ZOOM_ICON);
  gui_set_image (zoom_label, zoom_icon, NULL);
  al_destroy_bitmap (zoom_icon);

  ALLEGRO_BITMAP *zoom_none_icon =
    load_memory_bitmap (ZOOM_NONE_ICON);
  gui_set_image (zoom_none_button, zoom_none_icon, NULL);
  al_destroy_bitmap (zoom_none_icon);

  ALLEGRO_BITMAP *zoom_stretch_icon =
    load_memory_bitmap (ZOOM_STRETCH_ICON);
  gui_set_image (zoom_stretch_button, zoom_stretch_icon, NULL);
  al_destroy_bitmap (zoom_stretch_icon);

  ALLEGRO_BITMAP *zoom_ratio_icon =
    load_memory_bitmap (ZOOM_RATIO_ICON);
  gui_set_image (zoom_ratio_button, zoom_ratio_icon, NULL);
  al_destroy_bitmap (zoom_ratio_icon);

  ALLEGRO_BITMAP *zoom_in_icon =
    load_memory_bitmap (ZOOM_IN_ICON);
  gui_set_image (zoom_in_label, zoom_in_icon, NULL);
  al_destroy_bitmap (zoom_in_icon);

  ALLEGRO_BITMAP *zoom_out_icon =
    load_memory_bitmap (ZOOM_OUT_ICON);
  gui_set_image (zoom_out_label, zoom_out_icon, NULL);
  al_destroy_bitmap (zoom_out_icon);

  ALLEGRO_BITMAP *vh_icon = load_memory_bitmap (VERTICAL_HORIZONTAL_ICON);
  ALLEGRO_BITMAP *v_icon = load_memory_bitmap (VERTICAL_ICON);
  ALLEGRO_BITMAP *h_icon = load_memory_bitmap (HORIZONTAL_ICON);

  gui_set_image (zoom_in_vh_button, vh_icon, NULL);
  gui_set_image (zoom_in_v_button, v_icon, NULL);
  gui_set_image (zoom_in_h_button, h_icon, NULL);

  gui_set_image (zoom_out_vh_button, vh_icon, NULL);
  gui_set_image (zoom_out_v_button, v_icon, NULL);
  gui_set_image (zoom_out_h_button, h_icon, NULL);

  al_destroy_bitmap (vh_icon);
  al_destroy_bitmap (v_icon);
  al_destroy_bitmap (h_icon);

  ALLEGRO_BITMAP *nav_select_icon = load_memory_bitmap (NAV_SELECT_ICON);
  gui_set_image (nav_select_label, nav_select_icon, NULL);
  al_destroy_bitmap (nav_select_icon);

  ALLEGRO_BITMAP *nav_row_icon = load_memory_bitmap (NAV_ROW_ICON);
  gui_set_image (nav_row_label, nav_row_icon, NULL);
  al_destroy_bitmap (nav_row_icon);

  ALLEGRO_BITMAP *nav_page_icon = load_memory_bitmap (NAV_PAGE_ICON);
  gui_set_image (nav_page_label, nav_page_icon, NULL);
  al_destroy_bitmap (nav_page_icon);

  ALLEGRO_BITMAP *nav_left_icon = load_memory_bitmap (NAV_LEFT_ICON);
  ALLEGRO_BITMAP *nav_above_icon = load_memory_bitmap (NAV_ABOVE_ICON);
  ALLEGRO_BITMAP *nav_right_icon = load_memory_bitmap (NAV_RIGHT_ICON);
  ALLEGRO_BITMAP *nav_below_icon = load_memory_bitmap (NAV_BELOW_ICON);

  gui_set_image (nav_select_left_button, nav_left_icon, NULL);
  gui_set_image (nav_select_above_button, nav_above_icon, NULL);
  gui_set_image (nav_select_right_button, nav_right_icon, NULL);
  gui_set_image (nav_select_below_button, nav_below_icon, NULL);

  gui_set_image (nav_row_left_button, nav_left_icon, NULL);
  gui_set_image (nav_row_above_button, nav_above_icon, NULL);
  gui_set_image (nav_row_right_button, nav_right_icon, NULL);
  gui_set_image (nav_row_below_button, nav_below_icon, NULL);

  gui_set_image (nav_page_left_button, nav_left_icon, NULL);
  gui_set_image (nav_page_above_button, nav_above_icon, NULL);
  gui_set_image (nav_page_right_button, nav_right_icon, NULL);
  gui_set_image (nav_page_below_button, nav_below_icon, NULL);

  al_destroy_bitmap (nav_left_icon);
  al_destroy_bitmap (nav_above_icon);
  al_destroy_bitmap (nav_right_icon);
  al_destroy_bitmap (nav_below_icon);

  gui_run_callback_IFn ("_UPDATE_CB", ih);

  Ihandle *norm = IupGetHandle ("TILE_EXT_NORM");
  IupSetAttribute (norm, "NORMALIZE", "BOTH");

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
  if (state == IUP_SHOW) {
    char *ns = IupGetAttribute (ih, "NATURALSIZE");
    IupSetAttribute (ih, "MINSIZE", ns);
  }

  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "TILE_FG_CONTROL"), state);
  gui_run_callback_IFni
    ("SHOW_CB", IupGetDialogChild (ih, "TILE_BG_CONTROL"), state);

  Ihandle *c = IupGetDialogChild (ih, "TILE_EXT");
  char *name = tile_ext_control_name (&selection_pos);
  Ihandle *nsc = IupGetDialogChild (ih, name);
  IupSetAttribute (c, "VALUE_HANDLE", (void *) nsc);
  gui_run_callback_IFni ("SHOW_CB", nsc, state);

  return IUP_DEFAULT;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

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

  struct pos p;

  switch (c) {
  case K_UP:
    prel (&selection_pos, &p, -1, +0);
    break;
  case K_LEFT:
    prel (&selection_pos, &p, +0, -1);
    break;
  case K_RIGHT:
    prel (&selection_pos, &p, +0, +1);
    break;
  case K_DOWN:
    prel (&selection_pos, &p, +1, +0);
    break;
  default:
    return IUP_CONTINUE;
  }

  npos (&p, &p);

  if (p.room) {
    selection_pos = p;
    mr_focus_room (p.room);
  }

  return IUP_IGNORE;
}
