#if ! defined CFG66_UTIL_FILEWALKER_HPP
#define CFG66_UTIL_FILEWALKER_HPP

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
 * \file          filewalker.hpp
 *
 *    This module provides a different version of ftswalker using the
 *    <filesystem> API of C++17.
 *
 * \library       filewalker
 * \author        Chris Ahlstrom
 * \date          2026-04-02
 * \updates       2026-04-11
 *
 * \version       $Revision$
 * \license       GNU GPL v2 or above
 *
 *  Use the filewalker class when dealing with multiple directory/file
 *  entries found in search-paths, and when desiring to extend the
 *  functionality by defining callback functions to pass to the
 *  filewalker.
 *
 *  Use the "free" functions declared below for specific items.
 *
 *  There is a good deal of functional overlap between filewalker and
 *  ftswalker.
 *
 *  There is some functional overlap between the free functions in the
 *  filefunctions module and the functions in this module.
 */

#include <filesystem>                   /* directory-file functions         */
#include <map>                          /* std::multimap<>                  */
#include <string>

#include "cpp_types.hpp"                /* lib66::tokenization              */

namespace util
{

class filewalker
{

public:

    /**
     *
     */

    /**
     *  A function that can be called to process each file or directory
     *  found.
     *
     *  std::filesystem::file_type indicates the type of a file.
     *  Replaces ftswalker::FTS.
     */

    using function = bool (*)
    (
        const std::string &,            /* name of source file/directory    */
        std::filesystem::file_type      /* type of file, or an error        */
    );

    /**
     *  A function that can be called to process files/directories from
     *  one folder to another folder.
     */

    using bifunction = bool (*)
    (
        const std::string &,            /* name of source file/directory    */
        const std::string &,            /* destination directory            */
        std::filesystem::file_type      /* type of file, or an error        */
    );

    /**
     *  The "compar()" function used in the fts_open() function.
     */

    using comparator = int (*)
    (
        const std::filesystem::directory_entry & first,  // const FTSENT **
        const std::filesystem::directory_entry & second  // const FTSENT **
    );

    /**
     *
     */

    using pair = struct
    {
        std::filesystem::file_type pfs_type;
        std::string pfs_name;
    };

    /**
     *  The key to this multimap is the depth of a file (which ranges
     *  from 0 on up). We want to handle deeper files and directories
     *  first, so will use a reverse iterator.
     */

    using pairs = std::multimap<int, pair>;

private:

    /**
     *  The set of directories to search whenever a file needs to be found.
     *  We could use util::searchpath, which uses a tokenization and adds
     *  functions and operators to manipulate the PATH.
     */

    lib66::tokenization m_search_paths;

public:

    filewalker () = default;
    filewalker (const std::string & path);
    filewalker (const lib66::tokenization & paths);
    filewalker (filewalker &&) = default;
    filewalker & operator = (filewalker &&) = default;
    ~filewalker () = default;

    bool find_files
    (
        const std::string & target,
        lib66::tokenization & destination,
        bool notregex = false
    );
    bool find_regular_files (lib66::tokenization & destination);
    bool traverse (function fn, const std::string & path) const;
    bool process_path
    (
        const std::string & path,
        function fn,
        const std::string & target,
        comparator cfn = nullptr
    );
    bool process_files
    (
        function fn,
        const std::string & target,
        comparator cfn = nullptr
    );
    bool process_bi_path
    (
        const std::string & path,
        bifunction fn,
        const std::string & source,
        comparator cfn = nullptr
    );
    bool process_bi_files
    (
        bifunction fn,
        const std::string & destination,
        comparator cfn = nullptr
    );

    const lib66::tokenization search_paths () const
    {
        return m_search_paths;
    }

private:

};              // class filewalker

/*--------------------------------------------------------------------------
 * Free functions in the util::file namespace
 *--------------------------------------------------------------------------*/

namespace file
{

/*--------------------------------------------------------------------------
 * Free "compare" functions
 *-------------------------------------------------------------------------*/

extern int compare_files_before_dirs            /* ftswalker::comparator    */
(
    const std::filesystem::directory_entry & afirst,
    const std::filesystem::directory_entry & asecond
);

/*--------------------------------------------------------------------------
 * Free functions in the util:file namespace
 *--------------------------------------------------------------------------*/

extern bool is_actionable_file
(
    const std::filesystem::directory_entry & entry
);
extern bool show_target
(
    const std::string & match,
    std::filesystem::file_type ft
);
extern bool show_directory_entry
(
    const std::string & description,
    std::filesystem::file_type ft
);
extern bool item_copy
(
    const std::string & source,
    const std::string & target,
    std::filesystem::file_type ft
);
extern bool item_delete
(
    const std::string & item,
    std::filesystem::file_type ft
);
extern bool copy_directory
(
    const std::string & source,
    const std::string & dest
);
extern bool delete_directory (const std::string & path);
extern bool collect_files_from_path
(
    const std::string & path,
    filewalker::pairs & collection,
    bool includeroot = true
);
extern bool delete_directory_tree (const std::string & path);
extern bool delete_collection (const filewalker::pairs & collection);
extern bool find_file                   /* uses filewalker::find_files()    */
(
    const std::string & rootdir,
    const std::string & target
);
extern bool find_files_by_regex         /* uses filewalker::find_files()    */
(
    lib66::tokenization & collected,
    const lib66::tokenization & paths,
    const std::string & rgx,
    filewalker::comparator cfn = nullptr
);
extern std::string get_type_name (std::filesystem::file_type ft);
extern std::string get_last_directory (const std::string & path);
extern std::string build_destination_path
(
    const std::string & rootsource,
    const std::string & sourcedir,
    const std::string & rootdest
);

}               // namespace file

}               // namespace util

#endif          //  CFG66_UTIL_FILEWALKER_HPP

/*
 * filewalker.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
