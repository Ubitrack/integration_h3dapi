#include <H3DUbitrack/H3DUbitrack.hh>
#include <string.h>

H3D::LibraryInfo getLibraryInfo() {
  H3D::LibraryInfo r = H3D::LibraryInfo::createEmptyInfo();

  strcpy( r.name, "H3DUbitrack" );
  strcpy( r.version, H3DUbitrack_FULL_VERSION );
  strcpy( r.developer, "Ulrich Eck" );
  strcpy( r.developer_web, "http://www.magicvisionlab.com" );
  strcpy( r.info, "This is the H3DUbitrack package, an extension for H3D that provides glue to integrate Ubitrack into the H3D Platform." );
  return r;
}
