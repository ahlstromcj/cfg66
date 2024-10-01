#if ! defined CFG66_SESSION_LAYOUT_HPP
#define CFG66_SESSION_LAYOUT_HPP

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
 * \file          layout.hpp
 *
 *  This module declares an INI-handling class using cfg::configfile
 *  functions.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2024-09-29
 * \updates       2024-10-01
 * \license       GNU GPLv2 or above
 *
 *  A session layout is a structure that can be used to set up an initial
 *  configuration for a session.
 *
 *  Hmmmm, verbose and quiet:
 *
 *      -   msgfunctions
 *      -   parser
 *      -   options (add quiet!!!)
 */

#include "session/configuration.hpp"    /* configuration & directories objs */

namespace session
{

/**
 *  Wrapper class.
 */

class layout
{

public:

    /**
     *  Mirrors the "[Cfg66]" section of the .session file, including a few
     *  options supported by other modules. Note that "config-type" and
     *  "version" are values describing the configfiles, and are not
     *  use-specifiable (except by manually editing the file.)
     */

    using sessioncfg66 = struct
    {
        bool scfg_auto_option_save;
        bool scfg_auto_data_save;
        std::string scfg_home;
        bool scfg_quiet;
        bool scfg_verbose;
    };

    using session_configuration = struct
    {
        sessioncfg66 sc_cfg66_main;
        std::string sc_comments;                    /* "[comments]" section */
        int sc_section_count;                       /* size of "[cfg]"      */
        configuration::sections sc_section_list;    /* lib66::tokenization  */
        int sc_subdir_count;                        /* size of "[data]"     */
        configuration::subdirectories sc_subdirs;   /* lib66::tokenization  */
        directories::entries sc_entry_list;         /* vector of entry      */
    };

private:

    session_configuration m_full_layout;

public:

    layout () = default;
    layout (const session_configuration & lay);

    void clear ();

    // TODO: add various sub-setters

};

}           // namespace session

#endif      // CFG66_SESSION_LAYOUT_HPP

/*
 * layout.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

