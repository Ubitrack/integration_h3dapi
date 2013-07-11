/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef UBITRACKMEASUREMENTS_H_
#define UBITRACKMEASUREMENTS_H_


#include <H3D/X3DNode.h>

#include <H3D/SFString.h>
#include <H3D/SFVec3f.h>
#include <H3D/SFRotation.h>
#include <H3D/SFMatrix4f.h>

#include <H3D/ThreadSafeFields.h>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/UbitrackInstance.h>

#include <utFacade/SimpleFacade.h>
#include <utFacade/SimpleDatatypes.h>

#include <boost/shared_ptr.hpp>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

class UbitrackInstance;    

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
      MeasurementMode::Mode getMeasurementMode();
    };


    UbitrackMeasurement( 
		H3D::Inst< H3D::SFNode > _metadata = 0,
        H3D::Inst< H3D::SFString   > _name = 0,
		H3D::Inst< MeasurementMode > _mode = 0
		);

    virtual ~UbitrackMeasurement();

    virtual void update(unsigned long long ts) = 0;
    
	// mode: PUSH/PULL
	std::auto_ptr< MeasurementMode > mode;

	// name of dataflow component
	std::auto_ptr< H3D::SFString > name;
    
    /** called to connect push receivers/pull senders. */
    virtual bool connect(UbitrackInstance* instance) = 0;

    /** called to disconnect push receivers/pull senders. */
    virtual bool disconnect(UbitrackInstance* instance) = 0;

    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;
    
};	


class H3DUBITRACK_API PoseReceiver : public UbitrackMeasurement {
    
public:
    PoseReceiver(   H3D::Inst< H3D::SFNode               > _metadata = 0,
                    H3D::Inst< H3D::SFString             > _name = 0,
                    H3D::Inst< MeasurementMode           > _mode = 0,
                    H3D::Inst< TSSField(H3D::SFMatrix4f) > _matrix   = 0,
                    H3D::Inst< TSSField(H3D::SFVec3f)    > _translation = 0,
                    H3D::Inst< TSSField(H3D::SFRotation) > _rotation = 0
                    );
    
    std::auto_ptr< TSSField(H3D::SFMatrix4f) > matrix;
    std::auto_ptr< TSSField(H3D::SFVec3f)    > translation;
    std::auto_ptr< TSSField(H3D::SFRotation) > rotation;
    
    virtual string defaultXMLContainerField() { return "receiver"; }
    
    void update(unsigned long long ts);
    
    /** called to connect push receivers/pull senders. */
    virtual bool connect(UbitrackInstance* instance);
    
    /** called to disconnect push receivers/pull senders. */
    virtual bool disconnect(UbitrackInstance* instance);
    
    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;

    class PoseReceiverCB : public SimplePoseReceiver {
    public:
        PoseReceiverCB(PoseReceiver* _parent);
        virtual void receivePose( const SimplePose& pose ) throw();
        PoseReceiver* parent;
    };
    
    void receivePose(const SimplePose& pose);
    
    
protected:
    
    PoseReceiverCB* push_receiver;
    SimpleApplicationPullSinkPose* pull_receiver;
    
    bool connected;
    
};
    
    
} // namespace H3DUbitrack

#endif //UBITRACKMEASUREMENTS_H_
