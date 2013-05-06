/**
   Copyright 2010, Ulrich Eck
   

This file is part of H3DUbitrack.

H3DUbitrack is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

H3DUbitrack is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Candy; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/**
   \mainpage
   
   This is the documentation for the H3D CV package
   (
   \htmlinclude H3DUbitrack_VERSION for
   \htmlinclude H3D_VERSION
   )
   
   The H3DUbitrack package is an example for a H3D-API Extension.
*/



#ifndef _H3DUbitrack_HH_
#define _H3DUbitrack_HH_

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#ifdef H3DUbitrack_EXPORTS
#define H3DUbitrack_API __declspec(dllexport)
#else
#define H3DUbitrack_API __declspec(dllimport)
#endif
#ifdef _MSC_VER
// disable dll-interface warnings for stl-exports 
#pragma warning( disable: 4251 )
#endif


#endif

#if defined(__APPLE__) && defined(__MACH__)
#define H3DUbitrack_API
#define HAVE_SYS_TIME_H
#endif

#if defined(__linux)
#define H3DUbitrack_API
#define HAVE_SYS_TIME_H
#endif


#cmakedefine HAVE_Ubitrack


#define H3DUbitrack_FULL_VERSION "${H3DUbitrack_FULL_VERSION}"

#include <H3D/LibraryInfo.h>

extern "C" H3DUbitrack_API H3D::LibraryInfo getLibraryInfo();

#endif

