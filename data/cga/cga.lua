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
           package_file = mininim.debugger.src}

-- imports
local M = mininim
local common = require "script/common"
local to_color_range = common.to_color_range
local color = M.video.color
local coord = M.video.coordinate
local _debug = _debug




-- body
setfenv (1, P)

local PLACE_WIDTH = 32
local PLACE_HEIGHT = 63

local asset = {}
local hgc_selection = {}

local load_bitmap
local cga_selection_palette
local hgc_palette
local apply_palettes
local draw

function load_bitmap (filename)
   return common.load_bitmap (P, filename)
end

function cga_selection_palette (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r - 128, c.g -128, c.b - 128))
end

function hgc_selection.DUNGEON (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r - 96, c.g - 96, c.b - 96))
end

function hgc_selection.PALACE (c)
   if c.a == 0 then return c end
   return color (to_color_range (c.r + 64, c.g + 64, c.b + 64))
end

function hgc_palette (c)
   if c == color (85, 255, 255) then return color (170, 170, 170)
   elseif c == color (255, 85, 255) then return color (85, 85, 85)
   else return c
   end
end

function apply_palettes (b, p)
   local vm = M.setting.video_mode
   local em = M.video.env_mode

   if (vm == "HGC") then b = b.apply_palette (hgc_palette) end

   if p == M.mouse.position then
      local palette
      if vm == "CGA" then palette = cga_selection_palette
      elseif vm == "HGC" then palette = hgc_selection[em]
      end
      b = b.apply_palette (palette)
   end

   return b
end

function draw (b, c, p, x, y, w, h)
   b = apply_palettes (b, p)
   b.draw (c (p), x, y, w, h)
end




-- TORCH
local draw_torch
local torch_coord

function draw_torch (p)
   local em = M.video.env_mode, b
   draw (asset[em].torch, torch_coord, p)
end

function torch_coord (p)
   return coord (PLACE_WIDTH * (p.place + 1), PLACE_HEIGHT * p.floor + 23)
end



-- WALL
local draw_wall
local wall_left_coord
local wall_right_coord
local draw_wall_base
local draw_wall_left
local draw_wall_right
local draw_wall_top
local wall_top_coord

function wall_base_coord (p)
   return coord (PLACE_WIDTH * p.place, PLACE_HEIGHT * (p.floor + 1))
end

function wall_left_coord (p)
   return coord (PLACE_WIDTH * p.place, PLACE_HEIGHT * p.floor + 3)
end

function wall_right_coord (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor + 3)
end

function wall_top_coord (p)
   return coord (PLACE_WIDTH * (p.place + 1),
                 PLACE_HEIGHT * p.floor - 10)
end

function draw_wall (part, p, w)
   local t
      = {BASE = draw_wall_base,
         LEFT = draw_wall_left,
         RIGHT = draw_wall_right,
         TOP = draw_wall_top}
   t[part] (p, w)
end

function draw_wall_base (p)
   local em = M.video.env_mode
   draw (asset[em].wall_base, wall_base_coord, p)
end

function draw_wall_left (p)
   local em = M.video.env_mode
   draw (asset[em].wall_left, wall_left_coord, p)
end

function draw_wall_right (p, w)
   local em = M.video.env_mode
   local wc = p.wall_correlation
   if wc == "SWS" or wc == "WWS" then
      draw (asset[em].wall_right, wall_right_coord, p, 0, 0, w)
   end
end

function draw_wall_top (p, w)
   local em = M.video.env_mode
   local wc = p.wall_correlation
   if wc == "SWS" or wc == "WWS" then
      draw (asset[em].wall_top, wall_top_coord, p, 0, 0, w)
   end
end




-- CGA video mode interface
function M.video.CGA (...)
   if arg[1] == "DRAW" then
      if arg[2] == "TORCH" then draw_torch (arg[3])
      elseif arg[2] == "WALL" then draw_wall (arg[3], arg[4], arg[5])
      end
   end
end




-- Load assets
function load ()

   M.video.HGC = M.video.CGA

   asset.DUNGEON = {}
   asset.PALACE = {}

   -- TORCH
   asset.DUNGEON.torch = load_bitmap ("torch/dungeon.png")
   asset.PALACE.torch = load_bitmap ("torch/palace.png")

   -- WALL (DUNGEON)
   asset.DUNGEON.wall_base = load_bitmap ("wall/dungeon/base.png")
   asset.DUNGEON.wall_left = load_bitmap ("wall/dungeon/left.png")
   asset.DUNGEON.wall_right = load_bitmap ("wall/dungeon/right.png")
   asset.DUNGEON.wall_top = load_bitmap ("wall/dungeon/top.png")

   -- WALL (PALACE)
   asset.PALACE.wall_base = load_bitmap ("wall/palace/base.png")
   asset.PALACE.wall_left = load_bitmap ("wall/palace/left.png")
   asset.PALACE.wall_right = load_bitmap ("wall/palace/right.png")
   asset.PALACE.wall_top = load_bitmap ("wall/palace/top.png")

   return P
end

-- end
return P
