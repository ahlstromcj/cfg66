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
 * \file          configfile.cpp
 *
 *  This module defines an INI-handling class using configfile functions.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2024-09-09
 * \updates       2024-09-28
 * \license       GNU GPLv2 or above
 *
 */

#include "cfg/appinfo.hpp"              /* cfg::<appinfo functions>         */
#include "session/configuration.hpp"    /* cfg::configuration class         */
#include "session/configfile.hpp"       /* session::configfile class        */
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

configfile::configfile
(
    configuration & parent,
    const std::string & filename,
    const std::string & cfgtype
) :
    cfg::configfile (filename, cfgtype),
    m_parent        (parent)
{
    // no code needed
}

/**
 *  Reads in all of the sections using configfile member functions.
 */

bool
configfile::parse ()
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

            if (result)
            {
                /*
                 * This code does not assume any ordering of the named
                 * sections from the list. It can't count on it, given that
                 * a human can edit the .session file.
                 */

                for (const auto & s : sects)
                {
                    directories::entry e = parse_dir_entry(file, s);
                    if (! e.ent_active)
                        break;
                }
            }


            // more to come
        }
    }
    return result;
}

/**
 *  Parses the given section (e.g. "[rc]") for the following variables:
 *
 *      -   active
 *      -   directory
 *      -   basename
 *      -   ext
 *
 * \param s
 *      Provides the name of the section, of the form "[xyz]".
 *
 * \return
 *      If all the values for the section were found, they are filled in
 *      to a session::directories::entry structure, and entry::ent_active
 *      is true.
 */

directories::entry
configfile::parse_dir_entry (std::ifstream & file, const std::string & s)
{
    directories::entry result;
    bool ok = section_name_valid(s);
    if (ok)
    {
        int filepos = position_of_section(file, s);
        bool flag = get_boolean(file, s, "active", filepos);
        result.ent_active = flag;

        std::string value = get_variable(file, s, "directory", filepos);
        if (value.empty())
        {
            util::error_message(s, "'directory' missing");
            result.ent_active = flag;
        }
        else
        {
            result.ent_directory = value;
            value = get_variable(file, s, "basename", filepos);
            if (! value.empty())
                result.ent_basename = value;

            value = get_variable(file, s, "ext", filepos);
            if (value.empty())
                result.ent_extension = value;
            else
                result.ent_extension = strip_section_name(s);
        }
    }
    if (ok)
        result.ent_active = false;
    else
        util::error_message("Section not found", s);

    return result;
}

/**
 *  Writes out all of the sections using configfile member functions.
 */

bool
configfile::write ()
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
         *  const inisection & cfg66sec = get_configfile_cfg66_section();
         *  write_section(file, cfg66sec);
         *  const inisection & commsec = get_configfile_comment_section();
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
 * configfile.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

