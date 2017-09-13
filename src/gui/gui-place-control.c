/*
  gui-place-control.c -- GUI place control module;

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
gui_create_place_control (struct pos *p, char *norm_group)
{
  Ihandle *ih = IupSetCallbacks
    (gui_create_directional_control
     ("L. place", "PLACE_SEL_LOCK_ICON", button_action_cb, norm_group),
     "_UPDATE_CB", _update_cb,
     NULL);

  IupSetAttribute (ih, "_POS", (void *) p);

  Ihandle *c_button = (void *) IupGetAttribute (ih, "_C_BUTTON");
  IupSetAttribute (c_button, "TIP", "Place kid at selection");

  return ih;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  gui_control_active (ih, selection_locked);
  if (! selection_locked) return IUP_DEFAULT;

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

  struct pos *p = (void *) IupGetAttribute (ih, "_POS");

  struct pos_survey ps;
  pos_survey (&ps, p);

  gui_control_active (l_button, ps.l.room);
  gui_control_active (r_button, ps.r.room);
  gui_control_active (a_button, ps.a.room);
  gui_control_active (b_button, ps.b.room);

  if (selected == v_toggle) {
    gui_control_active (al_button, ps.al.room);
    gui_control_active (ar_button, ps.ar.room);
    gui_control_active (bl_button, ps.bl.room);
    gui_control_active (br_button, ps.br.room);
  } else if (selected == h_toggle) {
    gui_control_active (al_button, ps.la.room);
    gui_control_active (ar_button, ps.ra.room);
    gui_control_active (bl_button, ps.lb.room);
    gui_control_active (br_button, ps.rb.room);
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

  struct pos *p = (void *) IupGetAttribute (ih, "_POS");

  if (ih == c_button) ui_place_kid (get_actor_by_id (current_kid_id), p);
  else if (ih == l_button)
    ui_move_locked_place_selection (&global_mr, LEFT);
  else if (ih == r_button)
    ui_move_locked_place_selection (&global_mr, RIGHT);
  else if (ih == a_button)
    ui_move_locked_place_selection (&global_mr, ABOVE);
  else if (ih == b_button)
    ui_move_locked_place_selection (&global_mr, BELOW);
  else if (ih == al_button) {
    if (selected == v_toggle) {
      ui_move_locked_place_selection (&global_mr, ABOVE);
      ui_move_locked_place_selection (&global_mr, LEFT);
    } else if (selected == h_toggle) {
      ui_move_locked_place_selection (&global_mr, LEFT);
      ui_move_locked_place_selection (&global_mr, ABOVE);
    }
  } else if (ih == ar_button) {
    if (selected == v_toggle) {
      ui_move_locked_place_selection (&global_mr, ABOVE);
      ui_move_locked_place_selection (&global_mr, RIGHT);
    } else if (selected == h_toggle) {
      ui_move_locked_place_selection (&global_mr, RIGHT);
      ui_move_locked_place_selection (&global_mr, ABOVE);
    }
  } else if (ih == bl_button) {
    if (selected == v_toggle) {
      ui_move_locked_place_selection (&global_mr, BELOW);
      ui_move_locked_place_selection (&global_mr, LEFT);
    } else if (selected == h_toggle) {
      ui_move_locked_place_selection (&global_mr, LEFT);
      ui_move_locked_place_selection (&global_mr, BELOW);
    }
  } else if (ih == br_button) {
    if (selected == v_toggle) {
      ui_move_locked_place_selection (&global_mr, BELOW);
      ui_move_locked_place_selection (&global_mr, RIGHT);
    } else if (selected == h_toggle) {
      ui_move_locked_place_selection (&global_mr, RIGHT);
      ui_move_locked_place_selection (&global_mr, BELOW);
    }
  }

  return IUP_DEFAULT;
}
