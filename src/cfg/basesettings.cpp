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
 * \file          basesettings.cpp
 *
 *  This module declares/defines some basic settings items.
 *
 * \library       cfg66 application
 * \author        Chris Ahlstrom
 * \date          2019-01-17
 * \updates       2023-07-21
 * \license       GNU GPLv2 or above
 *
 */

#include "cfg/basesettings.hpp"         /* cfg::basesettings class          */
#include "util/msgfunctions.hpp"        /* util::error_message()            */

/**
 *  Indicates the "version" of the format of the "rc" ("rc", "ctrl",
 *  "mutes", and "playlist" files.  We hope to increment this number only
 *  rarely.
 */

#define CFG66_ORDINAL_VERSION           0

/*
 *  Do not document a namespace; it breaks Doxygen.
 */

namespace cfg
{

/**
 *  Default constructor.
 */

basesettings::basesettings
(
    const std::string & name,
    const std::string & cfgformat,
    const std::string & cfgtype,
    const std::string & comtext,
    int version
) :
    m_file_name         (name),
    m_modified          (false),
    m_config_format     (cfgformat),
    m_config_type       (cfgtype),
    m_ordinal_version   (version),
    m_comments_block    (comtext),                          /* [comments]   */
    m_error_message     (),
    m_is_error          (false)
{
    // Empty body; it's no use to call normalize() here, see set_defaults().
}

/**
 *  Sets the default values.  For the m_midi_buses and m_instruments members,
 *  this function can only iterate over the current size of the vectors.  But
 *  the default size is zero!
 */

void
basesettings::set_defaults ()
{
    m_modified = false;
    m_ordinal_version = CFG66_ORDINAL_VERSION;
    m_file_name.clear();
    m_error_message.clear();
    m_is_error = false;
    normalize();                            // recalculate derived values
}

/**
 *  Calculate the derived values from the already-set values.
 */

void
basesettings::normalize ()
{
    // \todo
}

/**
 * \return
 *      Returns false if there is an error message in force.
 */

bool
basesettings::set_error_message (const std::string & em) const
{
    bool result = em.empty();
    if (result)
    {
        m_error_message.clear();
        m_is_error = false;
    }
    else
    {
        if (! m_error_message.empty())
            m_error_message += "; ";

        m_error_message += em;
        (void) util::error_message(em);
    }
    return result;
}

}           // namespace cfg

/*
 * basesettings.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

