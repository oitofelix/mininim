/*
  gui-tile-transform-control.c -- editor GUI tile transform control module;

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

static int _update_cb (Ihandle *ih);
static int button_action_cb (Ihandle *ih);


Ihandle *
gui_create_tile_transform_control
(struct pos *p, struct sel_ring *sr, char *norm_group)
{
  Ihandle *ih, *vbox, *gridbox;

  Ihandle *clear_button, *randomize_button, *decorate_button, *fix_button,
    *unfake_button, *fg_fake_button;

  Ihandle *sel_ring_toggle, *radio, *place_toggle, *room_toggle, *level_toggle;

  clear_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = CLEAR_ICON,"
      "TIP = \"Clear\""),
     "ACTION", button_action_cb,
     NULL);

  randomize_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = RANDOM_ICON,"
      "TIP = \"Randomize\""),
     "ACTION", button_action_cb,
     NULL);

  decorate_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = DECORATION_ICON,"
      "TIP = \"Decorate\""),
     "ACTION", button_action_cb,
     NULL);

  fix_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = FIX_ICON,"
      "TIP = \"Fix\""),
     "ACTION", button_action_cb,
     NULL);

  unfake_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = UNMASK_ICON,"
      "TIP = \"Unfake\""),
     "ACTION", button_action_cb,
     NULL);

  fg_fake_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = MASK_EXCHANGE_ICON,"
      "TIP = \"Foreground <-> Fake\""),
     "ACTION", button_action_cb,
     NULL);

  gridbox = IupSetAttributes
    (IupGridBox
     (clear_button,
      randomize_button,
      decorate_button,
      fix_button,
      unfake_button,
      fg_fake_button,
      NULL),
     "ORIENTATION = HORIZONTAL,"
     "NUMDIV = 3,"
     "SIZECOL = -1,"
     "SIZELIN = -1,"
     "NORMALIZESIZE = BOTH,");

  sel_ring_toggle = IupSetAttributes
    (IupToggle ("S", NULL),
     "VALUE = YES,"
     "TIP = \"Apply to selection set\",");

  place_toggle = IupSetAttributes
    (IupToggle ("P", NULL),
     "TIP = \"Place scope\"");

  room_toggle = IupSetAttributes
    (IupToggle ("R", NULL),
     "TIP = \"Room scope\"");

  level_toggle = IupSetAttributes
    (IupToggle ("L", NULL),
     "TIP = \"Level scope\"");

  radio = IupRadio
    (IupHbox
     (place_toggle,
      room_toggle,
      level_toggle,
      NULL));

  ih = IupSetCallbacks
    (IupSetAttributes
     (IupFrame
      (vbox = IupSetAttributes
       (IupVbox
        (IupFill (),
         gridbox,
         sel_ring_toggle,
         radio,
         IupFill (),
         NULL),
        "ALIGNMENT = ACENTER")),
      "TITLE = Transform"),
     "_UPDATE_CB", _update_cb,
     NULL);

  IupSetAttribute (vbox, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_GRIDBOX", (void *) gridbox);
  IupSetAttribute (ih, "_CLEAR_BUTTON", (void *) clear_button);
  IupSetAttribute (ih, "_RANDOMIZE_BUTTON", (void *) randomize_button);
  IupSetAttribute (ih, "_DECORATE_BUTTON", (void *) decorate_button);
  IupSetAttribute (ih, "_FIX_BUTTON", (void *) fix_button);
  IupSetAttribute (ih, "_UNFAKE_BUTTON", (void *) unfake_button);
  IupSetAttribute (ih, "_FG_FAKE_BUTTON", (void *) fg_fake_button);

  IupSetAttribute (ih, "_SEL_RING_TOGGLE", (void *) sel_ring_toggle);
  IupSetAttribute (ih, "_RADIO", (void *) radio);
  IupSetAttribute (ih, "_PLACE_TOGGLE", (void *) place_toggle);
  IupSetAttribute (ih, "_ROOM_TOGGLE", (void *) room_toggle);
  IupSetAttribute (ih, "_LEVEL_TOGGLE", (void *) level_toggle);

  IupSetAttribute (ih, "_POS", (void *) p);
  IupSetAttribute (ih, "_SEL_RING", (void *) sr);

  return ih;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  struct pos *p = (void *) IupGetAttribute (ih, "_POS");
  struct sel_ring *sr = (void *) IupGetAttribute (ih, "_SEL_RING");
  gui_control_active
    (ih, is_valid_pos (p) || sel_ring_ss_c_nmemb (sr));

  Ihandle *unfake_button = (void *) IupGetAttribute (ih, "_UNFAKE_BUTTON");
  Ihandle *fg_fake_button = (void *) IupGetAttribute (ih, "_FG_FAKE_BUTTON");

  Ihandle *place_toggle = (void *) IupGetAttribute (ih, "_PLACE_TOGGLE");
  Ihandle *radio = (void *) IupGetAttribute (ih, "_RADIO");
  Ihandle *selected = (void *) IupGetAttribute (radio, "VALUE_HANDLE");

  Ihandle *sel_ring_toggle = (void *) IupGetAttribute (ih, "_SEL_RING_TOGGLE");
  gui_control_active (sel_ring_toggle, sel_ring_ss_c_nmemb (sr));
  bool use_sel_ring = IupGetInt (sel_ring_toggle, "VALUE");

  Ihandle *gridbox = (void *) IupGetAttribute (ih, "_GRIDBOX");
  gui_control_active (gridbox, is_valid_pos (p)
                      || (sel_ring_ss_c_nmemb (sr) && use_sel_ring));

  bool enable_fake =
    (is_valid_pos (p) && (is_fake (p) || selected != place_toggle))
    || (sel_ring_ss_c_nmemb (sr) && use_sel_ring);
  gui_control_active (unfake_button, enable_fake);
  gui_control_active (fg_fake_button, enable_fake);

  return IUP_DEFAULT;
}

int
button_action_cb (Ihandle *ih)
{
  Ihandle *clear_button = (void *) IupGetAttribute (ih, "_CLEAR_BUTTON");
  Ihandle *randomize_button =
    (void *) IupGetAttribute (ih, "_RANDOMIZE_BUTTON");
  Ihandle *decorate_button =
    (void *) IupGetAttribute (ih, "_DECORATE_BUTTON");
  Ihandle *fix_button = (void *) IupGetAttribute (ih, "_FIX_BUTTON");
  Ihandle *unfake_button = (void *) IupGetAttribute (ih, "_UNFAKE_BUTTON");
  Ihandle *fg_fake_button = (void *) IupGetAttribute (ih, "_FG_FAKE_BUTTON");

  Ihandle *place_toggle = (void *) IupGetAttribute (ih, "_PLACE_TOGGLE");
  Ihandle *room_toggle = (void *) IupGetAttribute (ih, "_ROOM_TOGGLE");
  Ihandle *level_toggle = (void *) IupGetAttribute (ih, "_LEVEL_TOGGLE");
  Ihandle *radio = (void *) IupGetAttribute (ih, "_RADIO");
  Ihandle *selected = (void *) IupGetAttribute (radio, "VALUE_HANDLE");

  Ihandle *sel_ring_toggle = (void *) IupGetAttribute (ih, "_SEL_RING_TOGGLE");
  bool use_sel_ring = IupGetInt (sel_ring_toggle, "VALUE");

  struct pos *p = (void *) IupGetAttribute (ih, "_POS");
  struct sel_ring *sr = (void *) IupGetAttribute (ih, "_SEL_RING");

  enum scope scope;
  if (selected == place_toggle)
    scope = sel_ring_ss_c_nmemb (sr) && use_sel_ring
      ? PLACE_SEL_RING_SCOPE : PLACE_SCOPE;
  else if (selected == room_toggle)
    scope = sel_ring_ss_c_nmemb (sr) && use_sel_ring
      ? ROOM_SEL_RING_SCOPE : ROOM_SCOPE;
  else if (selected == level_toggle) scope = LEVEL_SCOPE;
  else assert (false);

  pos_trans trans;
  char *desc;
  if (ih == clear_button) {
    trans = clear_tile;
    desc = "CLEAR";
  } else if (ih == randomize_button) {
    trans = random_tile;
    desc = "RANDOMIZE";
  } else if (ih == decorate_button) {
    trans = decorate_tile;
    desc = "DECORATE";
  } else if (ih == fix_button) {
    trans = fix_tile;
    desc = "FIX";
  } else if (ih == unfake_button) {
    trans = unfake_tile;
    desc = "UNFAKE";
  } else if (ih == fg_fake_button) {
    trans = fg_fake_exchange;
    desc = "FG<->FAKE";
  } else assert (false);

  apply_to_scope (p, sr, trans, NULL, desc, scope);

  return IUP_DEFAULT;
}
