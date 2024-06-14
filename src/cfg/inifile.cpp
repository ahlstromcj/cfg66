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
 * \file          inifile.cpp
 *
 *  This module defines an INI-handling class using configfile functions.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2018-11-23
 * \updates       2024-06-14
 * \license       GNU GPLv2 or above
 *
 */

// #include <cctype>                       /* std::isspace(), std::isdigit()   */
// #include <iomanip>                      /* std::hex, std::setw()            */

// #include "cfg/appinfo.hpp"              /* informational functions          */
#include "cfg/inifile.hpp"              /* cfg::inifile class               */
// #include "util/filefunctions.hpp"       /* util::filename_base() etc.       */
// #include "util/msgfunctions.hpp"        /* util::error_message() etc.       */

namespace cfg
{

/**
 *  Provides the string constructor for a configuration file.
 *
 * \param name
 *      The name of the configuration file.
 *
 * \param fileext
 *      The file-extension for this type of configuration file.
 */

inifile::inifile
(
    inisections & sections,
    const std::string & name,
    const std::string & fileext
) :
    configfile      (name, fileext),
    m_ini_sections  (sections)
{
    // no code needed
}

/**
 *  Writes out all of the sections using configfile member functions.
 */

bool
inifile::write ()
{
    return false;   // TODO
}

}           // namespace cfg

/*
 * inifile.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

