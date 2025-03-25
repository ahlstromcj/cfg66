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
 * \file          ftswalker.hpp
 *
 *    This module ...
 *
 * \library       ftswalker
 * \author        Chris Ahlstrom
 * \date          2025-03-10
 * \updates       2025-03-25
 * \version       $Revision$
 * \license       GNU GPL v2 or above
 *
 *   To do.
 */

#include <string>
#include <fts.h>                        /* function to traverse directories */

#include "cpp_types.hpp"                /* lib66::tokenization              */

namespace util
{

class ftswalker
{

public:

    /**
     *  Indicates the types of files that the function callback can
     *  handle. We do not care about the rest. At minimun, the function
     *  should handle D and F.  Not included are FTS_INIT, and FTS_W.
     *
     *  Why reiterate the values in an enumeration? To avoid callers
     *  needing to include a C header.
     */

    enum class FTS      /* FTS is already a type in the fts(3) module       */
    {
        D,              /* directory (preorder)                             */
        DC,             /* directory that causes a cycle in the tree        */
        DEFAULT,        /* one of the other file types except FTS_DP        */
        DNR,            /* directory that cannot be read (an error)         */
        DOT,            /* the "." or ".." file if not set via fts_open()   */
        DP,             /* a post-order directory                           */
        ERR,            /* an error return, fts_errno is set to the error   */
        F,              /* a regular file                                   */
        NS,             /* no stat(2) information available (an error)      */
        NSOK,           /* no stat(2) information requested                 */
        SL,             /* a symbolic link                                  */
        SLNONE,         /* a symbolic link with a non-existent target       */
        UNKNOWN         /* our own code for a bad fts_info field            */
    };

    /**
     *  A function that can be called to process each file or directory
     *  found.
     */

    using function = bool (*)
    (
        const std::string &,            /* name of source file/directory    */
        FTS                             /* type of file, or an error        */
    );

    /**
     *  A function that can be called to process files/directories from
     *  one folder to another folder.
     */

    using bifunction = bool (*)
    (
        const std::string &,            /* name of source file/directory    */
        const std::string &,            /* destination directory            */
        FTS                             /* type of file, or an error        */
    );

    /**
     *  The "compar()" function used in the fts_open() function.
     */

    using comparator = int (*)
    (
        const FTSENT ** first,
        const FTSENT ** second
    );

private:

    /**
     *  The set of directories to search when ever a file needs to be found.
     */

    lib66::tokenization m_search_directories;

    /**
     *  The same set of directories in pointer format.
     */

    char ** m_paths;

public:

    ftswalker () = default;
    ftswalker (const std::string & path);
    ftswalker (const lib66::tokenization & paths);
    virtual ~ftswalker ();

    bool find_file
    (
        const std::string & target,
        lib66::tokenization & destination
    );
    bool find_regular_files (lib66::tokenization & destination);
    bool process_files
    (
        function fn,
        const std::string & target  = "",
        comparator cfn              = nullptr
    );
    bool process_files
    (
        bifunction fn,
        const std::string & target,
        comparator cfn = nullptr
    );

private:

    void make_paths ();
    void delete_paths ();

    char * const * paths () const
    {
        return m_paths;
    }

};              // class ftswalker

/*--------------------------------------------------------------------------
 * Free functions in the util namespace
 *--------------------------------------------------------------------------*/

extern ::FTSENT * fts_read_entry (::FTS * ftsp);
extern bool fts_delete_directory (const std::string & path);
extern bool fts_find_file
(
    const std::string & rootdir,
    const std::string & target
);
extern bool fts_copy_directory
(
    const std::string & source,
    const std::string & dest
);
extern bool fts_delete_directory (const std::string & path);

/*--------------------------------------------------------------------------
 * Free "compare" functions
 *-------------------------------------------------------------------------*/

extern int compare_files_before_dirs            /* ftswalker::comparator    */
(
    const FTSENT ** first,
    const FTSENT ** second
);

/*--------------------------------------------------------------------------
 * Useful ftswalker callbacks.
 *--------------------------------------------------------------------------*/

extern bool fts_show_target             /* provides a very simple test      */
(
    const std::string & match,
    util::ftswalker::FTS ft
);
extern bool fts_item_copy               /* makes directories, copies files  */
(
    const std::string & source,
    const std::string & destination,
    util::ftswalker::FTS ft
);
extern bool fts_item_delete
(
    const std::string & item,
    util::ftswalker::FTS ft
);

}               // namespace util

#endif          //  CFG66_UTIL_FTSWALKER_HPP

/*
 * ftswalker.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

