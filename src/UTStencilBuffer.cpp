#include <H3DUbitrack/UTStencilBuffer.h>

using namespace H3D;
using namespace H3DUbitrack;


H3DNodeDatabase UTStencilBuffer::database
( "UTStencilBuffer",
  &(newInstance<UTStencilBuffer>),
  typeid( UTStencilBuffer ),
  &X3DChildNode::database );

namespace UTStencilBufferInternals {
	FIELDDB_ELEMENT( UTStencilBuffer, texture, INPUT_OUTPUT );
}

void UTStencilBuffer::MFImageTextureNode::onAdd( Node *n) {
  MFImageTextureNode_t::onAdd(n);
  UTStencilBuffer *parent =
    static_cast<UTStencilBuffer*>(owner);
  UTImageTexture *t = dynamic_cast<UTImageTexture*>(n);

  t->repeatS->route(parent->displayList);

}

void UTStencilBuffer::MFImageTextureNode::onRemove( Node *n) {
  MFImageTextureNode_t::onRemove(n);

  if( n == NULL ){ return; }

  UTStencilBuffer *parent = static_cast<UTStencilBuffer*>(owner);
  UTImageTexture *t = dynamic_cast<UTImageTexture*>(n);

  t->repeatS->unroute(parent->displayList);
}


UTStencilBuffer::UTStencilBuffer(Inst< SFNode    > _metadata,
                                      Inst< DisplayList > _displayList,
                                      Inst< MFImageTextureNode  > _texture)
  : X3DChildNode( _metadata )
  , H3DDisplayListObject( _displayList )
  , texture( _texture )
{

  type_name = "UTStencilBuffer";
  database.initFields( this );
  displayList->setOwner( this );

  displayList->touch();

  texture->route( displayList );
}



void UTStencilBuffer::render() {
	X3DChildNode::render();
	Vec4f viewport = Vec4f(0, 1, 0, 1);
	Viewpoint::EyeMode eye_mode = Viewpoint::MONO;
	int texture_idx = 0;

	if (texture->size() > 1) {
		UTCameraViewpoint *cvp = dynamic_cast<UTCameraViewpoint *>(UTCameraViewpoint::getActive());
		if (cvp) {
			viewport = cvp->getViewport();
			eye_mode = cvp->getCurrentEye();
		}
		if (eye_mode == Viewpoint::RIGHT_EYE) {
			texture_idx = 1;
		}

	} else if (!(texture->size() > 0)) {
		return;
	}
	UTImageTexture *_texture = dynamic_cast<UTImageTexture *>(texture->getValueByIndex(texture_idx, id));

	_texture->drawAsStencilBuffer();

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL,1,1);
	glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);

}

