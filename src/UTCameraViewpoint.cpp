/**
based on Candy HMDViewpoint

*/


#include <H3DUbitrack/UTCameraViewpoint.h>

#include <H3D/Scene.h>

using namespace H3DUbitrack;
using namespace H3D;

H3DNodeDatabase UTCameraViewpoint::database
( "UTCameraViewpoint",
  &(newInstance<UTCameraViewpoint>),
  typeid( UTCameraViewpoint ),
  &Viewpoint::database );

namespace UTCameraViewpointInternals {
  FIELDDB_ELEMENT( UTCameraViewpoint, tracker, INPUT_OUTPUT );
  FIELDDB_ELEMENT( UTCameraViewpoint, frustumLL, INPUT_OUTPUT );
  FIELDDB_ELEMENT( UTCameraViewpoint, frustumUR, INPUT_OUTPUT );
  FIELDDB_ELEMENT( UTCameraViewpoint, eyeTransform, INPUT_OUTPUT );
}

UTCameraViewpoint::UTCameraViewpoint
( Inst< SFSetBind > _set_bind,
  Inst< SFVec3f > _centerOfRotation,
  Inst< SFString > _description,
  Inst< SFFloat > _fieldOfView,
  Inst< SFBool > _jump,
  Inst< SFNode > _metadata,
  Inst< SFOrientation > _orientation,
  Inst< SFPosition > _position,
  Inst< SFBool > _retainUserOffsets,
  Inst< SFTime > _bindTime,
  Inst< SFBool > _isBound,
  Inst< SFMatrix4f > _accForwardMatrix,
  Inst< SFMatrix4f > _accInverseMatrix,
  Inst< SFPoseReceiver > _tracker,
  Inst< MFVec3f > _frustumLL,
  Inst< MFVec3f > _frustumUR,
  Inst< MFMatrix4f > _eyeTransform )

  : Viewpoint(_set_bind,
              _centerOfRotation,
              _description,
              _fieldOfView,
              _jump,
              _metadata,
              _orientation,
              _position,
              _retainUserOffsets,
              _bindTime,
              _isBound,
              _accForwardMatrix,
              _accInverseMatrix),
    tracker( _tracker ),
    frustumLL( _frustumLL ),
    frustumUR( _frustumUR ),
    current_eye( MONO ),
    viewport(Vec4f()),
	eyeTransform( _eyeTransform ) {

  type_name = "UTCameraViewpoint";
  database.initFields( this );
  position->setValue(Vec3f(0,0,0), id);
}

void UTCameraViewpoint::SFPoseReceiver::onAdd(Node *n){
  TypedSFNode<PoseReceiver>::onAdd(n);
  UTCameraViewpoint *parent =
    static_cast<UTCameraViewpoint*>(owner);
  PoseReceiver *tracker = dynamic_cast<PoseReceiver*>(n);
  tracker->translation->route(parent->position);
  tracker->rotation->route(parent->orientation);
}

void UTCameraViewpoint::SFPoseReceiver::onRemove(Node *n){
  TypedSFNode<PoseReceiver>::onRemove(n);

  if( n == NULL ){ return; }

  UTCameraViewpoint *parent = static_cast<UTCameraViewpoint*>(owner);
  PoseReceiver *tracker = dynamic_cast<PoseReceiver*>(n);

  tracker->translation->unroute(parent->position);
  tracker->rotation->unroute(parent->orientation);
}

void UTCameraViewpoint::setupProjection( EyeMode eye_mode, H3DFloat width, H3DFloat height, H3DFloat clip_near,
		H3DFloat clip_far, StereoInfo* stereo_info ){
	double f_left, f_right, f_bottom, f_top;
	Vec3f LL, UR;
	bool custom_frustum = false;
	double ratio;

	// save current eye for UbitrackImageBackground
	current_eye = eye_mode;
	// calculate viewport for UbitrackImageBackground
	if (eye_mode == RIGHT_EYE) {
		viewport = Vec4f(width, 2*width, 0, height);
	} else {
		viewport = Vec4f(0, width, 0, height);
	}

	if ((frustumLL->size() >= 2) && (frustumUR->size() >= 2) && (eye_mode == RIGHT_EYE)) {
		LL = frustumLL->getValueByIndex(1, id);
		UR = frustumUR->getValueByIndex(1, id);
		custom_frustum = true;
    } else if ((frustumLL->size() >= 1) && (frustumUR->size() >= 1) && 
		((eye_mode == LEFT_EYE) || (eye_mode == MONO))) {
		LL = frustumLL->getValueByIndex(0, id);
		UR = frustumUR->getValueByIndex(0, id);
		custom_frustum = true;
	} 
	
	if (custom_frustum) {
		if ((LL.x != 0) && (LL.y != 0) && (LL.z != 0) &&
			(UR.x != 0) && (UR.y != 0) && (UR.z != 0) &&
			(UR.z == LL.z) && (UR.y != 0)) {
			ratio = clip_near / LL.z;
			f_left =  ratio * LL.x;
			f_right = ratio * UR.x;
			f_top = ratio * UR.y;
			f_bottom = ratio * LL.y;
			//Console(3) << "GLfrustum(" << f_left << "," << f_right << "," 
			//	<< f_bottom << "," << f_top << "," << clip_near << "," << clip_far << ")" << std::endl;
			glFrustum( f_left, f_right, f_bottom, f_top, clip_near, clip_far );
		} else {
			Console(4) << "Invalid Frustum Specification, using default calculation" << std::endl;
			Viewpoint::setupProjection(eye_mode, width, height, clip_near, clip_far, stereo_info);
		}
	} else {
	    Viewpoint::setupProjection(eye_mode, width, height, clip_near, clip_far, stereo_info);
	}

}


void UTCameraViewpoint::setupViewMatrix(EyeMode eye_mode,
								   StereoInfo * stereo_info) {
	  const Vec3f &vp_position = totalPosition->getValue();
	  const Rotation &vp_orientation = totalOrientation->getValue();
	  const Matrix4f &vp_inv_m = accInverseMatrix->getValue();
	  // OpenGL version of vp_inv_m
	  GLfloat vp_inv_transform[] = {
	    vp_inv_m[0][0], vp_inv_m[1][0], vp_inv_m[2][0], 0,
	    vp_inv_m[0][1], vp_inv_m[1][1], vp_inv_m[2][1], 0,
	    vp_inv_m[0][2], vp_inv_m[1][2], vp_inv_m[2][2], 0,
	    vp_inv_m[0][3], vp_inv_m[1][3], vp_inv_m[2][3], 1 };

	  setupStereoView(eye_mode, stereo_info);


	  // set up view matrix based on X3DViewpointNode field values.
	  glRotatef( (H3DFloat) -(180/Constants::pi)*vp_orientation.angle,
	             vp_orientation.axis.x,
	             vp_orientation.axis.y,
	             vp_orientation.axis.z );

	  glTranslatef( -vp_position.x,
	                -vp_position.y,
	                -vp_position.z );
	  glMultMatrixf( vp_inv_transform );
}

void UTCameraViewpoint::setupStereoView(EyeMode eye_mode, StereoInfo * stereo_info) {
	  if ((eye_mode == RIGHT_EYE) && (eyeTransform->size() > 1)) {
		  const Matrix4f &eye_transform = eyeTransform->getValueByIndex(1, id);
		  GLfloat eye_transform_gl[] = { 
			eye_transform[0][0], eye_transform[1][0], eye_transform[2][0], 0,
			eye_transform[0][1], eye_transform[1][1], eye_transform[2][1], 0,
			eye_transform[0][2], eye_transform[1][2], eye_transform[2][2], 0,
			eye_transform[0][3], eye_transform[1][3], eye_transform[2][3], 1 };
		  glMultMatrixf(eye_transform_gl);
	  } else if ((eye_mode == LEFT_EYE) && (eyeTransform->size() > 0)) {
		  const Matrix4f &eye_transform = eyeTransform->getValueByIndex(1, id);
		  GLfloat eye_transform_gl[] = { 
			eye_transform[0][0], eye_transform[1][0], eye_transform[2][0], 0,
			eye_transform[0][1], eye_transform[1][1], eye_transform[2][1], 0,
			eye_transform[0][2], eye_transform[1][2], eye_transform[2][2], 0,
			eye_transform[0][3], eye_transform[1][3], eye_transform[2][3], 1 };
		  glMultMatrixf(eye_transform_gl);
	  } else {
		  if( eye_mode != MONO ) {
			// if stereo mode, viewpoint varies depending on what eye
			//is being rendered for.

			H3DFloat interocular_distance = 0.06f;
			Rotation head_tilt = Rotation(1,0,0,0 );
			if( !stereo_info ) stereo_info = StereoInfo::getActive();
			if( stereo_info ) {
			  interocular_distance = stereo_info->interocularDistance->getValue();
			  head_tilt = stereo_info->headTilt->getValue();
			}

			H3DFloat half_interocular_distance = interocular_distance / 2;

			// the vector from the viewpoint center to the left eye.
			Vec3f left_eye =
			  head_tilt * Vec3f( -half_interocular_distance, 0, 0 );

			// move to the eye position
			if( eye_mode == LEFT_EYE ) {
			  glTranslatef( -left_eye.x, -left_eye.y, -left_eye.z );
			} else if( eye_mode == RIGHT_EYE ) {
			  glTranslatef( left_eye.x, left_eye.y, left_eye.z );
			}
		  }
	  }
}
