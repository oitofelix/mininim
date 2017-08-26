/*
  gui-tile-move-control.c -- editor GUI tile move control module;

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

static int _update_cb (Ihandle *ih);


Ihandle *
gui_create_tile_move_control (struct pos *p, char *norm_group)
{
  Ihandle *ih, *vbox;

  Ihandle *move_label, *move_left_button,
    *move_right_button, *move_above_button,
    *move_below_button, *move_al_button,
    *move_ar_button, *move_bl_button, *move_br_button;

  ih = IupSetCallbacks
    (IupSetAttributes
     (IupFrame
      (vbox = IupSetAttributes
       (IupVbox
        (IupFill (),

         IupSetAttributes
         (IupGridBox
          (move_al_button = IupButton (NULL, NULL),
           move_above_button = IupButton (NULL, NULL),
           move_ar_button = IupButton (NULL, NULL),
           move_left_button = IupButton (NULL, NULL),
           move_label =
           IupSetAttributes
           (IupLabel (NULL),
            "ALIGNMENT = ACENTER:ACENTER"),
           move_right_button = IupButton (NULL, NULL),
           move_bl_button = IupButton (NULL, NULL),
           move_below_button = IupButton (NULL, NULL),
           move_br_button = IupButton (NULL, NULL),
           NULL),
          "ORIENTATION = HORIZONTAL,"
          "NUMDIV = 3,"
          "SIZECOL = -1,"
          "SIZELIN = -1,"
          "NORMALIZESIZE = BOTH,"),

         IupFill (),

         NULL),
        "ALIGNMENT = ACENTER")),
      "TITLE = Move"),
     "_UPDATE_CB", _update_cb,
     NULL);

  ALLEGRO_BITMAP *nav_place_icon = load_memory_bitmap (NAV_PLACE_ICON);
  gui_set_image (move_label, nav_place_icon, NULL);
  al_destroy_bitmap (nav_place_icon);

  ALLEGRO_BITMAP *nav_left_icon = load_memory_bitmap (NAV_LEFT_ICON);
  ALLEGRO_BITMAP *nav_above_icon = load_memory_bitmap (NAV_ABOVE_ICON);
  ALLEGRO_BITMAP *nav_right_icon = load_memory_bitmap (NAV_RIGHT_ICON);
  ALLEGRO_BITMAP *nav_below_icon = load_memory_bitmap (NAV_BELOW_ICON);

  ALLEGRO_BITMAP *nav_al_icon = load_memory_bitmap (NAV_AL_ICON);
  ALLEGRO_BITMAP *nav_ar_icon = load_memory_bitmap (NAV_AR_ICON);
  ALLEGRO_BITMAP *nav_bl_icon = load_memory_bitmap (NAV_BL_ICON);
  ALLEGRO_BITMAP *nav_br_icon = load_memory_bitmap (NAV_BR_ICON);

  gui_set_image (move_left_button, nav_left_icon, NULL);
  gui_set_image (move_above_button, nav_above_icon, NULL);
  gui_set_image (move_right_button, nav_right_icon, NULL);
  gui_set_image (move_below_button, nav_below_icon, NULL);

  gui_set_image (move_al_button, nav_al_icon, NULL);
  gui_set_image (move_ar_button, nav_ar_icon, NULL);
  gui_set_image (move_bl_button, nav_bl_icon, NULL);
  gui_set_image (move_br_button, nav_br_icon, NULL);

  IupSetAttribute (vbox, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_POS", (void *) p);

  return ih;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  struct pos *p = (void *) IupGetAttribute (ih, "_POS");
  gui_control_active (ih, is_valid_pos (p));

  return IUP_DEFAULT;
}
