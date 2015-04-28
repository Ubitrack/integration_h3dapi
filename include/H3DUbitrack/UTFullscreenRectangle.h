#ifndef _UTFULLSCREENRECTANGLE_HH_
#define _UTFULLSCREENRECTANGLE_HH_

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/UTCameraViewpoint.h>
#include <H3D/X3DAppearanceNode.h>

#include <H3D/X3DTexture2DNode.h>
#include <H3D/X3DTextureNode.h>
#include <H3D/X3DTextureCoordinateNode.h>
#include <H3D/DependentNodeFields.h>
#include <H3D/SFFloat.h>
#include <H3D/SFBool.h>

#include <H3D/X3DChildNode.h>
#include <H3D/H3DWindowNode.h>
#include <H3D/SFFloat.h>

namespace H3DUbitrack {
	using namespace H3D;
	/**
     This is a background node that renders the ImageBuffer
     to the background thereby providing a see-through Augmented
     Reality environment.
     
     \beginX3D
     \exposedField zoom \n1 SFFloat \n2 1 \n3
      The background image camera zoom. \eol
     \endX3D
  */
  class H3DUBITRACK_API UTFullscreenRectangle
    : public H3D::X3DChildNode,
      public H3DDisplayListObject {
  
  public:

    /// SFAppearanceNode is dependent on the displayList field of its
    /// encapsulated X3DAppearanceNode node, i.e. an event from that
    /// field will trigger an event from the SFAppearanceNode as well.
    typedef DependentSFNode< X3DAppearanceNode,
                             FieldRef<H3DDisplayListObject, 
                                      H3DDisplayListObject::DisplayList,
                                      &H3DDisplayListObject::displayList >, 
                             true > 
    SFAppearanceNode;



    /** Initializes local members */
    UTFullscreenRectangle(Inst< SFNode > _metadata = 0,
		       Inst< DisplayList       > _displayList = 0,
		       Inst< SFAppearanceNode  > _appearance  = 0,
			   Inst< SFBool            > _horizontalFlip = 0,
			   Inst< SFBool            > _verticalFlip = 0,
			   Inst< SFBool            > _depthMaskEnable = 0,
			   Inst< SFBool            > _depthTestEnable = 0);
    
    static H3D::H3DNodeDatabase database;
    

    /// Renders the background with OpenGL.
    virtual void render();

    /// The field containing the X3DAppearance node to be used when
    /// rendering the shape.
    /// 
    /// <b>Access type:</b> inputOutput
    /// 
    /// \dotfile X3DShapeNode_appearance.dot
    auto_ptr<    SFAppearanceNode  >  appearance;

	// horizontal flip
    auto_ptr< SFBool >  horizontalFlip;

	// vertical flip
    auto_ptr< SFBool >  verticalFlip;

	// enable DepthMask (writing to depth buffer)
    auto_ptr< SFBool >  depthMaskEnable;

	// enable DepthTest
    auto_ptr< SFBool >  depthTestEnable;

  protected:
    inline void renderTexCoordForTexture( const Vec3f &tc, 
                                          X3DTextureNode *t ) {
      X3DTextureCoordinateNode::renderTexCoordForTexture( tc, t );
    }

  };
}

#endif
