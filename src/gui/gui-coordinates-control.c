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
static void update_label (Ihandle *ih, int room0, int room1,
                          int room2, int room3);
static void update_link (Ihandle *ih, int room);
static int action_cb (Ihandle *ih);


Ihandle *
gui_create_coordinates_control (struct level *level, char *norm_group)
{
  Ihandle *ih;

  Ihandle *s_label, *l_label, *r_label, *a_label, *b_label,
    *al_label, *ar_label, *bl_label, *br_label,
    *la_label, *ra_label, *lb_label, *rb_label;

  Ihandle *s_link, *l_link, *r_link, *a_link, *b_link,
    *al_link, *ar_link, *bl_link, *br_link,
    *la_link, *ra_link, *lb_link, *rb_link;

  ih = IupSetCallbacks
    (IupSetAttributes
     (IupGridBox
      (
       /* AL/LA */
       IupHbox
       (al_label = IupSetAttributes
        (IupLabel ("AL:"),
         "TIP = \"Above-left room\","
         "ALIGNMENT = ARIGHT"),

        al_link = IupSetCallbacks
        (IupSetAttributes
         (IupLink (NULL, NULL),
          "FONTFACE = Courier"),
         "ACTION", action_cb,
         NULL),

        la_label = IupSetAttributes
        (IupLabel ("LA:"),
         "TIP = \"Left-above room\","
         "ALIGNMENT = ARIGHT"),

        la_link = IupSetCallbacks
        (IupSetAttributes
         (IupLink (NULL, NULL),
          "FONTFACE = Courier"),
         "ACTION", action_cb,
         NULL),
        NULL),

       /* A */
       IupHbox
       (a_label = IupSetAttributes
        (IupLabel ("A:"),
         "TIP = \"Above room\","
         "ALIGNMENT = ARIGHT"),

        a_link = IupSetCallbacks
        (IupSetAttributes
         (IupLink (NULL, NULL),
          "FONTFACE = Courier"),
         "ACTION", action_cb,
         NULL),
        NULL),

       /* AR/RA */
       IupHbox
       (ar_label = IupSetAttributes
        (IupLabel ("AR:"),
         "TIP = \"Above-right room\","
         "ALIGNMENT = ARIGHT"),

        ar_link = IupSetCallbacks
        (IupSetAttributes
         (IupLink (NULL, NULL),
          "FONTFACE = Courier"),
         "ACTION", action_cb,
         NULL),

        ra_label = IupSetAttributes
        (IupLabel ("RA:"),
         "TIP = \"Right-above room\","
         "ALIGNMENT = ARIGHT"),

        ra_link = IupSetCallbacks
        (IupSetAttributes
         (IupLink (NULL, NULL),
          "FONTFACE = Courier"),
         "ACTION", action_cb,
         NULL),
        NULL),

       /* L */
       IupHbox
       (l_label = IupSetAttributes
        (IupLabel ("L:"),
         "TIP = \"Left room\","
         "ALIGNMENT = ARIGHT"),

        l_link = IupSetCallbacks
        (IupSetAttributes
         (IupLink (NULL, NULL),
          "FONTFACE = Courier"),
         "ACTION", action_cb,
         NULL),
        NULL),

       /* S */
       IupHbox
       (s_label = IupSetAttributes
        (IupLabel ("S:"),
         "TIP = \"Current room\","
         "ALIGNMENT = ARIGHT"),

        s_link = IupSetCallbacks
        (IupSetAttributes
         (IupLink (NULL, NULL),
          "FONTFACE = Courier"),
         "ACTION", action_cb,
         NULL),
        NULL),

       /* R */
       IupHbox
       (r_label = IupSetAttributes
        (IupLabel ("R:"),
         "TIP = \"Right room\","
         "ALIGNMENT = ARIGHT"),

        r_link = IupSetCallbacks
        (IupSetAttributes
         (IupLink (NULL, NULL),
          "FONTFACE = Courier"),
         "ACTION", action_cb,
         NULL),
        NULL),

       /* BL/LB */
       IupHbox
       (bl_label = IupSetAttributes
        (IupLabel ("BL:"),
         "TIP = \"Below-left room\","
         "ALIGNMENT = ARIGHT"),

        bl_link = IupSetCallbacks
        (IupSetAttributes
         (IupLink (NULL, NULL),
          "FONTFACE = Courier"),
         "ACTION", action_cb,
         NULL),

        lb_label = IupSetAttributes
        (IupLabel ("LB:"),
         "TIP = \"Left-below room\","
         "ALIGNMENT = ARIGHT"),

        lb_link = IupSetCallbacks
        (IupSetAttributes
         (IupLink (NULL, NULL),
          "FONTFACE = Courier"),
         "ACTION", action_cb,
         NULL),
        NULL),

       /* B */
       IupHbox
       (b_label = IupSetAttributes
        (IupLabel ("B:"),
         "TIP = \"Below room\","
         "ALIGNMENT = ARIGHT"),

        b_link = IupSetCallbacks
        (IupSetAttributes
         (IupLink (NULL, NULL),
          "FONTFACE = Courier"),
         "ACTION", action_cb,
         NULL),
        NULL),

       /* BR/RB */
       IupHbox
       (br_label = IupSetAttributes
        (IupLabel ("BR:"),
         "TIP = \"Below-right room\","
         "ALIGNMENT = ARIGHT"),

        br_link = IupSetCallbacks
        (IupSetAttributes
         (IupLink (NULL, NULL),
          "FONTFACE = Courier"),
         "ACTION", action_cb,
         NULL),

        rb_label = IupSetAttributes
        (IupLabel ("RB:"),
         "TIP = \"Right-below room\","
         "ALIGNMENT = ARIGHT"),

        rb_link = IupSetCallbacks
        (IupSetAttributes
         (IupLink (NULL, NULL),
          "FONTFACE = Courier"),
         "ACTION", action_cb,
         NULL),
        NULL),

       NULL),
      "ORIENTATION = HORIZONTAL,"
      "NUMDIV = 3,"
      "SIZECOL = -1,"
      "SIZELIN = -1,"
      /* "NORMALIZESIZE = BOTH," */
      /* "HOMOGENEOUSLIN = YES," */
      /* "HOMOGENEOUSCOL = YES," */
      "ALIGNMENTLIN = ACENTER,"
      "ALIGNMENTCOL = ACENTER,"
      /* "FITTOCHILDREN = YES," */
      ),
     "_UPDATE_CB", _update_cb,
     NULL);

  IupSetAttribute (ih, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_LEVEL", (void *) level);

  IupSetAttribute (ih, "_S_LABEL", (void *) s_label);
  IupSetAttribute (ih, "_L_LABEL", (void *) l_label);
  IupSetAttribute (ih, "_R_LABEL", (void *) r_label);
  IupSetAttribute (ih, "_A_LABEL", (void *) a_label);
  IupSetAttribute (ih, "_B_LABEL", (void *) b_label);

  IupSetAttribute (ih, "_AL_LABEL", (void *) al_label);
  IupSetAttribute (ih, "_AR_LABEL", (void *) ar_label);
  IupSetAttribute (ih, "_BL_LABEL", (void *) bl_label);
  IupSetAttribute (ih, "_BR_LABEL", (void *) br_label);

  IupSetAttribute (ih, "_LA_LABEL", (void *) la_label);
  IupSetAttribute (ih, "_RA_LABEL", (void *) ra_label);
  IupSetAttribute (ih, "_LB_LABEL", (void *) lb_label);
  IupSetAttribute (ih, "_RB_LABEL", (void *) rb_label);

  IupSetAttribute (ih, "_S_LINK", (void *) s_link);
  IupSetAttribute (ih, "_L_LINK", (void *) l_link);
  IupSetAttribute (ih, "_R_LINK", (void *) r_link);
  IupSetAttribute (ih, "_A_LINK", (void *) a_link);
  IupSetAttribute (ih, "_B_LINK", (void *) b_link);

  IupSetAttribute (ih, "_AL_LINK", (void *) al_link);
  IupSetAttribute (ih, "_AR_LINK", (void *) ar_link);
  IupSetAttribute (ih, "_BL_LINK", (void *) bl_link);
  IupSetAttribute (ih, "_BR_LINK", (void *) br_link);

  IupSetAttribute (ih, "_LA_LINK", (void *) la_link);
  IupSetAttribute (ih, "_RA_LINK", (void *) ra_link);
  IupSetAttribute (ih, "_LB_LINK", (void *) lb_link);
  IupSetAttribute (ih, "_RB_LINK", (void *) rb_link);

  _update_cb (ih);

  return ih;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  struct level *level = (void *) IupGetAttribute (ih, "_LEVEL");

  Ihandle *l_label = (void *) IupGetAttribute (ih, "_L_LABEL");
  Ihandle *r_label = (void *) IupGetAttribute (ih, "_R_LABEL");
  Ihandle *a_label = (void *) IupGetAttribute (ih, "_A_LABEL");
  Ihandle *b_label = (void *) IupGetAttribute (ih, "_B_LABEL");

  Ihandle *al_label = (void *) IupGetAttribute (ih, "_AL_LABEL");
  Ihandle *ar_label = (void *) IupGetAttribute (ih, "_AR_LABEL");
  Ihandle *bl_label = (void *) IupGetAttribute (ih, "_BL_LABEL");
  Ihandle *br_label = (void *) IupGetAttribute (ih, "_BR_LABEL");

  Ihandle *la_label = (void *) IupGetAttribute (ih, "_LA_LABEL");
  Ihandle *ra_label = (void *) IupGetAttribute (ih, "_RA_LABEL");
  Ihandle *lb_label = (void *) IupGetAttribute (ih, "_LB_LABEL");
  Ihandle *rb_label = (void *) IupGetAttribute (ih, "_RB_LABEL");

  Ihandle *s_link = (void *) IupGetAttribute (ih, "_S_LINK");
  Ihandle *l_link = (void *) IupGetAttribute (ih, "_L_LINK");
  Ihandle *r_link = (void *) IupGetAttribute (ih, "_R_LINK");
  Ihandle *a_link = (void *) IupGetAttribute (ih, "_A_LINK");
  Ihandle *b_link = (void *) IupGetAttribute (ih, "_B_LINK");

  Ihandle *al_link = (void *) IupGetAttribute (ih, "_AL_LINK");
  Ihandle *ar_link = (void *) IupGetAttribute (ih, "_AR_LINK");
  Ihandle *bl_link = (void *) IupGetAttribute (ih, "_BL_LINK");
  Ihandle *br_link = (void *) IupGetAttribute (ih, "_BR_LINK");

  Ihandle *la_link = (void *) IupGetAttribute (ih, "_LA_LINK");
  Ihandle *ra_link = (void *) IupGetAttribute (ih, "_RA_LINK");
  Ihandle *lb_link = (void *) IupGetAttribute (ih, "_LB_LINK");
  Ihandle *rb_link = (void *) IupGetAttribute (ih, "_RB_LINK");

  int s = mr.room;
  int l = roomd (level, s, LEFT);
  int r = roomd (level, s, RIGHT);
  int a = roomd (level, s, ABOVE);
  int b = roomd (level, s, BELOW);

  int lr = roomd (level, l, RIGHT);
  int rl = roomd (level, r, LEFT);
  int ab = roomd (level, a, BELOW);
  int ba = roomd (level, b, ABOVE);

  int al = roomd (level, a, LEFT);
  int ar = roomd (level, a, RIGHT);
  int bl = roomd (level, b, LEFT);
  int br = roomd (level, b, RIGHT);

  int la = roomd (level, l, ABOVE);
  int ra = roomd (level, r, ABOVE);
  int lb = roomd (level, l, BELOW);
  int rb = roomd (level, r, BELOW);

  update_label (l_label, s, l, s, lr);
  update_label (r_label, s, r, s, rl);
  update_label (a_label, s, a, s, ab);
  update_label (b_label, s, b, s, ba);

  update_link (s_link, s);
  update_link (l_link, l);
  update_link (r_link, r);
  update_link (a_link, a);
  update_link (b_link, b);

  update_label (al_label, a, l, al, la);
  update_label (ar_label, a, r, ar, ra);
  update_label (bl_label, b, l, bl, lb);
  update_label (br_label, b, r, br, rb);

  update_link (al_link, al);
  update_link (ar_link, ar);
  update_link (bl_link, bl);
  update_link (br_link, br);

  update_label (la_label, l, a, la, al);
  update_label (ra_label, r, a, ra, ar);
  update_label (lb_label, l, b, lb, bl);
  update_label (rb_label, r, b, rb, br);

  update_link (la_link, la);
  update_link (ra_link, ra);
  update_link (lb_link, lb);
  update_link (rb_link, rb);

  return IUP_DEFAULT;
}

void
update_label (Ihandle *ih, int room0, int room1, int room2, int room3)
{
  if (! room0 || ! room1 || room2 == room3)
    gui_control_attribute (ih, "FGCOLOR", "0 0 0");
  else gui_control_attribute (ih, "FGCOLOR", "255 0 0");
}

void
update_link (Ihandle *ih, int room)
{
  gui_control_attribute_strf (ih, "TITLE", "%2i", room);
  gui_control_active (ih, room > 0);
}

int
action_cb (Ihandle *ih)
{
  int room = IupGetInt (ih, "TITLE");
  mr_focus_room (room);
  return IUP_DEFAULT;
}
