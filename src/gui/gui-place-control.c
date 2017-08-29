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
  /* ACTION (center button): place kid at selection */
  Ihandle *ih = IupSetCallbacks
    (gui_create_directional_control
     ("Place", "PLACE_ICON", button_action_cb, norm_group),
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
  struct pos pl, pr, pa, pb;

  prel (p, &pl, +0, -1); npos (&pl, &pl);
  prel (p, &pr, +0, +1); npos (&pr, &pr);
  prel (p, &pa, -1, +0); npos (&pa, &pa);
  prel (p, &pb, +1, +0); npos (&pb, &pb);

  gui_control_active (l_button, pl.room);
  gui_control_active (r_button, pr.room);
  gui_control_active (a_button, pa.room);
  gui_control_active (b_button, pb.room);

  if (selected == v_toggle) {
    struct pos pal, par, pbl, pbr;

    prel (&pa, &pal, +0, -1); npos (&pal, &pal);
    prel (&pa, &par, +0, +1); npos (&par, &par);
    prel (&pb, &pbl, +0, -1); npos (&pbl, &pbl);
    prel (&pb, &pbr, +0, +1); npos (&pbr, &pbr);

    gui_control_active (al_button, pal.room);
    gui_control_active (ar_button, par.room);
    gui_control_active (bl_button, pbl.room);
    gui_control_active (br_button, pbr.room);
  } else if (selected == h_toggle) {
    struct pos pla, pra, plb, prb;

    prel (&pl, &pla, -1, +0); npos (&pla, &pla);
    prel (&pl, &plb, +1, +0); npos (&plb, &plb);
    prel (&pr, &pra, -1, +0); npos (&pra, &pra);
    prel (&pr, &prb, +1, +0); npos (&prb, &prb);

    gui_control_active (al_button, pla.room);
    gui_control_active (ar_button, pra.room);
    gui_control_active (bl_button, plb.room);
    gui_control_active (br_button, prb.room);
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
  else if (ih == l_button) ui_move_locked_place_selection (LEFT);
  else if (ih == r_button) ui_move_locked_place_selection (RIGHT);
  else if (ih == a_button) ui_move_locked_place_selection (ABOVE);
  else if (ih == b_button) ui_move_locked_place_selection (BELOW);
  else if (ih == al_button) {
    if (selected == v_toggle) {
      ui_move_locked_place_selection (ABOVE);
      ui_move_locked_place_selection (LEFT);
    } else if (selected == h_toggle) {
      ui_move_locked_place_selection (LEFT);
      ui_move_locked_place_selection (ABOVE);
    }
  } else if (ih == ar_button) {
    if (selected == v_toggle) {
      ui_move_locked_place_selection (ABOVE);
      ui_move_locked_place_selection (RIGHT);
    } else if (selected == h_toggle) {
      ui_move_locked_place_selection (RIGHT);
      ui_move_locked_place_selection (ABOVE);
    }
  } else if (ih == bl_button) {
    if (selected == v_toggle) {
      ui_move_locked_place_selection (BELOW);
      ui_move_locked_place_selection (LEFT);
    } else if (selected == h_toggle) {
      ui_move_locked_place_selection (LEFT);
      ui_move_locked_place_selection (BELOW);
    }
  } else if (ih == br_button) {
    if (selected == v_toggle) {
      ui_move_locked_place_selection (BELOW);
      ui_move_locked_place_selection (RIGHT);
    } else if (selected == h_toggle) {
      ui_move_locked_place_selection (RIGHT);
      ui_move_locked_place_selection (BELOW);
    }
  }

  return IUP_DEFAULT;
}
