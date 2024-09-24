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
 * \file          cfgfile.cpp
 *
 *  This module defines an INI-handling class using configfile functions.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2024-09-09
 * \updates       2024-09-ww
 * \license       GNU GPLv2 or above
 *
 */

#include "cfg/appinfo.hpp"              /* cfg::<appinfo functions>         */
#include "session/configuration.hpp"    /* cfg::configuration class         */
#include "session/cfgfile.hpp"          /* session::cfgfile class           */
#include "util/msgfunctions.hpp"        /* util::msgfunctions module        */

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

cfgfile::cfgfile
(
    configuration & parent,
    const std::string & filename,
    const std::string & cfgtype
) :
    configfile  (filename, cfgtype),
    m_parent    (parent)
{
    // no code needed
}

/**
 *  Reads in all of the sections using configfile member functions.
 */

bool
cfgfile::parse ()
{
    std::ifstream file(file_name(), std::ios::in | std::ios::ate);
    bool result = set_up_ifstream(file);
    if (result)
    {
        /*
         *  Get the version immediately and use it to evaluate the
         *  configuration file. It is loaded into the section in
         *  the for-loop below.
         */

        util::file_message("Parse", file_name());
        std::string section = cfg::get_main_cfg_section_name();
        std::string s = parse_version(file);
        std::string cfgtype = get_variable(file, section, "config-type");
        bool correct = cfgtype == "session";
        if (correct && (s.empty() || file_version_old(file)))
        {
            bool b = get_boolean(file, section, "auto-option-save");
            parent().auto_option_save(b);
            b = get_boolean(file, section, "auto-save");
            parent().auto_save(b);
            b = get_boolean(file, section, "quiet");
            parent().quiet(b);
            b = get_boolean(file, section, "verbose");
            parent().verbose(b);

            std::string h = get_variable(file, section, "home");
            parent().home(h);

            std::string c = parse_comments(file);
            parent().comments_block().set(c);

            lib66::tokenization sects;
            int count = parse_list(file, "[cfg]", sects, "section");
            if (count > 0)
                result = parent().section_list_fill(sects);
            else
                result = false;     // fatal or okay?

            // more to come
        }
    }
    return result;
}

/**
 *  Writes out all of the sections using configfile member functions.
 */

bool
cfgfile::write ()
{
    std::ofstream file(file_name(), std::ios::out | std::ios::trunc);
    bool result = file.is_open();
    if (result)
    {
        /*
         * Stock INI file header.
         */

        util::file_message("Write", file_name());
        write_date(file);

        /*
         * Stock [Cfg66] and [comments] sections.
         * It's too problematic to have these built in.
         *
         *  const inisection & cfg66sec = get_cfgfile_cfg66_section();
         *  write_section(file, cfg66sec);
         *  const inisection & commsec = get_cfgfile_comment_section();
         *  write_section(file, commsec);
         */

        /*
         * Write the rest of the sections. These are different from the
         * standard Cfg66 options.
         */

        // TODO

        /*
         * Write the stock INI file footer.
         */

        write_cfg66_footer(file);
    }
    else
    {
        util::file_error("Write open fail", file_name());
        result = false;
    }
    return result;
}

}           // namespace session

/*
 * cfgfile.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

