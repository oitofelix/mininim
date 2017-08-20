/*
  gui-editor.c -- graphical user interface editor module;

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

static int editor_hotkeys_cb (Ihandle *ih, int c);
static int move_selection_hotkeys_cb (Ihandle *ih, int c);

static Ihandle *editor_dialog (void);
static void update_editor_dialog (struct pos *p);

static Ihandle *tile_fg_control (void);
static int tile_fg_button_action_cb (Ihandle *ih);
static void update_tile_fg_control (struct pos *p);

static int change_tile_fg_from_button_cb (Ihandle *ih);
static int change_tile_fg_from_list_cb (Ihandle *ih);
static int change_tile_fg_from_spin_cb (Ihandle *ih);

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
editor_hotkeys_cb (Ihandle *ih, int c)
{
  Ihandle *selection_hotkeys_toggle =
    IupGetDialogChild (editor_dialog (), "SELECTION_HOTKEYS_TOGGLE");

  bool selection_hotkeys =
    IupGetInt (selection_hotkeys_toggle, "VALUE");

  Ihandle *game_hotkeys_toggle =
    IupGetDialogChild (editor_dialog (), "GAME_HOTKEYS_TOGGLE");

  bool game_hotkeys =
    IupGetInt (game_hotkeys_toggle, "VALUE");

  if (selection_hotkeys && move_selection_hotkeys_cb (ih, c) == IUP_IGNORE)
    return IUP_IGNORE;
  else if (game_hotkeys && animation_hotkeys_cb (NULL, c) == IUP_IGNORE)
    return IUP_IGNORE;
  else if (game_hotkeys && level_hotkeys_cb (NULL, c) == IUP_IGNORE)
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
     (IupVbox
      (IupSetAttributes
       (IupTabs
        (IupHbox
         (tile_fg_control (),
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
        "TABTITLE5 = Level"),
       IupSetAttributes
       (IupHbox
        (IupSetAttributes
         (IupFrame
          (IupVbox
           (IupSetAttributes
            (IupToggle ("&Selection", NULL),
             "NAME = SELECTION_HOTKEYS_TOGGLE,"
             "TIP = \"Arrow keys move the locked tile selection\","),
            IupSetAttributes
            (IupToggle ("&Game", NULL),
             "NAME = GAME_HOTKEYS_TOGGLE,"
             "TIP = \"Usual game key bindings work anywhere\""),
            NULL)),
          "TITLE = Hotkeys"),
         IupFill (),
         IupSetCallbacks
         (IupButton ("&Close", NULL),
          "ACTION", hide_dialog,
          NULL),
         NULL),
        "ALIGNMENT = ACENTER,"
        "PADDING = 16"),
       NULL)),
     "TITLE = \"MININIM: Editor\","
     "ICON = logo_icon,"
     "MARGIN = 5,"
     "GAP = 5");

  struct pos p; new_pos (&p, &global_level, room_view, 0, 0);
  update_editor_dialog (&p);

  dialog_fit_natural_size (ih);

  IupSetCallback (ih, "K_ANY", (Icallback) editor_hotkeys_cb);

  return ih;
}

void
update_editor_dialog (struct pos *p)
{
  update_tile_fg_control (p);
}



/*********************************************************************
 * tile_fg_control
 *********************************************************************/

Ihandle *
tile_fg_control (void)
{
  static Ihandle *ih;
  if (ih) return ih;

  Ihandle *list, *spin;

  ih = IupSetAttributes
    (IupFrame
     (IupSetAttributes
      (IupVbox
       (IupFill (),
        IupSetCallbacks
        (IupSetAttributes
         (IupButton (NULL, NULL),
          "NAME = TILE_FG_BUTTON,"
          "IMAGE = NOIMAGE"),
         "ACTION", tile_fg_button_action_cb,
         NULL),
        list = IupSetCallbacks
        (IupSetAttributes
         (IupList (NULL),
          "NAME = TILE_FG_LIST,"
          "DROPDOWN = YES"),
         "VALUECHANGED_CB", change_tile_fg_from_list_cb,
         NULL),
        spin = IupSetCallbacks
        (IupSetAttributes
         (IupText (NULL),
          "NAME = TILE_FG_SPIN,"
          "SPIN = YES,"
          "SPINWRAP = YES,"
          "MASK = /d/d"),
         "VALUECHANGED_CB", change_tile_fg_from_spin_cb,
         "KILLFOCUS_CB", gui_empty_value_to_0,
         NULL),
        IupFill (),
        NULL),
       "ALIGNMENT = ACENTER")),
     "TITLE = Foreground");

  for (int i = 0; tile_fg_str[i]; i++)
    IupSetAttributeId (list, "", i + 1, tile_fg_str[i]);
  IupSetInt (list, "VISIBLEITEMS", TILE_FGS);

  IupSetInt (spin, "SPINMAX", TILE_FGS - 1);

  return ih;
}

int
tile_fg_button_action_cb (Ihandle *ih)
{
  bool visible = IupGetInt (tile_fg_dialog (), "VISIBLE");
  if (visible) IupHide (tile_fg_dialog ());
  else IupShow (tile_fg_dialog ());
  return IUP_DEFAULT;
}

void
update_tile_fg_control (struct pos *p)
{
  static struct {
    char *video_mode;
    enum em em;
    enum hue hue;
    struct tile tile;
  } last;

  Ihandle *ih = tile_fg_control ();
  gui_control_active (ih, is_valid_pos (p));

  struct tile *t;
  if (is_valid_pos (p)) t = tile (p);
  else t = &last.tile;

  if (last.video_mode
      && ! strcmp (video_mode, last.video_mode)
      && last.em == em
      && last.hue == hue
      && (fg_val (last.tile.fg) == fg_val (t->fg))
      && (ext_val (last.tile.fg, last.tile.ext) ==
          ext_val (t->fg, t->ext)
          || ! carpet_cs (fg_val (last.tile.fg)))) return;
  else {
    set_string_var (&last.video_mode, video_mode);
    last.em = em;
    last.hue = hue;
    last.tile = *t;
  }

  ALLEGRO_BITMAP *b = get_tile_fg_bitmap (t, 2);
  Ihandle *button = IupGetDialogChild (ih, "TILE_FG_BUTTON");
  set_button_bitmap (button, b);
  al_destroy_bitmap (b);

  Ihandle *list = IupGetDialogChild (ih, "TILE_FG_LIST");
  IupSetInt (list, "VALUE", fg_val (t->fg) + 1);

  Ihandle *spin = IupGetDialogChild (ih, "TILE_FG_SPIN");
  IupSetInt (spin, "SPINVALUE", fg_val (t->fg));
}


/*********************************************************************
 * change_tile_fg family
 *********************************************************************/

int
change_tile_fg_from_button_cb (Ihandle *ih)
{
  if (! is_valid_pos (&selection_pos)) return IUP_DEFAULT;
  char *name = IupGetAttribute (ih, "NAME");
  enum tile_fg f = str2enum (tile_fg_str, name);
  change_tile_fg (&selection_pos, f);
  return IUP_DEFAULT;
}

int
change_tile_fg_from_list_cb (Ihandle *ih)
{
  if (! is_valid_pos (&selection_pos)) return IUP_DEFAULT;
  enum tile_fg f = IupGetInt (ih, "VALUE") - 1;
  change_tile_fg (&selection_pos, f);
  return IUP_DEFAULT;
}

int
change_tile_fg_from_spin_cb (Ihandle *ih)
{
  if (! is_valid_pos (&selection_pos)) return IUP_DEFAULT;
  enum tile_fg f = fg_val (IupGetInt (ih, "SPINVALUE"));
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

  static Ihandle *vbox[TILE_FGS + 1];
  for (int i = 0; tile_fg_str [i]; i++) {
    Ihandle *button = IupButton (NULL, NULL);
    IupSetAttribute (button, "NAME", tile_fg_str[i]);
    Ihandle *label = IupLabel (tile_fg_str[i]);
    vbox[i] = IupVbox (button, label, NULL);
  }

  ih = IupSetAttributes
    (IupDialog
     (IupSetAttributes
      (IupVbox
       (IupSetAttributes (IupFill (), "RASTERSIZE = 0x10"),
        IupSetAttributes
        (IupGridBoxv (vbox),
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
     "TITLE = \"MININIM: Foreground\","
     "ICON = logo_icon");

  struct pos p; new_pos (&p, &global_level, room_view, 0, 0);
  update_tile_fg_dialog (&p);

  dialog_fit_natural_size (ih);

  IupSetCallback (ih, "K_ANY", (Icallback) editor_hotkeys_cb);

  for (int i = 0; tile_fg_str [i]; i++) {
    Ihandle *button =
      IupGetDialogChild (ih, tile_fg_str [i]);

    /* Set TILE_FG */
    IupSetAttribute (button, "TILE_FG", tile_fg_str [i]);

    /* make the button black so its selection is white (at least in
       Motif) */
    IupSetAttribute (button, "BGCOLOR", "0 0 0");

    /* centralize label */
    Ihandle *vbox = IupGetParent (button);
    IupSetAttribute (vbox, "ALIGNMENT", "ACENTER");

    /* set callback */
    IupSetCallback (button, "ACTION", change_tile_fg_from_button_cb);
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

  Ihandle *grid = IupGetDialogChild (tile_fg_dialog (), "GRID");
  gui_control_active (grid, is_valid_pos (p));

  if (last.video_mode
      && ! strcmp (video_mode, last.video_mode)
      && last.em == em
      && last.hue == hue) return;
  else {
    set_string_var (&last.video_mode, video_mode);
    last.em = em;
    last.hue = hue;
  }

  for (int i = 0; tile_fg_str [i]; i++) {
    Ihandle *button =
      IupGetDialogChild (tile_fg_dialog (), tile_fg_str [i]);

    /* update image */
    struct tile t = {.fg = i, .ext = 0};
    ALLEGRO_BITMAP *b = get_tile_fg_bitmap (&t, 1);
    set_button_bitmap (button, b);
    al_destroy_bitmap (b);
  }
}
