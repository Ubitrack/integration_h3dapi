/**

based on Candy HMDViewpoint

*/


#ifndef _H3DUTCAMERAVIEWPOINT_HH_
#define _H3DUTCAMERAVIEWPOINT_HH_

#include <H3DUbitrack/H3DUbitrack.h>

#include <H3D/Viewpoint.h>
#include <H3D/SFFloat.h>
#include <H3D/SFMatrix4f.h>
#include <H3D/MFMatrix4f.h>
#include <H3D/SFVec3f.h>
#include <H3D/MFVec3f.h>
#include <H3D/SFString.h>
#include <H3D/SFInt32.h>

#include <GL/glew.h>

#include <H3DUbitrack/PoseReceiver.h>

namespace H3DUbitrack {

	using namespace H3D;

  /**
     This is a Viewpoint node that produces a HMD style view, where
     the screen follows the head.

     \beginX3D
     \exposedField marker \n1 SFTracker \n2 (0) \n3
      The tracker that defines the world coordinates for this
      viewpoint. \eol
     \endX3D
  */
  class H3DUBITRACK_API UTCameraViewpoint
    : public H3D::Viewpoint {


  public:

    struct H3DUBITRACK_API SFPoseReceiver
      : H3D::TypedSFNode<PoseReceiver> {
      inline ~SFPoseReceiver(){ setValue(0); }
      void onAdd(Node *n);
      void onRemove(Node *n);
    };

    UTCameraViewpoint( H3D::Inst< H3D::Viewpoint::SFSetBind > _set_bind = 0,
                  H3D::Inst< H3D::SFVec3f > _centerOfRotation = 0,
                  H3D::Inst< H3D::SFString > _description = 0,
                  H3D::Inst< H3D::SFFloat > _fieldOfView = 0,
                  H3D::Inst< H3D::SFBool > _jump = 0,
                  H3D::Inst< H3D::SFNode > _metadata = 0,
                  H3D::Inst< H3D::Viewpoint::SFOrientation > _orientation = 0,
                  H3D::Inst< H3D::Viewpoint::SFPosition > _position = 0,
                  H3D::Inst< H3D::SFBool > _retainUserOffsets = 0,
                  H3D::Inst< H3D::SFTime > _bindTime = 0,
                  H3D::Inst< H3D::SFBool > _isBound = 0,
                  H3D::Inst< H3D::SFMatrix4f > _accForwardMatrix = 0,
                  H3D::Inst< H3D::SFMatrix4f > _accInverseMatrix = 0,
                  H3D::Inst< SFPoseReceiver > _tracker = 0,
                  H3D::Inst< H3D::MFVec3f > _frustumLL = 0,
                  H3D::Inst< H3D::MFVec3f > _frustumUR = 0,
                  H3D::Inst< H3D::MFMatrix4f > _eyeTransform = 0 );

    /** The tracker that controls this viewpoint. */
    auto_ptr< SFPoseReceiver > tracker;

	auto_ptr< H3D::MFVec3f > frustumLL;
	auto_ptr< H3D::MFVec3f > frustumUR;

	auto_ptr< H3D::MFMatrix4f > eyeTransform;

	/// uses default implementation in case no parameters are given,
	/// otherwise the frustum is calculated based on the calibration.
	virtual void setupProjection( EyeMode eye_mode,
                                   H3DFloat width, H3DFloat height,
                                   H3DFloat clip_near, H3DFloat clip_far,
								   StereoInfo* stereo_info );

    /// Modified to support subclasses with different stereo behaviour
    virtual void setupViewMatrix( EyeMode eye_mode,
                                  StereoInfo * stereo_info = 0 );


    /// allow subclasses to overwrite the stereo rendering default behaviour
    /// if you do so, you probably also want to overwrite setupProjection
    /// to reflect the actual stereo fov in the frustum.
    virtual void setupStereoView(EyeMode eye_mode, StereoInfo * stereo_info);

	/// needed for stereo background 
	inline EyeMode getCurrentEye() {
		return current_eye;
	}

	inline Vec4f getViewport() {
		return viewport;
	}

    static H3D::H3DNodeDatabase database;

  protected:
	  EyeMode current_eye;
	  Vec4f viewport;

  };
}

#endif
