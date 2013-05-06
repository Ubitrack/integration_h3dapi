#include <H3DCV/H3DCV.hh>
#include <string.h>

H3D::LibraryInfo getLibraryInfo() {
  H3D::LibraryInfo r = H3D::LibraryInfo::createEmptyInfo();

  strcpy( r.name, "H3DCV" );
  strcpy( r.version, H3DCV_FULL_VERSION );
  strcpy( r.developer, "Ulrich Eck" );
  strcpy( r.developer_web, "http://www.magicvisionlab.com" );
  strcpy( r.info, "This is the H3DCV package, an extension for H3D that implements additional funcationality for H3D Platform." );
  return r;
}
