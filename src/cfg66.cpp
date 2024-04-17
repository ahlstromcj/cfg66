/*
 *  This file is part of cfg66.
 *
 *  cfg66 is free software; you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation; either version 2 of the License, or (at your option) any later
 *  version.
 *
 *  cfg66 is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with cfg66; if not, write to the Free Software Foundation, Inc., 59 Temple
 *  Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * \file          cfg66.cpp
 *
 *  This module defines an informational function.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2024-04-15
 * \updates       2024-04-15
 * \license       GNU GPLv2 or above
 *
 */

#include "cfg66.hpp"                   /* no-namespace function library    */

std::string
cfg66_version ()
{
    static std::string s_info = "cfg66-0.1 library 2024-04-15";
    return s_info;
}

/*
 * cfg66.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

