/*
  editor-dialog.c -- editor dialog module;

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

static int editor_gui_hotkeys_cb (Ihandle *ih, int c);
static int move_selection_hotkeys_cb (Ihandle *ih, int c);

static Ihandle *editor_dialog (void);
static void update_editor_dialog (struct pos *p);

static Ihandle *tile_fg_button (void);
static int tile_fg_button_cb (Ihandle *ih);
static void update_tile_fg_button (struct pos *p);

static int change_tile_fg_cb (Ihandle *ih);

static Ihandle *tile_fg_dialog (void);
void update_tile_fg_dialog (struct pos *p);


/*********************************************************************
 * external interface
 *********************************************************************/

void
show_editor_gui (void)
{
  IupShow (editor_dialog ());
  bool was_visible = IupGetInt (tile_fg_dialog (), "WAS_VISIBLE");
  IupSetInt (tile_fg_dialog (), "VISIBLE", was_visible);
}

void
hide_editor_gui (void)
{
  IupHide (editor_dialog ());
  bool was_visible = IupGetInt (tile_fg_dialog (), "VISIBLE");
  IupSetInt (tile_fg_dialog (), "WAS_VISIBLE", was_visible);
  IupHide (tile_fg_dialog ());
}

void
update_editor_gui (void)
{
  if (IupGetInt (editor_dialog (), "VISIBLE"))
    update_editor_dialog (&selection_pos);
  if (IupGetInt (tile_fg_dialog (), "VISIBLE"))
    update_tile_fg_dialog (&selection_pos);
}



/*********************************************************************
 * hotkeys
 *********************************************************************/

int
editor_gui_hotkeys_cb (Ihandle *ih, int c)
{
  if (move_selection_hotkeys_cb (ih, c) == IUP_IGNORE)
    return IUP_IGNORE;
  else return IUP_CONTINUE;
}

int
move_selection_hotkeys_cb (Ihandle *ih, int c)
{
  if (! selection_locked) return IUP_CONTINUE;

  struct pos p;

  switch (c) {
  case K_UP:
    prel (&selection_pos, &p, -1, +0);
    break;
  case K_LEFT:
    prel (&selection_pos, &p, +0, -1);
    break;
  case K_RIGHT:
    prel (&selection_pos, &p, +0, +1);
    break;
  case K_DOWN:
    prel (&selection_pos, &p, +1, +0);
    break;
  default:
    return IUP_CONTINUE;
  }

  npos (&p, &p);

  if (p.room) {
    selection_pos = p;
    mr_focus_room (p.room);
  }

  return IUP_IGNORE;
}




/*********************************************************************
 * editor_dialog
 *********************************************************************/

Ihandle *
editor_dialog (void)
{
  static Ihandle *ih;
  if (ih) return ih;

  ih = IupSetAttributes
    (IupDialog
     (IupSetAttributes
      (IupTabs
       (IupVbox
        (IupFrame
         (IupHbox
          (tile_fg_button (),
           IupGridBox
           (IupLabel ("Name:"),
            IupLabel ("<name>"),
            NULL),
           NULL)),
         IupFrame (NULL),
         IupFrame (NULL),
         NULL),
        IupVbox (NULL),
        IupVbox (NULL),
        IupVbox (NULL),
        IupVbox (NULL),
        IupVbox (NULL),
        NULL),
       "TABTITLE0 = Tile,"
       "TABTITLE1 = Event,"
       "TABTITLE2 = Room,"
       "TABTITLE3 = Kid,"
       "TABTITLE4 = Guard,"
       "TABTITLE5 = Level")),
     "TITLE = \"MININIM: Editor\","
     "ICON = logo_icon");

  struct pos p; new_pos (&p, &global_level, room_view, 0, 0);
  update_editor_dialog (&p);

  dialog_fit_natural_size (ih);

  IupSetCallback (ih, "K_ANY", (Icallback) editor_gui_hotkeys_cb);

  return ih;
}

void
update_editor_dialog (struct pos *p)
{
  update_tile_fg_button (p);

  /* update tile_fg_name label */
  /* .... */
}



/*********************************************************************
 * tile_fg_button
 *********************************************************************/

Ihandle *
tile_fg_button (void)
{
  static Ihandle *ih;
  if (ih) return ih;
  return ih = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = NOIMAGE"),
     "ACTION", tile_fg_button_cb,
     NULL);
}

int
tile_fg_button_cb (Ihandle *ih)
{
  bool visible = IupGetInt (tile_fg_dialog (), "VISIBLE");
  if (visible) IupHide (tile_fg_dialog ());
  else IupShow (tile_fg_dialog ());
  return IUP_DEFAULT;
}

void
update_tile_fg_button (struct pos *p)
{
  static struct {
    char *video_mode;
    enum em em;
    enum hue hue;
    struct tile tile;
  } last;

  IupSetAttribute
    (tile_fg_button (), "ACTIVE", is_valid_pos (p) ? "YES" : "NO");

  struct tile *t;
  bool valid_pos = is_valid_pos (p);
  if (valid_pos) t = tile (p);

  if (last.video_mode
      && ! strcmp (video_mode, last.video_mode)
      && last.em == em
      && last.hue == hue
      && (! valid_pos || last.tile.fg == t->fg)
      && (! valid_pos || last.tile.ext == t->ext
          || ! carpet_cs (last.tile.fg))) return;
  else {
    set_string_var (&last.video_mode, video_mode);
    last.em = em;
    last.hue = hue;
    if (valid_pos) last.tile = *t;
  }

  ALLEGRO_BITMAP *b = get_tile_fg_bitmap (valid_pos ? t : &last.tile, 2);
  set_button_bitmap (tile_fg_button (), b);
  al_destroy_bitmap (b);
}


/*********************************************************************
 * change_tile_fg_cb
 *********************************************************************/

int
change_tile_fg_cb (Ihandle *ih)
{
  if (! is_valid_pos (&selection_pos)) return IUP_DEFAULT;
  char *name = IupGetAttribute (ih, "NAME");
  enum tile_fg f = str2enum (tile_fg_str, name);
  change_tile_fg (&selection_pos, f);
  return IUP_DEFAULT;
}



/*********************************************************************
 * tile_fg_dialog
 *********************************************************************/

Ihandle *tile_fg_image[TILE_FGS];

Ihandle *
tile_fg_dialog (void)
{
  static Ihandle *ih;
  if (ih) return ih;
  ih = IupSetAttributes
    (IupDialog
     (IupSetAttributes
      (IupVbox
       (IupSetAttributes (IupFill (), "RASTERSIZE = 0x5"),
        IupSetAttributes
        (IupGridBox
         (IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = NO_FLOOR"),
           IupLabel ("NO_FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = FLOOR"),
           IupLabel ("FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = BROKEN_FLOOR"),
           IupLabel ("BROKEN_FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = SKELETON_FLOOR"),
           IupLabel ("SKELETON_FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = LOOSE_FLOOR"),
           IupLabel ("LOOSE_FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = SPIKES_FLOOR"),
           IupLabel ("SPIKES_FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = OPENER_FLOOR"),
           IupLabel ("OPENER_FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = CLOSER_FLOOR"),
           IupLabel ("CLOSER_FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = STUCK_FLOOR"),
           IupLabel ("STUCK_FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = HIDDEN_FLOOR"),
           IupLabel ("HIDDEN_FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = PILLAR"),
           IupLabel ("PILLAR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = BIG_PILLAR_TOP"),
           IupLabel ("BIG_PILLAR_TOP"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = BIG_PILLAR_BOTTOM"),
           IupLabel ("BIG_PILLAR_BOTTOM"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = ARCH_BOTTOM"),
           IupLabel ("ARCH_BOTTOM"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = LEVEL_DOOR"),
           IupLabel ("LEVEL_DOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = DOOR"),
           IupLabel ("DOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = CARPET"),
           IupLabel ("CARPET"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = TCARPET"),
           IupLabel ("TCARPET"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = CHOMPER"),
           IupLabel ("CHOMPER"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = MIRROR"),
           IupLabel ("MIRROR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = WALL"),
           IupLabel ("WALL"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = ARCH_TOP_MID"),
           IupLabel ("ARCH_TOP_MID"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = ARCH_TOP_SMALL"),
           IupLabel ("ARCH_TOP_SMALL"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = ARCH_TOP_LEFT"),
           IupLabel ("ARCH_TOP_LEFT"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = ARCH_TOP_RIGHT"),
           IupLabel ("ARCH_TOP_RIGHT"),
           NULL),
          NULL),
         "NAME = GRID,"
         "ORIENTATION = HORIZONTAL,"
         "NUMDIV = 5,"
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
     "TITLE = \"MININIM: Editor/Foreground\","
     "ICON = logo_icon");

  struct pos p; new_pos (&p, &global_level, room_view, 0, 0);
  update_tile_fg_dialog (&p);

  dialog_fit_natural_size (ih);

  IupSetCallback (ih, "K_ANY", (Icallback) editor_gui_hotkeys_cb);

  for (int i = 0; i < TILE_FGS; i++) {
    Ihandle *button =
      IupGetDialogChild (ih, tile_fg_str [i]);

    /* make the button black so its selection is white (at least in
       Motif) */
    IupSetAttribute (button, "BGCOLOR", "0 0 0");

    /* centralize label */
    Ihandle *vbox = IupGetParent (button);
    IupSetAttribute (vbox, "ALIGNMENT", "ACENTER");

    /* set callback */
    IupSetCallback (button, "ACTION", change_tile_fg_cb);
  }

  return ih;
}

void
update_tile_fg_dialog (struct pos *p)
{
  static struct {
    char *video_mode;
    enum em em;
    enum hue hue;
  } last;

  /* update active status */
  Ihandle *grid = IupGetDialogChild (tile_fg_dialog (), "GRID");
  IupSetAttribute (grid, "ACTIVE", is_valid_pos (p) ? "YES" : "NO");

  if (last.video_mode
      && ! strcmp (video_mode, last.video_mode)
      && last.em == em
      && last.hue == hue) return;
  else {
    set_string_var (&last.video_mode, video_mode);
    last.em = em;
    last.hue = hue;
  }

  for (int i = 0; i < TILE_FGS; i++) {
    Ihandle *button =
      IupGetDialogChild (tile_fg_dialog (), tile_fg_str [i]);

    /* update image */
    struct tile t = {.fg = i, .ext = 0};
    ALLEGRO_BITMAP *b = get_tile_fg_bitmap (&t, 1);
    set_button_bitmap (button, b);
    al_destroy_bitmap (b);
  }
}
