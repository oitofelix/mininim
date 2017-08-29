/*
  gui-room-control.c -- GUI room control module;

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
gui_create_room_control (char *norm_group)
{
  Ihandle *ih = IupSetCallbacks
    (gui_create_directional_control
     ("Room", "ROOM_ICON", button_action_cb, norm_group),
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

  struct link_survey ls;
  link_survey (&ls, &global_level, mr.room);

  gui_control_active (l_button, ls.l);
  gui_control_active (r_button, ls.r);
  gui_control_active (a_button, ls.a);
  gui_control_active (b_button, ls.b);

  if (selected == v_toggle) {
    gui_control_active (al_button, ls.al);
    gui_control_active (ar_button, ls.ar);
    gui_control_active (bl_button, ls.bl);
    gui_control_active (br_button, ls.br);
  } else if (selected == h_toggle) {
    gui_control_active (al_button, ls.la);
    gui_control_active (ar_button, ls.ra);
    gui_control_active (bl_button, ls.lb);
    gui_control_active (br_button, ls.rb);
  }

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

  if (ih == c_button) mr_center_room (mr.room);
  else if (ih == l_button) mr_select_trans (LEFT);
  else if (ih == r_button) mr_select_trans (RIGHT);
  else if (ih == a_button) mr_select_trans (ABOVE);
  else if (ih == b_button) mr_select_trans (BELOW);
  else if (ih == al_button) {
    if (selected == v_toggle) {
      mr_select_trans (ABOVE);
      mr_select_trans (LEFT);
    } else if (selected == h_toggle) {
      mr_select_trans (LEFT);
      mr_select_trans (ABOVE);
    }
  } else if (ih == ar_button) {
    if (selected == v_toggle) {
      mr_select_trans (ABOVE);
      mr_select_trans (RIGHT);
    } else if (selected == h_toggle) {
      mr_select_trans (RIGHT);
      mr_select_trans (ABOVE);
    }
  } else if (ih == bl_button) {
    if (selected == v_toggle) {
      mr_select_trans (BELOW);
      mr_select_trans (LEFT);
    } else if (selected == h_toggle) {
      mr_select_trans (LEFT);
      mr_select_trans (BELOW);
    }
  } else if (ih == br_button) {
    if (selected == v_toggle) {
      mr_select_trans (BELOW);
      mr_select_trans (RIGHT);
    } else if (selected == h_toggle) {
      mr_select_trans (RIGHT);
      mr_select_trans (BELOW);
    }
  }

  return IUP_DEFAULT;
}
