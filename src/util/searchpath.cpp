/*
 * Copyright (C) 2007-2015 Tim Mayberry <mojofunk@gmail.com>
 * Copyright (C) 2008-2009 David Robillard <d@drobilla.net>
 * Copyright (C) 2008-2015 Paul Davis <paul@linuxaudiosystems.com>
 * Copyright (C) 2013-2014 John Emmas <john@creativepost.co.uk>
 * Copyright (C) 2014-2015 Robin Gareus <robin@gareus.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/**
 * \file          searchpath.cpp
 *
 *    Provides the implementations for safe replacements for the various C
 *    file functions.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2026-02-22
 * \updates       2026-02-26
 * \version       $Revision$
 *
 *  This module is a reworking of some of the PBD code as used in the source
 *  code (string_convert.cpp) for the Ardour DAW project.
 *
 *  We removed all the debug code such as the following. Let the caller
 *  decide how to handle an error.
 *
 *      DEBUG_SCONVERT
 *      (
 *          string_compose("float_to_string conversion failure for %1", val)
 *      );
 */

#include <algorithm>                    /* std::find()                      */

#include "util/searchpath.hpp"          /* class util::searchpath           */
#include "util/filefunctions.hpp"       /* util::file_concatenate()         */

namespace util
{

/**
 * Static member
 */

#if defined PLATFORM_WINDOWS
const std::string searchpath::sm_sp_separator { ";" };
#else
const std::string searchpath::sm_sp_separator { ":" };
#endif

/**
 *  Initialize searchpath from a string where the string contains one or
 *  more absolute paths to directories which are delimited by a path
 *  separation character. The path delimeter is a colon (":") on UNIX and
 *  a semi-colon (";") on Windows.
 *
 *  Each path contained in the search path may or may not resolve to
 *  an existing directory in the filesystem.
 *
 * \param path
 *      A single path string.
 */

searchpath::searchpath (const std::string & path)
{
    paths() = util::tokenize(path, sm_sp_separator);
    for (const auto & p : paths())
    {
        add_directory(p);
    }
}

/**
 * Initialize searchpath from a vector of paths that may or may not exist.
 *
 * \param paths
 *      A vector of paths to be added.
 */

searchpath::searchpath (const lib66::tokenization & paths)
{
    add_directories(paths);
}

/**
 *  Concatenates all of the stored paths, separated by ":" or ";" (Windows).
 *
 * \return
 *      Returns a search path string. The string that is returned contains
 *      the platform specific path separator.
 */

std::string
searchpath::to_string () const
{
    std::string result;
    for (const auto & p : paths())
    {
        result += p;
        result += sm_sp_separator;
    }
    result = result.substr(0, result.length() - 1); /* drop final separator */
    return result;
}

/**
 *  Similar to the above, but each path is on a separate line.
 */

std::string
searchpath::to_string_list () const
{
    std::string result;
    for (const auto & p : paths())
    {
        result += p;
        result += "\n";
    }
    return result;
}

/**
 *  Add all the directories in the path to the list of paths.
 */

searchpath &
searchpath::operator += (const searchpath & spath)
{
    for (const auto & p : spath.paths())
    {
        add_directory(p);
    }
    return *this;
}

/**
 *  Add one directory path to the search path.
 */

searchpath &
searchpath::operator += (const std::string & directorypath)
{
    add_directory(directorypath);
    return *this;
}

/**
 *  Concatenate another search path into the list.
 */

searchpath
searchpath::operator + (const std::string & directorypath)
{
    return searchpath(*this) += directorypath;
}

/**
 *  Concatenate another search path into the list.
 */

searchpath
searchpath::operator + (const searchpath & spath)
{
    return searchpath(*this) += spath;
}

/**
 *  Remove all the directories in path from this.
 */

searchpath &
searchpath::operator -= (const searchpath & spath)
{
    (void) remove_directories(spath.paths());
    return *this;
}

/**
 *  Remove a directory path from the search path.
 */

searchpath &
searchpath::operator -= (const std::string & directorypath)
{
    (void) remove_directory(directorypath);
    return *this;
}

static std::string
poor_mans_glob (std::string path)
{
    if (path.find('~') == 0)
    {
        path.replace (0, 1, util::user_home()); /* Glib::get_home_dir())    */
    }
    return path;
}

/**
 *  Add directory_path to this searchpath.
 */

bool
searchpath::add_directory (const std::string & directorypath)
{
    bool result { ! directorypath.empty() };
    if (result)
    {
        for (const auto & p : paths())
        {
            if (poor_mans_glob(p) == poor_mans_glob(directorypath))
            {
                result = false;
                break;
            }
        }
    }
    if (result)
        paths().push_back(directorypath);

    return result;
}

/**
 *  Add directories in paths to this searchpath.
 */

bool
searchpath::add_directories (const lib66::tokenization & paths)
{
    for (const auto & p : paths)
    {
        add_directory(p);
    }
    return ! paths.empty();
}

/**
 *  qAdd a sub-directory to each path in the search path.
 *
 *  Should these new paths just be added to the end of the search path,
 *  rather than replaced?
 *
 * \param subdir
 *      The directory name, it should not contain any path separating
 *      tokens.
 *
 * \return
 *      Returns a reference to itself.
 */

searchpath &
searchpath::add_subdirectory_to_paths (const std::string & subdir)
{
    for (auto & p : paths())
    {
        p = util::filename_concatenate(p, subdir);
    }
    return *this;
}

/**
 *  Remove directory_path from this searchpath.
 */

bool
searchpath::remove_directory (const std::string & directorypath)
{
    bool result { ! directorypath.empty() };
    if (result)
    {
        for (auto i = paths().begin(); i != paths().end() ; /* ++i */ )
        {
            if (*i == directorypath)
                i = paths().erase(i);
            else
                ++i;
        }
    }
    return result;
}

/**
 *  Remove all the directories in paths from this searchpath.
 */

bool
searchpath::remove_directories (const lib66::tokenization & paths)
{
    for (const auto & p : paths)
        remove_directory(p);

    return ! paths.empty();
}

/**
 * \return
 *      Returns true if searchpath already contains the path.
 */

bool
searchpath::contains (const std::string & path) const
{
    lib66::tokenization::const_iterator i
    {
        std::find(paths().begin(), paths().end(), path)
    };
    if (i == paths().end())
        return false;

    return true;
}

/*-------------------------------------------------------------------------
 * Free functions
 *-------------------------------------------------------------------------*/

std::string
search_path_expand (const std::string & path)
{
    std::string result;
    if (! path.empty())
    {
        lib66::tokenization s
        {
            util::tokenize(path, searchpath::sm_sp_separator)
        };
        lib66::tokenization n;
        for (const auto & token : s)
        {
            std::string exp { util::file_path_expand(token) };
            if (! exp.empty())
                n.push_back(exp);
        }
        for (const auto & token : n)
        {
            if (! result.empty())
                result += searchpath::sm_sp_separator;

            result += token;
        }
    }
    return result;
}

lib66::tokenization
parse_search_path (const std::string & path, bool check_if_exists)
{
    lib66::tokenization tmp
    {
        util::tokenize(path, searchpath::sm_sp_separator)
    };
    lib66::tokenization pathlist;
    for (const auto & token : tmp)
    {
        if (! token.empty())
        {
            std::string dir;

#if ! defined PLATFORM_WINDOWS

            if (token[0] == '~')
                dir = util::user_home(token.substr(1));
            else
#endif
            {
                dir = token;
            }
            if (! check_if_exists || util::file_is_directory(dir))
                pathlist.push_back(dir);
        }
    }
    return pathlist;
}

}           // namespace util

/*
 * searchpath.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

