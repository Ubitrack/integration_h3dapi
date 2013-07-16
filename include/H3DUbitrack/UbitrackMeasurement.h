/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef UBITRACKMEASUREMENTS_H_
#define UBITRACKMEASUREMENTS_H_


#include <H3D/X3DNode.h>

#include <H3D/SFBool.h>
#include <H3D/SFString.h>
#include <H3D/SFVec3f.h>
#include <H3D/SFRotation.h>
#include <H3D/SFMatrix4f.h>

#include <H3DUtil/Threads.h>

#include <H3DUbitrack/H3DUbitrack.h>

#include <utFacade/AdvancedFacade.h>
#include <utMeasurement/Measurement.h>

#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

class H3DUBITRACK_API UbitrackMeasurement : public H3D::X3DNode {

public:
    /// The mode for rendering specified as a string.
    class H3DUBITRACK_API MeasurementMode: public H3D::SFString {
    public:
      /// Thrown when the value of MeasurementMode is an invalid mode.
      H3D_VALUE_EXCEPTION( string, InvalidMeasurementMode );
      /// The different measurement modes supported.
      typedef enum {
        /// Pull
        PULL,
        /// Push
        PUSH
      } Mode;

      /// Get the current MeasurementMode
      /// \throws InvalidMeasurementMode if the string is an invalid MeasurementMode.
      inline MeasurementMode::Mode getMeasurementMode() {
      	upToDate();
      	if( value == "PULL" )
      		return PULL;
      	else if( value == "PUSH" )
      		return PUSH;
      	else {
      		stringstream s;
      		s << "Must be one of "
      		<< "PULL, "
      		<< "PUSH ";
      		throw InvalidMeasurementMode( value,
      								s.str(),
      								H3D_FULL_LOCATION );
      	}

      }

 	  inline bool is_push() {
		  if (value == "PUSH")
			  return true;
		  return false;
	  }

    };


    UbitrackMeasurement(
    	H3D::Inst< H3D::SFNode     > _metadata = 0,
        H3D::Inst< H3D::SFString   > _pattern = 0,
		H3D::Inst< MeasurementMode > _mode = 0
		);

    virtual ~UbitrackMeasurement() {};

	// mode: PUSH/PULL
	std::auto_ptr< MeasurementMode > mode;

	// pattern of dataflow component
	std::auto_ptr< H3D::SFString > pattern;
    

    virtual void initialize() = 0;

    virtual void update(unsigned long long ts) = 0;

	/** called to connect push receivers/pull senders. */
    virtual bool connect(Ubitrack::Facade::AdvancedFacade* facade) = 0;

    /** called to disconnect push receivers/pull senders. */
    virtual bool disconnect(Ubitrack::Facade::AdvancedFacade* facade) = 0;


};	



} // namespace H3DUbitrack

#endif //UBITRACKMEASUREMENTS_H_
