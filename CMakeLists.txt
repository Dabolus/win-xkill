 ########################################################################
 # Copyright © 2017 Giorgio Garasto.                                    #
 #                                                                      #
 # This file is part of win-xkill                                       #
 #                                                                      #
 # win-xkill is free software: you can redistribute it and/or modify    #
 # it under the terms of the GNU General Public License as published by #
 # the Free Software Foundation, either version 3 of the License, or    #
 # (at your option) any later version.                                  #
 #                                                                      #
 # win-xkill is distributed in the hope that it will be useful,         #
 # but WITHOUT ANY WARRANTY; without even the implied warranty of       #
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        #
 # GNU General Public License for more details.                         #
 #                                                                      #
 # You should have received a copy of the GNU General Public License    #
 # along with win-xkill. If not, see <http://www.gnu.org/licenses/>.    #
 ########################################################################

cmake_minimum_required(VERSION 3.8)
project(win-xkill)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES win-xkill.c cursor.h)
add_executable(xkill ${SOURCE_FILES})
