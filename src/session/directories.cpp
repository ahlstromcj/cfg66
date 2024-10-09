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
 * \file          directories.cpp
 *
 *  This module declares/defines a module for managing generic cfg66
 *  session directories and files.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2020-03-22
 * \updates       2024-09-27
 * \license       GNU GPLv2 or above
 *
 *  This class provides directories and files for a "session".  In this base
 *  implementation, a "session" is merely a single common location to put
 *  directories and files.
 *
 *  The first item needed comes from the "[Cfg66]:home" variable, which might
 *  be obtained from cfg::appinfo (and is called "$home", and can be modified
 *  by the "--home" command-line option), or be received from a session
 *  manager.
 *
 *  Configuration name components:
 *
 *   <--- m_session_path ----->
 *   /home/user/.config/seq66v2/seq66v2.session
 *            ^         ^       ^
 *            |         |       |
 *            |         |        ------------ 'session' file
 *            |         |-------------------- m_session_path
 *            |          -------------------- m_home_config_path
 *             ------------------------------ $HOME
 *
 *   <--- Provided by NSM Server ------------------>
 *   <--- m_session_path -------------------------->
 *   /home/user/NSM Sessions/RockNRoll/seq66v2.nLKKY/logs/jack.log
 *   /home/user/NSM Sessions/RockNRoll/seq66v2.nLKKY/rc/jack.rc
 *   /home/user/NSM Sessions/RockNRoll/seq66v2.nLKKY/midi (many files)
 *
 *  For brevity, let $HOME represent the configuration home directory.
 *  Then, given a list of directories::entries structures, then we might
 *  have a list of full paths like the following, for all active entries:
 *
 *      -   $HOME/seq66v2.rc or $HOME/seq66v2/cfg/seq66v2.rc
 *      -   $HOME/seq66v2.usr or $HOME/seq66v2/cfg/seq66v2.usr
 *      -   $HOME/logs/seq66v2.log
 *
 * Naming conventions:
 *
 *  "~"                 The HOME directory of the user, in Linux or Windows
 *  "$HOME"             HOME directory of the user
 *  "$home"             The directory holding all session files. It is assumed
 *                      if a specified directory name is "" or ".".
 *  "$prefix"           The parent installation location for the application.
 *
 *  The programmer should adopt a convention for subdirectories and
 *  app-name versus other-name, and stick with it.
 *
 * High-level process:
 *
 *      -   As early as possible, get the "home" configuration directory:
 *          -   As set by the application using appinfo's
 *              set_home_cfg_directory().
 *          -   As modified via the command-line options.
 *          -   As sent to the application by the session manager.
 *          Also get the main section name (usually [Cfg66]) and the
 *          name of the session file for later use.
 *
 * Operations to support:
 *
 *      -   Get the value of "home" from the main section of the session file,
 *          or use the value provided above.
 *      -   Get the values in the [cfg] section of the .session file.
 *          -   Append each to $home
 *          -   Make the directory.
 */

#include "platform_macros.h"            /* PLATFORM_UNIX, etc.              */
#include "cfg/appinfo.hpp"              /* cfg::get_app_name(), etc.        */
#include "session/directories.hpp"      /* sessions::directories class      */
#include "util/msgfunctions.hpp"        /* util::msgprintf()                */
#include "util/filefunctions.hpp"       /* util::file_readable() etc.       */

/*
 *  Do not document a namespace; it breaks Doxygen.
 */

namespace session
{

/**
 *  Default constructor.  Uses functions from the appinfo module.
 */

directories::directories () :
    m_file_entries          (),
    m_file_specs            (),
    m_home_config_path      (cfg::get_home_cfg_directory()),
    m_session_path          (cfg::get_home_cfg_directory()),
    m_session_path_override (false)
{
    (void) make_file_specs();
}

/**
 *  Fills in the parts of the path names.
 *
 * \param sessiondir
 *      The root of all evil.  Provides the application's "home" directory,
 *      where the configuration, logs, and optionally, other app-related
 *      directories will reside:
 *      -   User-supplied: "/home/user/MyStuff/Music/config" is an example.
 *      -   Session manager: The Non/New Session Manager will provide its
 *          own path to the session (configuration and application data).
 *      -   Empty:  the appinfo function cfg::get_home_cfg_directory() yields
 *          "$HOME/.config/appname"
 *      The m_session_path_override member is set to true if a hardwired path
 *      has been provided.  the directory immediately under \a sessiondir, such
 *      as "seq66" is included.
 *
 * \param fileentries
 *      Provides a list of file entry structures.
 */

#if defined USE_DIRECTORIES_ENTRIES_CTOR

directories::directories
(
    const std::string & sessiondir,
    directories::entries & fileentries
) :
    m_file_entries          (fileentries),
    m_file_specs            (),
    m_home_config_path      (cfg::get_home_cfg_directory()),
    m_session_path          (sessiondir),
    m_session_path_override (false)
{
    if (sessiondir.empty())
        m_session_path = cfg::get_home_cfg_directory();  /* ~/.config/seq66v2    */
    else
        m_session_path_override = true;     /* ~/NSM Sessions/X/seq66.nXYZ  */

    (void) make_file_specs();
}

#endif

directories::directories
(
    const std::string & sessiondir,
    const lib66::tokenization & fileentries
) :
    m_file_entries          (),
    m_file_specs            (),
    m_home_config_path      (cfg::get_home_cfg_directory()),
    m_session_path          (sessiondir),
    m_session_path_override (false)
{
    for (const auto & f : fileentries)
    {
        entry e = split_filename(f);
        m_file_entries.push_back(e);

        bool ok = make_file_spec(e);
        if (! ok)
            break;
    }
}

/**
 *  This might be a good place to save the files. Or not.
 */

directories::~directories ()
{
    // no code so far
}

bool
directories::add_entry (const entry & ent)
{
    file_entries().push_back(ent);
    return ent.ent_active;
}

bool
directories::add_entry
(
    bool active,
    const std::string & subdir,
    const std::string & basename,
    const std::string & ext
)
{
    bool result = ext.front() == '.';
    if (result)
    {
        entry e;
        e.ent_section = ext.substr(1);
        e.ent_active = active;
        e.ent_directory = subdir;
        e.ent_basename = basename;
        e.ent_extension = ext;
        result = add_entry(e);
    }
    return result;
}

/**
 *  Extracts components of a full path file-specification into an
 *  active entry (if valid)
 */

directories::entry
directories::split_filename (const std::string & fullpath)
{
    entry result;
    std::string path;                           /* can end up empty         */
    std::string filebare;                       /* will not have extension  */
    std::string ext;                            /* can end up empty         */
    bool ok = util::filename_split_ext(fullpath, path, filebare, ext);
    if (ok)
    {
        if (ext.empty())
        {
            ok = false;
        }
        else
        {
            ok = ext.front() == '.';
            if (ok)
            {
                result.ent_section = ext.substr(1);
                result.ent_extension = ext;
                result.ent_directory = path;
                result.ent_basename = filebare;
                result.ent_extension = ext;
            }
        }
    }
    result.ent_active = ok;
    return result;
}

/**
 *  Adds a directory entry to the list, and creates the directory as well.
 */

bool
directories::make_file_spec (const directories::entry & dentry)
{
    bool result = dentry.ent_active;
    if (result)
    {
        std::string directory = dentry.ent_directory;
        std::string basename = dentry.ent_basename;
        std::string extension = dentry.ent_extension;
        if (directory.empty())
        {
            directory = m_session_path; // cfg::get_home_cfg_directory();
        }
        else if (! util::name_has_path(directory))
        {
            std::string temp = m_session_path;  // cfg::get_home_cfg_directory();
            directory = util::filename_concatenate(temp, directory);
        }

        /*
         * Before we append the file-name, make the directory as desired to
         * make sure it works.
         */

        result = util::make_directory_path(directory);
        if (result)
        {
            if (basename.empty())
                basename = cfg::get_app_name();

            if (extension.empty())
                extension = "." + dentry.ent_section;

            basename += extension;

            std::string filepath =
                util::filename_concatenate(directory, basename);

            auto p = std::make_pair(dentry.ent_section, filepath);
            auto r = m_file_specs.insert(p);
            result = r.second;
        }
    }
    return result;
}

bool
directories::make_file_specs ()
{
    bool result = false;
    for (const auto & specentry : m_file_entries)
    {
        result = make_file_spec(specentry);
        if (! result)
            break;
    }
    return result;
}

/**
 *  Looks up the full configuration file specification for the file defined by
 *  the section name.
 *
 * \param section
 *      Provides the section of the session file that defines the file
 *      specification to be obtained.
 *
 *  \return
 *      Returns a copy of the file specification looked up from the file
 *      specification map.
 */

std::string
directories::get_file_spec (const std::string & section) const
{
    std::string result;
    auto r = m_file_specs.find(section);
    if (r != m_file_specs.end())
        result = r->second;

    return result;
}

/**
 *  Looks up the full configuration file specification for the file defined by
 *  the section name, then replaces the actual file with another file. This
 *  function is useful when changing the file to use on the fly.
 *
 * \param section
 *      Provides the section of the session file that defines the file
 *      specification to be obtained.
 *
 * \param filebase
 *      Provides the base-name of the file (e.g. "alternate.rc") to be used.
 *
 *  \return
 *      Returns a copy of the file specification looked up from the file
 *      specification map, but with the provided file base-name.
 */

std::string
directories::get_file_spec
(
    const std::string & section,
    const std::string & filebase
) const
{
    std::string result;
    auto r = m_file_specs.find(section);
    if (r != m_file_specs.end())
        result = util::file_base_set(r->second, filebase);

    return result;
}

/**
 *  Provides the directory for the sesson files, and also creates the
 *  directory if necessary.
 *
 *  The home directory is (in Linux) "/home/username/.config/seq66v2", and
 *  the files are "seq66v2.*" by default.
 *
 *  This function should also adapt to Windows conventions automatically.
 *  We shall see.  No, it does not.  But all we have to do is replace
 *  Window's HOMEPATH with its LOCALAPPDATA value.
 *
 * getenv(HOME):
 *
 *      -   Linux returns "/home/ahlstrom".  Append "/.config/seq66".
 *      -   Windows returns "\Users\ahlstrom".  A better value than HOMEPATH
 *          is LOCALAPPDATA, which gives us most of what we want:
 *          "C:\Users\ahlstrom\AppData\local", and then we append simply
 *          "seq66".
 *
 * \return
 *      Returns the created home files directory name.
 */

std::string
directories::home_config_path () const
{
    return m_home_config_path;
}

void
directories::home_config_path (const std::string & hcd)
{
    m_home_config_path = util::os_normalize_path(hcd);
}

void
directories::home_config_path
(
    const std::string & sesspath,       /* provided by session manager  */
    const std::string & configdir       /* e.g. "config"                */
)
{
    std::string path = util::pathname_concatenate(sesspath, configdir);
    m_home_config_path = util::os_normalize_path(path);
}

/**
 *  IS THIS STILL USEFUL???
 *
 *  We need a way to handle the following files-file paths:
 *
 *      -   "base.ext": Prepend the HOME directory.
 *      -   "subdirectory/base.ext": Prepend the HOME directory
 *      -   "/path/.../base.ext": Use the path as is.
 *      -   "C:/path/.../base.ext": Use the path as is.
 *
 *  Compare to make_config_filespec(), used only by rcfile and this class.
 *  That one treats the base and extension separately.
 */

std::string
directories::filespec_helper (const std::string & base_ext) const
{
    std::string result = base_ext;
    if (! result.empty())
    {
        bool use_as_is = false;
        if (util::name_has_path(base_ext))
        {
            if (util::name_has_root_path(base_ext))
                use_as_is = true;
        }
        if (! use_as_is)
        {
            result = home_config_path();
            result += base_ext;
        }
        result = util::os_normalize_path(result);   /* change to OS's slash */
    }
    return result;
}

}           // namespace session

/*
 * directories.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

