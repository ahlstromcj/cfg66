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
 * \file          appinfo.cpp
 *
 *  This module adds some functions that reveal the features compiled into
 *  the Seq66 application.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2017-03-12
 * \updates       2024-07-24
 * \license       GNU GPLv2 or above
 *
 *  The first part of this file defines a couple of global structure
 *  instances, followed by the global variables that these structures
 *  completely replace.
 */

#include <cstdlib>                      /* _fullpath(), getenv()            */
#include <sstream>                      /* std::ostringstream               */

#include "platform_macros.h"            /* detect the build platform        */
#include "cfg/appinfo.hpp"              /* application information          */

#if defined PLATFORM_UNIX
#include <unistd.h>                     /* C::isatty(3), STDOUT_FILENO, etc */
#endif

#if defined PLATFORM_WINDOWS

#include <io.h>                         /* C::_isatty() for Windows         */

#define STDIN_FILENO    0
#define STDOUT_FILENO   1
#define STDERR_FILENO   2

#endif

namespace cfg
{

/**
 *  The default path to the configuration files.  The circumflex (~) is
 *  converted to match the user's HOME directory as in:
 *
 *      -   Linux:      /home/username
 *      -   Windows:    C:/Users/username
 *
 *  See the functions util::normalize_path() and util::user_home().
 *
 *  Rarely needs to be overridden in derived implementations.
 *
 *  Does PLATFORM_UNIX cover MacOSX as well?
 */

#if defined PLATFORM_UNIX
static std::string s_path_separator     = "/";
#elif defined PLATFORM_WINDOWS
static std::string s_path_separator     = "\\";
#else
static std::string s_path_separator     = "/";
#endif


/**
 *  Hard-wired replacements for build macros.  Also modifiable at run-time
 *  via the "set" functions.  The build OS is obvious, but the build issue
 *  is an elaboration of that.  For example, it might be the contents of
 *  /etc/issue (such as "Ubuntu 18.04.6 LTS").
 */

#if defined PLATFORM_WINDOWS
static std::string s_app_build_os    = "Windows 10";    /* FIXME    */
static std::string s_app_build_issue = "Microsoft Windows";
#endif

#if defined PLATFORM_MACOSX
static std::string s_app_build_os    = "MacOSX";        /* FIXME    */
static std::string s_app_build_issue = "Apple MacOSX";
#endif

#if defined PLATFORM_UNIX
#if defined PLATFORM_LINUX
static std::string s_app_build_os    = "Linux";
static std::string s_app_build_issue = "Linux";         /* FIXME    */
#elif defined PLATFORM_FREEBSD
static std::string s_app_build_os    = "FreeBSD";
static std::string s_app_build_issue = "FreeBSD";       /* FIXME    */
#else
static std::string s_app_build_os    = "UNIX";
static std::string s_app_build_issue = "UNIX";          /* FIXME    */
#endif
#endif

/**
 *  A singleton appinfo.  See the initialize_appinfo() function.
 */

static appinfo &
app_info ()
{
    static appinfo s_app_info;
    return s_app_info;
}

/**
 *  Default constructor.
 */

appinfo::appinfo () :
    _app_kind               (appkind::indeterminate),
    _app_name               ("app"),
    _app_version            ("0"),
    _main_cfg_section_name  ("[Cfg66]"),
    _home_cfg_directory     (),
    _home_cfg_file          (),
    _client_name            ("app"),
    _app_tag                ("app-0"),
    _arg_0                  (),
    _package_name           ("NOPACKAGE"),
    _session_tag            (),
    _app_icon_name          (),
    _app_version_text       ("app v. 0"),
    _api_engine             (),
    _api_version            (),
    _gui_version            (),
    _client_name_short      ("app"),
    _client_name_tag        ()
{
    // no other code
}

/**
 *  Initializer list constructor.
 */

appinfo::appinfo
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
) :
    _app_kind               (apptype),
    _app_name               (appname),
    _app_version            (appversion),
    _main_cfg_section_name  (maincfgname),
    _home_cfg_directory     (homecfgdirectory), // can be empty
    _home_cfg_file          (homecfgfile),      // can be empty
    _client_name            (clientname),
    _app_tag                (apptag),
    _arg_0                  (arg0),
    _package_name           (packagename),
    _session_tag            (sessiontag),
    _app_icon_name          (appiconname),      // sometimes empty
    _app_version_text       (appversiontext),   // often reconstructed
    _api_engine             (apiengine),        // often empty
    _api_version            (apiversion),       // often empty
    _gui_version            (guiversion),       // usually filled in later
    _client_name_short      (clientnameshort),  // usually constructed
    _client_name_tag        (clientnametag)     // usually constructed
{
    // no other code
}

bool
appinfo::initialize (const std::string & arg0)
{
    std::string cname = _client_name;           /* might have a wart        */
    if (cname.empty())
        cname = _app_name;

    set_client_name(_client_name);
    _arg_0 = arg0;
    if (_app_version.empty())
        _app_version = "0.0";

    if (_api_engine.empty())
        _api_engine = "N/A";

    if (_app_version_text.empty())
    {
        if (! _app_name.empty())
        {
            _app_version_text = _app_name + " " + _app_version;
            _app_tag = _app_name + "-" + _app_version;
        }
    }
    return true;
}

/*------------------------------------------------------------------------
 * Global free functions. Setters.
 *------------------------------------------------------------------------*/

bool
initialize_appinfo (const appinfo & source, const std::string & arg0)
{
    app_info() = source;
    app_info().initialize(arg0);
    return true;
}

void
set_home_cfg_directory (const std::string & v)
{
    app_info()._home_cfg_directory = v;
}

void
set_main_cfg_section_name (const std::string & v)
{
    app_info()._main_cfg_section_name = v;
}

void
set_home_cfg_file (const std::string & v)
{
    app_info()._home_cfg_file = v;
}

void
set_gui_version (const std::string & v)
{
    app_info()._gui_version = v;
}

/**
 *  Sets the current name of the application.
 */

void
set_app_name (const std::string & aname)
{
    app_info()._app_name = aname;
}

/**
 *  Sets the current version of the application.
 */

void
set_app_version (const std::string & version)
{
    app_info()._app_version = version;
}

/**
 *  Sets the current type of the application.
 */

void
set_app_type (const std::string & atype)
{
    appkind temp = appkind::indeterminate;
    if (atype == "headless")
        temp = appkind::headless;
    else if (atype == "cli")
        temp = appkind::cli;
    else if (atype == "curses")
        temp = appkind::curses;
    else if (atype == "gui")
        temp = appkind::gui;
    else if (atype == "test")
        temp = appkind::test;

    app_info()._app_kind = temp;
}

void
set_app_cli (bool iscli)
{
    if (iscli)
        app_info()._app_kind = appkind::cli;
    else
        app_info()._app_kind = appkind::indeterminate;
}

void
set_app_headless (bool isheadless)
{
    if (isheadless)
        app_info()._app_kind = appkind::headless;
    else
        app_info()._app_kind = appkind::indeterminate;
}

void
set_api_engine (const std::string & aengine)
{
    app_info()._api_engine = aengine;
}

void
set_app_build_os (const std::string & abuild_os)
{
    s_app_build_os = abuild_os;
}

void
set_app_build_issue (const std::string & abuild_issue)
{
    s_app_build_issue = abuild_issue;
}

void
set_arg_0 (const std::string & arg)
{
    app_info()._arg_0 = arg;
}

void
set_client_name (const std::string & cname)
{
    auto pos = cname.find_first_of("./:");      /* session delimiters       */
    app_info()._client_name = cname;            /* base name of client      */
    app_info()._client_name_short = cname;
    if (pos != std::string::npos)               /* strip off the wart       */
        app_info()._client_name_short = cname.substr(0, pos);

    app_info()._client_name_tag = "[";
    app_info()._client_name_tag += app_info()._client_name_short;
    app_info()._client_name_tag += "]";
}

void
set_package_name (const std::string & pname)
{
    app_info()._package_name = pname;
}

void
set_session_tag (const std::string & sname)
{
    app_info()._session_tag = sname;
}

/*------------------------------------------------------------------------
 * Global free functions. Getters.
 *------------------------------------------------------------------------*/

/**
 *  This function obtains the HOME directory from the user's environment.
 *  It is not the configuration file path.
 *
 *  -   Linux/UNIX: $HOME, /home/username
 *  -   Mac OSX:    $HOME
 *  -   Windows
 *
 *  In this library, the "~" character is converted via this function.
 *
 * \return
 *      Returns the HOMe path path.
 */

const std::string &
get_home ()
{
    static bool s_got_home = false;
    static std::string s_home = "";
    if (! s_got_home)
    {
#if defined PLATFORM_UNIX
        char * env = std::getenv("HOME");
        if (not_nullptr(env))
        {
            s_got_home = true;
            s_home = std::string(env);
        }
#elif defined PLATFORM_WINDOWS
        char * envdrv = std::getenv("HOMEDRIVE");
        char * envpath = std::getenv("HOMEPATH");
        if (not_nullptr_2(envdr, envpath))
        {
            s_got_home = true;
            s_home = envdrv;
            s_home += s_path_separator;
            s_home += envpath;
        }
#endif
    }
    return s_home;
}

/**
 *  The home configuration path is the concatenation of:
 *
 *      -   HOME
 *      -   _home_cfg_directory
 *      -   _app_name
 */

std::string
get_home_cfg_directory ()
{
    std::string result = get_home();
    if (app_info()._home_cfg_directory.empty())
    {
        result = get_home();
        result += s_path_separator;
#if defined PLATFORM_UNIX
        result += ".config/";
#elif defined PLATFORM_WINDOWS
        result += "\\AppData\\Local\\";
#endif
        result += app_info()._app_name;
    }
    else
        result = app_info()._home_cfg_directory;

    return result;
}

std::string
get_main_cfg_section_name ()
{
    return app_info()._main_cfg_section_name;
}

std::string
get_home_cfg_file ()
{
    return app_info()._home_cfg_file;
}

std::string
get_home_cfg_filespec ()
{
    std::string result = get_home_cfg_directory();
    result += s_path_separator;
    result += app_info()._home_cfg_file;
    return result;
}

/**
 *  Returns the name of the application.  We could continue to use the macro
 *  APP_NAME, but we might eventually want to make this name
 *  configurable.  Not too likely, but possible.
 */

const std::string &
get_app_name ()
{
    return app_info()._app_name;
}

std::string
get_app_type ()
{
    std::string result;
    switch (app_info()._app_kind)
    {
    case appkind::cli:      result = "cli";         break;
    case appkind::gui:      result = "gui";         break;
    case appkind::headless: result = "headless";    break;
    case appkind::curses:   result = "curses";      break;
    case appkind::test:     result = "test";        break;
    default:                result = "unknown";     break;
    }
    return result;
}

bool
get_app_cli ()
{
    return app_info()._app_kind == appkind::cli;
}

bool
get_app_headless ()
{
    return app_info()._app_kind == appkind::headless;
}

const std::string &
get_app_build_os ()
{
    return s_app_build_os;
}

const std::string &
get_app_build_issue ()
{
    return s_app_build_issue;
}

const std::string &
get_arg_0 ()
{
    return app_info()._arg_0;
}

/**
 *  Returns the name of the client for the application.  It starts as the
 *  macro CLIENT_NAME ("cfg66"), but this name is now configurable.
 *  When session management is active, the session-manager's client ID, or
 *  something derived from it, is copied to this variable.
 */

const std::string &
get_client_name ()
{
    return app_info()._client_name;
}

const std::string &
get_client_name_tag ()
{
    return app_info()._client_name_tag;
}

const std::string &
get_client_short ()
{
    return app_info()._client_name_short;
}

const std::string &
get_app_icon_name ()
{
    return app_info()._app_icon_name;
}

/**
 *  Checks if the file-descriptor matches a TTY descriptor. The
 *  value -1 is used for STDOUT_FILENO so that most callers don't
 *  need to know it.
 */

bool
is_a_tty ()
{
    return is_a_tty(STDOUT_FILENO);
}

bool
is_a_tty (int fd)
{
    if (fd < 0)
        fd = STDOUT_FILENO;

#if defined PLATFORM_WINDOWS
    int fileno;
    switch (fd)
    {
        case STDIN_FILENO:  fileno = _fileno(stdin);    break;
        case STDOUT_FILENO: fileno = _fileno(stdout);   break;
        case STDERR_FILENO: fileno = _fileno(stderr);   break;
        default:            fileno = (-1);              break;
    }
    int rc = (fileno >= 0) ? _isatty(fileno) : 90 ;
    return rc == 1;                             /* fd refers to a terminal  */
#else
    int rc = isatty(fd);
    return rc == 1;                             /* fd refers to a terminal  */
#endif
}

/**
 *  Returns the terminal escape sequence to generate colored text.
 *  There's no consistent association with the main terminal color
 *  codes (e.g. 0 to 7).
 */

const std::string &
level_color (int index)
{
    static const std::string s_level_colors [] =
    {
        "\033[0m",          /* 0: goes back to normal console color */
        "\033[1;32m",       /* 1: info message green                */
        "\033[1;33m",       /* 2: warning message is yellow         */
        "\033[1;31m",       /* 3: error message is red              */
        "\033[1;34m",       /* 4: status message is blue            */
        "\033[1;36m",       /* 5: session message is cyan           */
        "\033[1;30m"        /* 6: debug message is black            */
    };
    if (index < 0 || index > 6)
        index = 0;

    return s_level_colors[index];
}

/**
 * Text color codes ('*' indicates the color is used below):
 *
 *  -   30 = black *
 *  -   31 = red *
 *  -   32 = green *
 *  -   33 = yellow *
 *  -   34 = blue *
 *  -   35 = magenta *
 *  -   36 = cyan
 *  -   37 = white
 */

std::string
get_client_tag (lib66::msglevel el)
{
    if (el == lib66::msglevel::none)
    {
        return app_info()._client_name_tag;
    }
    else
    {
        std::string result = "[";
        int index = static_cast<int>(el);
        bool iserror = el == lib66::msglevel::error ||
            el == lib66::msglevel::warn ||
            el == lib66::msglevel::debug;

        bool showcolor = is_a_tty(iserror ? STDERR_FILENO : STDOUT_FILENO);
        if (showcolor)
            result += level_color(index);

        result += get_client_short();
        if (showcolor)
            result += level_color(0);

        result += "]";
        return result;
    }
}

const std::string &
get_icon_name ()
{
    return app_info()._app_icon_name;
}

/**
 *  Returns the name of the package for the application. This is the name of
 *  the product ("Seq66") no matter what executable has been generated.
 */

const std::string &
get_package_name ()
{
    return app_info()._package_name;
}

std::string
get_session_tag (const std::string & refinement)
{
    std::string result = app_info()._session_tag;
    if (! refinement.empty())
    {
        result += " ";
        result += refinement;
    }
    return result;
}

/**
 *  Returns the name and version information of the application API,
 *  if applicable.
 */

const std::string &
get_api_engine ()
{
    return app_info()._api_engine;
}

const std::string &
get_api_version ()
{
    return app_info()._api_version;
}

const std::string &
get_api_subdirectory ()
{
    static bool s_uninitialized = true;
    static std::string s_subdirectory;
    if (s_uninitialized)
    {
        s_uninitialized = false;
        s_subdirectory = app_info()._client_name_short;
        s_subdirectory += "-";
        s_subdirectory += get_api_version();
    }
    return s_subdirectory;
}

const std::string &
get_app_version ()
{
    return app_info()._app_version;
}

/**
 *  Sets up the "hardwired" version text for the application.  This value
 *  ultimately comes from the build script (for now).
 *
 *  TODO:
 *  It holds, among other things, the hand-crafted date in that file.
 */

const std::string &
get_app_version_text ()
{
    return app_info()._app_version_text;
}

const std::string &
get_gui_version ()
{
    return app_info()._gui_version;
}

const std::string &
get_app_tag ()
{
    return app_info()._app_tag;
}

/**
 *  This section of variables provide static information about the options
 *  enabled or disabled during the build.
 */

#if defined PLATFORM_32_BIT
const static std::string s_bitness = "32-bit";
#else
const static std::string s_bitness = "64-bit";
#endif

/**
 *  Generates a string describing the features of the build.
 *
 * \return
 *      Returns an ordered, human-readable string enumerating the built-in
 *      features of this application.
 */

std::string
get_build_details ()
{
    std::ostringstream result;
#if defined PLATFORM_DEBUG
    std::string buildmode = "Debug";
#else
    std::string buildmode = "Release";
#endif

    result
        << "Built " << __DATE__ << " " << __TIME__ "\n"
        << "C++ version " << std::to_string(__cplusplus) << "\n"
#if defined PLATFORM_GNU
        << "GNU C++ " << __GNUC__ << "." << __GNUC_MINOR__
        << "." << __GNUC_PATCHLEVEL__ << "\n"
#endif
        << "Version: " << get_app_version_text() << "\n"
        << "App tag: " << get_app_tag() << "\n"
        << "Executable: " << get_app_name()
        << "; " << get_app_type() << " interface"
        << "; " << get_api_engine() << " "
        << get_api_version() << " engine" << "\n"
        ;

    result
        << "User Interface: " << get_gui_version() << "\n";
        ;

    result
        << "Package: " << get_package_name() << "\n"
        << "Client: " << get_client_name() << "\n"
        << "Tag: " << get_client_name_tag() << "\n"
        ;

    if (! get_icon_name().empty())
        result << "Icon: " << get_icon_name() << "\n";

    result
        << "Build OS: " << get_app_build_os() << "\n"
        << "Build Type: " << s_bitness << " " << buildmode << "\n"
        ;

    result << "Platform: " << get_app_build_issue() << "\n";
    result << std::endl;
    return result.str();
}

/**
 *  Gets the basic runtime information about the application.
 *
 *  Note that get_home_cfg_filespec() is just a concatenation of
 *  get_home_cfg_directory() and get_home_cfg_file().
 *
 * To do:
 *
 *      -   Add the app "kind".
 */

std::string
get_runtime_details ()
{
    std::ostringstream result;
    result
        << "Command invocation: " << get_arg_0() << "\n"
        << "HOME environment value: " << get_home() << "\n"
        << "Home config directory: " << get_home_cfg_directory() << "\n"
        << "Home config file: " << get_home_cfg_file() << "\n"
        ;
    if (! get_session_tag().empty())
        result << "Session tag: " << get_session_tag() << "\n";

    result << std::endl;
    return result.str();
}

}           // namespace cfg

/*
 * appinfo.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

