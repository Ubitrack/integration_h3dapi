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
//    A commercial license is also available. Please contact us at 
//    www.sensegraphics.com for more information.
//
//
//
/// \file H3DUbitrack.cpp
/// \brief cpp file for H3DUbitrack
///
//////////////////////////////////////////////////////////////////////////////

#include <H3DUbitrack/H3DUbitrack.h>
#include <sstream>
#include <cstring>

using namespace std;

H3D::LibraryInfo getLibraryInfo() {
  H3D::LibraryInfo r = H3D::LibraryInfo::createEmptyInfo();
  std::stringstream s;
  s << H3DUBITRACK_MAJOR_VERSION << "."
    << H3DUBITRACK_MINOR_VERSION << " (build "
    << H3DUBITRACK_BUILD_VERSION << ")";

  strcpy(r.name, "H3DUbitrack" );
  strcpy( r.version, s.str().c_str() );
  strcpy( r.web, "http://www.magicvisionlab.com" );
  strcpy( r.developer, "Magic Vision Lab" );
  strcpy( r.developer_web, "http://www.magicvisionlab.com.com" );
  strcpy( r.info, "H3DUbitrack provides a bridge to the UbiTrack Dataflow Library." );
  return r;

}
