--[[
   common.lua -- common script module;

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
local P = {package_type = "LIBRARY", package_name = "common",
           package_file = debug.getinfo (1, "S").short_src}

-- imports
local string = string
local M = mininim

-- body
setfenv (1, P)

function base_directory (filename)
   return string.gsub (filename, "[/\][^/\]*$", "")
end

function resource_filename (P, filename)
   return base_directory (P.package_file) .. "/" .. filename
end

function load_bitmap (P, filename)
   return M.video.bitmap (resource_filename (P, filename))
end

function load_sample (P, filename)
   return M.audio.source (resource_filename (P, filename), "SAMPLE")
end

function load_stream (P, filename)
   return M.audio.source (resource_filename (P, filename), "STREAM")
end

function opposite_direction (direction)
   if direction == "LEFT" then return "RIGHT" else return "LEFT" end
end

-- end
return P
