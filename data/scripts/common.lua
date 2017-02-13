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
local P = {type = "SCRIPT", name = "common"}

-- imports
local M = mininim

-- body
setfenv (1, P)

function invert_direction (direction)
   if direction == "LEFT" then return "RIGHT" else return "LEFT" end
end

function sample (filename)
   return M.audio.load (filename, "SAMPLE")
end

function stream (filename)
   return M.audio.load (filename, "STREAM")
end

-- end
return P
