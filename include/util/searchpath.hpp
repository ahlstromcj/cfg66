#if ! defined CFG66_UTIL_SEARCHPATH_HPP
#define CFG66_UTIL_SEARCHPATH_HPP

/*
 * Copyright (C) 2007-2015 Tim Mayberry <mojofunk@gmail.com>
 * Copyright (C) 2008-2009 David Robillard <d@drobilla.net>
 * Copyright (C) 2008-2015 Paul Davis <paul@linuxaudiosystems.com>
 * Copyright (C) 2009-2011 Carl Hetherington <carl@carlh.net>
 * Copyright (C) 2013-2014 John Emmas <john@creativepost.co.uk>
 * Copyright (C) 2013-2015 Robin Gareus <robin@gareus.org>
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
 *    Provides the implementations of a system of concatenated search paths.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2026-02-22
 * \updates       2026-02-26
 * \version       $Revision$
 *
 *  This module is a reworking of the PBD code as used in the source
 *  code (search_path.cpp) for the Ardour DAW project.
 */

#include "cpp_types.hpp"                /* lib66::tokenization              */
#include "util/strfunctions.hpp"        /* util::tokenize()                 */

namespace util
{

/**
 * The searchpath class is a helper class for getting a vector of paths
 * contained in a search path string where a "search path string" contains
 * absolute directory paths separated by a colon (":") or a semi-colon (";")
 * on Windows.
 *
 * The searchpath class does not test whether the paths exist or are
 * directories. It is basically just a container with some added
 * functionality.
 */

class searchpath
{
    friend std::string search_path_expand (const std::string &);
    friend lib66::tokenization parse_search_path (const std::string &, bool);

private:

    static const std::string sm_sp_separator;

    /**
     *  This member will contain a list of paths to be searched.
     *  The to_string() function will assemble these paths into
     *  a long string, with each path separated by a colon or
     *  a semicolon.
     */

    lib66::tokenization m_search_paths { };             /* vector<string>   */

public:

    searchpath () = default;
    searchpath (const std::string & search_path);
    searchpath (const lib66::tokenization & paths);
    searchpath (const searchpath &) = default;
    searchpath & operator = (const searchpath &) = default;
    searchpath (searchpath &&) = default;
    searchpath & operator = (searchpath &&) = default;
    ~searchpath () = default;

    searchpath & operator += (const searchpath & spath);
    searchpath & operator += (const std::string & directory_path);
    searchpath operator + (const searchpath & other);
    searchpath operator + (const std::string & directory_path);
    searchpath & operator -= (const searchpath & spath);
    searchpath & operator -= (const std::string & directory_path);

    /*
     * These are not (yet) provided:
     *
     *      searchpath operator + (const searchpath & other);
     *      searchpath operator + (const std::string & directory_path);
     */

    lib66::tokenization & paths ()
    {
        return m_search_paths;
    }

    const lib66::tokenization & paths () const
    {
        return m_search_paths;
    }

    int count () const
    {
        return int(paths().size());
    }

    bool empty () const
    {
        return paths().empty();
    }

    std::string to_string () const;
    std::string to_string_list () const;
    searchpath & add_subdirectory_to_paths (const std::string & subdir);
    bool add_directory (const std::string & directory_path);
    bool add_directories (const lib66::tokenization & paths);
    bool remove_directory (const std::string & directory_path);
    bool remove_directories (const lib66::tokenization & paths);
    bool contains (const std::string & path) const;
};

/*-------------------------------------------------------------------------
 * Free functions
 *-------------------------------------------------------------------------*/

extern std::string search_path_expand (const std::string & path);
extern lib66::tokenization parse_search_path
(
    const std::string & path, bool check_if_exists
);

}           // namespace util

#endif      // CFG66_UTIL_SEARCHPATH_HPP

/*
 * searchpath.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
