/*
  gui.h -- GUI module;

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

#ifndef MININIM_GUI_H
#define MININIM_GUI_H

/* functions */
void init_gui (int argc, char **argv);
bool load_led (const char *filename);
Ihandle *bitmap_to_iup_image (ALLEGRO_BITMAP *b, ALLEGRO_COLOR bg);
void set_button_bitmap (Ihandle *ih, ALLEGRO_BITMAP *b);
void dialog_fit_natural_size (Ihandle *ih);
int hide_dialog (Ihandle *ih);
void gui_control_active (Ihandle *ih, bool a);
int gui_default_key_cb (Ihandle *ih, int c);
int gui_empty_value_to_0 (Ihandle *ih);

#endif	/* MININIM_GUI_H */
