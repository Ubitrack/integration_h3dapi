#include <H3DUbitrack/UTStereoTextureSelector.h>

using namespace H3D;
using namespace H3DUbitrack;


H3DNodeDatabase UTStereoTextureSelector::database
( "UTStereoTextureSelector",
  &(newInstance<UTStereoTextureSelector>),
  typeid( UTStereoTextureSelector ),
  &X3DChildNode::database );

namespace UTStereoTextureSelectorInternals {
	FIELDDB_ELEMENT( UTStereoTextureSelector, texture, INPUT_OUTPUT );
	FIELDDB_ELEMENT( UTStereoTextureSelector, textureOut, OUTPUT_ONLY );
	FIELDDB_ELEMENT( UTStereoTextureSelector, textureId, OUTPUT_ONLY );
}

void UTStereoTextureSelector::MFImageTextureNode::onAdd( Node *n) {
  MFImageTextureNode_t::onAdd(n);
  UTStereoTextureSelector *parent =
    static_cast<UTStereoTextureSelector*>(owner);
  UTImageTexture *t = dynamic_cast<UTImageTexture*>(n);

//  t->repeatS->route(parent->displayList);

}

void UTStereoTextureSelector::MFImageTextureNode::onRemove( Node *n) {
  MFImageTextureNode_t::onRemove(n);

  if( n == NULL ){ return; }

  UTStereoTextureSelector *parent = static_cast<UTStereoTextureSelector*>(owner);
  UTImageTexture *t = dynamic_cast<UTImageTexture*>(n);

//  t->repeatS->unroute(parent->displayList);
}


UTStereoTextureSelector::UTStereoTextureSelector(Inst< SFNode   > _metadata,
                                      Inst< MFImageTextureNode  > _texture,
									  Inst< SFTextureNode       > _textureOut,
									  Inst< SFInt32             > _textureId)
  : X3DChildNode( _metadata )
  , texture( _texture )
  , textureOut(_textureOut)
  , textureId(_textureId)
{

  type_name = "UTStereoTextureSelector";
  database.initFields( this );
  textureId->setValue(0, id);

}


// this is a hack to set up the correct texture for each render pass during stereo rendering
// this works only in combination with UTImageTexture lists created from UbitrackImageReceivers
void UTStereoTextureSelector::render() {
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
	textureId->setValue( texture_idx, id );

	// this needs to happen during rendering in order to know which eye is rendered
	// field value changes should normally only happen during traverseSG
	// if there is a better way do do it - please let me know :)
	UTImageTexture *_texture = dynamic_cast<UTImageTexture *>(texture->getValueByIndex(texture_idx, id));
	if (_texture) {
		textureOut->setValue( _texture, id);
	}
}

