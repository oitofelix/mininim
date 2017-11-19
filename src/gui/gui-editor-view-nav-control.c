/*
  gui-editor-view-nav-control.c -- GUI editor view and navigation
  control module;

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


Ihandle *
gui_create_editor_view_nav_control (char *norm_group)
{
  Ihandle *vbox;

  Ihandle *zoom_ctrl = gui_create_zoom_control (NULL);

  Ihandle *place_ctrl = gui_create_place_control (&selection_pos, NULL);

  Ihandle *mr_origin_ctrl = gui_create_mr_origin_control (NULL);

  Ihandle *row_ctrl = gui_create_row_control (NULL);

  Ihandle *page_ctrl = gui_create_page_control (NULL);

  Ihandle *ih = IupSetCallbacks
    (IupSetAttributes
     (IupFrame
      (IupSetAttributes
       (vbox = IupVbox
        (IupFill (),
         IupSetAttributes
         (IupHbox
          (zoom_ctrl,
           IupFill (),
           place_ctrl,
           IupFill (),
           mr_origin_ctrl,
           IupFill (),
           row_ctrl,
           IupFill (),
           page_ctrl,
           NULL),
          "ALIGNMENT = ACENTER"),
         IupFill (),
         NULL),
        "ALIGNMENT = ACENTER")),
      "SUNKEN = YES,"),
     "SHOW_CB", (Icallback) show_cb,
     "_UPDATE_CB", _update_cb,
     NULL);

  IupSetAttribute (vbox, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_ZOOM_CTRL", (void *) zoom_ctrl);
  IupSetAttribute (ih, "_PLACE_CTRL", (void *) place_ctrl);
  IupSetAttribute (ih, "_MR_ORIGIN_CTRL", (void *) mr_origin_ctrl);
  IupSetAttribute (ih, "_ROW_CTRL", (void *) row_ctrl);
  IupSetAttribute (ih, "_PAGE_CTRL", (void *) page_ctrl);

  return ih;
}

int
show_cb (Ihandle *ih, int state)
{
  Ihandle *zoom_ctrl = (void *) IupGetAttribute (ih, "_ZOOM_CTRL");
  gui_run_callback_IFni ("SHOW_CB", zoom_ctrl, state);

  Ihandle *place_ctrl = (void *) IupGetAttribute (ih, "_PLACE_CTRL");
  gui_run_callback_IFni ("SHOW_CB", place_ctrl, state);

  Ihandle *mr_origin_ctrl = (void *) IupGetAttribute (ih, "_MR_ORIGIN_CTRL");
  gui_run_callback_IFni ("SHOW_CB", mr_origin_ctrl, state);

  Ihandle *row_ctrl =  (void *) IupGetAttribute (ih, "_ROW_CTRL");
  gui_run_callback_IFni ("SHOW_CB", row_ctrl, state);

  Ihandle *page_ctrl = (void *) IupGetAttribute (ih, "_PAGE_CTRL");
  gui_run_callback_IFni ("SHOW_CB", page_ctrl, state);

  return IUP_DEFAULT;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  Ihandle *zoom_ctrl = (void *) IupGetAttribute (ih, "_ZOOM_CTRL");
  gui_run_callback_IFn ("UPDATE_CB", zoom_ctrl);

  Ihandle *place_ctrl = (void *) IupGetAttribute (ih, "_PLACE_CTRL");
  gui_run_callback_IFn ("UPDATE_CB", place_ctrl);

  Ihandle *mr_origin_ctrl = (void *) IupGetAttribute (ih, "_MR_ORIGIN_CTRL");
  gui_run_callback_IFn ("UPDATE_CB", mr_origin_ctrl);

  Ihandle *row_ctrl =  (void *) IupGetAttribute (ih, "_ROW_CTRL");
  gui_run_callback_IFn ("UPDATE_CB", row_ctrl);

  Ihandle *page_ctrl = (void *) IupGetAttribute (ih, "_PAGE_CTRL");
  gui_run_callback_IFn ("UPDATE_CB", page_ctrl);

  return IUP_DEFAULT;
}
