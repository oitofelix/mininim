/*
  gui-directional-control.c -- GUI directional control module;

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


Ihandle *
gui_create_directional_control (char *title, char *c_image_hname,
                                Icallback action_cb, char *norm_group)
{
  Ihandle *ih, *vbox;

  Ihandle *c_label, *l_button, *r_button, *a_button, *b_button,
    *al_button, *ar_button, *bl_button, *br_button;

  ih = IupFrame
    (vbox = IupSetAttributes
     (IupVbox
      (IupFill (),
       IupSetAttributes
       (IupGridBox
        (al_button = IupSetCallbacks
         (IupSetAttributes
          (IupButton (NULL, NULL),
           "IMAGE = AL_ICON"),
          "ACTION", action_cb,
          NULL),
         a_button = IupSetCallbacks
         (IupSetAttributes
          (IupButton (NULL, NULL),
           "IMAGE = A_ICON"),
          "ACTION", action_cb,
          NULL),
         ar_button = IupSetCallbacks
         (IupSetAttributes
          (IupButton (NULL, NULL),
           "IMAGE = AR_ICON"),
          "ACTION", action_cb,
          NULL),
         l_button = IupSetCallbacks
         (IupSetAttributes
          (IupButton (NULL, NULL),
           "IMAGE = L_ICON"),
          "ACTION", action_cb,
          NULL),
         c_label = IupSetAttributes
         (IupLabel (NULL),
          "ALIGNMENT = ACENTER:ACENTER"),
         r_button = IupSetCallbacks
         (IupSetAttributes
          (IupButton (NULL, NULL),
           "IMAGE = R_ICON"),
          "ACTION", action_cb,
          NULL),
         bl_button = IupSetCallbacks
         (IupSetAttributes
          (IupButton (NULL, NULL),
           "IMAGE = BL_ICON"),
          "ACTION", action_cb,
          NULL),
         b_button = IupSetCallbacks
         (IupSetAttributes
          (IupButton (NULL, NULL),
           "IMAGE = B_ICON"),
          "ACTION", action_cb,
          NULL),
         br_button = IupSetCallbacks
         (IupSetAttributes
          (IupButton (NULL, NULL),
           "IMAGE = BR_ICON"),
          "ACTION", action_cb,
          NULL),
         NULL),
        "ORIENTATION = HORIZONTAL,"
        "NUMDIV = 3,"
        "SIZECOL = -1,"
        "SIZELIN = -1,"
        /* "NORMALIZESIZE = BOTH," */
        "ALIGNMENTLIN = ACENTER,"
        "ALIGNMENTCOL = ACENTER,"
        ),
       IupFill (),
       NULL),
      "ALIGNMENT = ACENTER"));

  IupSetAttribute (ih, "TITLE", title);
  IupSetAttribute (c_label, "IMAGE", c_image_hname);
  IupSetAttribute (vbox, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_AL_BUTTON", (void *) al_button);
  IupSetAttribute (ih, "_A_BUTTON", (void *) a_button);
  IupSetAttribute (ih, "_AR_BUTTON", (void *) ar_button);
  IupSetAttribute (ih, "_L_BUTTON", (void *) l_button);
  IupSetAttribute (ih, "_R_BUTTON", (void *) r_button);
  IupSetAttribute (ih, "_BL_BUTTON", (void *) bl_button);
  IupSetAttribute (ih, "_B_BUTTON", (void *) b_button);
  IupSetAttribute (ih, "_BR_BUTTON", (void *) br_button);

  return ih;
}
