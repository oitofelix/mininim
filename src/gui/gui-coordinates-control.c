/*
  gui-coordinates-control.c -- GUI coordinates control module;

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
static void update_link (Ihandle *ih, int room);
static int action_cb (Ihandle *ih);


Ihandle *
gui_create_coordinates_control (struct level *level, char *norm_group)
{
  Ihandle *ih;

  Ihandle *s_link, *l_link, *r_link, *a_link, *b_link,
    *lv_link, *al_link, *ar_link, *bl_link, *br_link;

  ih = IupSetCallbacks
    (IupSetAttributes
     (IupGridBox
      (
       IupSetAttributes
       (IupLabel ("S:"),
        "TIP = \"Current room\","
        "ALIGNMENT = ARIGHT"),

       s_link = IupSetCallbacks
       (IupLink (NULL, "0"),
        "ACTION", action_cb,
        NULL),

       IupSetAttributes
       (IupLabel ("L:"),
        "TIP = \"Left room\","
        "ALIGNMENT = ARIGHT"),

       l_link = IupSetCallbacks
       (IupLink (NULL, "0"),
        "ACTION", action_cb,
        NULL),

       IupSetAttributes
       (IupLabel ("R:"),
        "TIP = \"Right room\","
        "ALIGNMENT = ARIGHT"),

       r_link = IupSetCallbacks
       (IupLink (NULL, "0"),
        "ACTION", action_cb,
        NULL),

       IupSetAttributes
       (IupLabel ("A:"),
        "TIP = \"Above room\","
        "ALIGNMENT = ARIGHT"),

       a_link = IupSetCallbacks
       (IupLink (NULL, "0"),
        "ACTION", action_cb,
        NULL),

       IupSetAttributes
       (IupLabel ("B:"),
        "TIP = \"Below room\","
        "ALIGNMENT = ARIGHT"),

       b_link = IupSetCallbacks
       (IupLink (NULL, "0"),
        "ACTION", action_cb,
        NULL),

       IupSetAttributes
       (IupLabel ("LV:"),
        "TIP = \"Current Level\","
        "ALIGNMENT = ARIGHT"),

       lv_link = IupSetCallbacks
       (IupSetAttributes
        (IupLabel ("0"),
         "ACTIVE = YES"),
        "ACTION", action_cb,
        NULL),

       IupSetAttributes
       (IupLabel ("AL:"),
        "TIP = \"Above-left room\","
        "ALIGNMENT = ARIGHT"),

       al_link = IupSetCallbacks
       (IupLink (NULL, "0"),
        "ACTION", action_cb,
        NULL),

       IupSetAttributes
       (IupLabel ("AR:"),
        "TIP = \"Above-right room\","
        "ALIGNMENT = ARIGHT"),

       ar_link = IupSetCallbacks
       (IupLink (NULL, "0"),
        "ACTION", action_cb,
        NULL),

       IupSetAttributes
       (IupLabel ("BL:"),
        "TIP = \"Below-left room\","
        "ALIGNMENT = ARIGHT"),

       bl_link = IupSetCallbacks
       (IupLink (NULL, "0"),
        "ACTION", action_cb,
        NULL),

       IupSetAttributes
       (IupLabel ("BR:"),
        "TIP = \"Below-right room\","
        "ALIGNMENT = ARIGHT"),

       br_link = IupSetCallbacks
       (IupLink (NULL, "0"),
        "ACTION", action_cb,
        NULL),

       NULL),
      "ORIENTATION = HORIZONTAL,"
      "NUMDIV = 10,"
      "SIZECOL = -1,"
      "SIZELIN = -1,"
      "NORMALIZESIZE = BOTH,"
      "ALIGNMENTCOL = ARIGHT,"),
     "_UPDATE_CB", _update_cb,
     NULL);

  IupSetAttribute (ih, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_LEVEL", (void *) level);

  IupSetAttribute (ih, "_S_LINK", (void *) s_link);
  IupSetAttribute (ih, "_L_LINK", (void *) l_link);
  IupSetAttribute (ih, "_R_LINK", (void *) r_link);
  IupSetAttribute (ih, "_A_LINK", (void *) a_link);
  IupSetAttribute (ih, "_B_LINK", (void *) b_link);

  IupSetAttribute (ih, "_LV_LINK", (void *) lv_link);
  IupSetAttribute (ih, "_AL_LINK", (void *) al_link);
  IupSetAttribute (ih, "_AR_LINK", (void *) ar_link);
  IupSetAttribute (ih, "_BL_LINK", (void *) bl_link);
  IupSetAttribute (ih, "_BR_LINK", (void *) br_link);

  return ih;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  struct level *level = (void *) IupGetAttribute (ih, "_LEVEL");

  Ihandle *s_link = (void *) IupGetAttribute (ih, "_S_LINK");
  Ihandle *l_link = (void *) IupGetAttribute (ih, "_L_LINK");
  Ihandle *r_link = (void *) IupGetAttribute (ih, "_R_LINK");
  Ihandle *a_link = (void *) IupGetAttribute (ih, "_A_LINK");
  Ihandle *b_link = (void *) IupGetAttribute (ih, "_B_LINK");

  Ihandle *lv_link = (void *) IupGetAttribute (ih, "_LV_LINK");
  Ihandle *al_link = (void *) IupGetAttribute (ih, "_AL_LINK");
  Ihandle *ar_link = (void *) IupGetAttribute (ih, "_AR_LINK");
  Ihandle *bl_link = (void *) IupGetAttribute (ih, "_BL_LINK");
  Ihandle *br_link = (void *) IupGetAttribute (ih, "_BR_LINK");

  int s = mr.room;
  int l = roomd (level, s, LEFT);
  int r = roomd (level, s, RIGHT);
  int a = roomd (level, s, ABOVE);
  int b = roomd (level, s, BELOW);

  int lv = level->n;
  int al = roomd (level, a, LEFT);
  int ar = roomd (level, a, RIGHT);
  int bl = roomd (level, b, LEFT);
  int br = roomd (level, b, RIGHT);

  update_link (s_link, s);
  update_link (l_link, l);
  update_link (r_link, r);
  update_link (a_link, a);
  update_link (b_link, b);

  update_link (lv_link, lv);
  update_link (al_link, al);
  update_link (ar_link, ar);
  update_link (bl_link, bl);
  update_link (br_link, br);

  return IUP_DEFAULT;
}

void
update_link (Ihandle *ih, int room)
{
  if (IupGetInt (ih, "TITLE") != room) IupSetInt (ih, "TITLE", room);
  gui_control_active (ih, room > 0);
}

int
action_cb (Ihandle *ih)
{
  int room = IupGetInt (ih, "TITLE");
  mr_center_room (room);
  return IUP_DEFAULT;
}
