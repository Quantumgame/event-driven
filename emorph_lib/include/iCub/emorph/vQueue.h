// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/* 
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences -
 * Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini, modified by Arren Glover(10/14)
 * email:  ugo.pattacini@iit.it
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

///
/// \ingroup emorphlib
///
/// \title vQueue
///
/// A wrapper for a dequeue of event pointers with functionality for memory
/// management of events
///
/// \author Arren Glover
///

#ifndef __EMORPH_VQUEUE__
#define __EMORPH_VQUEUE__

#include <iCub/emorph/vtsHelper.h>
#include <iCub/emorph/vCodec.h>
#include <deque>

namespace emorph {


class vQueue : public std::deque<vEvent*>
{
private:

    //! sorting events by timestamp comparitor
    static bool temporalSortWrap(const vEvent *e1, const vEvent *e2);
    static bool temporalSortStraight(const vEvent *e1, const vEvent *e2);

    void destroyall();
    void referall();

public:

    vQueue() {}
    ~vQueue();

    virtual void clear();

    vQueue(const vQueue&);
    vQueue& operator=(const vQueue&);

    virtual void push_back(const value_type &__x);
    virtual void push_front(const value_type &__x);

    virtual void pop_back();
    virtual void pop_front();

    virtual iterator erase(iterator __first, iterator __last);
    virtual iterator erase(iterator __position);

    void sort();
    void wrapSort();

};

}

#endif
