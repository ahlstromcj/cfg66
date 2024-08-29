#if ! defined CFG66_SESSION_MANAGER_HPP
#define CFG66_SESSION_MANAGER_HPP

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
 * \file          manager.hpp
 *
 *  This module declares/defines the base class for handling many facets
 *  of administering a session of cfg66 usage.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2020-05-30
 * \updates       2024-08-27
 * \license       GNU GPLv2 or above
 *
 *  This class provides a process for starting, running, restarting, and
 *  closing down the Seq66 application, even without session management.
 */

#include <memory>                       /* std::shared_ptr<>, unique_ptr<>  */

#include "cli/parser.hpp"               /* cli::parser class                */
#include "session/configuration.hpp"    /* session::configuration basics    */
#include "session/directories.hpp"      /* session::directories, a helper   */

/*
 *  Do not document a namespace; it breaks Doxygen.
 */

namespace session
{

/**
 *  This class supports manager a run of cfg66.
 */

class manager
{

private:

    /**
     *  The main session configuration object. It provides base-settings,
     *  directory management and directories entries (via a reference).
     *  This gives access to the "home" configuration directory, the 'session'
     *  file, and the entries in the 'session' file
     */

    configuration m_session_config;

    /**
     *  Provides access to the command-line parser.
     */

    cli::parser m_parse_mgr;

    /**
     *  Holds the capabilities string (if applicable) for the application
     *  using this session manager.  Meant mainly for NSM, which needs to
     *  know the capabilities provided.
     */

    std::string m_capabilities;

    /**
     *  Holds the session manager's name. It can be the value returned by
     *  appinfo's cfg::get_client_name() or the name sent by the NSM (Non/New
     *  Session Manager), which should then be passed to cfg::set_client_name().
     *
     *  Note: the following four members' setters are generally overridden
     *  in a GUI application to pass the values along to this class and to
     *  dispaly the values in a window or dialog.
     */

    std::string m_manager_name;

    /**
     *  The NSM sends the path to the NSM session information, such as
     *  "/home/user/NSM Sessions/JackSession/seq66.nUKIE", and the manager
     *  stores it here.
     *
     *  TODO: this value is/should also be available from the
     *        session::directories::session_path() function.
     */

    std::string m_manager_path;

    /**
     *  The NSM sends the display name (the session name, such as
     *  "JackSession".
     */

    std::string m_display_name;

    /**
     *  The NSM sends the client ID (the session sub-directory, such as
     *  "seq66.nUKIE".
     */

    std::string m_client_id;

    /**
     *  Indicates if the --help or --version options were provided at
     *  start-up. In that case operation will change quite a bit.
     */

    bool m_is_help;

    /**
     *  Used in seeing if the "dirty" status has changed so that the session
     *  manager can be told about the change.
     */

    bool m_last_dirty_status;

    /**
     *  Holds the current error message.  Mutable because it is not part of
     *  the true state of the session manager.
     */

    mutable std::string m_extant_errmsg;

    /**
     *  Holds the current error state.  Mutable because it is not part of
     *  the true state of the session manager.
     */

    mutable bool m_extant_msg_active;

public:

    manager ();
    manager
    (
        const cfg::options & optset,
        directories & fileentries,
        const std::string & cfgname = "",
        const std::string & comtext = "",
        const std::string & caps = "",
        int version = 0,
        bool uselogfile = false
    );
    manager (manager &&) = delete;
    manager (const manager &) = default;                // delete;
    manager & operator = (manager &&) = delete;
    manager & operator = (const manager &) = default;   // delete;
    virtual ~manager ();

    bool error_active () const
    {
        return m_extant_msg_active;
    }

    const std::string & error_message () const
    {
        return m_extant_errmsg;
    }

    const std::string & config_filename () const
    {
        static std::string s_dummy{""};
//      return not_nullptr(config_ptr()) ?
//          config_ptr()->config_filename() : s_dummy ;
        return s_dummy;
    }

    const std::string & log_filename () const
    {
        static std::string s_dummy{""};
//      return not_nullptr(config_ptr()) ?
//          config_ptr()->log_filename() : s_dummy ;
        return s_dummy;
    }

    const std::string & capabilities () const
    {
        return m_capabilities;
    }

    bool last_dirty_status () const
    {
        return m_last_dirty_status;
    }

    bool is_help () const
    {
        return m_is_help;
    }

    bool internal_error_check (std::string & msg) const;
    void error_handling ();

    virtual bool internal_error_pending () const
    {
        return false;   // TODO
    }

    bool make_path_names
    (
        const std::string & path,
        std::string & outcfgpath
    );
    bool import_into_session
    (
        const std::string & path,
        const std::string & sourcebase
    );
    bool import_configuration
    (
        const std::string & sourcepath,
        const std::string & sourcebase,
        const std::string & cfgfilepath
    );

    const std::string & manager_name () const
    {
        return m_manager_name;
    }

    const std::string & manager_path () const
    {
        return m_manager_path;
    }

    const std::string & display_name () const
    {
        return m_display_name;
    }

    const std::string & client_id () const
    {
        return m_client_id;
    }

public:

    virtual bool parse_option_file (std::string & errmessage);
    virtual bool parse_command_line
    (
        int argc, char * argv[],
        std::string & errmessage
    );
    virtual bool write_option_file (std::string & errmessage);
    virtual bool create_configuration (std::string & errmessage);
    virtual bool create_configuration
    (
        int argc, char * argv [],
        const std::string & mainpath,
        const std::string & cfgfilepath
    );
    virtual bool read_configuration
    (
        int argc, char * argv [],
        const std::string & cfgfilepath
    );
    virtual bool create_directories (std::string & errmessage);
    virtual bool create_session (int argc = 0, char * argv [] = nullptr);
    virtual bool close_session (std::string & msg, bool ok = true);
    virtual bool save_session (std::string & msg, bool ok = true);
    virtual bool create_manager (int argc, char * argv []);
    virtual bool settings (int argc, char * argv []);

    /*
     * Pure virtual functions.
     */

    virtual bool run () // = 0;
    {
        return false;
    }

    virtual bool set_home (const std::string & homepath) // = 0;
    {
        return false
    }

    virtual bool create_project
    (
        int argc, char * argv [],
        const std::string & path
    ) // = 0;
    {
        (void) argc;
        (void) argv;
        (void) path;
        return false;
    }

    virtual void show_message
    (
        const std::string & tag,
        const std::string & msg
    ) const;
    virtual void show_error
    (
        const std::string & tag,
        const std::string & msg
    ) const;

    virtual void manager_name (const std::string & mgrname)
    {
        m_manager_name = mgrname;
    }

    virtual void manager_path (const std::string & pathname)
    {
        m_manager_path = pathname;
    }

    virtual void display_name (const std::string & dispname)
    {
        m_display_name = dispname;
    }

    virtual void client_id (const std::string & clid)
    {
        m_client_id = clid;
    }

protected:

    virtual bool create_window ()
    {
        return true;
    }

protected:

    const configuration & session_config () const
    {
        return m_session_config;
    }

    configuration & session_config ()
    {
        return m_session_config;
    }

    void log_filename (const std::string & fname)
    {
        (void) fname;
//      if (not_nullptr(session_config()))
//          session_config().log_filename(fname);
    }

    void config_filename (const std::string & fname)
    {
        (void) fname;
//      if (not_nullptr(session_config()))
//          session_config().config_filename(fname);
    }

    void last_dirty_status (bool flag)
    {
        m_last_dirty_status = flag;
    }

    void is_help (bool flag)
    {
        m_is_help = flag;
    }

    void append_error_message (const std::string & message = "") const;

};          // class manager

}           // namespace session

#endif      // CFG66_SESSION_MANAGER_HPP

/*
 * manager.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

