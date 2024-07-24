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
 * \file          history.cpp
 *
 *  This module summarizes or defines an undo/redo mechanism.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2023-01-06
 * \updates       2023-07-29
 * \license       GNU GPLv2 or above
 *
 *  See the "Design Patterns" book by Gamma et al., starting on page
 *  62. Also informative is https://www.scaler.com/topics/cpp/deque-in-cpp/.
 *  Also see the memento.cpp file.
 *
 *  The things that are needed for undo/redo are:
 *
 *      -   A History List. This keeps track of the states that were modified.
 *      -   A Caretaker.  This object takes care of managing mementos. Here,
 *          we implement a specific history template class.
 *      -   An Originator.  Creates a memento of its state, gives it to the
 *          Caretaker, and perhaps gets the memento back to restore a previous
 *          state.
 *      -   A deque. This data structure supports undo and redo in one
 *          object, unlike implementations using two vectors or two stacks.
 *
 *  Event sequence:
 *
 *      -   An Originator is created.
 *      -   A Caretaker is created, and requests the current state of the
 *          originator, which it "pushes" into a History List.
 *      -   As changes are made, the Caretaker "pushes" those changes.
 *
 *  States:
 *
 *      Assume for simplicity that we allow only 4 undo/redoes.
 *
 * Past      front <---- undo       redo ----> back     Future
 *
 *          Index:   0     1     2     3    Max size = 4
 * 0.                                       Before start-up
 * 1.               [0]                     Step #1 (initial state)
 *                   ^
 * 2.               [0]---[1]               Step #2 (first change)
 *                         ^
 * 3.               [0]---[1]---[2]         Step #3 (second change)
 *                               ^
 * 4.               [0]---[1]---[2]         Step #4 (undo)
 *                         ^
 * 5.               [0]---[1]---[2]         Step #5 (redo)
 *                               ^
 * 6.               [0]---[1]---[2]---[3]   Step #6 (third change)
 *                                     ^
 * 7.               [1]---[2]---[3]---[4]   Step #7, (fourth change, pop-front)
 *                                     ^
 *                                     |
 *                                  present
 *
 *      The index is a size_type value that ranges from 0 to the maximum
 *      allowed.
 *
 *      The "present" value is an index that "points" to the current state
 *      of the TYPE object. It is denoted by a "^" character.
 *
 * Step:
 *
 *  0.  The deque is empty. It is not active at this point.
 *  1.  Once the configuration is read and established, the application
 *      pushes/adds the start-up state to the deque. The original version
 *      is at element 0, and we call it "[0]".
 *  2.  The caller makes an edit and OKs it. The change is made to the TYPE
 *      object, then the application pushes/adds the new state to the history
 *      list and increments "present" to 1, which "points" to "[1]".
 *  3.  A second change is made and added.
 *  4.  The user does an undo. The previous state "[2]" remaims, but "present"
 *      now "points" back to "[1]" again. This state is copied back into the
 *      the applications current TYPE object.
 *  5.  The user changes mind and does a redo. We're at the same status as in
 *      step 3.
 *  6.  The user makes the third change, and "present" is at "[3]".
 *  7.  The user makes the fourth change. There's no more room in the deque,
 *      so item "[0]" is popped from the front and is lost for the duration
 *      of the program. The change is pushed/added, "present" is unchanged,
 *      and points to item "[4]" at index 3.
 */

#include <sstream>                      /* std::ostringstream for testing   */

#include "cfg/history.hpp"              /* cfg::history<> template class    */
#include "cfg/options.hpp"              /* cfg::options for testing         */

namespace cfg
{

/*
 * Test functions.
 */

std::string
options_history (const history<options> & h)
{
    std::string result;
    if (h.active())
    {
        std::ostringstream ost;
        ost
            << "Count: " << std::to_string(h.size()) << " mementos; "
            << "Present = " << std::to_string(h.present())
            << "; Max. size = " << std::to_string(h.max_size())
            << std::endl
            ;

        for (size_t index = 0; index < h.size(); ++index)
        {
            const options & current = h.get(index);
            ost
                << "(" << index << ")" << std::endl
                << current.debug_text()
                ;
        }
        result = ost.str();
    }
    else
        result = "Empty";

    return result;
}

}           // namespace cfg

/*
 * history.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

