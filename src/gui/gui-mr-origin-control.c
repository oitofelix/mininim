/*
  gui-mr-origin-control.c -- GUI MR origin control module;

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
gui_create_mr_origin_control (char *norm_group)
{
  Ihandle *ih = IupSetCallbacks
    (gui_create_directional_control
     ("MR origin", "VIEW_RING_ICON", button_action_cb, norm_group),
     "_UPDATE_CB", _update_cb,
     NULL);

  Ihandle *c_button = (void *) IupGetAttribute (ih, "_C_BUTTON");
  IupSetAttribute (c_button, "TIP", "Optimal view");

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

  struct link_survey ls;
  link_survey (&ls, global_level.link, global_level.room_nmemb,
               global_mr.room);

  gui_control_active (l_button, ls.l);
  gui_control_active (r_button, ls.r);
  gui_control_active (a_button, ls.a);
  gui_control_active (b_button, ls.b);

  gui_control_active (al_button, ls.al || ls.la);
  gui_control_active (ar_button, ls.ar || ls.ra);
  gui_control_active (bl_button, ls.bl || ls.lb);
  gui_control_active (br_button, ls.br || ls.rb);

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

  struct link_survey ls;
  link_survey (&ls, global_level.link, global_level.room_nmemb,
               global_mr.room);

  if (ih == c_button) mr_center_room (&global_mr, global_mr.room);
  else if (ih == l_button) mr_room_trans (&global_mr, LEFT);
  else if (ih == r_button) mr_room_trans (&global_mr, RIGHT);
  else if (ih == a_button) mr_room_trans (&global_mr, ABOVE);
  else if (ih == b_button) mr_room_trans (&global_mr, BELOW);
  else if (ih == al_button) {
    if (ls.al && (selected == v_toggle || ! ls.la)) {
      mr_room_trans (&global_mr, ABOVE);
      mr_room_trans (&global_mr, LEFT);
    } else if (ls.la) {
      mr_room_trans (&global_mr, LEFT);
      mr_room_trans (&global_mr, ABOVE);
    }
  } else if (ih == ar_button) {
    if (ls.ar && (selected == v_toggle || ! ls.ra)) {
      mr_room_trans (&global_mr, ABOVE);
      mr_room_trans (&global_mr, RIGHT);
    } else if (ls.ra) {
      mr_room_trans (&global_mr, RIGHT);
      mr_room_trans (&global_mr, ABOVE);
    }
  } else if (ih == bl_button) {
    if (ls.bl && (selected == v_toggle || ! ls.lb)) {
      mr_room_trans (&global_mr, BELOW);
      mr_room_trans (&global_mr, LEFT);
    } else if (ls.lb) {
      mr_room_trans (&global_mr, LEFT);
      mr_room_trans (&global_mr, BELOW);
    }
  } else if (ih == br_button) {
    if (ls.br && (selected == v_toggle || ! ls.rb)) {
      mr_room_trans (&global_mr, BELOW);
      mr_room_trans (&global_mr, RIGHT);
    } else if (ls.rb) {
      mr_room_trans (&global_mr, RIGHT);
      mr_room_trans (&global_mr, BELOW);
    }
  }

  return IUP_DEFAULT;
}
