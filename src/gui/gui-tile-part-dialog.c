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

static int show_cb (Ihandle *ih, int state);
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

  Ihandle *buttons[tile_parts + 1];
  memset (&buttons, 0, sizeof (buttons));

  for (int i = 0; i < tile_parts; i++) {
    Ihandle *button = IupButton (NULL, NULL);

    if (tile_part_str)
      IupSetAttribute (button, "NAME", tile_part_str[i]);
    else IupSetStrf (button, "NAME", "%i", i);

    IupSetAttribute (button, "BGCOLOR", "0 0 0");

    IupSetCallbacks (button, "ACTION", change_tile_part_from_button_cb,
                     "DESTROY_CB", gui_destroy_image_cb, NULL);

    if (tile_part_str) IupSetAttribute (button, "TIP", tile_part_str[i]);
    else IupSetStrf (button, "TIP", "%i", i);

    buttons[i] = button;
  }

  Ihandle *grid;

  Ihandle *ih = IupSetCallbacks
    (IupSetAttributes
     (IupDialog
      (IupSetAttributes
       (IupVbox
        (IupSetAttributes (IupFill (), "MINSIZE = 0x10"),
         IupSetAttributes
         (grid = IupGridBoxv (buttons),
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
          (IupButton ("&Close", NULL),
           "PADDING = 16,"),
          "ACTION", hide_dialog, NULL),
         IupSetAttributes (IupFill (), "MINSIZE = 0x10"),
         NULL),
        "ALIGNMENT = ACENTER,")),
      "ICON = LOGO_ICON"),
     "SHOW_CB", (Icallback) show_cb,
     "DESTROY_CB", destroy_cb,
     "K_ANY", k_any,
     "RESIZE_CB", gui_resize_cb,
     "_UPDATE_CB", _update_cb,
     NULL);

  IupSetInt (grid, "NUMDIV", tile_parts / (int) sqrt (tile_parts));

  IupSetStrf (ih, "TITLE", "MININIM: Tile %s",
              IupGetAttribute (parent, "TITLE"));

  IupSetAttribute (ih, "_PARENT", (void *) parent);

  struct last *last = xmalloc (sizeof (*last));
  memset (last, 0, sizeof (*last));
  IupSetAttribute (ih, "_LAST", (void *) last);

  IupSetAttribute (ih, "_GRID", (void *) grid);

  update (ih);

  dialog_fit_natural_size (ih);

  return ih;
}

int
show_cb (Ihandle *ih, int state)
{
  if (state == IUP_SHOW) IupSetInt (ih, "_WAS_VISIBLE", false);
  return IUP_DEFAULT;
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

  Ihandle *grid = (void *) IupGetAttribute (ih, "_GRID");

  gui_control_active (grid, is_valid_pos (p));

  /* focus button */
  if (! IupGetInt (ih, "_WAS_VISIBLE")) {
    IupSetInt (ih, "_WAS_VISIBLE", true);

    enum tile_part tile_part = IupGetInt (parent, "_TILE_PART");
    char **tile_part_str = (void *) IupGetAttribute (parent, "_TILE_PART_STR");

    int i;
    switch (tile_part) {
    case TILE_FG: i = fg (p); break;
    case TILE_BG: i = bg (p); break;
    case TILE_EXT_ITEM:
    case TILE_EXT_STEP_SPIKES_FLOOR:
    case TILE_EXT_STEP_CHOMPER:
    case TILE_EXT_DESIGN: i = ext (p); break;
    case TILE_FAKE: i = fake (p); break;
    default: assert (false);
    }

    char *name;
    if (tile_part_str) name = xasprintf ("%s", tile_part_str [i]);
    else name = xasprintf ("%i", i);
    Ihandle *button = IupGetDialogChild (ih, name);
    IupSetFocus (button);
    al_free (name);
  }

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
  int tile_parts = IupGetInt (parent, "_TILE_PARTS");

  /* update images */
  for (int i = 0; i < tile_parts; i++) {
    Ihandle *button;

    if (tile_part_str)
      button = IupGetDialogChild (ih, tile_part_str [i]);
    else {
      char *name = xasprintf ("%i", i);
      button = IupGetDialogChild (ih, name);
      al_free (name);
    }

    struct tile t;
    memset (&t, 0, sizeof (t));

    switch (tile_part) {
    case TILE_FG: t.fg = i; break;
    case TILE_BG: t.bg = i; break;
    case TILE_EXT_ITEM:
    case TILE_EXT_STEP_SPIKES_FLOOR:
    case TILE_EXT_STEP_CHOMPER:
    case TILE_EXT_DESIGN: t.ext = i; break;
    case TILE_FAKE: t.fake = i; break;
    default: assert (false); return;
    }

    ALLEGRO_BITMAP *b = get_tile_bitmap (&t, NULL, 1, tile_part);
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

  int i;
  if (tile_part_str) {
    char *name = IupGetAttribute (ih, "NAME");
    i = str2enum (tile_part_str, name);
  } else i = IupGetInt (ih, "NAME");

  struct tile t;
  t = *tile (p);
  change_tile_part (p, i);
  if (tile_eq (p->l, tile (p), &t)) IupHide (IupGetDialog (ih));

  return IUP_DEFAULT;
}
