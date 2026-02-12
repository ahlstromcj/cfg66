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
 * \date          2026-02-03
 * \updates       2026-02-12
 * \license       GNU GPLv2 or above
 *
 *  See the "Design Patterns" book by Gamma et al., starting on page
 *  62. Also informative is https://www.scaler.com/topics/cpp/deque-in-cpp/.
 *  Also see the memento.cpp file.
 *
 *  This file replaces history_deque, which has some issues.
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
    std::string result { "Empty" };
    if (h.active())
    {
        std::ostringstream ost;
        ost
            << "Size: " << std::to_string(h.max_size()) << "; "
            << "Count: " << std::to_string(h.change_count()) << " changes; "
            << "Max. size = " << std::to_string(h.max_size())
            << std::endl
            ;

#if USE_THIS_CODE
        for (size_t index = 0; index < h.size(); ++index)
        {
            const options & current = h.get(index);
            ost
                << "(" << index << ")" << std::endl
                << current.debug_text()
                ;
        }
#endif

        result = ost.str();
    }
    return result;
}

}           // namespace cfg

/*
 * history.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
