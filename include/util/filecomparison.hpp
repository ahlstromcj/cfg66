#if ! defined CFG66_UTIL_FILECOMPARISON_HPP
#define CFG66_UTIL_FILECOMPARISON_HPP

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
 * \file          filecomparison.hpp
 *
 *    Provides the declarations for safe replacements for some C++
 *    file functions.
 *
 * \author        Chris Ahlstrom
 * \date          2026-11-15
 * \updates       2026-11-15
 * \version       $Revision$
 *
 *    Also see the filecomparison.cpp module.  The functions here use
 *    old-school file-pointers and the new-fangled std::string.
 */

#include <cstdio>                       /* std::FILE *                      */
#include <string>                       /* std::string ubiquitous class     */

#include "cpp_types.hpp"                /* std::string, tokenization alias  */

namespace util
{

/*
 * Global function declarations.
 */

extern bool compare_binary_files
(
    const std::string & bfile0,
    const std::string & bfile1
);
extern bool compare_line_files
(
    const std::string & file0,
    const std::string & file1
);
extern bool compare_file_content
(
    const std::string & file0,
    const std::string & file1
);

}           // namespace util

#endif      // CFG66_UTIL_FILECOMPARISON_HPP

/*
 * filecomparison.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
