/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef UBITRACKMEASUREMENTS_H_
#define UBITRACKMEASUREMENTS_H_


#include <H3D/X3DNode.h>

#include <H3D/ThreadSafeFields.h>

#include <H3D/SFString.h>
#include <H3D/SFVec3f.h>
#include <H3D/SFRotation.h>
#include <H3D/SFMatrix4f.h>


#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/UbitrackInstance.h>

#include <utFacade/SimpleDatatypes.h>

namespace H3DUbitrack {

class H3DUBITRACK_API UbitrackMeasurement : public H3D::X3DNode {

public:
    /// The mode for rendering specified as a string.
    class H3DUBITRACK_API MeasurementType: public H3D::SFString {
    public:
      /// Thrown when the value of MeasurementType is an invalid mode.
      H3D_VALUE_EXCEPTION( string, InvalidMeasurementType );
      /// The different measurement modes supported.
      typedef enum {
        /// Pull sink
        PULL_SINK,
        /// Push sink
        PUSH_SINK,
        /// Pull source
        PULL_SOURCE,
        /// Push source
        PUSH_SOURCE
      } Type;
		
      /// Returns true if the measurement is a sink.
      inline bool isSink() { 
        upToDate();
		if ((value == "PULL_SINK") || (value == "PUSH_SINK"))
        	return true;
		return false;
      }

      /// Returns true if the measurement mode is push.
      inline bool isPush() { 
        upToDate();
		if ((value == "PUSH_SOURCE") || (value == "PUSH_SINK"))
        	return true;
		return false;
      }

      /// Get the current MeasurementType
      /// \throws InvalidMeasurmentType if the string is an invalid MeasurementType.
      MeasurementType::Type getMeasurementType();
    };


    UbitrackMeasurement( 
		H3D::Inst< H3D::SFNode > _metadata = 0,
		H3D::Inst< MeasurementType > _measurementType = 0
		);

    virtual ~UbitrackMeasurement();

    //virtual string defaultXMLContainerField() {
    //  return "receivers"; }

    virtual void traverseSG(H3D::TraverseInfo &ti);

	// mode: PUSH/PULL
	std::auto_ptr< MeasurementType > measurementType;

    /** called to connect push receivers/pull senders. */
    virtual void connect(UbitrackInstance &instance);

    /** called to disconnect push receivers/pull senders. */
    virtual void disconnect(UbitrackInstance &instance);


protected:
	virtual void connect_pull_source(UbitrackInstance &instance) = 0;
	virtual void connect_push_sink(UbitrackInstance &instance) = 0;
};	


class H3DUBITRACK_API PoseMeasurement : public UbitrackMeasurement {
    
public:
    PoseMeasurement(
                    H3D::Inst< H3D::SFNode               > _metadata = 0,
                    H3D::Inst< H3D::SFString             > _mode = 0,
                    H3D::Inst< TSSField(H3D::SFMatrix4f) > _matrix   = 0,
                    H3D::Inst< TSSField(H3D::SFVec3f)    > _translation = 0,
                    H3D::Inst< TSSField(H3D::SFRotation) > _rotation = 0
                    );
    
    std::auto_ptr< TSSField(H3D::SFMatrix4f) > matrix;
    std::auto_ptr< TSSField(H3D::SFVec3f)    > translation;
    std::auto_ptr< TSSField(H3D::SFRotation) > rotation;
    
    virtual void traverseSG(H3D::TraverseInfo &ti);
    
    /** called to connect push receivers/pull senders. */
    virtual void connect(UbitrackInstance &instance);
    
    /** called to disconnect push receivers/pull senders. */
    virtual void disconnect(UbitrackInstance &instance);
    
    static H3D::H3DNodeDatabase database;
    
protected:
    SimplePoseReceiver _receiver;
    
};
    
    
} // namespace H3DUbitrack

#endif //UBITRACKMEASUREMENTS_H_