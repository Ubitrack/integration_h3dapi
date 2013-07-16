/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef MEASUREMENTRECEIVER_H_
#define MEASUREMENTRECEIVER_H_

#include <H3DUtil/Threads.h>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/UbitrackMeasurement.h>

#include <utComponents/ApplicationPullSink.h>

#include <boost/bind.hpp>
#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

class H3DUBITRACK_API MeasurementReceiverBase : public UbitrackMeasurement {
public:
	H3D_VALUE_EXCEPTION( string, NotImplementedError );

	MeasurementReceiverBase(
	    	H3D::Inst< H3D::SFNode     > _metadata = 0,
	        H3D::Inst< H3D::SFString   > _pattern = 0,
	        H3D::Inst< H3D::SFBool     > _isSyncSource = 0,
			H3D::Inst< MeasurementMode > _mode = 0
			);

	// bool identifier if this measurement should be used as sync source
	std::auto_ptr< H3D::SFBool > isSyncSource;

    virtual string defaultXMLContainerField() { return "receiver"; }

	virtual void initialize() {
		// noop per default
	}

	virtual void update(unsigned long long ts) {
		throw NotImplementedError("MeasurementReceiverBase::update");
	}

	/** called to connect push receivers/pull senders. */
	virtual bool connect(Ubitrack::Facade::AdvancedFacade* facade) {
		throw NotImplementedError("MeasurementReceiverBase::connect");
	}

	/** called to disconnect push receivers/pull senders. */
	virtual bool disconnect(Ubitrack::Facade::AdvancedFacade* facade) {
		throw NotImplementedError("MeasurementReceiverBase::disconnect");
	}


    inline void notify_data_ready(unsigned long long int ts) {
    	//H3D::Console(4) << "notify data is ready: " << pattern->getValue(id) << std::endl;
    	data_ready.lock();
    	last_timestamp = ts;
    	data_ready.signal();
    	data_ready.unlock();
    }

    inline unsigned long long int wait_for_data_ready() {
    	data_ready.lock();
    	// milliseconds
    	while (!data_ready.timedWait(100)) {
    		H3D::Console(4) << "ubitrack sync timeout..: " << pattern->getValue(id) << std::endl;
    		if (!connected)
        		//H3D::Console(4) << " not connected" << std::endl;
    			break;
    	}
    	data_ready.unlock();
    	return last_timestamp;
    }

    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;

protected:
    H3DUtil::ConditionLock data_ready;
	H3DUtil::MutexLock lock;
    unsigned long long int last_timestamp;
    bool connected;
	//bool dirty;


};

template< class M , class S >
class H3DUBITRACK_API MeasurementReceiver : public MeasurementReceiverBase {
public:

	// type definitions for use in template/child classes
    typedef M measurement_type;
    typedef S pull_receiver_type;
    typedef MeasurementReceiver< M, S > this_type;



	MeasurementReceiver(
	    	H3D::Inst< H3D::SFNode     > _metadata = 0,
	        H3D::Inst< H3D::SFString   > _pattern = 0,
	        H3D::Inst< H3D::SFBool     > _isSyncSource = 0,
			H3D::Inst< MeasurementMode > _mode = 0
	) : MeasurementReceiverBase(_metadata, _pattern, _isSyncSource, _mode)
		, pull_receiver(NULL)
		{
		};


	virtual void updateMeasurement(const M& m ) {
		H3D::Console(4) << "Missing implementation for Receiver: " << pattern->getValue(id) << std::endl;
		throw NotImplementedError("MeasurementReceiver::updateMeasurement");
	}


	virtual void update( unsigned long long ts) {
		if (!connected)
			return;
	    if ((!mode->is_push()) && (pull_receiver != NULL)) {
	    	//H3D::Console(3) << "retrieve pull measurement: " << pattern->getValue(id) << std::endl;
	    	try {
				updateMeasurement(pull_receiver->get(ts));
	    	} catch (Ubitrack::Util::Exception &e) {
	    		H3D::Console(4) << "Error while pulling measurement: " << pattern->getValue(id) << ": " << e.what() << std::endl;
	    	}
	    } else if (mode->is_push()) {
	    	//H3D::Console(3) << "transfer push measurement" << std::endl;
	    	//transferMeasurements(ts);
	    }
	}

	virtual bool connect(Ubitrack::Facade::AdvancedFacade* sf) {
	    if (sf == NULL)
	        return false;

	    // check if already connected here !
		H3D::Console(4) << "Connect Receiver: " << pattern->getValue() << std::endl;

	    if (mode->is_push()) {
	    	try
	    	{
	    		sf->setCallback< M >( pattern->getValue(id).c_str(),
	    				boost::bind(&this_type::receiveMeasurement, this, _1) );
	    		connected = true;
	    	}
	    	catch ( const Ubitrack::Util::Exception& e )
	    	{
	    		//LOG4CPP_ERROR( logger, "Caught exception in PoseReceiver::setCallback( " << sCallbackName <<" ): " << e );
	    		connected = false;
	    	}

	        return connected;
	    } else {
	        // pull receiver
	    	try
	    	{
	    		pull_receiver = sf->componentByName< S >( pattern->getValue(id).c_str() ).get();
	    		connected = true;
	    	}
	    	catch ( const Ubitrack::Util::Exception& e )
	    	{
	    		//LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::getSimplePullSinkPose( " << sComponentName <<" ): " << e );
	    		connected = false;
	    	}
			return connected;
	    }
	}

	virtual bool disconnect(Ubitrack::Facade::AdvancedFacade* sf) {
	    if (sf == NULL)
	        return false;

	    H3D::Console(4) << "Disconnect Receiver: " << pattern->getValue() << std::endl;

	    if (mode->is_push()) {
	    	try
	    	{
	    		sf->setCallback< M >( pattern->getValue(id).c_str(), NULL );
	    	}
	    	catch ( const Ubitrack::Util::Exception& e )
	    	{
	    		//LOG4CPP_ERROR( logger, "Caught exception in PoseReceiver::setCallback( " << sCallbackName <<" ): " << e );
	    	}
	        return true;
	    } else {
	        // pull receiver
	        pull_receiver = NULL;
	        return true;
	    }
	}


	virtual void receiveMeasurement(const M& measurement) {
		H3D::Console(3) << "receiveMeasurement" << std::endl;

		updateMeasurement(measurement);

		H3D::Console(3) << "receiveMeasurement::done" << std::endl;

		//lock.lock();
		// THIS IS NOT CORRECT .. but caching currently does not work ...
		//if (parent != NULL) {
		//	parent->updateMeasurement(measurement);
		//}
        //applyMeasurement<P, M>(parent, measurement);
		//received_measurement = copyMeasurement< M >(measurement);
		//dirty = true;
		//lock.unlock();
		//H3D::Console(3) << "notify listeners: " << pattern->getValue(id) << std::endl;
		notify_data_ready(measurement.time());
	}
	/*
	void transferMeasurements(unsigned long long ts) {
		lock.lock();
		if (dirty) {
			applyMeasurement<P,M>(parent, received_measurement);
			dirty = false;
		}
		lock.unlock();
	};
	*/


protected:

    S* pull_receiver;
	//M received_measurement;


};



} // namespace H3DUbitrack

#endif //MEASUREMENTRECEIVER_H_
