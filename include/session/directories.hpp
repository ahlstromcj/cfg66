#if ! defined CFG66_SESSION_DIRECTORIES_HPP
#define CFG66_SESSION_DIRECTORIES_HPP

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
 * \file          directories.hpp
 *
 *  This module declares/defines the base class for handling many facets
 *  of administering a set of directories for cfg66 usage.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2020-05-30
 * \updates       2024-09-27
 * \license       GNU GPLv2 or above
 *
 *  This class provides directories for a "session".  In this base
 *  implementation, a "session" is merely a single common location to put
 *  directories and files.
 *
 *  Together, session::directories and session::configuration (which contains
 *  the former) specify the layout of a basic (i.e. no session manager in
 *  play) session.
 *
 *  These directories can be specified by a "*.session" file or by a session
 *  manager.
 *
 *  Compare this module to the cfg/appinfo module, which does not support the
 *  concept of a session, but can provide the basic/default configuration
 *  directory expected in the operating system.
 *
 *  See the cpp banner for more information.
 */

#include <map>                          /* std::map<>                       */
#include <memory>                       /* std::shared_ptr<>, unique_ptr<>  */
#include <string>                       /* std::string class                */
#include <vector>                       /* std::vector template class       */

#include "cpp_types.hpp"                /* lib66::tokenization              */

namespace session
{

/**
 *  Provides the session directories.  This replaces many of the variables
 *  in the old Seq66 rcsettings class, though eventually these will be
 *  stored in the 'rc' in a completely compatible manner.
 */

class directories
{
    friend class manager;

public:

    /**
     *  Provides a unique pointer to a directories configuration.
     */

    using pointer = std::unique_ptr<directories>;

    /**
     *  Provides a supported directory entry for an application.
     *  Configuration files are also included, and should spur an
     *  attempt to read/write the file.
     */

    using entry = struct
    {
        std::string ent_section;       /**< Name of the config-file section.   */
        bool ent_active;               /**< True if entry is used in session.  */
        std::string ent_directory;     /**< The location of file(s), if given. */
        std::string ent_basename;      /**< Base name of the file, if given.   */
        std::string ent_extension;     /**< The optional extension of files.   */
    };

    /**
     *  Provides the directories set that the application will support.
     */

    using entries = std::vector<entry>;

    /**
     *  Provides the full path specification of each file, constructed
     *  from the entries, and keyed by the section name.
     */

    using filespecs = std::map<std::string, std::string>;

private:

    /**
     *  Holds the set of file/directory entries and the file-specifications
     *  constructed from them.
     *
     *  We need to see if making a copy is too expensive, or if we need a pointer
     *  instead.
     */

    entries m_file_entries;

    /**
     *  Provies the file-specifications, which can be looked up by the section
     *  name.
     */

    filespecs m_file_specs;

    /**
     *  The full expanded path to the files directory.  This value is
     *  created, by default, by concatenating:
     *
     *      -   m_session_path
     *
     *  However it can be reset completely by the home_config_path()
     *  functions to what ever the user needs (e.g. for usage with the
     *  Non/New Session Manager).
     */

    mutable std::string m_home_config_path;

    /**
     *  Holds the path to the application configuration directory specified
     *  by the session file parameter "[Cfg66] session-path". If not specified
     *  (the value is ""), then the default is the OS-specific name of the
     *  $HOME directory plus the application name. Examples:
     *
     *      -   Linux:       /home/user/.config/seq66v2
     *      -   NSM (Linux): /home/user/NSM Sessions/MySession/seq66v2.nRSIQ
     *      -   Windows:     C:\Users\user\AppData\Local\seq66v2
     *
     *  Note that "~" is equivalent to the OS-specific $HOME value. Also note
     *  that NSM sends the session directory to the application after it
     *  starts it.
     *
     *  The user can supply a totally different directory, if desired. For
     *  example, this might be the directory returned by the session manager
     *  as shown above.
     *
     *  This path serves as the initial path to the "logs", "config", and
     *  other directories (e.g. "midi", "audio") specified in the sections
     *  of the 'session' file.
     */

    std::string m_session_path;

    /**
     *  A cool flag that indicates the caller (e.g. one working to interact
     *  with a session manager like NSM) has provided its own home
     *  configuration directory. This overrides the configuration specified
     *  in the appinfo module (e.g. "~/.config/seq66v2" replaced by
     *  "/home/user/NSM Sessions/RockNRoll/seq66v2.nLKKY/").
     */

    bool m_session_path_override;

public:

    directories ();
    directories
    (
        const std::string & sessiondir,
        entries & fileentries
    );
    directories
    (
        const std::string & sessiondir,
        const lib66::tokenization & fileentries
    );
    directories (const directories &) = default;
    directories (directories &&) = delete;
    directories & operator = (const directories &) = default;
    directories & operator = (directories &&) = delete;
    virtual ~directories ();

public:

    bool session_path_override () const
    {
        return m_session_path_override;
    }

    const std::string & session_path () const
    {
        return m_session_path;
    }

    std::string home_config_path () const;
    std::string get_file_spec (const std::string & section) const;
    std::string get_file_spec
    (
        const std::string & section,
        const std::string & filebase
    ) const;

    entries & file_entries ()
    {
        return m_file_entries;
    }

    const entries & file_entries () const
    {
        return m_file_entries;
    }

    filespecs & file_specs ()
    {
        return m_file_specs;
    }

    const filespecs & file_specs () const
    {
        return m_file_specs;
    }

    bool add_entry (const entry & ent);
    bool add_entry
    (
        bool active,
        const std::string & subdir,
        const std::string & basename,
        const std::string & ext
    );

protected:

    entry split_filename (const std::string & fullpath);
    bool make_file_spec (const directories::entry & dentry);
    bool make_file_specs ();

protected:

    std::string filespec_helper (const std::string & base_ext) const;
    void home_config_path (const std::string & hcd);
    void home_config_path
    (
        const std::string & sesspath,       /* provided by session manager  */
        const std::string & configdir       /* e.g. "config"                */
    );

};          // class directories

}           // namespace session

#endif      // CFG66_SESSION_DIRECTORIES_HPP

/*
 * directories.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

