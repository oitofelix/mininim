/*
  gui-zoom-control.c -- GUI zoom control module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

static int button_action_cb (Ihandle *ih);
static int _update_cb (Ihandle *ih);


Ihandle *
gui_create_zoom_control (char *norm_group)
{
  Ihandle *ih, *vbox;

  Ihandle *zoom_button, *zoom_none_button, *zoom_stretch_button,
    *zoom_ratio_button;

  Ihandle *zoom_in_vh_button, *zoom_in_v_button, *zoom_in_h_button;

  Ihandle *zoom_out_vh_button, *zoom_out_v_button, *zoom_out_h_button;

  ih = IupSetCallbacks
    (IupSetAttributes
     (IupFrame
      (vbox = IupVbox
       (
        IupFill (),

        IupSetAttributes
        (IupGridBox
         (
          /* ACTION: reset zoom */
          zoom_button = IupSetCallbacks
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "IMAGE = ZOOM_ICON,"
            "FLAT = YES,"
            "TIP = \"Reset\","),
           "ACTION", button_action_cb,
           NULL),

          zoom_none_button = IupSetCallbacks
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "IMAGE = ZOOM_NONE_ICON,"
            "TIP = \"Fit none (M)\""),
           "ACTION", button_action_cb,
           NULL),

          zoom_stretch_button = IupSetCallbacks
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "IMAGE = ZOOM_STRETCH_ICON,"
            "TIP = \"Fit stretch (M)\","),
           "ACTION", button_action_cb,
           NULL),

          zoom_ratio_button = IupSetCallbacks
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "IMAGE = ZOOM_RATIO_ICON,"
            "TIP = \"Fit ratio (M)\","),
           "ACTION", button_action_cb,
           NULL),

          IupSetAttributes
          (IupLabel (NULL),
           "IMAGE = ZOOM_IN_ICON"),

          zoom_in_v_button = IupSetCallbacks
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "IMAGE = V_ICON,"
            "TIP = \"In vertical (Alt+[)\","),
           "ACTION", button_action_cb,
           NULL),

          zoom_in_vh_button = IupSetCallbacks
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "IMAGE = VH_ICON,"
            "TIP = \"In both ([)\","),
           "ACTION", button_action_cb,
           NULL),

          zoom_in_h_button = IupSetCallbacks
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "IMAGE = H_ICON,"
            "TIP = \"In horizontal (Ctrl+[)\","),
           "ACTION", button_action_cb,
           NULL),

          IupSetAttributes
          (IupLabel (NULL),
           "IMAGE = ZOOM_OUT_ICON"),

          zoom_out_v_button = IupSetCallbacks
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "IMAGE = V_ICON,"
            "TIP = \"Out vertical (Alt+])\","),
           "ACTION", button_action_cb,
           NULL),

          zoom_out_vh_button = IupSetCallbacks
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "IMAGE = VH_ICON,"
            "TIP = \"Out both (])\","),
           "ACTION", button_action_cb,
           NULL),

          zoom_out_h_button = IupSetCallbacks
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "IMAGE = H_ICON,"
            "TIP = \"Out horizontal (Ctrl+])\","),
           "ACTION", button_action_cb,
           NULL),

          NULL),
         "ORIENTATION = HORIZONTAL,"
         "NUMDIV = 4,"
         "SIZECOL = -1,"
         "SIZELIN = -1,"
         /* "NORMALIZESIZE = BOTH," */
         "ALIGNMENTLIN = ACENTER,"
         "ALIGNMENTCOL = ACENTER,"),

        IupFill (),

        NULL)),
      "TITLE = Zoom"),
     "_UPDATE_CB", _update_cb,
     NULL);

  IupSetAttribute (vbox, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_ZOOM_BUTTON", (void *) zoom_button);
  IupSetAttribute (ih, "_ZOOM_NONE_BUTTON", (void *) zoom_none_button);
  IupSetAttribute (ih, "_ZOOM_STRETCH_BUTTON", (void *) zoom_stretch_button);
  IupSetAttribute (ih, "_ZOOM_RATIO_BUTTON", (void *) zoom_ratio_button);

  IupSetAttribute (ih, "_ZOOM_IN_VH_BUTTON", (void *) zoom_in_vh_button);
  IupSetAttribute (ih, "_ZOOM_IN_V_BUTTON", (void *) zoom_in_v_button);
  IupSetAttribute (ih, "_ZOOM_IN_H_BUTTON", (void *) zoom_in_h_button);

  IupSetAttribute (ih, "_ZOOM_OUT_VH_BUTTON", (void *) zoom_out_vh_button);
  IupSetAttribute (ih, "_ZOOM_OUT_V_BUTTON", (void *) zoom_out_v_button);
  IupSetAttribute (ih, "_ZOOM_OUT_H_BUTTON", (void *) zoom_out_h_button);

  return ih;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  Ihandle *zoom_in_vh_button = (void *)
    IupGetAttribute (ih, "_ZOOM_IN_VH_BUTTON");
  Ihandle *zoom_in_v_button = (void *)
    IupGetAttribute (ih, "_ZOOM_IN_V_BUTTON");
  Ihandle *zoom_in_h_button = (void *)
    IupGetAttribute (ih, "_ZOOM_IN_H_BUTTON");

  gui_control_active (zoom_in_vh_button, global_mr.w > 1 && global_mr.h > 1);
  gui_control_active (zoom_in_v_button, global_mr.h > 1);
  gui_control_active (zoom_in_h_button, global_mr.w > 1);

  return IUP_DEFAULT;
}

int
button_action_cb (Ihandle *ih)
{
  Ihandle *zoom_button = (void *) IupGetAttribute (ih, "_ZOOM_BUTTON");
  Ihandle *zoom_none_button = (void *)
    IupGetAttribute (ih, "_ZOOM_NONE_BUTTON");
  Ihandle *zoom_stretch_button = (void *)
    IupGetAttribute (ih, "_ZOOM_STRETCH_BUTTON");
  Ihandle *zoom_ratio_button = (void *)
   IupGetAttribute (ih, "_ZOOM_RATIO_BUTTON");

  Ihandle *zoom_in_vh_button = (void *)
    IupGetAttribute (ih, "_ZOOM_IN_VH_BUTTON");
  Ihandle *zoom_in_v_button = (void *)
    IupGetAttribute (ih, "_ZOOM_IN_V_BUTTON");
  Ihandle *zoom_in_h_button = (void *)
    IupGetAttribute (ih, "_ZOOM_IN_H_BUTTON");

  Ihandle *zoom_out_vh_button = (void *)
    IupGetAttribute (ih, "_ZOOM_OUT_VH_BUTTON");
  Ihandle *zoom_out_v_button = (void *)
    IupGetAttribute (ih, "_ZOOM_OUT_V_BUTTON");
  Ihandle *zoom_out_h_button = (void *)
    IupGetAttribute (ih, "_ZOOM_OUT_H_BUTTON");

  if (ih == zoom_button) {
    ui_zoom_fit (&global_mr, MR_FIT_NONE);
    ui_mr_set_dim (&global_mr, 1, 1, false);
  } else if (ih == zoom_none_button)
    ui_zoom_fit (&global_mr, MR_FIT_NONE);
  else if (ih == zoom_stretch_button)
    ui_zoom_fit (&global_mr, MR_FIT_STRETCH);
  else if (ih == zoom_ratio_button)
    ui_zoom_fit (&global_mr, MR_FIT_RATIO);
  else if (ih == zoom_in_vh_button)
    ui_mr_set_dim (&global_mr, global_mr.w - 1, global_mr.h - 1, false);
  else if (ih == zoom_in_v_button)
    ui_mr_set_dim (&global_mr, global_mr.w + 0, global_mr.h - 1, false);
  else if (ih == zoom_in_h_button)
    ui_mr_set_dim (&global_mr, global_mr.w - 1, global_mr.h + 0, false);
  else if (ih == zoom_out_vh_button)
    ui_mr_set_dim (&global_mr, global_mr.w + 1, global_mr.h + 1, false);
  else if (ih == zoom_out_v_button)
    ui_mr_set_dim (&global_mr, global_mr.w + 0, global_mr.h + 1, false);
  else if (ih == zoom_out_h_button)
    ui_mr_set_dim (&global_mr, global_mr.w + 1, global_mr.h + 0, false);

  return IUP_DEFAULT;
}
