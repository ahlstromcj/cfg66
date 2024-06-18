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
 * \updates       2024-06-17
 * \license       GNU GPLv2 or above
 *
 *  session::configuration contains and manages the data in the 'session'
 *  configuration file.
 */

#include <memory>                       /* std::shared_ptr<>, unique_ptr<>  */
#include <string>                       /* std::string                      */

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
 *          -   "verbose"
 *          -   "use-log-file"
 *      -   The [comments] section.
 *      -   The [auto-option-save] section
 *      -   Modified status and error messages.
 */

class configuration : public cfg::basesettings
{
    friend class manager;

public:

    /**
     *  Provides a unique pointer to a configuration.
     */

    using pointer = std::unique_ptr<configuration>;

private:

    /**
     *  Provides the file/directory data needed by the session.
     *  But the directories class already provides one of these!!! TODO
     *
     *      directories::entries m_file_entries;
     */

    /**
     *  Provides basic session directory management.
     */

    directories m_dir_manager;

    /**
     *  If true, more information is emitted (to the console).
     */

    bool m_verbose;

    /**
     *  If true, reroute console outpu the a log file.
     */

    bool m_use_log_file;

    /**
     *  [auto-option-save]
     *
     *  If true (the default), all modified configuration files are saved.
     */

    bool m_auto_option_save;

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
    configuration (configuration &&) = delete;
    configuration (const configuration &) = default;
    configuration & operator = (configuration &&) = delete;
    configuration & operator = (const configuration &) = default;
    virtual ~configuration ();

public:

    directories & dir_manager ()
    {
        return m_dir_manager;
    }

    const directories & dir_manager () const
    {
        return m_dir_manager;
    }

    directories::entries & file_entries ()
    {
        return dir_manager().file_entries();
    }

    const directories::entries & file_entries () const
    {
        return dir_manager().file_entries();
    }

/*
    const std::string & home_directory () const
    {
        return m_dirmanager.xxxxxxxx();
    }
*/

    bool verbose () const
    {
        return m_verbose;
    }

    bool use_log_file () const
    {
        return m_use_log_file;
    }

    bool auto_option_save () const
    {
        return m_auto_option_save;
    }

protected:

    void verbose (bool flag)
    {
        m_verbose = flag;
    }

    void use_log_file (bool flag)
    {
        m_use_log_file = flag;
    }

    void auto_option_save (bool flag)
    {
        m_auto_option_save = flag;
    }

};          // class configuration

}           // namespace session

#endif      // CFG66_SESSION_CONFIGURATION_HPP

/*
 * configuration.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

