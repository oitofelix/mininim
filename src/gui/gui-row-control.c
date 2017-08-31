/*
  gui-row-control.c -- GUI row control module;

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

static int button_action_cb (Ihandle *ih);
static int _update_cb (Ihandle *ih);


Ihandle *
gui_create_row_control (char *norm_group)
{
  Ihandle *ih = IupSetCallbacks
    (gui_create_directional_control
     ("Row", "ROW_ICON", button_action_cb, norm_group),
     "_UPDATE_CB", _update_cb,
     NULL);

  Ihandle *c_button = (void *) IupGetAttribute (ih, "_C_BUTTON");
  IupSetAttribute (c_button, "TIP", "Center view");

  return ih;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  return IUP_DEFAULT;
}

int
button_action_cb (Ihandle *ih)
{
  Ihandle *c_button = (void *) IupGetAttribute (ih, "_C_BUTTON");

  Ihandle *l_button = (void *) IupGetAttribute (ih, "_L_BUTTON");
  Ihandle *r_button = (void *) IupGetAttribute (ih, "_R_BUTTON");
  Ihandle *a_button = (void *) IupGetAttribute (ih, "_A_BUTTON");
  Ihandle *b_button = (void *) IupGetAttribute (ih, "_B_BUTTON");

  Ihandle *al_button = (void *) IupGetAttribute (ih, "_AL_BUTTON");
  Ihandle *ar_button = (void *) IupGetAttribute (ih, "_AR_BUTTON");
  Ihandle *bl_button = (void *) IupGetAttribute (ih, "_BL_BUTTON");
  Ihandle *br_button = (void *) IupGetAttribute (ih, "_BR_BUTTON");

  Ihandle *radio = (void *) IupGetAttribute (ih, "_RADIO");
  Ihandle *selected = (void *) IupGetAttribute (radio, "VALUE_HANDLE");
  Ihandle *v_toggle = (void *) IupGetAttribute (ih, "_V_TOGGLE");
  Ihandle *h_toggle = (void *) IupGetAttribute (ih, "_H_TOGGLE");

  if (ih == c_button) mr_simple_center_room (mr.room);
  else if (ih == l_button) mr_row_trans (LEFT);
  else if (ih == r_button) mr_row_trans (RIGHT);
  else if (ih == a_button) mr_row_trans (ABOVE);
  else if (ih == b_button) mr_row_trans (BELOW);
  else if (ih == al_button) {
    if (selected == v_toggle) {
      mr_row_trans (ABOVE);
      mr_row_trans (LEFT);
    } else if (selected == h_toggle) {
      mr_row_trans (LEFT);
      mr_row_trans (ABOVE);
    }
  } else if (ih == ar_button) {
    if (selected == v_toggle) {
      mr_row_trans (ABOVE);
      mr_row_trans (RIGHT);
    } else if (selected == h_toggle) {
      mr_row_trans (RIGHT);
      mr_row_trans (ABOVE);
    }
  } else if (ih == bl_button) {
    if (selected == v_toggle) {
      mr_row_trans (BELOW);
      mr_row_trans (LEFT);
    } else if (selected == h_toggle) {
      mr_row_trans (LEFT);
      mr_row_trans (BELOW);
    }
  } else if (ih == br_button) {
    if (selected == v_toggle) {
      mr_row_trans (BELOW);
      mr_row_trans (RIGHT);
    } else if (selected == h_toggle) {
      mr_row_trans (RIGHT);
      mr_row_trans (BELOW);
    }
  }

  return IUP_DEFAULT;
}
