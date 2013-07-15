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

#include <utFacade/AdvancedFacade.h>
#include <utMeasurement/Measurement.h>
#include <utComponents/ApplicationPullSink.h>

#include <boost/bind.hpp>
#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

class H3DUBITRACK_API MeasurementReceiverBase {
public:
	virtual ~MeasurementReceiverBase() {};
	virtual void update( unsigned long long ts) = 0;
	virtual bool connect(Ubitrack::Facade::AdvancedFacade* sf) = 0;
	virtual bool disconnect(Ubitrack::Facade::AdvancedFacade* sf) = 0;
	virtual unsigned long long int wait_for_data_ready() = 0;
};


template< class P, class M , class S >
class H3DUBITRACK_API MeasurementReceiver : public MeasurementReceiverBase {
public:

	template< typename T>
	struct callback_handler {
		callback_handler(MeasurementReceiver<P,M,S>* _p) : p(_p) {};
		void operator()(const T& m) {
			p->receiveMeasurement(m);
		}
		MeasurementReceiver<P,M,S>* p;
	};

	MeasurementReceiver(P* _parent, const string _name , bool _is_push)
		: parent(_parent)
		, name(_name)
		//, dirty(false)
		, is_push(_is_push)
		, pull_receiver(NULL)
		, last_timestamp(0)
		, data_ready()
		{

		};



	void update( unsigned long long ts) {
	    if ((!is_push) && (pull_receiver != NULL)) {
	    	H3D::Console(3) << "retrieve pull measurement: " << name << std::endl;
	    	try {
	    		if (parent != NULL) {
	    			parent->updateMeasurement(pull_receiver->get(ts));
	    		}
	    		//applyMeasurement<P, M>(parent, pull_receiver->get(ts));
	    	} catch (Ubitrack::Util::Exception &e) {
	    		H3D::Console(4) << "Error while pulling measurement: " << name << ": " << e.what() << std::endl;
	    	}
	    } else if (is_push) {
	    	//H3D::Console(3) << "transfer push measurement" << std::endl;
	    	//transferMeasurements(ts);
	    }

	}

	bool connect(Ubitrack::Facade::AdvancedFacade* sf) {
		bool connected = false;
	    if (sf == NULL)
	        return false;

	    if (is_push) {
	    	try
	    	{
	    		sf->setCallback< M >( name.c_str(), boost::bind<void>(callback_handler<M>(this), _1) );
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
	    		pull_receiver = sf->componentByName< S >( name.c_str() ).get();
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

	bool disconnect(Ubitrack::Facade::AdvancedFacade* sf) {
	    if (sf == NULL)
	        return false;

	    if (is_push) {
	    	try
	    	{
	    		sf->setCallback< M >( name.c_str(), NULL );
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


	void receiveMeasurement(const M& measurement) {
		//H3D::Console(3) << "receiveMeasurement" << std::endl;
		//lock.lock();
		// THIS IS NOT CORRECT .. but caching currently does not work ...
		//if (parent != NULL) {
		//	parent->updateMeasurement(measurement);
		//}
        //applyMeasurement<P, M>(parent, measurement);
		//received_measurement = copyMeasurement< M >(measurement);
		//dirty = true;
		//lock.unlock();
		//H3D::Console(3) << "notify listeners: " << name << std::endl;
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

    void notify_data_ready(unsigned long long int ts) {
    	data_ready.lock();
    	last_timestamp = ts;
    	data_ready.signal();
    	data_ready.unlock();
    }

    unsigned long long int wait_for_data_ready() {
    	data_ready.lock();
    	// milliseconds
    	while (!data_ready.timedWait(100)) {
    		H3D::Console(4) << "ubitrack sync timeout..: " << name << std::endl;
    	}
    	data_ready.unlock();
    	return last_timestamp;
    }

    typedef P parent_type;
    typedef M measurement_type;
    typedef S pull_receiver_type;

	P* parent;
	string name;
	S* pull_receiver;
	H3DUtil::MutexLock lock;
	//M received_measurement;
	//bool dirty;
	bool is_push;

	H3DUtil::ConditionLock data_ready;
    unsigned long long int last_timestamp;
};



} // namespace H3DUbitrack

#endif //MEASUREMENTRECEIVER_H_
