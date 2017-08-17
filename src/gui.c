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

void
init_gui (int argc, char **argv)
{
  /* Initialize IUP */
  IupOpen(&argc, &argv);
  Ihandle *logo_icon_image =
    bitmap_to_iup_image (logo_icon, TRANSPARENT_COLOR);
  IupSetHandle ("logo_icon", logo_icon_image);
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
bitmap_to_iup_image (ALLEGRO_BITMAP *b, ALLEGRO_COLOR bg)
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
      if (color_eq (c, TRANSPARENT_COLOR)) c = bg;
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
set_button_bitmap (Ihandle *ih, ALLEGRO_BITMAP *b)
{
  Ihandle *old_image = IupGetAttributeHandle (ih, "IMAGE");
  Ihandle *new_image = bitmap_to_iup_image (b, BLACK);
  IupSetAttributeHandle (ih, "IMAGE", new_image);
  IupSetAttribute (ih, "RASTERSIZE", NULL);
  /* ATTENTION: the old bitmap is destroyed! */
  IupDestroy (old_image);
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
