#if ! defined CFG66_RECENT_HPP
#define CFG66_RECENT_HPP

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
 * \file          recent.hpp
 *
 *  This module declares/defines a container for "recent" entries.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2018-03-29
 * \updates       2024-08-03
 * \license       GNU GPLv2 or above
 *
 *  It is based on the "recent" class of Seq66, with some additional
 *  convenience functions.
 */

#include <deque>
#include <string>

namespace cfg
{

/**
 *  This class provides a standard container and operations for a recent-files
 *  list.  The container should, if possible, contain only unique strings...
 *  no file-path should be included more than once.
 */

class recent
{

private:

    /**
     *  Provide the type definition for the recent-files container.  It is
     *  currently a standard deque.  A deque (pronounced like "deck") is an
     *  irregular acronym of double-ended queue. Double-ended queues are
     *  sequence containers with dynamic sizes that can be expanded or
     *  contracted on both ends (either its front or its back), like a deck of
     *  cards where the dirty dealer can deal from the bottom.
     */

    using container = std::deque<std::string>;

private:

    /**
     *  Holds the list of recent files.
     */

    container m_recent_list;

    /**
     *  Holds the constraint on the number of recent files.  Usually a value
     *  like 12.
     */

    const int m_maximum_size;

public:

    recent ();
    recent (const recent & source) = default;
    recent (recent && source) = default;
    recent & operator = (const recent & source);
    recent & operator = (recent && source) = delete;
    ~recent ();

    void clear ()
    {
        m_recent_list.clear();
    }

    int count () const
    {
        return int(m_recent_list.size());
    }

    int maximum () const
    {
        return m_maximum_size;
    }

    /**
     *  Gets the "most recent" file-name.
     */

    const std::string & front () const
    {
        return m_recent_list.front();
    }

    std::string get (int index) const;
    std::string file (int index, bool shorten = true) const;
    bool append (const std::string & item);
    bool add (const std::string & item);
    bool remove (const std::string & item);

};          // class recent

}           // namespace cfg

#endif      // CFG66_RECENT_HPP

/*
 * recent.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

