#if ! defined CFG66_CFG_APPINFO_HPP
#define CFG66_CFG_APPINFO_HPP

/*
 *  This file is part of cfg66.
 *
 *  cfg66 is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  cfg66 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with cfg66; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * \file          appinfo.hpp
 *
 *    This module summarizes or defines all of the configure and build-time
 *    options available for Seq66.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2018-11-24
 * \updates       2024-07-11
 * \license       GNU GPLv2 or above
 *
 *    Provides some useful functions for displaying information about the
 *    application.  More flexible than macros.
 */

#include <string>

#include "cpp_types.hpp"                /* string, vector, lib66::msglevel  */
#include "util/msgfunctions.hpp"        /* enum clas msglevel               */

namespace cfg
{

/**
 *  A mostly informational enumeration.
 */

enum class appkind
{
    indeterminate,
    headless,
    cli,
    curses,
    gui,
    test
};

/**
 *  Useful for encapsulating the most basic or common settings.
 */

struct appinfo
{
    friend bool initialize_appinfo (const appinfo &, const std::string &);

public:

    /**
     *  Indicates the type of application.
     */

    appkind _app_kind;

    /**
     *  Provides the short name for the application, usually the executable
     *  name (minus the ".exe" in Windows), such as "qcfg66".
     */

    std::string _app_name;

    /**
     *  Provides the short and long version information for the application.
     *  The short version is something like "0.99.2", and the long version is
     *  preceded by the app name, as in "qcfg66 0.99.2".
     */

    std::string _app_version;

    /**
     *  Holds the name of the main section in an INI file. It
     *  defaults to "[Cfg66]". Contains no options that set anything,
     *  nor are the options saved.
     */

    std::string _main_cfg_section_name;

    /**
     *  The caller normally sets this to an empty string to indicate
     *  the usual HOME configuration directory for the build platform.
     *  This is the directory directly under HOME, such as ".config"
     *  or "\AppData\Local". Or it can be completely changed.
     *
     *  Corresponds to the --home, -H options, and can be set by
     *  set_home_cfg_directory().
     */

    std::string _home_cfg_directory;

    /**
     *  Holds the name of the main configuration file, such as "myapp.rc".
     *
     *  Corresponds to the --config, -c options, and can be set by
     *  set_home_cfg_file().
     */

    std::string _home_cfg_file;

    /*
     *  The following members are all changed via set_client_name() and
     *  are based on the user-supplied/build-supplied client name.
     */

    /**
     *  Like the app-name, but it might be more generic (like "cfg66" vs.
     *  "qcfg66"), or have a wart or unique string appended to it.
     *  An example under NSM would be "cfg66.nUKIE".
     */

    std::string _client_name;

    /**
     *  Provides the short name of the application plus the version number
     *  of the application.  Set it via the build system.
     */

    std::string _app_tag;

    /**
     *  Holds the full path to the executable file for the application.
     */

    std::string _arg_0;

    /**
     *  Provides the name of the package, obtained from the build system.
     *  An example is "Seq66".
     */

    std::string _package_name;

    /**
     *  Useful in long error/warning/info messages. For example, as in
     *  a call to file_message(session_tag("path", pathname).
     */

    std::string _session_tag;                  /* shown with an adornment  */

    /**
     *  Provides the base name of the application icon, such as "qcfg66".
     *  If empty, then there is no application icon.
     */

    std::string _app_icon_name;

    /**
     *  See _app_version above.  If the caller leaves this empty, then it is
     *  reconstructed.
     */

    std::string _app_version_text;

    /**
     *  Indicates the API or framework for the main functionality (MIDI, audio,
     *  networking, database, etc.) of the application. For example, for a MIDI
     *  application it might be "rtmidi", "rtl66", or "portmidi".
     */

    std::string _api_engine;

    /**
     *  Provides the API version of this library.
     */

    std::string _api_version;

    /**
     *  Indicates the GUI version, such as "Qt 6.1" or "Gtkmm 3.0".
     *  Normally set via a call to a GUI function or by accessing a
     *  macro in the API header files.
     */

    std::string _gui_version;

    /**
     *  Provides the bare name of an application where it is a client of some
     *  kind of engine (such as JACK).  An example is "cfg66". Never has a
     *  wart appended to it.
     */

    std::string _client_name_short;

    /**
     *  Provides the name to show on the console in error/warning/info messages.
     *  It should be the "const client name" surrounded by brackets, such as
     *  "[cfg66]".
     */

    std::string _client_name_tag;

public:

    appinfo ();
    appinfo
    (
        appkind apptype,
        const std::string & appname,
        const std::string & appversion,
        const std::string & maincfgname,
        const std::string & homecfgdirectory,
        const std::string & homecfgfile,
        const std::string & clientname,
        const std::string & apptag,
        const std::string & arg0,
        const std::string & packagename,
        const std::string & sessiontag,
        const std::string & appiconname,
        const std::string & appversiontext,
        const std::string & apiengine,
        const std::string & apiversion,
        const std::string & guiversion,
        const std::string & clientnameshort,
        const std::string & clientnametag
    );
    appinfo (const appinfo &) = default;
    appinfo (appinfo &&) = default;
    appinfo & operator = (const appinfo &) = default;
    virtual ~appinfo () = default;

private:

    bool initialize (const std::string & arg0);

};

/*--------------------------------------------------------------------------
 * Global (free) functions.  Setters.
 *--------------------------------------------------------------------------*/

extern bool initialize_appinfo          /* copy to static appinfo           */
(
    const appinfo & source, const std::string & arg0
);
extern void set_home_cfg_directory (const std::string & v);
extern void set_main_cfg_section_name (const std::string & v);
extern void set_home_cfg_file (const std::string & v);
extern void set_gui_version (const std::string & v);
extern void set_app_build_os (const std::string & abuild_os);
extern void set_app_build_issue (const std::string & abuild_issue);
extern void set_api_engine (const std::string & aengine);
extern void set_app_name (const std::string & aname);
extern void set_app_version (const std::string & version);
extern void set_app_type (const std::string & atype);
extern void set_app_cli (bool iscli);
extern void set_app_headless (bool isheadless);
extern void set_arg_0 (const std::string & arg);
extern void set_client_name (const std::string & cname);
extern void set_package_name (const std::string & pname);
extern void set_session_tag (const std::string & pname);

/*--------------------------------------------------------------------------
 * Global (free) functions.  Getters.
 *--------------------------------------------------------------------------*/

extern const std::string & get_home ();
extern std::string get_home_cfg_directory ();
extern std::string get_main_cfg_section_name ();
extern std::string get_home_cfg_file ();
extern std::string get_home_cfg_filespec ();
extern const std::string & get_app_name ();
extern const std::string & get_app_tag ();
extern std::string get_app_type ();
extern const std::string & get_arg_0 ();
extern const std::string & get_client_name ();
extern const std::string & get_client_name_tag ();
extern const std::string & get_client_short ();
extern const std::string & get_icon_name ();
extern const std::string & get_package_name ();
extern const std::string & get_api_version ();
extern const std::string & get_api_subdirectory ();
extern const std::string & get_app_version ();
extern const std::string & get_app_version_text ();
extern const std::string & get_gui_version ();
extern const char * level_color (int index);
extern std::string get_client_tag (lib66::msglevel el = lib66::msglevel::none);
extern std::string get_session_tag (const std::string & refinement = "");
extern std::string get_build_details ();
extern bool get_app_cli ();
extern bool get_app_headless ();
extern bool is_a_tty ();
extern bool is_a_tty (int fd);

}           // namespace cfg

#endif      // CFG66_CFG_APPINFO_HPP

/*
 * appinfo.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

