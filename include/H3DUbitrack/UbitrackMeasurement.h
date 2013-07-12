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
#include <H3DUbitrack/UbitrackInstance.h>

#include <utFacade/AdvancedFacade.h>
#include <utMeasurement/Measurement.h>

#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

class UbitrackInstance;    


template< class R, typename M >
void applyMeasurement(R* receiver, const M& measurement) {
	receiver->updateMeasurement(measurement);
};


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
    	};
    };


    UbitrackMeasurement( 
		H3D::Inst< H3D::SFNode > _metadata = 0,
        H3D::Inst< H3D::SFString   > _name = 0,
        H3D::Inst< H3D::SFBool   > _isSyncSource = 0,
		H3D::Inst< MeasurementMode > _mode = 0
		): X3DNode(_metadata)
    , name(_name)
    , mode(_mode)
    , isSyncSource(_isSyncSource)
    , last_timestamp(0)
    {

        type_name = "UbitrackMeasurement";
        // is abstract type .. n
        //database.initFields( this );

        mode->addValidValue( "PULL" );
        mode->addValidValue( "PUSH" );
        mode->setValue("PUSH");

        isSyncSource->setValue(false, id );
    };

    virtual ~UbitrackMeasurement() {};

	// mode: PUSH/PULL
	std::auto_ptr< MeasurementMode > mode;

	// name of dataflow component
	std::auto_ptr< H3D::SFString > name;
    
	// bool identifier if this measurement should be used as sync source
	std::auto_ptr< H3D::SFBool > isSyncSource;


    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;



    virtual void update(unsigned long long ts) = 0;

	/** called to connect push receivers/pull senders. */
    virtual bool connect(UbitrackInstance* instance) = 0;

    /** called to disconnect push receivers/pull senders. */
    virtual bool disconnect(UbitrackInstance* instance) = 0;

    // if this node is a sync source, this will be used by the instance
    // to coordinate the rendering
    inline void notify_data_ready(unsigned long long int ts) {
    	data_ready.lock();
    	last_timestamp = ts;
    	data_ready.signal();
    	data_ready.unlock();
    }

    inline unsigned long long int wait_for_data_ready() {
    	data_ready.lock();
    	data_ready.wait();
    	data_ready.unlock();
    	return last_timestamp;
    }



    template< class P, class M >
    class ReceiverCB {
    public:

    	inline ReceiverCB(P* _parent) : parent(_parent), dirty(false) {};

    	inline void receiveMeasurement(const M& measurement) {
    		lock.lock();
    		received_measurements.push_back(measurement);
    	    dirty = true;
    	    lock.unlock();
	    	parent->notify_data_ready(measurement.time());
    	}

    	inline void transferMeasurements() {
        	lock.lock();
        	if (dirty) {
        		for (typename std::vector<M>::iterator it = received_measurements.begin();
        			 it != received_measurements.end(); ++it) {
        			applyMeasurement<P,M>(parent, *it);
        		}
        		dirty = false;
        	}
        	lock.unlock();
        };

        P* parent;
        H3DUtil::MutexLock lock;
        std::vector<M> received_measurements;
        bool dirty;
    };


protected:

    H3DUtil::ConditionLock data_ready;
    unsigned long long int last_timestamp;


};	



} // namespace H3DUbitrack

#endif //UBITRACKMEASUREMENTS_H_
