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
           package_file = MININIM.debugger.src}

-- imports
local MININIM = MININIM
local string = string
local loadstring = loadstring
local assert = assert
local table = table
local type = type
local pairs = pairs
local tostring = tostring
local io = io
local math = math
local os = os
local debug = debug
local _debug = _debug
local print = print
local getmetatable = getmetatable
local setmetatable = setmetatable
local unpack = unpack
local rawget = rawget
local rawset = rawset
local ipairs = ipairs
local unpack = table.unpack

-- body
local _ENV = P

function merge_recursively (metatable, o)
   for k, v in pairs (metatable) do
      local t = rawget (o, k)
      if type (v) == "table"
         and (t == nil or type (t) == "table")
         and not string.find (k, "^_")
      then
         t = t or {}
         rawset (o, k, t)
         if not rawget (t, "_parent") then
            rawset (t, "_parent", o) end
         new (v, t)
      elseif type (v) ~= "table"
         and o[k] == nil
         and getmetatable (o) ~= metatable
         and not string.find (k, "^_")
      then rawset (o, k, v) end
   end

   local meta_metatable = getmetatable (metatable)
   if meta_metatable
      and getmetatable (o) ~= metatable
   then return merge_recursively (meta_metatable, o)
   else return o end
end

function new (metatable, o, ...)
   o = o or {}
   o.__index = o

   if metatable then
      local original_metatable = getmetatable (o)

      if not original_metatable then
         if not rawget (metatable, __index) then
            metatable.__index = metatable end
         setmetatable (o, metatable)
      end

      merge_recursively (metatable, o)

      if metatable._new and not original_metatable then
         metatable:_new (o, ...)
      end
   end

   return o
end

function instanceof (o, metatable)
   local m = getmetatable (o)
   if m == metatable then return true
   elseif not m then return false
   else return instanceof (m, metatable)
   end
end

function meta (metatable, instance)
   if instanceof (instance, metatable)
   then return instance else return metatable end
end

-- Resources

function base_directory (filename)
   return string.gsub (filename, "[/\\][^/\\]*$", "")
end

function resource_filename (P, filename, ...)
   return base_directory (P.package_file) .. "/"
      .. string.format (filename, ...)
end

function load_shader (P, ...)
   local arg = {...}
   for i, filename in ipairs (arg) do
      arg[i] = base_directory (P.package_file) .. "/" .. arg[i]
   end
   return MININIM.video.shader (unpack (arg))
end

function load_bitmap (P, filename, ...)
   return MININIM.video.bitmap (
      resource_filename (P, filename, ...))
end

function load_font (P, filename, ...)
   return MININIM.video.font (
      resource_filename (P, filename, ...))
end

function load_sample (P, filename, ...)
   return MININIM.audio.source (
      resource_filename (P, filename, ...), "SAMPLE")
end

function load_stream (P, filename, ...)
   return MININIM.audio.source (
      resource_filename (P, filename, ...), "STREAM")
end

function eval_clipboard ()
   assert (loadstring (MININIM.clipboard)) ()
end

function print_table (tt, indent, done)
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
        print_table (value, indent + 7, done)
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

function palette_table_color (t, c)
   for k, v in pairs (t) do
      if k == c then return v end
   end
   return c
end

function palette_table_to_shader (t)
   if t == nil then return end
   local palette_table_in, palette_table_out = {}, {}
   for k, v in pairs (t) do
      local i = {k.r / 255, k.g / 255, k.b / 255, k.a / 255}
      local o = {v.r / 255, v.g / 255, v.b / 255, v.a / 255}
      table.insert (palette_table_in, i)
      table.insert (palette_table_out, o)
   end
   if #palette_table_in < 64
   then table.insert (palette_table_in, {-1, -1, -1, -1})
   end
   local p = string.lower (MININIM.video.shader_platform)
   local shader = load_shader (P, "default."..p..".vert",
                               "palette-table."..p..".frag")
   if shader then
      shader.palette_table_in = palette_table_in
      shader.palette_table_out = palette_table_out
   end
   return shader
end

function opposite_direction (direction)
   if direction == "LEFT" then return "RIGHT" else return "LEFT" end
end

function safenav (t, k, ...)
   if t == nil or k == nil then
      return t
   else
      return safenav (t[k], ...)
   end
end

-- end
return P
