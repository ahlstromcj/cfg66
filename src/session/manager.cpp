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
 * \file          manager.cpp
 *
 *  This module declares/defines a module for managing a generic cfg66
 *  session.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2020-03-22
 * \updates       2023-02-18
 * \license       GNU GPLv2 or above
 *
 *  Note that this module is part of the libcfg66 library, not the libsessions
 *  library.  That is because it provides functionality that is useful even if
 *  session support is not enabled.
 *
 *  The process:
 *
 *      -# Call settings(argc, argv).  It sets defaults, does some parsing
 *         of command-line options and files.  It saves the MIDI file-name, if
 *         provided.
 *      -# If a user-interface is needed, create a unique-pointer to it, then
 *         show it.  This will remove any previous pointer.  The function is
 *         virtual, create_window().
 */

#include <cstring>                      /* std::strlen()                    */

#include "cfg/appinfo.hpp"              /* cfg::set_app_name()              */
#include "cfg/options.hpp"              /* cfg::options::container          */
#include "session/manager.hpp"          /* session::manager()               */
#include "util/msgfunctions.hpp"        /* util::file_message() etc.        */
#include "util/filefunctions.hpp"       /* util::file_readable() etc.       */

/*
 *  Do not document a namespace; it breaks Doxygen.
 */

namespace session
{

/*------------------------------------------------------------------------
 * Additional options
 *------------------------------------------------------------------------*/

/*
 *  The following string keeps track of the characters used so far.  An 'x'
 *  means the character is used.  A ':' means it is used and requires an
 *  argument. An 'a' indicates we could repurpose the key with minimal impact.
 *  An asterisk '*' indicates the option is reserved for overflow options (-o,
 *  --option); those options have no character-code.
 *
\verbatim
        0123456789#@AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz
                                   x       x     *            xx
                                           *
\endverbatim
 *
 *   Name, Code, Kind, Enabled, Default, Value, FromCli, Dirty,
 *   Description
 */

/*
 * Note that the default name here is empty.  If supplied on the command
 * line, then it will change the appinfo's client-name.
 */

static cfg::options::container s_option_client_name // or a single option?
{
    {
        "client-name",
        {
            'l', "string", true, "", "", false, false,
            "Overrides the normal client-name (short app name).", false
        }
    }
};

/**
 *  Default constructor
 */

manager::manager () :
    m_session_config        (),
    m_capabilities          (),
    m_manager_name          (),
    m_manager_path          (),
    m_display_name          (),
    m_client_id             (),
    m_is_help               (false),
    m_last_dirty_status     (false),
    m_extant_errmsg         (),
    m_extant_msg_active     (false)
{
    // set_configuration_defaults();
}

/**
 *  Principle constructor
 *
 *  The filename and section name are meant to be used when reading and writing
 *  to/from a configuration file. However, we DO NOT YET HAVE these parameters
 *  in the options::add() functions!!!
 */

manager::manager
(
    const cfg::options & optset,
    const directories::entries & fileentries,
    const std::string & cfgname,
    const std::string & comtext,
    const std::string & caps,
    int version,
    bool uselogfile
) :
    m_session_config
    (
        fileentries, cfgname, comtext, version, uselogfile
    ),
    m_parse_mgr             (optset),
    m_capabilities          (caps),
    m_manager_name          (),         // TODO
    m_manager_path          (),         // TODO
    m_display_name          (),         // TODO
    m_client_id             (),         // TODO
    m_is_help               (false),
    m_last_dirty_status     (false),
    m_extant_errmsg         (),
    m_extant_msg_active     (false)
{
    // set_configuration_defaults();
}

/**
 *  We found that on a Debian developer laptop, this destructor took a couple
 *  of seconds to call get_deleter().  Works fine on our Ubuntu developer
 *  laptop.  Weird.  Actually might have been a side-effect of installing a
 *  KxStudio PPA.
 */

manager::~manager ()
{
    if (! is_help())
        (void) util::info_message("Exiting session manager");
}

/**
 *  The standard C/C++ entry point to this application, after the main()
 *  function has set up the application name (which a session manager might
 *  modify slightly) and some default ("primitive") settings. See the
 *  call to initialize_appinfo() in main().
 *
 *  The first thing is to set the various session defaults, and then try to
 *  read the configuration file(s).  The last thing is to override any other
 *  settings via the command-line parameters.
 *
 * \param argc
 *      The number of command-line parameters, including the path to the
 *      application as parameter 0.
 *
 * \param argv
 *      The array of pointers to the command-line parameters.
 *
 * \return
 *      Returns true if this code worked properly and was not a request for
 *      help/version information.
 */

bool
manager::settings (int argc, char * argv [])
{
    std::string appname = cfg::get_app_name();  /* see initialize_appinfo() */
    bool result = ! appname.empty();
    if (result)
    {
        std::string errmessage;                 /* just in case!            */
        bool ishelp = false;    // TODO
        if (ishelp)
        {
            is_help(true);
        }
        if (result)
        {
    //          (void) cmdlineopts::parse_log_option(argc, argv);

            /*
             *  If parsing fails, report it and disable usage of the application and
             *  saving bad garbage out when exiting.
             */

            result = parse_option_file(errmessage);
            if (result)
            {
                result = parse_command_line(argc, argv, errmessage);
                if (! result)
                    append_error_message(errmessage);
            }
            else
                append_error_message(errmessage);
        }
    }
    return result;
}

bool
manager::parse_option_file (std::string & errmessage)
{
    bool result = true;
    errmessage = "parse_option_file() not implemented";
#if THIS_CODE_IS_READY
    if (result)
        result = session_config.parse_file(errmessage);
#endif

    return result;
}

bool
manager::parse_command_line
(
    int argc, char * argv[],
    std::string & errmessage
)
{
    bool result = true;
    errmessage = "parse_command_line() not implemented";
    (void) argc;
    (void) argv;
#if THIS_CODE_IS_READY
    if (result)
        result = session_config().parse_command_line(argc, argv, errmessage);
#endif

    return result;
}

bool
manager::write_option_file (std::string & errmessage)
{
    errmessage = "write_option_file() not yet implemented";
    return false;
}

bool
manager::create_configuration (std::string & errmessage)
{
    bool result = false;
    errmessage = "create_configuration() not yet implemented";
    return result;
}

bool
manager::create_directories (std::string & errmessage)
{
    bool result = false;
    errmessage = "create_directories() not yet implemented";
    return result;
}

/**
 *  Sets up a session.
 *
 *  It does not support the xpc::session_xxx() functions, as we don't want
 *  this library to depend up other libraries.
 */

bool
manager::create_session (int /*argc*/, char * /*argv*/ [])
{
    // xpc::session_setup();        /* daemonize: set basic signal handlers */
    return true;
}

/**
 *  Closes the session, with an option to handle errors in the session.
 *
 *  Note that we do not save if in a session, as we rely on the session
 *  manager to tell this application to save before forcing this application
 *  to end.
 *
 * \param [out] msg
 *      Provides a place to store any error message for the caller to use.
 *
 * \param ok
 *      Indicates if an error occurred, or not.  The default is true, which
 *      indicates "no problem".
 *
 * \return
 *      Returns the ok parameter if false, otherwise, the result of finishing
 *      up is returned.
 */

bool
manager::close_session (std::string & msg, bool ok)
{
    bool result = save_session(msg, true); // result);
    if (result)
    {
    }
    if (ok)
    {
    }
    // TODO
    // (void) xpc::session_close();           /* daemonize signals exit   */
    return result;
}

/**
 *  This function saves the following files (so far):
 *
 *      -   *.rc
 *      -   *.ctrl (via the 'rc' file)
 *      -   *.mutes (via the 'rc' file)
 *      -   *.usr
 *      -   *.drums
 *
 *  The clinmanager::save_session() function saves the MIDI file to the
 *  session, if applicable.  That function also clears the message parameter
 *  before the saving starts.
 *
 * \param [out] msg
 *      Provides a place to store any error message for the caller to use.
 *
 * \param ok
 *      Indicates if an error occurred, or not.  The default is true, which
 *      indicates "no problem".
 */

bool
manager::save_session (std::string & msg, bool ok)
{
    bool result = true;
    if (ok)
    {
        std::string errmessage;
        bool save = session_config().modified();

        if (save)
        {
            util::file_message("Save session", "Options");
            if (! write_option_file(errmessage))
            {
                msg = "Config write failed: ";
                msg += errmessage;
            }
        }
    }
    else
    {
        result = false;
        if (! is_help())
        {
            std::string errmessage;

//          session_config().config_filename("erroneous");
//          (void) write_option_file(errmessage);

            if (error_active())
            {
                append_error_message(error_message());
                msg = error_message();
            }
        }
    }
    return result;
}

/**
 *  Sets the error flag and appends to the error message, which are both mutable
 *  so that we can safely call this function under any circumstances.
 *
 * \param message
 *      Provides the message to be set. If empty, the message-active flag and
 *      the message are both cleared.
 */

void
manager::append_error_message (const std::string & msg) const
{
    if (msg.empty())
    {
        m_extant_errmsg.clear();
        m_extant_msg_active = false;
    }
    else
    {
        m_extant_msg_active = true;
        if (m_extant_errmsg.empty())
        {
            /* m_extant_errmsg += "? "; */
        }
        else
        {
            util::error_message(msg);
            m_extant_errmsg += "\n";
        }
        m_extant_errmsg += msg;
    }
}

/*
 *
 */

void
manager::show_message (const std::string & tag, const std::string & msg) const
{
    std::string fullmsg = tag + ": " + msg;
    util::info_message(fullmsg);       /* checks for "debug" and adds "[]" */
}

void
manager::show_error (const std::string & tag, const std::string & msg) const
{
    std::string fullmsg = tag + ": " + msg;
    util::error_message(msg);
}

/**
 *  Checks for an internal (e.g. PortMidi) error, storing the message if
 *  applicable.
 *
 * \param [out] errmsg
 *      Provides a destination for the PortMidi error.  It is cleared if
 *      there is no error.
 *
 * \return
 *      Returns true if there is an error.  In this case, the caller should
 *      show the error message.
 */

bool
manager::internal_error_check (std::string & errmsg) const
{
    std::string pmerrmsg;
    errmsg.clear();

    bool result = internal_error_pending();
    if (result)
    {
        pmerrmsg +=
            " Check to see which MIDI I/O ports are disabled. Check log file."
            ;
        append_error_message(pmerrmsg);
        errmsg = pmerrmsg;
    }
    return result;
}

void
manager::error_handling ()
{
    std::string errmsg;
    if (internal_error_check(errmsg))
        show_error("Session error", errmsg);

//  std::string logpath = log_filename("cfg66.log");
//  (void) cfg66::file_append_log(path, errmsg);
}

/**
 *  Refactored so that the basic NSM session can be set up before launch(), as
 *  per NSM rules.
 *
 *  The following call detects a session, creates an nsmclient, sends an NSM
 *  announce message, waits for the response, uses it to set the session
 *  information.  What we really see:
 *
 *      nsmclient::announce()       Send announcement, wait for response
 *      <below>                     Gets manager path!!!
 *      nsmclient::open()           Sets manager path
*
 *  We run() the window, get the exit status, and close the session in the
 *  main() function of the application.
 *
 *  Call sequence summary:
 *
 *      -   settings()
 *      -   create_session()
 *      -   create_project()
 *      -   create_player()
 *      -   create_window()
 *      -   run(), done in main()
 *      -   close_session(), done in main()
 */

bool
manager::create_manager (int argc, char * argv [])
{
    bool result = settings(argc, argv);
    if (result)
    {
        bool ok = create_session(argc, argv);   /* get path, client ID, etc */
        if (ok)
        {
            std::string homedir = manager_path();
            if (homedir.empty())
                homedir = "GET FROM files CLASS";   // c_home_directory;

            util::file_message("Session manager path", homedir);

            //
            // TODO TODO TODO
            // (void) create_project(argc, argv, homedir);
        }

        /*
         * Here we could create the engine of the application.
         */

        if (result)
        {
            result = create_window();
            if (result)
            {
                error_handling();
            }
            else
            {
                std::string msg;                        /* maybe errmsg? */
                result = close_session(msg, false);
            }

            /*
             * TODO:  expose the error message to the user here
             */
        }
//      if (! is_help())
//          cmdlineopts::show_locale();
    }
    else
    {
        if (! is_help())
        {
            std::string msg;
            (void) create_window();
            error_handling();
            (void) create_session();
            (void) run();
            (void) close_session(msg, false);
        }
    }
    return result;
}

bool
manager::create_configuration
(
    int argc, char * argv [],
    const std::string & mainpath,
    const std::string & cfgfilepath
)
{
    bool result = ! cfgfilepath.empty();
    if (result)
    {
        std::string rcbase = config_filename();
        std::string rcfile = util::filename_concatenate(cfgfilepath, rcbase);
        bool already_created = util::file_exists(rcfile);
        // midi_filename(midifilepath);                    /* do this first    */
        if (already_created)
        {
            util::file_message("File exists", rcfile);  /* comforting       */
            result = read_configuration(argc, argv, cfgfilepath);
            if (result)
            {
#if defined MOVE_TO_DERIVED
                if (usr().in_nsm_session())
                {
                    rc().auto_rc_save(true);
                }
                else
                {
                    bool u = rc().auto_usr_save();      /* --user-save?     */
                    rc().set_save_list(false);          /* save them all    */
                    rc().auto_usr_save(u);              /* restore it       */
                }
#endif
            }
        }
        else
        {
            result = util::make_directory_path(mainpath);
            if (result)
            {
                util::file_message("Ready", mainpath);
                result = util::make_directory_path(cfgfilepath);
                if (result)
                {
                    util::file_message("Ready", cfgfilepath);
//                  rc().home_config_path(cfgfilepath);
                }
            }
//          rc().set_save_list(true);                   /* save all configs */
#if defined USE_NSM
            if (usr().in_nsm_session())
            {
                usr().session_visibility(false);        /* new session=hide */

#if defined NSM_DISABLE_LOAD_MOST_RECENT
                rc().load_most_recent(false;            /* don't load MIDI  */
#else
                rc().load_most_recent(true);            /* issue #41        */
#endif
            }
#endif
#if defined DO_NOT_BELAY_UNTIL_EXIT
            if (result)
            {
                util::file_message("Saving session configuration", cfgfilepath);
                result = cmdlineopts::write_options_files();
            }
#endif
        }
    }
    return result;
}

bool
manager::read_configuration
(
    int argc, char * argv [],
    const std::string & // cfgfilepath
)
{
//  session_config().home_config_path(cfgfilepath);    /* set NSM dir      */

    std::string errmessage;
    bool result = parse_option_file(errmessage);
    if (result)
    {
        /*
         * Perhaps at some point, the "rc"/"usr" options might affect NSM
         * usage.  In the meantime, we still need command-line options, if
         * present, to override the file-specified options.  One big example
         * is the --buss override. The manager::settings() function is
         * called way before create_project();
         */

        if (argc > 1)
        {
            int rcode = parse_command_line(argc, argv, errmessage);
            result = rcode != (-1);
            if (! result)               // DONE above parse_o_options(argc, argv);
                is_help(true);          /* a hack to avoid create_window()  */
        }
    }
    else
    {
        // TODO: add full-path getters to configuration and rtlconfiguration.
//      util::file_error(errmessage, rc().config_filespec());
    }
    return result;
}

bool
manager::make_path_names
(
    const std::string & path,
    std::string & outcfgpath
)
{
    bool result = ! path.empty();
    if (result)
    {
        std::string cfgpath = path;

        // FIXME
        if (result) // TODO:  in session call? in_nsm_session()) // nsm_active()
        {
        // FIXME
            cfgpath = util::pathname_concatenate(cfgpath, "config");
        }
        else
        {
        // FIXME
            /*
             * There's no "config" subdirectory outside of an NSM session.
             */
        }
        outcfgpath = cfgpath;
    }
    return result;
}

/**
 *  Function for the main window to call.
 *
 *  When this function succeeds, we need to signal a session-reload and the
 *  following settings:
 *
 *  rc().load_most_recent(false);               // don't load MIDI  //
 *  rc().set_save_list(true);                   // save all configs //
 *
 *  We don't really need to care that NSM is active here
 */

bool
manager::import_into_session
(
    const std::string & sourcepath,
    const std::string & sourcebase              /* e.g. qrcfg66.rc */
)
{
    bool result = ! sourcepath.empty() && ! sourcebase.empty();
    if (result)
    {
#if defined THIS_CODE_IS_READY
        std::string destdir = rc().home_config_directory();
        std::string destbase = rc().config_filename();
        std::string cfgpath;
        result = make_path_names(destdir, cfgpath);
        if (result)
            result = delete_configuration(cfgpath, destbase);

        if (result)
            result = copy_configuration(sourcepath, sourcebase, cfgpath);

        if (result)
        {
            result = import_configuration
            (
                sourcepath, sourcebase, cfgpath
            );
        }
#endif  // defined THIS_CODE_IS_READY
    }
    return result;
}

/**
 *  This function is like create_configuration(), but the source is not the
 *  standard configuration-file directory, but a directory chosen by the user.
 *  This function should be called only for importing a configuration into an
 *  NSM session directory.
 *
 * \param sourcepath
 *      The source path is the path to the configuration files chosen for
 *      import by the user.  Often it is the usual ~/.config/cfg66 directory.
 *
 * \param sourcebase
 *      This is the actual configuration file chosen by the user.  It can be
 *      any file qcfg66.*, only the first part of the name (e.g. "qcfg66") is
 *      used.
 *
 * \param cfgfilepath
 *      This is the configuration directory used in this NSM session.  It is
 *      often something like "~/NSM Sessions/MySession/cfg66.nLTQC/config.
 *      This is the destination for the imported configuration files.
 *
 * \return
 *      Returns true if the import succeeded.
 */

bool
manager::import_configuration
(
    const std::string & sourcepath,
    const std::string & sourcebase,
    const std::string & // cfgfilepath
)
{
    bool result = ! sourcepath.empty() && ! sourcebase.empty();
    if (result)
    {
#if defined THIS_CODE_IS_READY
        std::string rcbase = util::file_extension_set(sourcebase, ".rc");
        std::string rcfile = util::filename_concatenate(sourcepath, rcbase);
        result = util::file_exists(rcfile);             /* a valid source   */
        if (result)
        {
            config_filename(rcfile);

            std::string errmessage;
            result = parse_option_file(rcfile, errmessage);
            if (result)
            {
#if defined NSM_DISABLE_LOAD_MOST_RECENT
                if (usr().in_nsm_session())
                    rc().load_most_recent(false);       /* don't load MIDI  */
#else
//              if (usr().in_nsm_session())
//                  rc().load_most_recent(true);        /* issue #41        */
#endif
            }
        }
        if (result)
        {
            util::file_message("Saving imported configuration", cfgfilepath);
            result = write_option_file();
            session_config().modified(true);
        }
#endif  // defined THIS_CODE_IS_READY
    }
    return result;
}

}           // namespace session

/*
 * manager.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

