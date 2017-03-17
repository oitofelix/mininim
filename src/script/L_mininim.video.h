/*
  L_mininim.video.h -- mininim.video script module;

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

#ifndef MININIM_L_MININIM_VIDEO_H
#define MININIM_L_MININIM_VIDEO_H

#define L_MININIM_VIDEO L_MININIM ".video"

extern int REAL_WIDTH;
extern int REAL_HEIGHT;

void define_L_mininim_video (lua_State *L);

void video (ALLEGRO_BITMAP *bitmap, int nret, const char *command,
            const char *object, uintptr_t index, uintptr_t part,
            struct pos *p, int actor_id, int width);
struct coord *_coord_object_index_part (struct coord *c_ret,
                                        const char *object,
                                        uintptr_t index, uintptr_t part,
                                        struct pos *p, int actor_id);
ALLEGRO_BITMAP *_bitmap_object_index_part (const char *object,
                                           uintptr_t index, uintptr_t part);
bool is_valid_video_mode (char *vm);
char *next_video_mode (char *current_vm);
void setup_video_mode (char *requested_vm);

#define draw_object(bitmap, object, p)            \
  video (bitmap, 0, "DRAW", object, 0, 0, p, -1, -1)

#define draw_object_width(bitmap, object, p, width) \
  video (bitmap, 0, "DRAW", object, 0, 0, p, -1, width)

#define draw_object_part(bitmap, object, part, p)               \
  video (bitmap, 0, "DRAW", object, 0, (uintptr_t) (part), p, -1, -1)

#define draw_object_part_width(bitmap, object, part, p, width)      \
  video (bitmap, 0, "DRAW", object, 0, (uintptr_t) (part), p, -1, width)

#define draw_object_index_part(bitmap, object, index, part, p)          \
  video (bitmap, 0, "DRAW", object, (uintptr_t) (index),                \
         (uintptr_t) (part), p, -1, -1)

#define draw_object_index_part_width(bitmap, object, index, part,       \
  p, width)                                                             \
  video (bitmap, 0, "DRAW", object, (uintptr_t) (index),                \
         (uintptr_t) (part), p, -1, width)

#define coord_object(c_ret, object, p)              \
  _coord_object_index_part (c_ret, object, 0, 0, p, -1)

#define coord_object_part(c_ret, object, part, p)                 \
  _coord_object_index_part (c_ret, object, 0, (uintptr_t) (part), p, -1)

#define coord_object_index_part(c_ret, object, index, part, p)       \
  _coord_object_index_part (c_ret, object, (uintptr_t) (index),      \
                            (uintptr_t) (part), p, -1)

#define bitmap_object(object)                   \
  _bitmap_object_index_part (object, 0, 0)

#define bitmap_object_index(object, index)                \
  _bitmap_object_index_part (object, (uintptr_t) (index), 0)

#define bitmap_object_part(object, part)                  \
  _bitmap_object_index_part (object, 0, (uintptr_t) (part))

#define bitmap_object_index_part(object, index, part)            \
  _bitmap_object_index_part (object, (uintptr_t) (index), \
                             (uintptr_t) (part))

#endif	/* MININIM_L_MININIM_VIDEO_H */
