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
 * \file          recent.cpp
 *
 *  This module declares/defines a class for keeping track of recently-used
 *  files.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2018-03-29
 * \updates       2024-09-05
 * \license       GNU GPLv2 or above
 *
 *  The cfg66::recent class simply keeps track of recently-used files for the
 *  "Recent Files" menu.
 */

#include <algorithm>                    /* std::find()                      */

#include "cfg/recent.hpp"               /* cfg66 recent-files container     */
#include "util/filefunctions.hpp"       /* util::get_full_path()            */

namespace cfg
{

/**
 *  Indicates the maximum number of recently-opened file-names we will
 *  store.
 */

static const int sc_recent_files_max = 12;

/**
 *  This construction creates an empty recent-files list and sets the maximum
 *  size of the list.
 */

recent::recent () :
    m_recent_list   (),
    m_maximum_size  (sc_recent_files_max)
{
    // no code
}

/**
 *  The conventional, but rote, principal assignent operator.  It has to be
 *  created to comment out re-assigning a constant.
 */

recent &
recent::operator = (const recent & source)
{
    if (this != &source)
    {
        m_recent_list   = source.m_recent_list;

        /*
         * A constant, cannot be reassigned:
         *
         * m_maximum_size  = source.m_maximum_size;
         */
    }
    return *this;
}

/**
 *  This function is meant to be used when loading the recent-files list from
 *  a configuration file.  Unlike the add() function, this one will not pop
 *  off an existing item to allow the current item to be put into the
 *  container.  If the entry is already in the list, it is ignored. It assumes
 *  the container had been cleared before this series of append() calls.
 *
 * \param item
 *      Provides the file-name to append.  It is converted to the full path to
 *      the file before being added.  Also, it is set to UNIX conventions,
 *      using the forward slash as a path separator.
 *
 * \return
 *      Returns true if the file-name was appended.
 */

bool
recent::append (const std::string & item)
{
    bool result = count() < maximum();
    if (result)
    {
        std::string path = util::get_full_path(util::normalize_path(item));
        result = ! path.empty();
        if (result)
            result = util::file_readable(path);

        if (result)
        {
            const auto & it = std::find
            (
                m_recent_list.cbegin(), m_recent_list.cend(), path
            );
            if (it == m_recent_list.end())              /* not found?   */
                m_recent_list.push_back(path);          /* append it!   */
        }
    }
    return result;
}

/**
 *  This function is meant to be used when adding a file that the user
 *  selected.  If the file is already in the list, it is moved to the "top"
 *  (the beginning of the list); the original entry is removed.  If the list is
 *  full, the last entry is removed, in order to make room for the new entry.
 *
 * \param item
 *      Provides the file-name to add.  It is converted to the full path to
 *      the file before being added.  Also, it is set to UNIX conventions,
 *      using the forward slash as a path separator.
 *
 * \return
 *      Returns true if the file-name was added.  The file must be readable.
 */

bool
recent::add (const std::string & item)
{
    std::string path = util::get_full_path(util::normalize_path(item));
    bool result = ! path.empty();
    if (result)
        result = util::file_readable(path);

    if (result)
    {
        const auto & it = std::find
        (
            m_recent_list.cbegin(), m_recent_list.cend(), path
        );
        if (it != m_recent_list.end())
            (void) m_recent_list.erase(it);

        result = count() < maximum();
        if (! result)
        {
            m_recent_list.pop_back();   /* remove last entry to make room   */
            result = true;
        }
        if (result)
            m_recent_list.push_front(path);
    }
    return result;
}

/**
 *  Removes the path from the recent-files list, if it was found in that list.
 *
 * \todo
 *      The fix of 2018-12-05 should be added/tested in seq64!!!
 *
 * \param item
 *      Provides the path to be removed.
 *
 * \return
 *      Returns true if the item was found and removed.
 */

bool
recent::remove (const std::string & item)
{
    std::string path = util::get_full_path(util::normalize_path(item));
    bool result = ! path.empty();
    if (result)
    {
        const auto & it = std::find
        (
            m_recent_list.cbegin(), m_recent_list.cend(), path
        );
        if (it != m_recent_list.end())
            (void) m_recent_list.erase(it);
        else
            result = false;
    }
    return result;
}

/**
 *  Gets the file-path at the given position.
 *
 * \param index
 *      Provides the index into the container.  It is checked for validity.
 *
 * \return
 *      Returns the file-path, if available.  Otherwise, an empty string is
 *      returned.  The path is converted to the slash/backslash conventions
 *      appropriate for the operating system on which this code was built.
 */

std::string
recent::get (int index) const
{
    std::string result;
    if (index >= 0 && index < count())
    {
        result = m_recent_list[container::size_type(index)];
        result = util::normalize_path(result);  /* UNIX, no '/' terminator  */
    }
    return result;
}

/**
 * \getter m_recent_list
 *
 *  Gets the desired recent file-name, if present.
 *
 * \param index
 *      Provides the desired index into the recent-files deque.
 *
 * \param shorten
 *      If true, remove the path-name from the file-name.  True by default.
 *      It needs to be short for the menu entry (though now using the full
 *      path is an option), but the full path-name for the "rc" file.
 *
 * \return
 *      Returns m_recent_list[index], perhaps shortened.  An empty string is
 *      returned if there is no such animal.
 */

std::string
recent::file (int index, bool shorten) const
{
    std::string result = get(index);
    if (shorten && ! result.empty())
    {
        if (util::name_has_path(result))
        {
            std::string destpath, destbase;
            bool ok = util::filename_split(result, destpath, destbase);
            if (ok)
                result = destbase;
        }
    }
    return result;
}

}           // namespace cfg

/*
 * recent.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

