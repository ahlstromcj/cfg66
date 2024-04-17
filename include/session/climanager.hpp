#if ! defined SEQ66_SESSION_CLIMANAGER_HPP
#define SEQ66_SESSION_CLIMANAGER_HPP

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
 * \updates       2024-01-24
 * \license       GNU GPLv2 or above
 *
 *  Provides a base class that can be used to manage the command-line version
 *  of Seq66v2, as well as a helper for the Qt6 version.
 */

#include <memory>                       /* std::unique_ptr, shared_ptr<>    */

#include "session/manager.hpp"          /* session::manager                 */

namespace session
{

/**
 *  Provides command-line and user-interface support for session management.
 *  If NSM support is not built in, there is still some minor management tasks
 *  that can be done.
 */

class climanager : public manager
{

private:

    /**
     *
     */

    bool m_session_active;

    /**
     *  Holds a copy of the user-interface redraw rate.
     */

    int m_poll_period_ms;

public:

    climanager (const std::string & caps = c_cli_nsm_capabilities);
    virtual ~climanager ();

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
    virtual void session_manager_name (const std::string & mgrname) override;
    virtual void session_manager_path (const std::string & pathname) override;
    virtual void session_display_name (const std::string & dispname) override;
    virtual void session_client_id (const std::string & clid) override;

protected:

    void session_active (bool flag)
    {
        m_session_active = flag;
    }

private:

    bool read_configuration
    (
        int argc, char * argv [],
        const std::string & cfgfilepath,
        const std::string & midifilepath
    );
    bool detect_session (std::string & url);

};          // class climanager

}           // namespace session

#endif      // SEQ66_SESSION_CLIMANAGER_HPP

/*
 * climanager.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

