--[[
   ega.lua -- EGA video mode;

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
--]]

-- header
local P = {package_type = "VIDEO MODE", package_name = "EGA",
           package_file = debug.getinfo (1, "S").short_src}

-- imports
local M = mininim
local common = require "script/common"
local to_color_range = common.to_color_range
local color = M.video.color

local function b (filename)
   return common.load_bitmap (P, filename)
end

-- body
setfenv (1, P)

local ASSET = {}

local function selection_palette (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r + 64, c.g + 64, c.b + 64))
end

function M.video.EGA (object, part, index, position)
   local em = M.video.env_mode
   local b, x, y

   b = ASSET[object][em][part][index].bitmap
   if (M.mouse.position == position) then
      b = b.apply_palette (selection_palette) end
   x, y = ASSET[object][em][part][index].coord (position)

   return b, x, y
end

function load ()
   local W = M.place_width
   local H = M.place_height

   -- TORCH
   local function torch_coord (p)
      return W * (p.place + 1), H * p.floor + 23
   end
   ASSET.TORCH = {DUNGEON = {BASE = {{bitmap = b "torch/dungeon/00.png",
                                      coord = torch_coord}}},
                  PALACE = {BASE = {{bitmap = b "torch/palace/00.png",
                                     coord = torch_coord}}}}

   return P
end

-- end
return P
