/****************************************************************************/
/// @file    RORDLoader_SUMOBase.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// The base class for SUMO-native route handlers
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "RORDLoader_SUMOBase.h"
#include "ROVehicleType.h"
#include "RORouteDef.h"
#include "RONet.h"
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/gfx/GfxConvHelper.h>
#include "ROVehicleType_Krauss.h"
#include "ROVehicleBuilder.h"
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
RORDLoader_SUMOBase::RORDLoader_SUMOBase(ROVehicleBuilder &vb, RONet &net,
        SUMOTime begin, SUMOTime end,
        const std::string &dataName,
        const std::string &file)
        : ROTypedXMLRoutesLoader(vb, net, begin, end, file),
        myDataName(dataName), myHaveNextRoute(false)
{}


RORDLoader_SUMOBase::~RORDLoader_SUMOBase()
{}


void
RORDLoader_SUMOBase::myStartElement(int element,
                                    const std::string &/*name*/,
                                    const Attributes &attrs)
{
    switch (element) {
    case SUMO_TAG_ROUTE:
        startRoute(attrs);
        break;
    case SUMO_TAG_VEHICLE:
        // try to parse the vehicle definition
        if (!SUMOBaseRouteHandler::openVehicle(*this, attrs, true)) {
            mySkipCurrent = true;
        }
        break;
    case SUMO_TAG_VTYPE:
        startVehType(attrs);
        break;
    default:
        break;
    }
}


void
RORDLoader_SUMOBase::myEndElement(int element, const std::string &/*name*/)
{
    if (element==SUMO_TAG_VEHICLE) {
        closeVehicle();
    }
}


void
RORDLoader_SUMOBase::closeVehicle()
{
    SUMOBaseRouteHandler::closeVehicle();
    // get the vehicle id
    if (myCurrentDepart<myBegin||myCurrentDepart>=myEnd) {
        mySkipCurrent = true;
        return;
    }
    // get vehicle type
    ROVehicleType *type = _net.getVehicleTypeSecure(myCurrentVType);
    // get the route
    RORouteDef *route = _net.getRouteDef(myCurrentRouteName);
    if (route==0) {
        route = _net.getRouteDef("!" + myActiveVehicleID);
    }
    if (route==0) {
        getErrorHandlerMarkInvalid()->inform("The route of the vehicle '" + myActiveVehicleID + "' is not known.");
        return;
    }
    // get the vehicle color
    // build the vehicle
    // get further optional information
    if (!MsgHandler::getErrorInstance()->wasInformed()) {
        if (myCurrentDepart<myBegin||myCurrentDepart>=myEnd) {
            _net.removeRouteSecure(route);
            // !!! was ist mit type?
            return;
        }
        _net.addVehicle(myActiveVehicleID,
                        myVehicleBuilder.buildVehicle(
                            myActiveVehicleID, route, myCurrentDepart, type, myCurrentVehicleColor,
                            myRepOffset, myRepNumber));
    }
}



std::string
RORDLoader_SUMOBase::getDataName() const
{
    return myDataName;
}



void
RORDLoader_SUMOBase::startVehType(const Attributes &attrs)
{
    // get the vehicle type id
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        getErrorHandlerMarkInvalid()->inform("Missing id in vtype.");
        return;
    }
    // get the other values
    SUMOReal maxspeed =
        getFloatReporting(*this, attrs, SUMO_ATTR_MAXSPEED, id, "maxspeed");
    SUMOReal length =
        getFloatReporting(*this, attrs, SUMO_ATTR_LENGTH, id, "length");
    SUMOReal accel =
        getFloatReporting(*this, attrs, SUMO_ATTR_ACCEL, id, "accel");
    SUMOReal decel =
        getFloatReporting(*this, attrs, SUMO_ATTR_DECEL, id, "decel");
    SUMOReal sigma =
        getFloatReporting(*this, attrs, SUMO_ATTR_SIGMA, id, "sigma");
    RGBColor color = parseColor(*this, attrs, "vehicle type", id);
    SUMOVehicleClass vclass = parseVehicleClass(*this, attrs, "vehicle type", id);
    // build the vehicle type after checking
    //  by now, only vehicles using the krauss model are supported
    if (maxspeed>0&&length>0&&accel>0&&decel>0&&sigma>0) {
        _net.addVehicleType(
            new ROVehicleType_Krauss(
                id, color, length, vclass, accel, decel, sigma, maxspeed));
    }
}


SUMOTime
RORDLoader_SUMOBase::getCurrentTimeStep() const
{
    return myCurrentDepart;
}


MsgHandler *
RORDLoader_SUMOBase::getErrorHandlerMarkInvalid()
{
    mySkipCurrent = true;
    return
        OptionsSubSys::getOptions().getBool("continue-on-unbuild")
        ? MsgHandler::getWarningInstance()
        : MsgHandler::getErrorInstance();
}


bool
RORDLoader_SUMOBase::nextRouteRead()
{
    return myHaveNextRoute;
}


void
RORDLoader_SUMOBase::beginNextRoute()
{
    myHaveNextRoute = false;
}



/****************************************************************************/

