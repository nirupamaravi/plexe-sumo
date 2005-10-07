//---------------------------------------------------------------------------//
//                        TrackerValueDesc.cpp -
//  Storage for a tracked value
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.5  2005/10/07 11:45:56  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/23 06:09:38  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.3  2005/09/15 12:20:06  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2004/12/16 12:21:38  dkrajzew
// debugging
//
// Revision 1.1  2004/11/23 10:38:31  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:56  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.8  2004/08/02 11:43:31  dkrajzew
// ported to fox 1.2; patched missing unlock on unwished program termination
//
// Revision 1.7  2004/07/02 08:26:11  dkrajzew
// aggregation debugged and saving option added
//
// Revision 1.6  2004/03/19 12:42:59  dkrajzew
// porting to FOX
//
// Revision 1.5  2003/11/11 08:43:04  dkrajzew
// synchronisation problems of parameter tracker updates patched
//
// Revision 1.4  2003/07/30 08:50:42  dkrajzew
// tracker debugging (not yet completed)
//
// Revision 1.3  2003/07/18 12:30:14  dkrajzew
// removed some warnings
//
// Revision 1.2  2003/06/05 11:38:47  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <utils/gfx/RGBColor.h>
#include <utils/gui/globjects/GUIGlObject.h>
//#include <guisim/GUINet.h>
#include "TrackerValueDesc.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * method definitions
 * ======================================================================= */
TrackerValueDesc::TrackerValueDesc(const std::string &name,
                                   const RGBColor &col,
                                   GUIGlObject *o,
                                   size_t recordBegin)
    : myName(name), myObject(o),
    myActiveCol(col), myInactiveCol(col),
    myAmActive(true),
    myMin(0), myMax(0),
    myAggregationInterval(1), myInvalidValue(-1), myValidNo(0),
	myRecordingBegin(recordBegin), myTmpLastAggValue(0)
{
}


TrackerValueDesc::~TrackerValueDesc()
{
    // just to quit cleanly on a failure
    if(myLock.locked()) {
        myLock.unlock();
    }
}


void
TrackerValueDesc::addValue(SUMOReal value)
{
    if(myValues.size()==0) {
        myMin = (SUMOReal) value;
        myMax = (SUMOReal) value;
    } else {
        myMin = (SUMOReal) value < myMin ? (SUMOReal) value : myMin;
        myMax = (SUMOReal) value > myMax ? (SUMOReal) value : myMax;
    }
    myLock.lock();
    myValues.push_back((SUMOReal) value);
    if(value!=myInvalidValue) {
        myTmpLastAggValue += (SUMOReal) value;
        myValidNo++;
    }
    // check what to do with aggregated values
    if(myValues.size()!=0&&myValues.size()%myAggregationInterval==0) {
        // ok, a new aggregation is filled completely. Set.
        if(myValidNo!=0) {
            myAggregatedValues.push_back(
                myTmpLastAggValue / (SUMOReal) myValidNo);
        } else {
            myAggregatedValues.push_back(0);
        }
        myTmpLastAggValue = 0;
        myValidNo = 0;
    } else {
        // remove the one previously set
        if(myAggregatedValues.size()!=0) {
            myAggregatedValues.erase(myAggregatedValues.end()-1);
        }
        // append newly computed
        if(myValidNo!=0) {
            myAggregatedValues.push_back(
                myTmpLastAggValue / (SUMOReal) myValidNo);
        } else {
            myAggregatedValues.push_back(0);
        }
    }
    myLock.unlock();
}


SUMOReal
TrackerValueDesc::getRange() const
{
    getMin();
    getMax();
    return myMax - myMin;
}


SUMOReal
TrackerValueDesc::getMin() const
{
    return myMin;
}


SUMOReal
TrackerValueDesc::getMax() const
{
    return myMax;
}


SUMOReal
TrackerValueDesc::getYCenter() const
{
    getMin();
    getMax();
    return (myMin + myMax) / 2.0f;
}


const RGBColor &
TrackerValueDesc::getColor() const
{
    return myActiveCol;
}


const std::vector<SUMOReal> &
TrackerValueDesc::getValues()
{
    myLock.lock();
    return myValues;
}


const std::vector<SUMOReal> &
TrackerValueDesc::getAggregatedValues()
{
    myLock.lock();
    return myAggregatedValues;
}


const std::string &
TrackerValueDesc::getName() const
{
    return myName;
}

void
TrackerValueDesc::unlockValues()
{
    myLock.unlock();
}


void
TrackerValueDesc::setAggregationSpan(size_t as)
{
    myLock.lock();
    if(myAggregationInterval!=as) {
        // ok, the aggregation has changed,
        //  let's recompute the list of aggregated values
        myAggregatedValues.clear();
        std::vector<SUMOReal>::iterator i;
        for(i=myValues.begin(); i!=myValues.end(); ) {
            SUMOReal value = 0;
            myValidNo = 0;
            for(size_t j=0; j<as&&i!=myValues.end(); j++, ++i) {
                if((*i)!=myInvalidValue) {
                    value += (*i);
                    myValidNo++;
                }
            }
            if(myValidNo==0) {
                myAggregatedValues.push_back(0);
                myTmpLastAggValue = 0;
            } else {
                myAggregatedValues.push_back(value / (SUMOReal) myValidNo);
                myTmpLastAggValue = value / (SUMOReal) myValidNo;
            }
        }
    }
    myAggregationInterval = as;
    myLock.unlock();
}


size_t
TrackerValueDesc::getAggregationSpan() const
{
    return myAggregationInterval;
}


size_t
TrackerValueDesc::getRecordingBegin() const
{
    return myRecordingBegin;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


