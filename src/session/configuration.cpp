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
 * \file          configuration.cpp
 *
 *  This module declares/defines a module for managing a generic cfg66
 *  session.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2020-03-22
 * \updates       2024-09-11
 * \license       GNU GPLv2 or above
 *
 *  What is a session configuration? It is a setup of the following
 *  categories of items:
 *
 *      -   Session directories and files. The directories indicate where
 *          the configuration, log, and data (e.g. MIDI) files are stored.
 *      -   Command-line options.  These will overlap with configuration
 *          items stored in configuration files.
 *      -   Sub-configuration items.
 *
 *  In Seq66, the configuration is stored in a number of files (the most
 *  important being the "rc" file.  Each file as a number of sections marked
 *  by a "tag" (e.g. "[jack-transport]").  Each section has one or more
 *  value lines.  Most are of the form "name = value", but there are sone other
 *  variations. (These can be treat like "name values..." string-pairs, as
 *  a first pass.)
 *
 *  Here are the operations needed for a configuration:
 *
 *      -   Parse a configuration file and parcel the results to various
 *          sub-configurations.
 *      -   Parse a command-line and parcel the results to various
 *          sub-configurations.
 *      -   Transfer each value to the appropriate run-time objects.
 *      -   Take changes made to the run-time objects and copy them back
 *          to the appropriate sub-configuration.
 *      -   Write the configuration or sub-configurations to the appropriate
 *          files.
 *
 *  In Seq66, the basic configuration items were somewhat scattered between the
 *  rc()/rcfile and usr()/usrfile object, with additions for MIDI control,
 *  playlists, note-mapping (drums), and palettes.  As an example, the "JACK
 *  MIDI" flag:
 *
 *      -   Read from the "rc" file, section "jack-transport", name
 *          "jack-midi", value true/false, and stored in rcsettings, accessed
 *          via the "with_jack_midi()" function..
 *      -   Read from the command-line options "--jack" or "--jack-midi".
 *          Stored as above.
 *      -   Used at start-up to select the usage of JACK. See mastermidibus
 *          and rtmidi_info in Seq66.
 *      -   Modified by the user in qseditoptions::slot_jack_midi(), which
 *          turns on an "rc-modified" flag.
 *      -   When the "rc" file is saved, the boolean is obtained with the
 *          rc().with_jack_midi() accessor and written to the file.
 *
 *  Is there any way to simplify this setup?  Is there any way to unify the
 *  command-line and file configuration settings? The proposal is this:
 *
 *      -   Make each "options" object a sub-configuration. This is the cfg
 *          version, which contains the source-file name and a "section" name.
 *      -   The values in the options object already have names.  Add a
 *          section name to the object that matches the section name in the
 *          "rc" file (for example).
 *      -   Also add a file-value such as "local.rc" that shows the source-file
 *          for the options section and where it is to be written at save time.
 *      -   Add a tree of options sections.
 *
 *  So we will have list of configuration files (at least one or more), each
 *  of which has a list or map of cfg::options objects with a link to the
 *  source file and a secton name.
 *
 *  We have to be able to backtrack from a value name up to the section name,
 *  then up to the source file name. The section name is stored with the
 *  cfg::options object, and so is the source file name.
 *
 */

#include <cstring>                      /* std::strlen()                    */

#include "cfg/appinfo.hpp"              /* cfg::get_home_cfg_directory()    */
#include "session/configuration.hpp"    /* sessions::configuration class    */

/*
 *  Do not document a namespace; it breaks Doxygen.
 */

namespace session
{

configuration::configuration () :
    cfg::basesettings
    (
        "Default Session", "session",
        "A session specifying only the configuration and log directories.",
        0
    ),
    m_dir_manager       (),                 /* default rc & log directories */
    m_section_list      (),                 /* vector of section names      */
    m_data_directories  (),                 /* vector of subdirectory names */
    m_auto_option_save  (false),            /* save rc, usr, etc.           */
    m_auto_save         (false),            /* save edited data file        */
    m_quiet             (false),            /* true suppresses startup errs */
    m_verbose           (false),            /* true shows more status       */
    m_home              (),                 /* the base app directory       */

    m_use_log_file      (false),
    m_log_file          ()
{
    // set_configuration_defaults();
}

/**
 *  Principal constructor.
 */

configuration::configuration
(
    directories & fileentries,
    const std::string & cfgname,
    const std::string & comtext,
    int version,
    bool uselogfile
) :
    cfg::basesettings   (cfgname, "INI", "session", comtext, version),
    m_dir_manager       (fileentries),
    m_section_list      (),                 /* vector of section names      */
    m_data_directories  (),                 /* vector of subdirectory names */
    m_auto_option_save  (false),            /* save rc, usr, etc.           */
    m_auto_save         (false),            /* save edited data file        */
    m_quiet             (false),            /* true suppresses startup errs */
    m_verbose           (false),            /* true shows more status       */
    m_home              (),                 /* the base app directory       */

    m_use_log_file      (uselogfile),
    m_log_file          ()
{
    // set_configuration_defaults();
}

/**
 *  This might be a good place to save the configuration.
 */

configuration::~configuration ()
{
    // no code so far
}

#if defined ENABLE_THIS_CODE

/**
 *      -   Parse log option.
 *      -   Parse --option/-o options into a structure?
 *      -   Other options.
 *
 *  If "-o log=file.ext" occurred, handle it early on in startup.
 *
 *  The user migh specify -o options that are also set up in the configuration
 *  file; the command line must take precedence. The "log" option is processed
 *  early in the startup sequence.
 *
 * \param argc
 *      The number of command-line arguments.
 *
 * \param argv
 *      The array of command-line arguments.
 *
 * \param [out] errmessage
 *      Provides a string to hold any error message that occurs.
 *
 * \return
 *      Returns 
 *      Returns the value of optind if no help-related options were provided.
 *      Returns (-1) if an error occurred.
 */

int
configuration::parse_command_line
(
    int argc, char * argv [],
    std::string & errmessage
)
{
    int optionindex = -1;
    (void) argc;
    (void) argv;
    errmessage = "Not implemented, please program an implementation";
#if defined THIS_CODE_IS_READY
//      optionindex = parse_command_line(argc, argv, errmessage);
//      result = optionindex >= 0;
//      result = parse_o_options(argc, argv);

    std::string logfile = usr().option_logfile();
    if (usr().option_use_logfile())
        (void) xpc::reroute_stdio(logfile);

    if (optionindex < argc)                 /* MIDI filename given? */
    {
        std::string fname = argv[optionindex];
        std::string errmsg;
        if (util::file_readable(fname))
        {
            std::string path;               /* not used here        */
            std::string basename;
        }
        else
        {
            char temp[512];
            (void) snprintf
            (
                temp, sizeof temp,
                "MIDI file not readable: '%s'", fname.c_str()
            );
//          append_error_message(temp);     /* raises the message   */
        }
    }

#endif
    return optionindex;
}

/**
 *  Parsing is patterned after cfg::inifile::parse(), but does not
 *  depend on filling in an options list. Instead, the data is stored
 *  in class members.
 *
 * Note:
 *
 *  App-info settings can be made via a cfg::appinfo structure initialization
 *  and passing it to cfg::initialize(); via modifications from the command line
 *  or by calling appinfo setter functions.
 *
 * Assumptions:
 *
 *      -   Optional calls to the "set" versions of these functions in
 *          appinfo.
 *          -   get_app_name()
 *          -   get_home_cfg_directory() ["~/.config/appname"]
 *          -   get_main_cfg_section_name() ["[Cfg66]"]
 */

bool
configuration::parse
(
)
{
}

#endif

}           // namespace session

/*
 * configuration.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

