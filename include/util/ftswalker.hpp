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
 * \updates       2025-03-10
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

#if defined THIS_CODE_IS_READY

class ftswalker
{

public:

    /**
     *  Indicates the types of files that the function callback can
     *  handle. We do not care about the rest. At minimun, the function
     *  should handle D and F.
     */

    enum class FTS
    {
        D,              /* directory                                        */
        DEFAULT,        /* one of the other file types                      */
        ERR,            /* an (unspecified) error occurred; errno is set    */
        F,              /* a regular file                                   */
    };

    /**
     *  A function that can be called to process each file or directory
     *  found.
     */

    using function = bool (ftswalker::*)
    {
        const std::string &,            /* name of file or directory        */
        FTS                             /* type of file, or an error        */
    };

    MORE TO COME

};          // class ftswalker

#endif

/*-------------------------------------------------------------------------
 * Free functions in the util namespace
 *-------------------------------------------------------------------------*/

extern int compare_files_before_dirs
(
    const FTSENT ** first,
    const FTSENT ** second
);
extern bool fts_find_file
(
    const std::string & rootdir,
    const std::string & target
);

}               // namespace util

#endif          //  CFG66_UTIL_FTSWALKER_HPP

/*
 * ftswalker.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

