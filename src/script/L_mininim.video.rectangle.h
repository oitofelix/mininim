/*
  L_mininim.video.rectangle.h -- mininim.video.rectangle script module;

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

#ifndef MININIM_L_MININIM_VIDEO_RECTANGLE_H
#define MININIM_L_MININIM_VIDEO_RECTANGLE_H

#define L_MININIM_VIDEO_RECTANGLE L_MININIM_VIDEO ".rectangle"

void define_L_mininim_video_rectangle (lua_State *L);
void L_pushrectangle (lua_State *L, struct rect *p);

DECLARE_LUA (L_mininim_video_rectangle);

#endif	/* MININIM_L_MININIM_VIDEO_RECTANGLE_H */
