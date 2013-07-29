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


    UbitrackMeasurement(
    	H3D::Inst< H3D::SFNode     > _metadata = 0,
        H3D::Inst< H3D::SFString   > _pattern = 0
		);

    virtual ~UbitrackMeasurement() {};

	// pattern of dataflow component
	std::auto_ptr< H3D::SFString > pattern;

    virtual void initialize() = 0;

    virtual void update(H3D::TraverseInfo &ti, unsigned long long ts) = 0;

	/** called to connect push receivers/pull senders. */
    virtual bool connect(Ubitrack::Facade::AdvancedFacade* facade) = 0;

    /** called to disconnect push receivers/pull senders. */
    virtual bool disconnect(Ubitrack::Facade::AdvancedFacade* facade) = 0;


};	



} // namespace H3DUbitrack

#endif //UBITRACKMEASUREMENTS_H_
