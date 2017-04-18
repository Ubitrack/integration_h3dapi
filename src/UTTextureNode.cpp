//
// Created by Ulrich Eck on 13.04.17.
//



#include <H3DUbitrack/UTTextureNode.h>

using namespace H3DUbitrack;
using namespace H3D;

H3DNodeDatabase UTTextureNode::database(
        "UTTextureNode",
        NULL,
        typeid( UTTextureNode ),
        &X3DTextureNode::database
);

namespace UTTextureNodeInternals {
FIELDDB_ELEMENT( UTTextureNode, repeatS, INPUT_OUTPUT );
FIELDDB_ELEMENT( UTTextureNode, repeatT, INPUT_OUTPUT );
FIELDDB_ELEMENT( UTTextureNode, scaleToPowerOfTwo, INPUT_OUTPUT );
FIELDDB_ELEMENT( UTTextureNode, textureProperties, INPUT_OUTPUT );
}
