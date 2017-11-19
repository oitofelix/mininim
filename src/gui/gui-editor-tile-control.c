/*
  gui-editor-tile-control.c -- GUI editor tile control module;

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
static char *tile_ext_control_attribute (struct pos *p);


Ihandle *
gui_create_editor_tile_control (char *norm_group, struct level *level)
{
  Ihandle *tile_fg_ctrl = gui_create_tile_part_control
    (&selection_pos, TILE_FG, "TILE_PART_NORM");

  Ihandle *tile_bg_ctrl = gui_create_tile_part_control
    (&selection_pos, TILE_BG, "TILE_PART_NORM");

  Ihandle *tile_ext_none_ctrl = IupSetAttributes
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
     "ACTIVE = NO");

  Ihandle *tile_ext_item_ctrl = gui_create_tile_part_control
    (&selection_pos, TILE_EXT_ITEM, "TILE_PART_NORM");

  Ihandle *tile_ext_design_ctrl = gui_create_tile_part_control
    (&selection_pos, TILE_EXT_DESIGN, "TILE_PART_NORM");

  Ihandle *tile_ext_step_spikes_floor_ctrl = gui_create_tile_part_control
    (&selection_pos, TILE_EXT_STEP_SPIKES_FLOOR, "TILE_PART_NORM");

  Ihandle *tile_ext_step_chomper_ctrl = gui_create_tile_part_control
    (&selection_pos, TILE_EXT_STEP_CHOMPER, "TILE_PART_NORM");

  Ihandle *tile_ext_step_door_ctrl = gui_create_tile_part_control
    (&selection_pos, TILE_EXT_STEP_DOOR, "TILE_PART_NORM");

  Ihandle *tile_ext_step_level_door_ctrl = gui_create_tile_part_control
    (&selection_pos, TILE_EXT_STEP_LEVEL_DOOR, "TILE_PART_NORM");

  Ihandle *tile_ext_event_ctrl = gui_create_tile_part_control
    (&selection_pos, TILE_EXT_EVENT, "TILE_PART_NORM");

  Ihandle *tile_ext_fall_ctrl = gui_create_tile_part_control
    (&selection_pos, TILE_EXT_FALL, "TILE_PART_NORM");

  Ihandle *tile_ext_zbox = IupSetAttributes
    (IupZbox
     (tile_ext_none_ctrl,
      tile_ext_item_ctrl,
      tile_ext_design_ctrl,
      tile_ext_step_spikes_floor_ctrl,
      tile_ext_step_chomper_ctrl,
      tile_ext_step_door_ctrl,
      tile_ext_step_level_door_ctrl,
      tile_ext_event_ctrl,
      tile_ext_fall_ctrl,
      NULL),
     "ALIGNMENT = ACENTER");

  Ihandle *tile_fake_ctrl = gui_create_tile_part_control
    (&selection_pos, TILE_FAKE, "TILE_PART_NORM");

  Ihandle *tile_clipboard_ctrl = gui_create_tile_clipboard_control
    (&selection_pos, &global_sel_ring, "TILE_PART_NORM");

  Ihandle *tile_transform_ctrl = gui_create_tile_transform_control
    (&selection_pos, &global_sel_ring, "TILE_PART_NORM");

  Ihandle *tile_mirror_ctrl = gui_create_tile_mirror_control
    (&selection_pos, &global_sel_ring, "TILE_PART_NORM");

  Ihandle *tile_move_ctrl = gui_create_tile_move_control
    (&selection_pos, "TILE_PART_NORM");

  Ihandle *ih = IupSetCallbacks
    (IupSetAttributes
     (IupGridBox
      (tile_fg_ctrl,
       tile_bg_ctrl,
       tile_ext_zbox,
       tile_fake_ctrl,
       tile_clipboard_ctrl,
       tile_transform_ctrl,
       tile_mirror_ctrl,
       tile_move_ctrl,
       NULL),
      "ORIENTATION = HORIZONTAL,"
      "NUMDIV = 4,"
      "SIZECOL = -1,"
      "SIZELIN = -1,"
      "NGAPCOL = 10,"
      "NGAPLIN = 2,"),
     "SHOW_CB", (Icallback) show_cb,
     "_UPDATE_CB", _update_cb,
     NULL);

  IupSetAttribute (ih, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_TILE_FG_CTRL", (void *) tile_fg_ctrl);
  IupSetAttribute (ih, "_TILE_BG_CTRL", (void *) tile_bg_ctrl);

  IupSetAttribute (ih, "_TILE_EXT_NONE_CTRL", (void *) tile_ext_none_ctrl);
  IupSetAttribute (ih, "_TILE_EXT_ITEM_CTRL", (void *) tile_ext_item_ctrl);
  IupSetAttribute (ih, "_TILE_EXT_DESIGN_CTRL", (void *) tile_ext_design_ctrl);
  IupSetAttribute (ih, "_TILE_EXT_STEP_SPIKES_FLOOR_CTRL",
                   (void *) tile_ext_step_spikes_floor_ctrl);
  IupSetAttribute (ih, "_TILE_EXT_STEP_CHOMPER_CTRL",
                   (void *) tile_ext_step_chomper_ctrl);
  IupSetAttribute (ih, "_TILE_EXT_STEP_DOOR_CTRL",
                   (void *) tile_ext_step_door_ctrl);
  IupSetAttribute (ih, "_TILE_EXT_STEP_LEVEL_DOOR_CTRL",
                   (void *) tile_ext_step_level_door_ctrl);
  IupSetAttribute (ih, "_TILE_EXT_EVENT_CTRL", (void *) tile_ext_event_ctrl);
  IupSetAttribute (ih, "_TILE_EXT_FALL_CTRL", (void *) tile_ext_fall_ctrl);
  IupSetAttribute (ih, "_TILE_EXT_ZBOX", (void *) tile_ext_zbox);

  IupSetAttribute (ih, "_TILE_FAKE_CTRL", (void *) tile_fake_ctrl);
  IupSetAttribute (ih, "_TILE_CLIPBOARD_CTRL", (void *) tile_clipboard_ctrl);
  IupSetAttribute (ih, "_TILE_TRANSFORM_CTRL", (void *) tile_transform_ctrl);
  IupSetAttribute (ih, "_TILE_MIRROR_CTRL", (void *) tile_mirror_ctrl);
  IupSetAttribute (ih, "_TILE_MOVE_CTRL", (void *) tile_move_ctrl);

  Ihandle *norm = IupGetHandle ("TILE_PART_NORM");
  IupSetAttribute (norm, "NORMALIZE", "BOTH");

  return ih;
}

int
show_cb (Ihandle *ih, int state)
{
  Ihandle *tile_fg_ctrl = (void *) IupGetAttribute (ih, "_TILE_FG_CTRL");
  gui_run_callback_IFni ("SHOW_CB", tile_fg_ctrl, state);

  Ihandle *tile_bg_ctrl = (void *) IupGetAttribute (ih, "_TILE_BG_CTRL");
  gui_run_callback_IFni ("SHOW_CB", tile_bg_ctrl, state);

  Ihandle *c = (void *) IupGetAttribute (ih, "_TILE_EXT_ZBOX");
  char *a = tile_ext_control_attribute (&selection_pos);
  Ihandle *nsc = (void *) IupGetAttribute (ih, a);
  IupSetAttribute (c, "VALUE_HANDLE", (void *) nsc);
  gui_run_callback_IFni ("SHOW_CB", nsc, state);

  Ihandle *tile_fake_ctrl = (void *) IupGetAttribute (ih, "_TILE_FAKE_CTRL");
  gui_run_callback_IFni ("SHOW_CB", tile_fake_ctrl, state);

  Ihandle *tile_clipboard_ctrl =
    (void *) IupGetAttribute (ih, "_TILE_CLIPBOARD_CTRL");
  gui_run_callback_IFni ("SHOW_CB", tile_clipboard_ctrl, state);

  Ihandle *tile_transform_ctrl =
    (void *) IupGetAttribute (ih, "_TILE_TRANSFORM_CTRL");
  gui_run_callback_IFni ("SHOW_CB", tile_transform_ctrl, state);

  Ihandle *tile_mirror_ctrl =
    (void *) IupGetAttribute (ih, "_TILE_MIRROR_CTRL");
  gui_run_callback_IFni ("SHOW_CB", tile_mirror_ctrl, state);

  Ihandle *tile_move_ctrl = (void *) IupGetAttribute (ih, "_TILE_MOVE_CTRL");
  gui_run_callback_IFni ("SHOW_CB", tile_move_ctrl, state);

  return IUP_DEFAULT;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  Ihandle *tile_fg_ctrl = (void *) IupGetAttribute (ih, "_TILE_FG_CTRL");
  gui_run_callback_IFn ("_UPDATE_CB", tile_fg_ctrl);

  Ihandle *tile_bg_ctrl = (void *) IupGetAttribute (ih, "_TILE_BG_CTRL");
  gui_run_callback_IFn ("_UPDATE_CB", tile_bg_ctrl);

  Ihandle *c = (void *) IupGetAttribute (ih, "_TILE_EXT_ZBOX");
  Ihandle *csc = (void *) IupGetAttribute (c, "VALUE_HANDLE");
  if (is_valid_pos (&selection_pos)) {
    char *a = tile_ext_control_attribute (&selection_pos);
    Ihandle *nsc = (void *) IupGetAttribute (ih, a);
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

  Ihandle *tile_fake_ctrl = (void *) IupGetAttribute (ih, "_TILE_FAKE_CTRL");
  gui_run_callback_IFn ("_UPDATE_CB", tile_fake_ctrl);

  Ihandle *tile_clipboard_ctrl =
    (void *) IupGetAttribute (ih, "_TILE_CLIPBOARD_CTRL");
  gui_run_callback_IFn ("_UPDATE_CB", tile_clipboard_ctrl);

  Ihandle *tile_transform_ctrl =
    (void *) IupGetAttribute (ih, "_TILE_TRANSFORM_CTRL");
  gui_run_callback_IFn ("_UPDATE_CB", tile_transform_ctrl);

  Ihandle *tile_mirror_ctrl =
    (void *) IupGetAttribute (ih, "_TILE_MIRROR_CTRL");
  gui_run_callback_IFn ("_UPDATE_CB", tile_mirror_ctrl);

  Ihandle *tile_move_ctrl =
    (void *) IupGetAttribute (ih, "_TILE_MOVE_CTRL");
  gui_run_callback_IFn ("_UPDATE_CB", tile_move_ctrl);

  return IUP_DEFAULT;
}

char *
tile_ext_control_attribute (struct pos *p)
{
  if (is_item_fg (p)) return "_TILE_EXT_ITEM_CTRL";
  else if (is_fall_fg (p)) return "_TILE_EXT_FALL_CTRL";
  else if (is_event_fg (p)) return "_TILE_EXT_EVENT_CTRL";
  else if (fg (p) == SPIKES_FLOOR)
    return "_TILE_EXT_STEP_SPIKES_FLOOR_CTRL";
  else if (fg (p) == DOOR)
    return "_TILE_EXT_STEP_DOOR_CTRL";
  else if (fg (p) == LEVEL_DOOR)
    return "_TILE_EXT_STEP_LEVEL_DOOR_CTRL";
  else if (fg (p) == CHOMPER)
    return "_TILE_EXT_STEP_CHOMPER_CTRL";
  else if (is_design_fg (p)) return "_TILE_EXT_DESIGN_CTRL";
  else return "_TILE_EXT_NONE_CTRL";
}
