//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2013, SenseGraphics AB
//
//    This file is part of H3DUbitrack.
//
//    H3DUbitrack is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    H3DUbitrack is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with H3DUbitrack; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
//
/// \file H3DUbitrack.h
/// \brief Base header file for H3DUbitrack
///
//////////////////////////////////////////////////////////////////////////////

/// \mainpage H3DUbitrack documentation
/// Copyright 2013, <a href="http://www.magicvisionlab.com">Magic Vision Lab</a>

#ifndef __H3DUBITRACK_H__
#define __H3DUBITRACK_H__

// The following ifdef block is the standard way of creating macros
// which make exporting from a DLL simpler. All files within this DLL
// are compiled with the H3DUBITRACK_EXPORTS symbol defined on the command
// line. this symbol should not be defined on any project that uses
// this DLL. This way any other project whose source files include
// this file see H3DUBITRACK_API functions as being imported from a DLL,
// whereas this DLL sees symbols defined with this macro as being
// exported.
#ifdef WIN32
#include <windows.h>
#ifdef H3DUBITRACK_EXPORTS
#define H3DUBITRACK_API __declspec(dllexport)
#else
#define H3DUBITRACK_API __declspec(dllimport)
#endif
#ifdef _MSC_VER
// disable dll-interface warnings for stl-exports 
#pragma warning( disable: 4251 )
#endif


#endif

#if defined(__APPLE__) && defined(__MACH__)
#define H3DUBITRACK_API
#endif

#if defined(__linux)
#define H3DUBITRACK_API
#endif

#define H3DUBITRACK_DEBUG() std::cerr << __FUNCTION__ << " --- Line " << __LINE__ << " in " << __FILE__ << std::endl;

#define H3DUBITRACK_MAJOR_VERSION ${H3DUBITRACK_MAJOR_VERSION}
#define H3DUBITRACK_MINOR_VERSION ${H3DUBITRACK_MINOR_VERSION}
#define H3DUBITRACK_BUILD_VERSION ${H3DUBITRACK_BUILD_VERSION}

// is OpenHaptics available
#cmakedefine HAVE_OPENHAPTICS

// also might want to check for CalibratedPhantom ..

#include <H3D/LibraryInfo.h>

extern "C" H3DUBITRACK_API H3D::LibraryInfo getLibraryInfo();

/// \defgroup H3DUbitrackClasses H3DUbitrack Classes
/// Extra classes in H3DUbitrack used for various things.

/// \ingroup Nodes
/// \defgroup H3DUbitrackNodes H3DUbitrack Node Classes.
/// These are the Node classes belonging to H3DUbitrack.

#endif

