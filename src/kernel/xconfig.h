/*
  xconfig.h -- configuration module;

  Copyright (C) 2015, 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

ALLEGRO_CONFIG *create_config (void);
ALLEGRO_CONFIG *load_config_file (const char *filename);
ALLEGRO_CONFIG *load_config_file_f (ALLEGRO_FILE *file);
void save_config_file (const char *filename,
                       const ALLEGRO_CONFIG *config);
void save_config_file_f (ALLEGRO_FILE *file,
                         const ALLEGRO_CONFIG *config);
ALLEGRO_CONFIG *
merge_config (const ALLEGRO_CONFIG *cfg1,
              const ALLEGRO_CONFIG *cfg2);

#endif	/* MININIM_XCONFIG_H */
