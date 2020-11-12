/*
  dialog.h -- dialog module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

#ifndef MININIM_DIALOG_H
#define MININIM_DIALOG_H

/* functions */
void init_dialog (void);
void finalize_dialog (void);
void print_text_log (char const *template, ...)
  __attribute__ ((format (gnu_printf, 1, 2)));
void close_text_log (ALLEGRO_EVENT *event);
bool file_overwrite_dialog (char *filename);

/* variables */
extern ALLEGRO_TEXTLOG *text_log;

#endif	/* MININIM_DIALOG_H */
