--[[
   vga.lua -- VGA video mode;

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
local P = {package_type = "VIDEO MODE", package_name = "VGA",
           package_file = debug.getinfo (1, "S").short_src}

-- imports
local M = mininim
local common = require "script/common"
local to_color_range = common.to_color_range
local color = M.video.color

local pairs = pairs
local print = print

local function b (filename)
   return common.load_bitmap (P, filename)
end

-- body
setfenv (1, P)

local ASSET = {}
local HUE = {}
local SELECTION = {}

function HUE.GREEN (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r - 12, c.g + 16, c.b - 29))
end

function HUE.GRAY (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r + 13, c.g - 7, c.b - 52))
end

function HUE.YELLOW (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r + 78, c.g + 25, c.b - 64))
end

function HUE.BLUE (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r - 96, c.g - 80, c.b + 64))
end

function SELECTION.DUNGEON (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r + 64, c.g + 64, c.b + 64))
end

function SELECTION.PALACE (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r + 32, c.g + 32, c.b + 32))
end

function M.video.VGA (object, part, index, position)
   local em = M.video.env_mode
   local hue = M.video.hue_mode
   local b, x, y

   b = ASSET[object][em][part][index].bitmap
   if (hue ~= "NONE") then b = b.apply_palette (HUE[hue]) end
   if (M.mouse.position == position) then
      b = b.apply_palette (SELECTION[em]) end
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
