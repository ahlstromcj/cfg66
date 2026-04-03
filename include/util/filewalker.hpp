#if ! defined CFG66_UTIL_FTSWALKER_HPP
#define CFG66_UTIL_FTSWALKER_HPP

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
 *    This module ...
 *
 * \library       filewalker
 * \author        Chris Ahlstrom
 * \date          2026-04-02
 * \updates       2026-04-03
 * \version       $Revision$
 * \license       GNU GPL v2 or above
 *
 *   To do.
 */

#include <filesystem>                   /* directory-file functions         */
#include <string>

#include "cpp_types.hpp"                /* lib66::tokenization              */

namespace util
{

class filewalker
{

public:

    /**
     * std::filesystem::file_type
     *
     *      Indicates the type of a file. Replaces ftswalker::FTS.
     */

    /**
     *  A function that can be called to process each file or directory
     *  found.
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

private:

    /**
     *  The set of directories to search whenever a file needs to be found.
     *  We could use util::searchpath, which uses a tokenization and adds
     *  functions and operators to manipulate the PATH.
     */

    lib66::tokenization m_search_directories;

public:

    filewalker () = default;
    filewalker (const std::string & path);
    filewalker (const lib66::tokenization & paths);
    filewalker (filewalker &&) = default;
    filewalker & operator = (filewalker &&) = default;
    ~filewalker () = default;

    bool find_file
    (
        const std::string & target,
        lib66::tokenization & destination
    );
    bool find_regular_files (lib66::tokenization & destination);
    bool traverse (const std::string & path) const;
    bool process_files
    (
        function fn,
        const std::string & target,
        comparator cfn = nullptr
    );
    bool process_bi_files
    (
        bifunction fn,
        const std::string & source,
        const std::string & dest,
        comparator cfn = nullptr
    );

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

extern bool show_target
(
    const std::string & match,
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
extern bool find_file
(
    const std::string & rootdir,
    const std::string & target
);
extern bool find_files_by_regex
(
    lib66::tokenization & collected,
    const lib66::tokenization & paths,
    const std::string & rgx,
    filewalker::comparator cfn = nullptr
);
extern std::string get_type_name (std::filesystem::file_type ft);

}               // namespace file

}               // namespace util

#endif          //  CFG66_UTIL_FTSWALKER_HPP

/*
 * filewalker.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
