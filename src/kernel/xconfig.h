/*
  xconfig.h -- configuration module;

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

#ifndef MININIM_XCONFIG_H
#define MININIM_XCONFIG_H

void config_entry_file_type (const char *key, const char *value,
                             enum file_type *file_type);
char * file_type2str (enum file_type file_type);
bool remove_config_entry (ALLEGRO_CONFIG *config, char *section, char *key);
bool remove_config_section (ALLEGRO_CONFIG *config, char *section);

#endif	/* MININIM_XCONFIG_H */
