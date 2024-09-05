/*
 *  This file is part of rtl66.
 *
 *  rtl66 is free software; you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation; either version 2 of the License, or (at your option) any later
 *  version.
 *
 *  rtl66 is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with rtl66; if not, write to the Free Software Foundation, Inc., 59 Temple
 *  Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * \file          climanager.cpp
 *
 *  This module declares/defines the base module for command-line operation
 *  under a session.
 *
 * \library       climanager application
 * \author        Chris Ahlstrom
 * \date          2020-08-31
 * \updates       2024-09-05
 * \license       GNU GPLv2 or above
 *
 *  This object also works if there is no session manager in the build.  It
 *  handles non-session startup as well.
 */

// #include "xpc/daemonize.hpp"         /* xpc::session_setup(), _close()   */
// #include "xpc/timing.hpp"            /* xpc::millisleep()                */
// #include "util/strfunctions.hpp"     /* util::contains()                 */

#include "cfg/appinfo.hpp"              /* cfg::get_session_tag()           */
#include "session/climanager.hpp"       /* session::climanager class        */
#include "util/msgfunctions.hpp"        /* util::file_message() etc.        */

namespace session
{

/**
 *  Note that this object is created before there is any chance to get the
 *  configuration, because the manager base class is what gets the
 *  configuration, well after this constructor.
 */

climanager::climanager
(
    directories & fileentries,
    cfg::inimanager & inimgr,
    const std::string & caps
) :
    manager             (fileentries, caps),
    m_multi_parser      (inimgr),
    m_session_active    (false),
    m_poll_period_ms    (3 * 45)    // usr().window_redraw_rate()
{
    // no code
}

/**
 *  Detects if the 'usr' file defines usage of NSM and a valid URL for the
 *  nsmd daemon, or, if not, is there an NSM URL in the environment. Also, if
 *  there is an NSM URL in the environment, it overrides the one specified in
 *  the 'usr' file.
 *
 *  Also sanity-checks the URL: "osc.udp://hostname.domain:PORT#"
 *
 * \param [out] url
 *      Holds the URL that was found.  Use it only if this function returns
 *      true.
 *
 * \return
 *      Returns true if a usable NSM URL was found and nsmd was found to be
 *      running.
 */

bool
climanager::detect_session (std::string & url)      // MAKE VIRTUAL for NSM
{
    bool result = false;
    url.clear();

    /*
     * TODO
     */

    return result;
}

/**
 *  This function first determines if the user wants an NSM session.  If so,
 *  it determines if it can get a valid NSM_URL environment variable.  If not,
 *  that may simply be due to nsmd running in different console window or as a
 *  daemon.  In that cause, it checks if there was a non-empty
 *  "[user-session]" "url" value.  This is useful in trouble-shooting, for
 *  example.  Finally, just in case the user has set up the "usr" file for
 *  running NSM, but hasn't started non-session-manager or nsmd itself, we use
 *  the new pid_exists() function to make sure that nsmd is indeed running.  Ay!
 *
 *  If all is well, a new nsmclient is created, and an announce/open handshake
 *  starts.  This function is called before create_window().
 */

bool
climanager::create_session (int argc, char * argv [])   // MAKE VIRTUAL FOR NSM
{
    bool result = manager::create_session(argc, argv);
    session_active(result);
    return result;
}

/**
 *  Somewhat of the inverse of create_session().
 */

bool
climanager::close_session (std::string & msg, bool ok)      // MAKE VIRTUAL NSM
{
    session_active(false);
    return manager::close_session(msg, ok);
}

/**
 *  Saves the active MIDI file, and then calls the base-class version of
 *  save_session().
 */

bool
climanager::save_session (std::string & msg, bool ok)
{
    bool result = ok; // not_nullptr(perf());
    if (ok)
        msg.clear();

    if (result)
    {
        result = manager::save_session(msg, ok);
        if (result)
        {
            /*
             * Only show the message if not running under a session
             * manager.  This is because the message-box will hang the
             * application until the user clicks OK.
             */

            if (! session_active())
                show_message(cfg::get_session_tag(), msg);
        }
        else
            show_error(cfg::get_session_tag(), msg);
    }
    return result;
}

/**
 *  This function is useful in the command-line version of the application.
 *  For the Qt version, see the qt5nsmanager class.
 */

bool
climanager::run ()
{
    bool result = setup_session();          /* daemonize: session_setup()   */
    std::string msg;
    while (! close_session(msg))            /* daemonize: session_close()   */
    {
        result = true;
        if (manager::save_session())        /* daemonize: session_save()    */
        {
            result = save_session(msg, true);
            if (! result)
            {
                util::file_error(msg, "CLI");
            }
        }
        // millisleep(m_poll_period_ms);
    }
    return true;
}

/**
 *  Creates a session path specified by the Non Session Manager.  This
 *  function is meant to be called after receiving the /nsm/client/open
 *  message.
 *
 *  A sample session path:
 *
 *      /home/ahlstrom/NSM Sessions/QSeq66 Installed/rtl66.nYMVC
 *
 *  The NSM daemon creates the directory for this project after dropping the
 *  client ID (rtl66.nYMVC).  We append the client ID and create this
 *  directory, followng the lead of Non-Mixer and Qtractor.
 *
 *  Note: At this point, the performer [perf()] does not yet exist, but
 *  the 'rc', 'usr', 'ctrl', and 'mutes' files have been parsed.
 *
 *  The first thing to do is grab the current (and default) configuration
 *  directory (in the user's HOME area).  We may need it, in order to find the
 *  original files to recreate.  Next we see if the configuration has already
 *  been created, using the "rc" file as the test case.  The normal base-name
 *  (e.g. "qrtl66") is always used in an NSM session.  We will read/write the
 *  configuration from the NSM path.  We assume (for now) that the "midi"
 *  directory was also created.
 *
 * \param argc
 *      The command-line argument count.
 *
 * \param argv
 *      The command-line argument list.
 *
 * \param path
 *      The base configuration path.  For NSM usage, this will be a directory
 *      for the project in the NSM session directory created by nsmd.  The
 *      sub-directories "config" and "midi" will be created for use by NSM.
 *      For normal usage, this directory will be the standard home directory
 *      or the one specified by the --home option.
 */

bool
climanager::create_project
(
    int argc, char * argv [],
    const std::string & path
)
{
    bool result = ! path.empty();
    if (result)
    {
        std::string cfgpath;
        std::string midipath;
        result = make_path_names(path, cfgpath);    //, midipath);
        if (result)
            result = create_configuration(argc, argv, path, cfgpath); //, midipath);
    }
    return result;
}

void
climanager::session_manager_name (const std::string & mgrname)
{
    // session_manager_name(mgrname);
    if (! mgrname.empty())
        util::file_message(cfg::get_session_tag(), mgrname);
}

void
climanager::session_manager_path (const std::string & pathname)
{
    // session_manager_path(pathname);
    if (! pathname.empty())
        util::file_message(cfg::get_session_tag("path"), pathname);
}

void
climanager::session_display_name (const std::string & dispname)
{
    // session_display_name(dispname);
    if (! dispname.empty())
        util::file_message(cfg::get_session_tag("name"), dispname);
}

void
climanager::session_client_id (const std::string & clid)
{
    // session_client_id(clid);
    if (! clid.empty())
        util::file_message(cfg::get_session_tag("client ID"), clid);
}

/*
 * Not yet present in inimanager
 *
bool
climanager::add_inisections (cfg::inisections::specification & op)
{
    return ini_manager().add_inisections(op);
}

bool
climanager::add_inisections (cfg::inimanager::sections_specs & ops)
{
    bool result = true;
    for (auto & secptr : ops)
    {
        result = add_inisections(*secptr);
        if (! result)
            break;
    }
    return result;
}
 */

/**
 *  Shows the collected messages in the console, and recommends the user
 *  exit and check the configuration.
 */

void
climanager::show_error
(
    const std::string & tag,
    const std::string & msg
) const
{
    if (msg.empty())
    {
        std::string msg = error_message();
        msg += "Please exit and fix the configuration.";
        show_message(tag, msg);
    }
    else
    {
        append_error_message(msg);
        show_message(tag, msg);
    }
}

}           // namespace session

/*
 * climanager.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

