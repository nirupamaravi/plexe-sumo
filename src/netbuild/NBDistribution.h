#ifndef NBDistribution_h
#define NBDistribution_h
//---------------------------------------------------------------------------//
//                        NBDistribution.h -
//  The base class for statistical distribution descriptions
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
// $Log$
// Revision 1.7  2005/10/07 11:38:18  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/15 12:02:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/04/27 11:48:25  dkrajzew
// level3 warnings patched; made containers non-static
//
// Revision 1.1.2.1  2005/04/15 09:53:22  dksumo
// using a single SUMOTime type for time values; level3 warnings removed; made the containers unstatic; debugging; added the possibility to change tls offsets
//
// Revision 1.1  2004/10/22 12:49:48  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.4  2003/06/05 11:43:34  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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
#include <map>
#include <utils/distribution/Distribution.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NBDistribution {
public:
     static bool dictionary(const std::string &type, const std::string &id,
        Distribution *d);
     static Distribution *dictionary(const std::string &type,
        const std::string &id);
     static void clear();
private:
    typedef std::map<std::string, Distribution*> DistDict;
    typedef std::map<std::string, DistDict> TypedDistDict;
    static TypedDistDict myDict;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

