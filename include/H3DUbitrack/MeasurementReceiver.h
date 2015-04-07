/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef MEASUREMENTRECEIVER_H_
#define MEASUREMENTRECEIVER_H_

#include <boost/thread.hpp>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/UbitrackMeasurement.h>

#include <utComponents/ApplicationPullSink.h>
#include <utUtil/OS.h>

#include <boost/bind.hpp>
#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

class H3DUBITRACK_API MeasurementReceiverBase : public UbitrackMeasurement {
public:
	H3D_VALUE_EXCEPTION( string, NotImplementedError );

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


	MeasurementReceiverBase(
	    	H3D::Inst< H3D::SFNode     > _metadata = 0,
	        H3D::Inst< H3D::SFString   > _pattern = 0,
	        H3D::Inst< H3D::SFBool     > _isActive = 0,
	        H3D::Inst< H3D::SFBool     > _isSyncSource = 0,
	        H3D::Inst< H3D::SFBool     > _isDataAvailable = 0,
			H3D::Inst< MeasurementMode > _mode = 0
			);

	// mode: PUSH/PULL
	std::auto_ptr< MeasurementMode > mode;

	std::auto_ptr< H3D::SFBool > isActive;

	// bool identifier if this measurement should be used as sync source
	std::auto_ptr< H3D::SFBool > isSyncSource;

	std::auto_ptr< H3D::SFBool > isDataAvailable;

    virtual string defaultXMLContainerField() { return "receiver"; }

	virtual void initialize() {
		// noop per default
	}

	virtual void update(H3D::TraverseInfo &ti, unsigned long long ts) {
		throw NotImplementedError("MeasurementReceiverBase::update");
	}

	/** called to connect push receivers/pull senders. */
	virtual bool connect(FacadePtr facade) {
		throw NotImplementedError("MeasurementReceiverBase::connect");
	}

	/** called to disconnect push receivers/pull senders. */
	virtual bool disconnect(FacadePtr facade) {
		throw NotImplementedError("MeasurementReceiverBase::disconnect");
	}


    inline void notify_data_ready(unsigned long long int ts) {
    	//H3D::Console(4) << "notify data is ready: " << pattern->getValue(id) << std::endl;
    	{
    		boost::lock_guard<boost::mutex> lock(cond_lock);
        	last_timestamp = ts;
        	data_ready = true;

    	}
    	cond.notify_all();
    }

    inline unsigned long long int wait_for_data_ready() {
    	boost::unique_lock<boost::mutex> lock(cond_lock);
    	while (!data_ready) {
    		cond.wait(lock);
    	}
    	data_ready = false;
    	return last_timestamp;
    }

    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;

protected:
    boost::condition_variable cond;
	boost::mutex cond_lock;
	boost::mutex data_lock;
	//boost::mutex push_lock;
    unsigned long long int last_timestamp;
    bool connected;
    bool data_ready;
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
	        H3D::Inst< H3D::SFBool     > _isActive = 0,
	        H3D::Inst< H3D::SFBool     > _isSyncSource = 0,
	        H3D::Inst< H3D::SFBool     > _isDataAvailable = 0,
			H3D::Inst< MeasurementMode > _mode = 0
	) : MeasurementReceiverBase(_metadata, _pattern, _isActive, _isSyncSource, _isDataAvailable, _mode)
		, pull_receiver(NULL)
		{
		};


	virtual void updateMeasurement(const M& m ) {
		H3D::Console(4) << "Missing implementation for Receiver: " << pattern->getValue(id) << std::endl;
		throw NotImplementedError("MeasurementReceiver::updateMeasurement");
	}


	virtual void update( H3D::TraverseInfo &ti, unsigned long long ts ) {
		if (!connected)
			return;
		if (!isActive->getValue( id ))
			return;
	    if ((!mode->is_push()) && (pull_receiver != NULL)) {
	    	try {
				updateMeasurement(pull_receiver->get(ts));
				isDataAvailable->setValue(true, id);
	    	} catch (Ubitrack::Util::Exception /* &e */) {
				isDataAvailable->setValue(false, id);
	    	}
	    } else if (mode->is_push()) {
	    	// no action needed
	    } else {
			isDataAvailable->setValue(false, id);
	    }
	}

	virtual bool connect(FacadePtr sf) {
	    if (!sf)
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
	    		H3D::Console(4) <<  "Caught exception in MeasurementReceiver::setCallback( " << pattern->getValue(id) <<" ): " << e << std::endl;
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
	    		H3D::Console(4) <<  "Caught exception in componentByName( " << pattern->getValue(id) <<" ): " << e << std::endl;
	    		connected = false;
	    	}
			return connected;
	    }
	}

	virtual bool disconnect(FacadePtr sf) {
	    if (!sf)
	        return false;

	    H3D::Console(4) << "Disconnect Receiver: " << pattern->getValue() << std::endl;

	    if (mode->is_push()) {
	    	try
	    	{
	    		sf->setCallback< M >( pattern->getValue(id).c_str(), NULL );
	    	}
	    	catch ( const Ubitrack::Util::Exception& e )
	    	{
	    		H3D::Console(4) <<  "Caught exception in PoseReceiver::setCallback( " << pattern->getValue(id) <<" ): " << e << std::endl;
	    	}
	        return true;
	    } else {
	        // pull receiver
	        pull_receiver = NULL;
	        return true;
	    }
	}


	virtual void receiveMeasurement(const M& measurement) {
		updateMeasurement(measurement);
		isDataAvailable->setValue(true, id);
		notify_data_ready(measurement.time());
	}


protected:

    S* pull_receiver;
	boost::shared_ptr<M> received_measurement;


};



} // namespace H3DUbitrack

#endif //MEASUREMENTRECEIVER_H_
