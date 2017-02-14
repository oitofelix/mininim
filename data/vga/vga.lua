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

local function b (filename)
   return common.load_bitmap (P, filename)
end

-- body
setfenv (1, P)

function load ()
   local W = M.place_width
   local H = M.place_height

   local V = {}

   -- TORCH
   local function torch_coord (f, p)
      return W * (p + 1), H * f + 23
   end
   V.TORCH = {DUNGEON = {{bitmap = b "torch/dungeon/00.png",
                          coord = torch_coord}},
              PALACE = {{bitmap = b "torch/palace/00.png",
                         coord = torch_coord}}}

   M.video[package_name] = V
   return P
end

-- end
return P
