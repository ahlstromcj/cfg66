#if ! defined CFG66_SESSION_CLIMANAGER_HPP
#define CFG66_SESSION_CLIMANAGER_HPP

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
 * \file          climanager.hpp
 *
 *  This module declares/defines the main module for the JACK/ALSA "qt5"
 *  implementation of this application.
 *
 * \library       climanager application
 * \author        Chris Ahlstrom
 * \date          2020-08-31
 * \updates       2024-09-02
 * \license       GNU GPLv2 or above
 *
 *  Provides a base class that can be used to manage the command-line version
 *  of Seq66v2, as well as a helper for the Qt6 version.
 */

#include <memory>                       /* std::unique_ptr, shared_ptr<>    */

#include "cfg/inimanager.hpp"           /* cfg::inimanager class            */
#include "cli/multiparser.hpp"          /* cli::multiparser class           */
#include "session/manager.hpp"          /* session::manager class           */

namespace session
{

static const std::string c_cli_nsm_capabilities{};

/**
 *  Provides command-line and user-interface support for session management.
 *  If NSM support is not built in, there is still some minor management tasks
 *  that can be done.
 */

class climanager : public manager
{

private:

    /**
     *  Holds a parser capable of parsing CLI options that apply to multiple
     *  INI files.
     */

    cli::multiparser m_multi_parser;

    /**
     * TBD
     */

    bool m_session_active;

    /**
     *  Holds a copy of the user-interface redraw rate.
     */

    int m_poll_period_ms;

public:

    climanager
    (
        directories & fileentries,
        cfg::inimanager & inimgr,
        const std::string & caps = c_cli_nsm_capabilities
    );
    virtual ~climanager ();

    cli::multiparser & multi_parser ()
    {
        return m_multi_parser;
    }

    const cli::multiparser & multi_parser () const
    {
        return m_multi_parser;
    }

    bool session_active () const
    {
        return m_session_active;
    }

    virtual bool create_session
    (
        int argc = 0,
        char * argv [] = nullptr
    ) override;
    virtual bool close_session (std::string & msg, bool ok = true) override;
    virtual bool save_session (std::string & msg, bool ok = true) override;
    virtual bool create_project
    (
        int argc, char * argv [],
        const std::string & path
    ) override;
    virtual void show_error
    (
        const std::string & tag,
        const std::string & msg
    ) const override;
    virtual bool run () override;
    virtual bool set_home (const std::string & homepath) override;

    virtual bool add_inisections (cfg::inisections::specification & op) override;

    bool add_inisections (cfg::inimanager::sections_specs & ops);

    virtual void session_manager_name (const std::string & mgrname); // override;
    virtual void session_manager_path (const std::string & pathname); // override;
    virtual void session_display_name (const std::string & dispname); // override;
    virtual void session_client_id (const std::string & clid); // override;

protected:

    void session_active (bool flag)
    {
        m_session_active = flag;
    }

private:

    virtual bool read_configuration
    (
        int argc, char * argv [],
        const std::string & cfgfilepath
    );
    bool detect_session (std::string & url);

};          // class climanager

}           // namespace session

#endif      // CFG66_SESSION_CLIMANAGER_HPP

/*
 * climanager.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

