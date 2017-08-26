/*
  gui-tile-transform-control.c -- editor GUI tile transform control module;

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
gui_create_tile_transform_control (struct pos *p, char *norm_group)
{
  Ihandle *ih, *vbox, *transform_radio;

  Ihandle *transform_label, *clear_button, *randomize_button,
    *decorate_button, *fix_button;

  ih = IupSetCallbacks
    (IupSetAttributes
     (IupFrame
      (vbox = IupSetAttributes
       (IupVbox
        (IupFill (),

         IupSetAttributes
         (IupGridBox
          (transform_label = IupLabel (NULL),
           clear_button = IupSetAttributes
           (IupButton (NULL, NULL),
            "TIP = \"Clear\""),
           randomize_button = IupSetAttributes
           (IupButton (NULL, NULL),
            "TIP = \"Randomize\""),
           decorate_button = IupSetAttributes
           (IupButton (NULL, NULL),
            "TIP = \"Decorate\""),
           fix_button = IupSetAttributes
           (IupButton (NULL, NULL),
            "TIP = \"Fix\""),
           NULL),
          "ORIENTATION = HORIZONTAL,"
          "NUMDIV = 5,"
          "SIZECOL = -1,"
          "SIZELIN = -1,"
          "NORMALIZESIZE = BOTH,"),

         transform_radio = IupRadio
         (IupHbox
          (IupSetAttributes
           (IupToggle ("P", NULL),
            "TIP = \"Place tooling scope\""),
           IupSetAttributes
           (IupToggle ("R", NULL),
            "TIP = \"Room tooling scope\""),
           IupSetAttributes
           (IupToggle ("L", NULL),
            "TIP = \"Level tooling scope\""),
           NULL)),

         IupFill (),

         NULL),
        "ALIGNMENT = ACENTER")),
      "TITLE = Transform"),
     "_UPDATE_CB", _update_cb,
     NULL);

  ALLEGRO_BITMAP *transform_icon = load_memory_bitmap (TOOLS_ICON);
  gui_set_image (transform_label, transform_icon, NULL);
  al_destroy_bitmap (transform_icon);

  ALLEGRO_BITMAP *clear_icon = load_memory_bitmap (CLEAR_ICON);
  gui_set_image (clear_button, clear_icon, NULL);
  al_destroy_bitmap (clear_icon);

  ALLEGRO_BITMAP *random_icon = load_memory_bitmap (RANDOM_ICON);
  gui_set_image (randomize_button, random_icon, NULL);
  al_destroy_bitmap (random_icon);

  ALLEGRO_BITMAP *decoration_icon = load_memory_bitmap (DECORATION_ICON);
  gui_set_image (decorate_button, decoration_icon, NULL);
  al_destroy_bitmap (decoration_icon);

  ALLEGRO_BITMAP *fix_icon = load_memory_bitmap (FIX_ICON);
  gui_set_image (fix_button, fix_icon, NULL);
  al_destroy_bitmap (fix_icon);

  IupSetAttribute (vbox, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_TRANSFORM_RADIO", (void *) transform_radio);
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
