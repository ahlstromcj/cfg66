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
 * \file          layout.cpp
 *
 *  This module defines an INI-handling class using layout functions.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2024-09-30
 * \updates       2024-10-01
 * \license       GNU GPLv2 or above
 *
 */

#include "session/layout.hpp"           /* session::layout class            */

namespace session
{

/**
 *  Provides the string constructor for a configuration file.
 *
 * \param filename
 *      The name of the configuration file.
 *
 * \param cfgtype
 *      The file-extension for this type of configuration file.
 */

layout::layout (const session_configuration & lay) :
    m_full_layout   (lay)
{
    // no code needed
}

/**
 *  Reads in all of the sections using layout member functions.
 */

void
layout::clear ()
{
    m_full_layout.sc_cfg66_main.scfg_auto_option_save = false;
    m_full_layout.sc_cfg66_main.scfg_auto_data_save = false;
    m_full_layout.sc_cfg66_main.scfg_home = "$home";
    m_full_layout.sc_cfg66_main.scfg_quiet = false;
    m_full_layout.sc_cfg66_main.scfg_verbose = false;
    m_full_layout.sc_comments.clear();
    m_full_layout.sc_section_count = 0;
    m_full_layout.sc_section_list.clear();
    m_full_layout.sc_subdir_count = 0;
    m_full_layout.sc_subdirs.clear();

    /*
     * This next one is a vector of structs without a count. The count
     * is sc_section_count.
     */

    m_full_layout.sc_entry_list.clear();
}

}           // namespace session

/*
 * layout.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

