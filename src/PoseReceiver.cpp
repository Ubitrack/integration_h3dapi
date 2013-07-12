#include <H3DUbitrack/PoseReceiver.h>
#include <boost/bind.hpp>


using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase PoseReceiver::database(
                "PoseReceiver",
                &(newInstance<PoseReceiver>),
                typeid( PoseReceiver ),
                NULL );


namespace UbitrackMeasurementInternals {
    // PoseReceiver
	FIELDDB_ELEMENT( PoseReceiver, mode, INITIALIZE_ONLY );
	FIELDDB_ELEMENT( PoseReceiver, name, INITIALIZE_ONLY );
	FIELDDB_ELEMENT( PoseReceiver, isSyncSource, INITIALIZE_ONLY );
    FIELDDB_ELEMENT( PoseReceiver, matrix, OUTPUT_ONLY );
    FIELDDB_ELEMENT( PoseReceiver, translation, OUTPUT_ONLY );
    FIELDDB_ELEMENT( PoseReceiver, rotation, OUTPUT_ONLY );
}


PoseReceiver::PoseReceiver(H3D::Inst< H3D::SFNode > _metadata,
                           H3D::Inst< H3D::SFString   > _name,
                           H3D::Inst< H3D::SFBool     > _isSyncSource,
                           H3D::Inst< MeasurementMode > _mode,
                           H3D::Inst< H3D::SFMatrix4f > _matrix,
                           H3D::Inst< H3D::SFVec3f    > _translation,
                           H3D::Inst< H3D::SFRotation > _rotation
                           )
: UbitrackMeasurement(_metadata, _name, _isSyncSource, _mode)
, matrix(_matrix)
, translation(_translation)
, rotation(_rotation)
, connected(false)
, push_receiver(NULL)
, pull_receiver(NULL)
{
    type_name = "PoseReceived";
    database.initFields( this );
}

bool PoseReceiver::connect(UbitrackInstance* instance)
{
    // check if already connected here !
    Console(4) << "Connect Receiver: " << name->getValue() << std::endl;

    AdvancedFacade* sf = instance->getFacadePtr();
    if (sf == NULL)
        return false;

    MeasurementMode::Mode _mode = mode->getMeasurementMode();
    if (_mode == MeasurementMode::PUSH) {
        // push receiver needs a registered callback
        push_receiver = new ReceiverCB<PoseReceiver, Ubitrack::Measurement::Pose>(this);

    	try
    	{
    		sf->setCallback< Ubitrack::Measurement::Pose >( name->getValue( id ).c_str(), boost::bind(&ReceiverCB<PoseReceiver, Ubitrack::Measurement::Pose>::receiveMeasurement, boost::ref(push_receiver), _1) );
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
    		pull_receiver = sf->componentByName< Ubitrack::Components::ApplicationPullSinkPose >( name->getValue( id ).c_str() ).get();
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

bool PoseReceiver::disconnect(UbitrackInstance* instance)
{
    Console(4) << "Disconnect Receiver: " << name->getValue() << std::endl;

    if (!connected)
        return true;

    AdvancedFacade* sf = instance->getFacadePtr();
    if (sf == NULL)
        return false;

    MeasurementMode::Mode _mode = mode->getMeasurementMode();
    if (_mode == MeasurementMode::PUSH) {
    	try
    	{
    		sf->setCallback< Ubitrack::Measurement::Pose >( name->getValue( id ).c_str(), NULL );
    		connected = false;
    	}
    	catch ( const Ubitrack::Util::Exception& e )
    	{
    		//LOG4CPP_ERROR( logger, "Caught exception in PoseReceiver::setCallback( " << sCallbackName <<" ): " << e );
    	}

        push_receiver = NULL;
        return true;
    } else {
        // pull receiver
        pull_receiver = NULL;
        connected = false;
        return true;
    }
}

void PoseReceiver::update(unsigned long long int ts)
{
	if (!connected)
		return;
    MeasurementMode::Mode _mode = mode->getMeasurementMode();
    if ((_mode == MeasurementMode::PULL) && (pull_receiver != NULL)) {
        Ubitrack::Measurement::Pose pose = pull_receiver->get(ts);
        applyMeasurement<PoseReceiver, Ubitrack::Measurement::Pose>(this, pose);
    } else if ((_mode == MeasurementMode::PUSH) && (push_receiver != NULL)) {
    	push_receiver->transferMeasurements();
    }
}

void PoseReceiver::updateMeasurement(const Ubitrack::Measurement::Pose& pose)
{
    H3D::Vec3f t((H3DFloat)(pose->translation()(0)), (H3DFloat)(pose->translation()(1)), (H3DFloat)(pose->translation()(2)));
    translation->setValue( t , id );

    H3D::Quaternion r((H3DFloat)(pose->rotation().x()), (H3DFloat)(pose->rotation().y()), (H3DFloat)(pose->rotation().z()), (H3DFloat)(pose->rotation().w()));
    rotation->setValue( Rotation(r) , id );

    H3D::Matrix4f m(r);
    m.setElement(0,1, t.x);
    m.setElement(0,2, t.y);
    m.setElement(0,3, t.z);
    matrix->setValue( m , id );
}

