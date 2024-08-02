#if ! defined CFG66_CFG_SESSION_SPEC_HPP
#define CFG66_CFG_SESSION_SPEC_HPP

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
 * \file          session_spec.hpp
 *
 *      Provides a way to hold all options from multiple INI-style files and
 *      multiple INI file sections.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2023-07-22
 * \updates       2024-07-16
 * \license       See above.
 *
 *  This is the first of a set of test/demo header files to set up a large
 *  configuration file for the inisections/inisection/options hierarchy. These
 *  files are useful for testing, but will likely be suitable for usage in
 *  a real application (Seq66v2).
 *
 *  File types supported in the set of "spec" files:
 *
 *      "session" (NEW, replaces the top part of the 'rc' file)
 *      "rc"
 *      "usr"
 *      "ctrl"
 *      "drums"
 *      "metro" (NEW) (or keep it in 'rc'?)
 *      "mutes"
 *      "palette"
 *      "playlist"
 *      "ports" (NEW) (or keep them in 'rc'?)
 *
 *  For this file, all of the sections of the 'session' file are defined by
 *  the inisections::specification session_data declaration near the end of this
 *  file.
 *
 *  Note that this header file should be included only once in an application.
 *  It defines global structures that can be read from the 'session' file,
 *  be modified, and then be written back to the 'session' file.
 *
 *  Although "--log" is a built-in option, we should be able to specify it here.
 *  TODO TODO TODO
 */

#include "cfg/inisections.hpp"          /* cfg::inisections class           */

/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace cfg
{

/*------------------------------------------------------------------------
 * 'session' file
 *------------------------------------------------------------------------*/

/**
 *  We assume the file sections have been moved to the session file.
 *  These are:
 *  `
 *      [midi-control-file]
 *      [mute-group-file]
 *      [usr-file]
 *      [playlist-file]
 *      [note-mapper-file]
 *      [palette-file]
 *      [user-session] ???
 *
 *  Additional sections, common to all files:
 *
 *      [Cfg66]         Replaces the app-specific [Seq66] section.
 *      [comments]      Universally useful for documentation.
 */

const std::string session_extension{"session"};

inisection::specification session_cfg66_data
{
    "[Cfg66]",              /* can replace via set_main_cfg_section_name()  */
    {
"'version' is used by the application to detect older configuration files,\n"
"which are upgraded to the new version when saved.\n"
"\n"
"'quiet' suppresses start-up error messages. Useful when they are not\n"
"relevant. It's not the opposite of 'verbose'.\n"
"\n"
"'verbose' is temporary, same as --verbose; it is set to false at exit.\n"
"The other sections in this file specify the names and locations of the\n"
"true configuration file(s).\n"

        /*
         * TODO: Should we move session-manager information to here?
         *       We should do the same for the log-files!!!
         */
    },
    {
        /*
         * Name, Name, Code, Kind, Enabled, Default, Value,
         * FromCli, Dirty, Description, Built-in
         */
        {
            "config-type",
            {
                options::code_null, options::kind::string, options::disabled,
                "session", "", false, false,
                "Session configuration file.", false
            }
        },
        {
            "version",
            {
                options::code_null, options::kind::integer, options::disabled,
                "0", "", false, false,
                "Configuration file version.", false
            }
        },
        {
            "quiet",
            {
                options::code_null, options::kind::boolean, options::enabled,
                "false", "", false, false,
                "Disable startup error prompts.", false
            }
        },
        {
            "verbose",
            {
                options::code_null, options::kind::boolean, options::enabled,
                "false", "", false, false,
                "Show informational messages.", false
            }
        }
    }
};


/*
 *  See inisections.hpp for a stock version of this initialization.
 *
 *      const std::string inifile_comment_section_name{"[comments]"};
 *      const std::string inifile_comment_description;
 *
 *  This item could be named "[session-file]". But this option really makes
 *  sense only from the command-line (or an environment variable?). It
 *  determines the "home" for the configuration, and can alter the base-name of
 *  the configuration files.
 *
 *  Note that an application under session management can override "home"
 *  and "data". The rest of the sections are not modifiable from the
 *  command-line:
 *
 *      -   [rc-file]. Seq66 provided an option "rc"/"f".
 *      -   [usr-file]. Seq66 provided an option "usr"/"F".
 *      -   [palette-file].
 *      -   [midi-ctrl-file],
 *      -   [mute-group-file].
 *      -   [note-mapper-file].
 *      -   [playlist-file].
 *
 *  We might consider making the names in these sections unique and thus
 *  overridable from the command line. For now, changing the 'session' file
 *  determines the set of other configuration files.
 *
 *  Other options to consider adding to 'session':
 *
 *      -   locale/L
 *      -   User/Z
 *      -   Native/z
 */


inisection::specification session_config_file_data
{
    "[config-file]",
    {
"This section sets up the locations of: user configuration area ('home');\n"
"the main configuration file or it's base-name ('config'); and the location\n"
"of the data. These items are all optional, and defaults might be specified\n"
"by the application or by a session manager.\n"
    },
    {
        {
            "active",
            {
                options::code_null, options::kind::boolean, options::enabled,
                "false", "", false, false,
                "Usage of configuration file; otherwise a default is used.",
                false
            }
        },
        {
            "home",                     /* provides the 'session' directory */
            {
                'H', options::kind::filename, options::enabled,
                "", "", false, false,
                "The main configuration/data area for the session.", false
            }
        },
        {
            "config",                   /* provides the 'session' file-name */
            {
                'c', options::kind::filename, options::enabled,
                "", "", false, false,
                "The actual main configuration file for the session.", false
            }
        },
        {
            "data",                     /* 'D' replaces "legacy-record"     */
            {
                'D', options::kind::filename, options::enabled,
                "", "", false, false,
                "Overrides the default data directory.", false
            }
        }
    }
};

/**
 *  The next one is a sample of an application-specific configuration
 *  file. The "[config-file]" settings above could be used as well.
 *
 *  The programmer is free to add as many sections like this as necessary.
 *  The same three variables must be supported.
 */

inisection::specification session_rc_file_data
{
    "[rc-file]",
    {
"This section specifies the status, directory, and name of the 'rc' file.\n"
"That file holds information about the I/O ports, transport settings, recent\n"
"files, and other options.\n"
    },
    {
        {
            "active",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Usage of the specified file.", false
            }
        },
        {
            "directory",
            {
                options::code_null, options::kind::filename, options::disabled,
                "", "", false, false,
                "The directory where this file is stored.", false
            }
        },
        {
            "rc",                       /* instead of the generic "name"    */
            {
                'f', options::kind::filename, options::enabled,
                "seq66v2.rc", "", false, false,
                "The base file-name of the ports/settings file.", false
            }
        },
        {
            "rc-save",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Saving of the 'rc' file at exit.", false
            }
        }
    }
};

inisection::specification session_usr_file_data
{
    "[usr-file]",
    {
"This section specifies the status, directory, and name of the 'usr' file.\n"
"That file holds information about port and channel naming, user-interface\n"
"settings, common MIDI tune settings, log file, daemonization, and more.\n"
    },
    {
        {
            "active",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Usage of the specified file.", false
            }
        },
        {
            "directory",
            {
                options::code_null, options::kind::filename, options::disabled,
                "", "", false, false,
                "The directory where this file is stored.", false
            }
        },
        {
            "usr",                      /* instead of the generic "name"    */
            {
                'F', options::kind::filename, options::enabled,
                "seq66v2.usr", "", false, false,
                "The base file-name of the user file.", false
            }
        },
        {
            "usr-save",
            {
                'u', options::kind::boolean, options::enabled,
                "false", "", false, false,
                "Saving of the 'usr' file at exit.", false
            }
        }
    }
};

/**
 *  Additional testing samples appropriated from Seq66.
 */

inisection::specification session_palette_file_data
{
    "[palette-file]",
    {
"This section specifies an alternate palette for the app-drawn GUI items\n"
"such as the piano rolls and live grid slots.\n"
    },
    {
        {
            "active",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Usage of the specified file.", false
            }
        },
        {
            "directory",
            {
                options::code_null, options::kind::filename, options::disabled,
                "", "", false, false,
                "The directory where this file is stored.", false
            }
        },
        {
            "name",
            {
                options::code_null, options::kind::filename, options::disabled,
                "seq66v2.palette", "", false, false,
                "The base file-name of the palette file.", false
            }
        }
    }
};

inisection::specification session_ctrl_file_data
{
    "[midi-ctrl-file]",
    {
"Specifies an alternate set of keystroke and MIDI control/display control\n"
"items.\n"
    },
    {
        {
            "active",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Usage of the specified file.", false
            }
        },
        {
            "directory",
            {
                options::code_null, options::kind::filename, options::disabled,
                "", "", false, false,
                "The directory where this file is stored.", false
            }
        },
        {
            "name",
            {
                options::code_null, options::kind::filename, options::disabled,
                "seq66v2.ctrl", "", false, false,
                "The base file-name of the 'ctrl' file.", false
            }
        }
    }
};

inisection::specification session_mutes_file_data
{
    "[mute-group-file]",
    {
"Specifies a set of pattern mute-group specifications for easy selection\n"
"of pattern arrangements.\n"
    },
    {
        {
            "active",
            {
                options::code_null, options::kind::boolean, options::enabled,
                "false", "", false, false,
                "Usage of the specified file.", false
            }
        },
        {
            "directory",
            {
                options::code_null, options::kind::filename, options::enabled,
                "", "", false, false,
                "The directory where this file is stored.", false
            }
        },
        {
            "name",
            {
                options::code_null, options::kind::filename, options::enabled,
                "seq66v2.mutes", "", false, false,
                "The base file-name of the 'mutes' file.", false
            }
        }
    }
};

inisection::specification session_drums_file_data
{
    "[note-mapper-file]",
    {
"Specifies a mapping using in changing drum notes to General MIDI.\n"
    },
    {
        {
            "active",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Usage of the specified file.", false
            }
        },
        {
            "directory",
            {
                options::code_null, options::kind::filename, options::disabled,
                "", "", false, false,
                "The directory where this file is stored.", false
            }
        },
        {
            "name",
            {
                options::code_null, options::kind::filename, options::disabled,
                "seq66v2.drums", "", false, false,
                "The base file-name of the 'drums' file.", false
            }
        }
    }
};

inisection::specification session_playlist_file_data
{
    "[playlist-file]",
    {
        "Specifies a play-list(s) file.\n"
    },
    {
        {
            "active",
            {
                options::code_null, options::kind::boolean, options::enabled,
                "false", "", false, false,
                "Usage of the specified file.", false
            }
        },
        {
            "directory",
            {
                options::code_null, options::kind::filename, options::enabled,
                "", "", false, false,
                "The directory where this file is stored.", false
            }
        },
        {
            "playlist",                 /* instead of the generic "name".   */
            {
                'X', options::kind::filename, options::enabled,
                "seq66v2.playlist", "", false, false,
                "The base file-name of the 'playlist' file.", false
            }
        }
    }
};

/*------------------------------------------------------------------------
 * All sections of the 'session' configuration
 *------------------------------------------------------------------------*/

inisection::specification session_comments = inifile_comment_data;

inisections::specification session_data
{
    "session",      /* the file extension for any 'session' file.           */
    "",             /* use value from appinfo's get_home_cfg_directory()    */
    "",             /* use value derived from appinfo's get_home_cfg_file() */
    "This file holds the main configuration for Cfg66-compliant applications.\n"
    "It follows a format similar to the old INI files of MS-DOS.\n"
    ,
    {
        std::ref(session_cfg66_data),
        std::ref(session_comments),     /* used by every Cfg66 config file  */
        std::ref(session_config_file_data),
        std::ref(session_rc_file_data),
        std::ref(session_usr_file_data),
        std::ref(session_palette_file_data),
        std::ref(session_ctrl_file_data),
        std::ref(session_mutes_file_data),
        std::ref(session_drums_file_data),
        std::ref(session_playlist_file_data)
    }
};

}           // namespace cfg

#endif      // CFG66_CFG_SESSION_SPEC_HPP

/*
* session_spec.hpp
*
* vim: sw=4 ts=4 wm=4 et ft=cpp
*/

