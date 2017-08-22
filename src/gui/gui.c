/*
  gui.c -- GUI module;

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

Ihandle *gui_editor_dialog;

void
init_gui (int argc, char **argv)
{
  /* Initialize IUP */
  IupOpen (&argc, &argv);

  IupSetGlobal ("AUTOREPEAT", "YES");

  Ihandle *logo_icon_image = bitmap_to_iup_image (logo_icon, NULL);
  IupSetHandle ("LOGO_ICON", logo_icon_image);

  gui_editor_dialog = gui_create_editor_dialog ();
}

void
finalize_gui (void)
{
  IupDestroy (gui_editor_dialog);

  IupExitLoop ();
  IupFlush ();
  IupClose ();
}

const char *load_led_error_msg;

intptr_t
load_led_sub (const char *filename)
{
  if (! al_filename_exists (filename)) return false;
  const char *msg = IupLoad (filename);
  if (msg) {
    load_led_error_msg = msg;
    return false;
  }
  return true;
}

bool
load_led (const char *filename)
{
  load_led_error_msg = NULL;
  bool success = load_resource (filename, (load_resource_f) load_led_sub,
                                true);
  if (success) return true;
  if (load_led_error_msg) error (0, 0, "%s", load_led_error_msg);
  else error (0, 0, "LED file \"%s\" not found", filename);
  return false;
}

Ihandle *
bitmap_to_iup_image (ALLEGRO_BITMAP *b, palette p)
{
  if (! b) return NULL;

  int x, y;
  int w = get_bitmap_width (b);
  int h = get_bitmap_height (b);
  int s = al_get_pixel_size (ALLEGRO_PIXEL_FORMAT_RGBA_8888);
  unsigned char *bs = xcalloc (w * h, s);

  al_lock_bitmap (b, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
  for (y = 0; y < h; y++)
    for (x = 0; x < w; x++) {
      ALLEGRO_COLOR c = al_get_pixel (b, x, y);
      if (p) c = p (c);
      unsigned char r, g, b, a;
      al_unmap_rgba (c, &r, &g, &b, &a);
      int pos = (y * w + x) * s;
      bs[pos + 0] = r;
      bs[pos + 1] = g;
      bs[pos + 2] = b;
      bs[pos + 3] = a;
    }
  al_unlock_bitmap (b);

  Ihandle *iup_image = IupImageRGBA (w, h, bs);
  al_free (bs);
  return iup_image;
}

void
gui_set_image (Ihandle *ih, ALLEGRO_BITMAP *b, palette p)
{
  Ihandle *old_image = IupGetAttributeHandle (ih, "IMAGE");
  Ihandle *new_image = bitmap_to_iup_image (b, p);
  IupSetAttributeHandle (ih, "IMAGE", new_image);
  IupSetAttribute (ih, "RASTERSIZE", NULL);
  /* ATTENTION: the old bitmap is destroyed! */
  IupDestroy (old_image);
}

ALLEGRO_COLOR
transp_to_black (ALLEGRO_COLOR c)
{
  if (color_eq (c, TRANSPARENT_COLOR)) return BLACK;
  else return c;
}

ALLEGRO_COLOR
white_to_transp (ALLEGRO_COLOR c)
{
  if (color_eq (c, WHITE)) return TRANSPARENT_COLOR;
  else return c;
}

void
dialog_fit_natural_size (Ihandle *ih)
{
  IupSetAttribute (ih, "RASTERSIZE", NULL);
  IupRefresh (ih);
  const char *ns = IupGetAttribute (ih, "NATURALSIZE");
  IupSetAtt (NULL, ih, "MINSIZE", ns, "MAXSIZE", ns, NULL);
#if WINDOWS_PORT
  IupSetAttribute (ih, "MAXBOX", "NO");
#endif
}

int
hide_dialog (Ihandle *ih)
{
  IupHide (IupGetDialog (ih));
  return IUP_DEFAULT;
}

/* Update active status.  This function based on the method of first
   assessing the current control state is necessary because (specially
   for Windows) the IUP driver may insist in redrawing the control
   even when there is no change in state, resulting in random
   graphical glitches if done recurrently. */
void
gui_control_active (Ihandle *ih, bool a)
{
  /* Only consider changing the active status of the control if its
     already mapped, otherwise a bug in Motif might cause the
     application to hang up. */
  if (! IupGetAttribute (ih, "WID")) return;

  bool b = IupGetInt (ih, "ACTIVE");
  if (! equiv (a, b)) IupSetInt (ih, "ACTIVE", a);
}

int
gui_default_key_cb (Ihandle *ih, int c)
{
  return IUP_DEFAULT;
}

int
gui_debug_print_key_cb (Ihandle *ih, int c)
{
  fprintf (stderr, "Key pressed: %x\n", c);
  return IUP_CONTINUE;
}

int
gui_empty_value_to_0 (Ihandle *ih)
{
  char *v = IupGetAttribute (ih, "VALUE");
  if (! strlen (v)) IupSetAttribute (ih, "VALUE", "0");
  return IUP_DEFAULT;
}

int
gui_run_callback_IFn (const char *name, Ihandle *ih)
{
  if (! ih) return IUP_DEFAULT;
  IFn cb = (IFn) IupGetCallback (ih, name);
  if (cb) return cb (ih);
  else return IUP_DEFAULT;
}

int
gui_run_callback_IFni (const char *name, Ihandle *ih, int i)
{
  if (! ih) return IUP_DEFAULT;
  IFni cb = (IFni) IupGetCallback (ih, name);
  if (cb) return cb (ih, i);
  else return IUP_DEFAULT;
}
