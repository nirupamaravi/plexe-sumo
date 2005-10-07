#ifndef NILoader_h
#define NILoader_h
/***************************************************************************
                          NILoader.h
                          An interface to the loading operations of the
                          netconverter
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.7  2005/10/07 11:41:37  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:13:19  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/04/27 12:24:24  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.3  2004/07/02 09:34:37  dkrajzew
// elmar and tiger import added
//
// Revision 1.2  2003/08/18 12:40:17  dkrajzew
// xerces 2.2 and later compatibility patched
//
// Revision 1.1  2003/07/17 12:15:39  dkrajzew
// NBLoader moved to netimport; NBNetBuilder performs now all the building steps
//
// Revision 1.6  2003/06/19 07:17:21  dkrajzew
// new message subsystem paradigm applied
//
// Revision 1.5  2003/06/18 11:13:13  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.4  2003/03/20 16:23:09  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.3  2003/03/12 16:47:54  dkrajzew
// extension for artemis-import
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.5  2002/07/25 08:36:43  dkrajzew
// Visum7.5 and Cell import added
//
// Revision 1.4  2002/06/11 16:00:40  dkrajzew
// windows eol removed; template class definition inclusion depends
//  now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.3  2002/05/14 04:42:55  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:11  dkrajzew
// Windows eol removed; minor SUMOReal to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
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
#include <netbuild/NLLoadFilter.h>
#include <sax2/SAX2XMLReader.hpp>


/* =========================================================================
 * xerces 2.2 compatibility
 * ======================================================================= */
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


/* =========================================================================
 * class definitions
 * ======================================================================= */
class OptionsCont;
class SUMOSAXHandler;
class LineReader;
class LineHandler;
class NBNetBuilder;


/* =========================================================================
 * class declarations
 * ======================================================================= */
/**
 * NILoader
 * This class loads the specified data using a loading method that accords
 * to the data type specified by the user.
 * Subclasses may be used for the loading process, too, as it is done while
 * loading the XML-descriptions.
 */
class NILoader {
public:
    NILoader(NBNetBuilder &nb);
    ~NILoader();

    /** loads data from the files specified in the given option container */
    void load(OptionsCont &oc);

private:
    /** loads data from sumo-files */
    void loadSUMO(OptionsCont &oc);

    /** loads net or logics */
    void loadSUMOFiles(OptionsCont &oc, LoadFilter what,
        const std::string &files, const std::string &type);

    /** loads data from XML-files */
    void loadXML(OptionsCont &oc);

    /** loads data from the list of xml-files of certain type */
    void loadXMLType(SUMOSAXHandler *handler,
        const std::string &files, const std::string &type);

    /** loads data from a single xml-file */
    void loadXMLFile(SAX2XMLReader &parser, const std::string &file,
        const std::string &type);

    /** loads data from cell-input-files */
    void loadCell(OptionsCont &oc);

    /** loads data from artemis-input-files */
    void loadArtemis(OptionsCont &oc);

    /** reads using a file reader */
    bool useLineReader(LineReader &lr, const std::string &file,
        LineHandler &lh);

    /** loads data from visum-input-file */
    void loadVisum(OptionsCont &oc);

    /** loads data from arcview-files */
    void loadArcView(OptionsCont &oc);

    /** loads data from vissim-input-file */
    void loadVissim(OptionsCont &oc);

    /// loads Elmar's data parsed from GDF
    void loadElmar(OptionsCont &oc);

    void loadTiger(OptionsCont &oc);

private:
    NBNetBuilder &myNetBuilder;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
