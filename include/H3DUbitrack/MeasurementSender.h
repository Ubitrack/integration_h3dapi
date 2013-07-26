/*
 * UbitrackContext.h
 *
 *  Created on: 03/08/2010
 *      Author: mvl
 */

#ifndef MEASUREMENTSENDER_H_
#define MEASUREMENTSENDER_H_

#include <boost/thread.hpp>

#include <H3DUbitrack/H3DUbitrack.h>
#include <H3DUbitrack/UbitrackMeasurement.h>

#include <H3D/Field.h>
#include <H3D/FieldTemplates.h>

#include <utComponents/ApplicationPushSource.h>
#include <utUtil/OS.h>

#include <boost/bind.hpp>
#include <vector>

using namespace Ubitrack::Facade;

namespace H3DUbitrack {

class H3DUBITRACK_API MeasurementSenderBase : public UbitrackMeasurement {
public:
	H3D_VALUE_EXCEPTION( string, NotImplementedError );

	typedef H3D::EventCollectingField< H3D::SFBool > ChangesCollectorField;


	MeasurementSenderBase(
	    	H3D::Inst< H3D::SFNode     > _metadata = 0,
	        H3D::Inst< H3D::SFString   > _pattern = 0
			);

    virtual string defaultXMLContainerField() { return "sender"; }

    auto_ptr< ChangesCollectorField > hasChanges;

	virtual void initialize() {
		// noop per default
	}

	virtual void update(unsigned long long ts) {
		throw NotImplementedError("MeasurementSenderBase::update");
	}

	/** called to connect push receivers/pull senders. */
	virtual bool connect(Ubitrack::Facade::AdvancedFacade* facade) {
		throw NotImplementedError("MeasurementSenderBase::connect");
	}

	/** called to disconnect push receivers/pull senders. */
	virtual bool disconnect(Ubitrack::Facade::AdvancedFacade* facade) {
		throw NotImplementedError("MeasurementSenderBase::disconnect");
	}

    /// Add this node to the H3DNodeDatabase system.
    static H3D::H3DNodeDatabase database;

protected:
	boost::mutex data_lock;
    bool connected;

};

template< class M, class S >
class H3DUBITRACK_API MeasurementSender : public MeasurementSenderBase {
public:

	// type definitions for use in template/child classes
    typedef M measurement_type;
    typedef MeasurementSender< M, S > this_type;



	MeasurementSender(
	    	H3D::Inst< H3D::SFNode     > _metadata = 0,
	        H3D::Inst< H3D::SFString   > _pattern = 0
	) : MeasurementSenderBase(_metadata, _pattern)
    , push_sender(NULL)
		{
		};

	virtual M getMeasurement(unsigned long long ts) {
		H3D::Console(4) << "Missing implementation for Sender: " << pattern->getValue(id) << std::endl;
		throw NotImplementedError("MeasurementSender::getMeasurement");
	}


	virtual void update( unsigned long long ts) {
		if (!connected)
			return;
		try {
			// check if field is dirty here !!
			if(push_sender != NULL) {
				push_sender->send(getMeasurement(ts));
			}
		} catch (Ubitrack::Util::Exception &e) {
			//H3D::Console(4) << "Error while pushing measurement: " << pattern->getValue(id) << ": " << e.what() << std::endl;
		}
	}

	virtual bool connect(Ubitrack::Facade::AdvancedFacade* sf) {
	    if (sf == NULL)
	        return false;

	    // check if already connected here !
		H3D::Console(4) << "Connect Sender: " << pattern->getValue() << std::endl;

		// push sender
		try
		{
			push_sender = sf->componentByName< S >( pattern->getValue(id).c_str() ).get();
			connected = true;
		}
		catch ( const Ubitrack::Util::Exception& e )
		{
			H3D::Console(4) << "Caught exception in componentByName( " << pattern->getValue(id).c_str() <<" ): " << e  << std::endl;
			connected = false;
		}
		return connected;
	}

	virtual bool disconnect(Ubitrack::Facade::AdvancedFacade* sf) {
	    if (sf == NULL)
	        return false;

	    H3D::Console(4) << "Disconnect Sender: " << pattern->getValue() << std::endl;

		// pull receiver
		push_sender = NULL;
		return true;
	}

protected:

    S* push_sender;

};



} // namespace H3DUbitrack

#endif //MEASUREMENTSENDER_H_