#if ! defined CFG66_SESSION_CONFIGURATION_HPP
#define CFG66_SESSION_CONFIGURATION_HPP

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
 * \file          configuration.hpp
 *
 *  This module declares/defines the base class for handling many facets
 *  of administering a session of cfg66 usage.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2020-05-30
 * \updates       2024-09-28
 * \license       GNU GPLv2 or above
 *
 *  session::configuration contains and manages the data in a 'session'
 *  configuration file. See extras/notes/cfg66.session for an example.
 */

#include <memory>                       /* std::shared_ptr<>, unique_ptr<>  */
#include <string>                       /* std::string                      */
#include <vector>                       /* std::vector                      */

#include "cfg/basesettings.hpp"         /* cfg::basesettings class          */
#include "session/directories.hpp"      /* session::directories             */

/*
 *  Do not document a namespace; it breaks Doxygen.
 */

namespace session
{

/**
 *  Provides the session configuration.  This replaces many of the variables
 *  in the old Seq66 rcsettings class, though eventually these will be
 *  stored in the 'rc' in a completely compatible manner.
 *
 *  The basesettings class supports the following items in the ".session"
 *  file:
 *
 *      -   The [cfg66] section except for the following handled here:
 *          -   "use-log-file"
 *      -   The [comments] section.
 *      -   The [auto-option-save] section
 *      -   Modified status and error messages.
 */

class configuration : public cfg::basesettings
{
    friend class configfile;
    friend class manager;

public:

    /**
     *  Provides a unique pointer to a configuration.
     */

    using pointer = std::unique_ptr<configuration>;

    /**
     *  Provides a list of section names found in the session file.
     */

    using sections = lib66::tokenization;   /* std::vector<std::string> */

    /**
     *  Provides a list of sub-directories to be created in the $home
     *  directory. These are in addition to any sub-directories specified
     *  for [session], [log], etc.
     */

    using subdirectories = lib66::tokenization;

private:

    /**
     *  Provides the file/directory data needed by the session.
     *  Provides basic session directory management. This example shows
     *  the format:
     *
     *      [rc]
     *      active = true
     *      directory = "cfg"   // or empty ""
     *      basename = ""
     *      ext = ".rc"
     */

    directories m_dir_manager;

    /**
     *  Provides a list of sections to be process. Each section refers to
     *  a group of variables to indicate active status, sub-directory,
     *  base-name of the configuration file, and the configuration-file's
     *  extension.
     */

    sections m_section_list;

    /**
     *  Holds the list of sub-directories to be created. They are optional.
     */

    subdirectories m_data_directories;

    /**
     *  [Cfg66] : auto-option-save
     *
     *  If true (the default), all modified configuration files are saved.
     */

    bool m_auto_option_save;

    /**
     *  [Cfg66] : auto-save
     *
     *  If true (the default), all modified data files are saved.
     */

    bool m_auto_save;

    /**
     *  [Cfg66] : quiet
     *
     *  If true, bypass startup error prompts.
     */

    bool m_quiet;

    /**
     *  [Cfg66] : verbose
     *
     *  If true, show additional status data. Does not negate quiet.
     */

    bool m_verbose;

    /**
     *  [Cfg66] : home
     *
     *  Provides the base configuration and data directory for the application.
     *  If empty, then the system default (e.g. "~/.config/appname") is
     *  used. Another value is "$home", which also default to that.
     */

    std::string m_home;

    /**
     *  If true, reroute console output to a log file. Note that
     *  the log file can also be set via cli::parser, which should take
     *  precedence.
     */

    bool m_use_log_file;
    std::string m_log_file;

public:

    configuration ();
    configuration
    (
        directories & fileentries,
        const std::string & cfgname,
        const std::string & comtext = "",
        int version = 0,
        bool uselogfile = false
    );
    configuration (const configuration &) = default;
    configuration (configuration &&) = delete;
    configuration & operator = (const configuration &) = default;
    configuration & operator = (configuration &&) = delete;
    virtual ~configuration ();

public:

    const directories & dir_manager () const
    {
        return m_dir_manager;
    }

    const directories::entries & file_entries () const
    {
        return dir_manager().file_entries();
    }

    bool add_dir_entry (const directories::entry & ent)
    {
        return dir_manager().add_entry(ent);
    }

    bool add_dir_entry
    (
        bool active,
        const std::string & subdir,
        const std::string & basename,
        const std::string & ext
    )
    {
        return dir_manager().add_entry(active, subdir, basename, ext);
    }

    const sections & section_list () const
    {
        return m_section_list;
    }

    bool section_list_fill (const lib66::tokenization & sects);

    const subdirectories & data_directories () const
    {
        return m_data_directories;
    }

    bool use_log_file () const
    {
        return m_use_log_file;
    }

    const std::string log_file () const
    {
        return m_log_file;
    }

    bool auto_option_save () const
    {
        return m_auto_option_save;
    }

    bool auto_save () const
    {
        return m_auto_save;
    }

    bool quiet () const
    {
        return m_quiet;
    }

    bool verbose () const
    {
        return m_verbose;
    }

    const std::string & home () const
    {
        return m_home;
    }

private:

    directories & dir_manager ()
    {
        return m_dir_manager;
    }

    directories::entries & file_entries ()
    {
        return dir_manager().file_entries();
    }

    sections & section_list ()
    {
        return m_section_list;
    }

    subdirectories & data_directories ()
    {
        return m_data_directories;
    }

    void use_log_file (bool flag)
    {
        m_use_log_file = flag;
    }

    void log_file (const std::string & filename)
    {
        m_log_file = filename;
    }

    void auto_option_save (bool flag)
    {
        m_auto_option_save = flag;
    }

    void auto_save (bool flag)
    {
        m_auto_save = flag;
    }

    void quiet (bool flag)
    {
        m_quiet = flag;
    }

    void verbose (bool flag)
    {
        m_verbose = flag;
    }

    void home (const std::string & h)
    {
        m_home = h;
    }


};          // class configuration

}           // namespace session

#endif      // CFG66_SESSION_CONFIGURATION_HPP

/*
 * configuration.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

