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
local loadstring = loadstring
local assert = assert
local table = table
local type = type
local pairs = pairs
local tostring = tostring
local io = io
local math = math

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

function eval_clipboard ()
   assert (loadstring (M.clipboard)) ()
end

function table_print (tt, indent, done)
  done = done or {}
  indent = indent or 0
  if type(tt) == "table" then
    for key, value in pairs (tt) do
      io.write(string.rep (" ", indent)) -- indent it
      if type (value) == "table" and not done [value] then
        done [value] = true
        io.write(string.format("[%s] => table\n", tostring (key)));
        io.write(string.rep (" ", indent+4)) -- indent it
        io.write("(\n");
        table_print (value, indent + 7, done)
        io.write(string.rep (" ", indent+4)) -- indent it
        io.write(")\n");
      else
        io.write(string.format("[%s] => %s\n",
            tostring (key), tostring(value)))
      end
    end
  else
    io.write(tt .. "\n")
  end
end

function to_color_range (r, g, b)
   r = math.max (0, r)
   r = math.min (r, 255)
   g = math.max (0, g)
   g = math.min (g, 255)
   b = math.max (0, b)
   b = math.min (b, 255)
   return r, g, b
end

function opposite_direction (direction)
   if direction == "LEFT" then return "RIGHT" else return "LEFT" end
end

-- end
return P
