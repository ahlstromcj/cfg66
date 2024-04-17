#if ! defined CFG66_CFG_MEMENTO_HPP
#define CFG66_CFG_MEMENTO_HPP

/*
 *  This file is part of cfg66.
 *
 *  cfg66 is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  cfg66 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with cfg66; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * \file          memento.hpp
 *
 *    This module summarizes or defines part of an undo/redo mechanism.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2022-08-03
 * \updates       2024-01-22
 * \license       GNU GPLv2 or above
 *
 *  Documented in the cpp file. Also note the type alias, option_memento,
 *  defined at the bottom of this module.
 *
 *
 */

#include "cpp_types.hpp"                /* string, vector, msglevel         */
#include "cfg/options.hpp"              /* cfg::options (with source names) */

/*
 * Do not attempt to Doxygenate the documentation here; it breaks Doxygen.
 */

namespace cfg
{

/**
 *  Currently just a skeletal implementation.
 *
 *  It is possible for a memento to store only the incremental changes in
 *  state, as well.
 *
 *  It is possible to implement some changes using "execute" and
 *  "unexecute" functions for changes that are undoable.
 *
 *  TYPE must have:
 *
 *      -   Default constructor
 *      -   Copy construction
 *      -   Principal assignment operator
 */

template <typename TYPE>
class memento
{
    /*
     * friend class originator;
     */

private:

    /**
     *  Provides a copy of a state.  We obviously can't use references, and
     *  pointers are a pain.  We will assume that each memento state is
     *  relatively small.  We will also limit the number of mementos.
     *  See the history class.
     */

    TYPE m_state;

public:                                     /* narrow public interface      */

    virtual ~memento () = default;

public:                                     /* originator's wide interface  */

    memento () : m_state ()                 /* an empty state; useful?      */
    {
        // no other code
    }

    memento (const TYPE & s) : m_state (s)
    {
        // no other code
    }

    memento (memento &&) = default;
    memento (const memento &) = default;
    memento & operator = (const memento &) = default;

    bool set_state (const TYPE & s)
    {
        m_state = s;                            /* wipes out the old state  */
        return true;
    }

    const TYPE & get_state () const
    {
        return m_state;
    }

};          // class memento

/**
 *  This will be a commonly-used type in Seq66v2.
 *
 *  Actually, no, we can hide the usage of a memento in a history
 *  function.
 *
 *      using option_memento = memento<options>;
 */

}           // namespace cfg

#endif      // CFG66_CFG_MEMENTO_HPP

/*
 * memento.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

