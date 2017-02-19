--[[
   cga.lua -- CGA and HGC video modes;

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
local P = {package_type = "VIDEO MODE", package_name = "CGA",
           package_file = debug.getinfo (1, "S").short_src}

-- imports
local M = mininim
local common = require "script/common"
local to_color_range = common.to_color_range
local color = M.video.color
local coordinate = M.video.coordinate
local setmetatable = setmetatable

local function b (filename)
   return common.load_bitmap (P, filename)
end

-- body
setfenv (1, P)

local ASSET = {}
local HGC_SELECTION = {}

local function cga_selection_palette (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r - 128, c.g -128, c.b - 128))
end

function HGC_SELECTION.DUNGEON (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r - 96, c.g - 96, c.b - 96))
end

function HGC_SELECTION.PALACE (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r + 64, c.g + 64, c.b + 64))
end

local function hgc_palette (c)
   if c == color (85, 255, 255) then return color (170, 170, 170)
   elseif c == color (255, 85, 255) then return color (85, 85, 85)
   else return c
   end
end

function M.video.CGA (object, part, index, position)
   local vm = M.setting.video_mode
   local em = M.video.env_mode
   local b, x, y

   b = ASSET[object][em][part][index].bitmap

   if (vm == "HGC") then b = b.apply_palette (hgc_palette) end

   if (M.mouse.position == position) then
      local palette
      if vm == "CGA" then palette = cga_selection_palette
      elseif vm == "HGC" then palette = HGC_SELECTION[em]
      end
      b = b.apply_palette (palette)
   end
   c = ASSET[object][em][part][index].coordinate (position)

   b.draw (c)
end

function load ()
   local W = M.place_width
   local H = M.place_height

   M.video.HGC = M.video.CGA

   -- TORCH
   local function torch_coordinate (p)
      return coordinate (W * (p.place + 1), H * p.floor + 23)
   end
   ASSET.TORCH = {DUNGEON = {BASE = {{bitmap = b "torch/dungeon/00.png",
                                      coordinate = torch_coordinate}}},
                  PALACE = {BASE = {{bitmap = b "torch/palace/00.png",
                                     coordinate = torch_coordinate}}}}

   return P
end

-- end
return P
