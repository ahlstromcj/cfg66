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
 * \file          memento.cpp
 *
 *    This module summarizes or defines an undo/redo mechanism.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2023-01-06
 * \updates       2023-01-06
 * \license       GNU GPLv2 or above
 *
 *    See the "Design Patterns" book by Gamma et al., starting on page
 *    283.
 *
 *    The Originator (e.g. a MIDI track object) would be something like:
 *
 \verbatim
        class state;
        class originator
        {
        private:
            state * m_state;

        public:
            memento * create_memento ();
            bool set_memento (const memento *);
        };
 \endverbatim
 *
 \verbatim
    class undoable
    {
    public:
        void undo () = 0;
        void redo () = 0;
    }
 \endverbatim
 *
 * Mementos:
 *
 *      -#  Preserve encapsulation boundaries.
 *      -#  Simplifies the originator.
 *      -#  Might be expensive to use.
 *      -#  Defines both wide (for originators) and narrow (for caretakers)
 *          interfaces.
 *      -#  There are hidden costs in caring for mementos.
 *
 * Usage:
 *
 *      See the history class.
 */

#include "cfg/memento.hpp"              /* cfg::memento<> template class    */

#if defined PLATFORM_DEBUG
#include <iostream>
#endif

/*
 * Do not attempt to Doxygenate the documentation here; it breaks Doxygen.
 */

namespace cfg
{

#if defined PLATFORM_DEBUG

/*
 * To do: Test functions?
 */

#endif      // defined PLATFORM_DEBUG

}           // namespace cfg

/*
 * memento.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

