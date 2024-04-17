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
 * \file          comments.cpp
 *
 *  This module declares/defines just some of the global (gasp!) variables
 *  in this application.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2018-11-26
 * \updates       2022-12-28
 * \license       GNU GPLv2 or above
 *
 */

#include "cfg/comments.hpp"             /* cfg::comments class              */

/*
 *  Do not document a namespace; it breaks Doxygen.
 */

namespace cfg
{

/**
 *  Default constructor.  Here, we do not use the set() function.  Makes
 *  debugging what caller's are doing with set() a little easier.
 */

comments::comments (const std::string & comtext) :
    m_comments_block    (comtext),
    m_comment_is_set    (false)
{
    if (comtext.empty())
        m_comments_block = "Add your comment block here\n";
}

void
comments::clear ()
{
    m_comments_block.clear();
    m_comment_is_set = false;
}

void
comments::set (const std::string & block)
{
    m_comments_block = block;
    m_comment_is_set = ! m_comments_block.empty();
}

}           // namespace cfg

/*
 * comments.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

