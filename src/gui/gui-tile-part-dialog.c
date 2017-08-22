/*
  gui-tile-part-dialog.c -- editor GUI tile part dialog module;

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

struct last {
  char *video_mode;
  enum em em;
  enum hue hue;
};

static int destroy_cb (Ihandle *ih);
static int k_any (Ihandle *ih, int c);
static int _update_cb (Ihandle *ih);
static void update (Ihandle *ih);
static int change_tile_part_from_button_cb (Ihandle *ih);


Ihandle *
gui_create_tile_part_dialog (Ihandle *parent)
{
  int tile_parts = IupGetInt (parent, "_TILE_PARTS");
  char **tile_part_str = (void *) IupGetAttribute (parent, "_TILE_PART_STR");

  Ihandle *vbox[tile_parts + 1];
  memset (&vbox, 0, sizeof (vbox));

  for (int i = 0; tile_part_str [i]; i++) {
    Ihandle *button = IupButton (NULL, NULL);
    IupSetAtt (NULL, button, "NAME", tile_part_str[i],
               "BGCOLOR", "0 0 0", NULL);
    IupSetCallback (button, "ACTION", change_tile_part_from_button_cb);
    Ihandle *label = IupLabel (tile_part_str[i]);
    vbox[i] = IupVbox (button, label, NULL);
    IupSetAttribute (vbox[i], "ALIGNMENT", "ACENTER");
  }

  Ihandle *grid;

  Ihandle *ih = IupSetCallbacks
    (IupSetAttributes
     (IupDialog
      (IupSetAttributes
       (IupVbox
        (IupSetAttributes (IupFill (), "RASTERSIZE = 0x10"),
         IupSetAttributes
         (grid = IupGridBoxv (vbox),
          "NAME = GRID,"
          "ORIENTATION = HORIZONTAL,"
          "SIZECOL = -1,"
          "SIZELIN = -1,"
          "NORMALIZESIZE = BOTH,"
          "NMARGIN = 5,"
          "NGAPCOL = 5,"
          "NGAPLIN = 5"),
         IupSetAttributes (IupFill (), "RASTERSIZE = 0x15"),
         IupSetCallbacks
         (IupSetAttributes
          (IupButton ("&Close", NULL), "PADDING = 32"),
          "ACTION", hide_dialog, NULL),
         IupSetAttributes (IupFill (), "RASTERSIZE = 0x10"),
         NULL),
        "ALIGNMENT = ACENTER")),
      "ICON = LOGO_ICON"),
     "DESTROY_CB", destroy_cb,
     "K_ANY", k_any,
     "_UPDATE_CB", _update_cb,
     NULL);

  IupSetInt (grid, "NUMDIV", sqrt (tile_parts));

  IupSetStrf (ih, "TITLE", "MININIM: Tile %s",
              IupGetAttribute (parent, "TITLE"));

  IupSetAttribute (ih, "_PARENT", (void *) parent);

  struct last *last = xmalloc (sizeof (*last));
  memset (last, 0, sizeof (*last));
  IupSetAttribute (ih, "_LAST", (void *) last);

  update (ih);

  dialog_fit_natural_size (ih);

  return ih;
}

int
destroy_cb (Ihandle *ih)
{
  Ihandle *parent = (void *) IupGetAttribute (ih, "_PARENT");
  IupSetAttribute (parent, "_TILE_PART_DIALOG", NULL);
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");
  al_free (last);
  return IUP_DEFAULT;
}

int
k_any (Ihandle *ih, int c)
{
  Ihandle *parent = (void *) IupGetAttribute (ih, "_PARENT");
  return gui_run_callback_IFni ("K_ANY", IupGetDialog (parent), c);
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  Ihandle *parent = (void *) IupGetAttribute (ih, "_PARENT");
  struct pos *p = (void *) IupGetAttribute (parent, "_POS");

  Ihandle *grid = IupGetDialogChild (ih, "GRID");

  gui_control_active (grid, is_valid_pos (p));

  struct last *last = (void *) IupGetAttribute (ih, "_LAST");

  if (last->video_mode
      && ! strcmp (video_mode, last->video_mode)
      && last->em == em
      && last->hue == hue) return IUP_DEFAULT;
  else update (ih);

  return IUP_DEFAULT;
}

void
update (Ihandle *ih)
{
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");

  set_string_var (&last->video_mode, video_mode);
  last->em = em;
  last->hue = hue;

  Ihandle *parent = (void *) IupGetAttribute (ih, "_PARENT");
  enum tile_part tile_part = IupGetInt (parent, "_TILE_PART");
  char **tile_part_str = (void *) IupGetAttribute (parent, "_TILE_PART_STR");

  /* update images */
  for (int i = 0; tile_part_str [i]; i++) {
    Ihandle *button = IupGetDialogChild (ih, tile_part_str [i]);

    struct tile t;

    switch (tile_part) {
    case TILE_FG: t.fg = i; break;
    case TILE_BG: t.bg = i; break;
    default: assert (false); return;
    }

    t.ext = 0;
    ALLEGRO_BITMAP *b = get_tile_bitmap (&t, 1, tile_part);
    gui_set_image (button, b, transp_to_black);
    al_destroy_bitmap (b);
  }
}

int
change_tile_part_from_button_cb (Ihandle *ih)
{
  Ihandle *parent = (void *) IupGetAttribute (ih, "_PARENT");
  struct pos *p = (void *) IupGetAttribute (parent, "_POS");

  if (! is_valid_pos (p)) return IUP_DEFAULT;

  char **tile_part_str =
    (void *) IupGetAttribute (parent, "_TILE_PART_STR");
  change_tile_part_t change_tile_part = (void *)
    IupGetAttribute (parent, "_CHANGE_TILE_PART");

  char *name = IupGetAttribute (ih, "NAME");
  int i = str2enum (tile_part_str, name);
  change_tile_part (p, i);

  return IUP_DEFAULT;
}
