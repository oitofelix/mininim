/*
  file.c -- file module;

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

#include "mininim.h"

struct load_resource
{
  const char *filename;
  load_resource_f lrf;
  bool success;
  intptr_t resource;
};

char *current_resource_filename;

bool
do_for_each_data_dir (bool (*f) (void *args, const char *dir),
		      void *args)
{
  const char *dirs[] =
    {
      user_data_dir,
      data_dir,
      ".",
      resources_dir,
      system_data_dir
    };
  const int dirs_count = sizeof (dirs) / sizeof (*dirs);
  for (const char **dir = dirs;
       dir < dirs + dirs_count;
       dir++)
    {
      if (! *dir) continue;
      if (f (args, *dir))
	return true;
    }
  return false;
}

bool
load_resource_from_dir (void *data,
			const char *basedir)
{
  struct load_resource *lr = data;
  lr->resource = ! lr->success;
  ALLEGRO_PATH *path = al_create_path_for_directory (basedir);
  assert (path);
  al_set_path_filename (path, lr->filename);
  const char *path_cstr = al_path_cstr (path, ALLEGRO_NATIVE_PATH_SEP);
  /* Allegro thinks that trying to load a nonexistent file is worth a
     warning. */
  if (al_filename_exists (path_cstr))
    {
      if (load_callback)
	{
	  char *dir = dir_name (lr->filename);
	  assert (dir);
	  dir = base_name (dir);
	  assert (dir);
	  if (! current_resource_filename
	      || strcmp (current_resource_filename, dir))
	    {
	      free (current_resource_filename);
	      current_resource_filename = dir;
	      show ();
	    }
	  else free (dir);
	}
      lr->resource = lr->lrf (path_cstr);
    }
  al_destroy_path (path);
  return equiv (lr->resource, lr->success);
}

intptr_t
load_resource (const char *filename, load_resource_f lrf, bool success)
{
  struct load_resource lr =
    {
      .filename = filename,
      .lrf = lrf,
      .success = success,
      .resource = ! success
    };
  if (do_for_each_data_dir (load_resource_from_dir,
			    &lr))
    return lr.resource;
  else
    return ! success;
}

ALLEGRO_FILE *
xfopen_r (char *filename)
{
  return al_fopen (filename, "rb");
}

int8_t *
load_file (char *filename)
{
  ALLEGRO_FILE *f = xfopen_r (filename);
  if (! f) return NULL;
  int64_t s = al_fsize (f);
  if (s == -1) return NULL;
  int8_t *mf = xmalloc (s);
  al_fread (f, mf, s);
  al_fclose (f);
  return mf;
}
