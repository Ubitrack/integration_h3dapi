#include <H3DUbitrack/UbitrackMeasurement.h>

using namespace H3D;
using namespace H3DUbitrack;


// Add the nodes to the H3DNodeDatabase system.
H3DNodeDatabase UbitrackMeasurement::database( 
                "UbitrackMeasurement", 
                NULL,
                typeid( UbitrackMeasurement ),
                &X3DNode::database );

H3DNodeDatabase PoseReceiver::database( 
                "PoseReceiver",
                &(newInstance<PoseReceiver>),
                typeid( PoseReceiver ),
                &UbitrackMeasurement::database );


namespace UbitrackMeasurementInternals {
    // all Measurements
    FIELDDB_ELEMENT( UbitrackMeasurement, mode, INITIALIZE_ONLY );
    FIELDDB_ELEMENT( UbitrackMeasurement, name, INITIALIZE_ONLY );

    // PoseReceiver
    FIELDDB_ELEMENT( PoseReceiver, matrix, OUTPUT_ONLY );
    FIELDDB_ELEMENT( PoseReceiver, translation, OUTPUT_ONLY );
    FIELDDB_ELEMENT( PoseReceiver, rotation, OUTPUT_ONLY );

}


UbitrackMeasurement::UbitrackMeasurement(H3D::Inst< H3D::SFNode     > _metadata,
                                         H3D::Inst< H3D::SFString   > _name,
                                         H3D::Inst< MeasurementMode > _mode)
: X3DNode(_metadata)
, name(_name)
, mode(_mode)
{

    type_name = "UbitrackMeasurement";
    // is abstract type .. n
    //database.initFields( this );

    mode->addValidValue( "PULL" );
    mode->addValidValue( "PUSH" );
    mode->setValue("PUSH");
}

UbitrackMeasurement::~UbitrackMeasurement() 
{
}

UbitrackMeasurement::MeasurementMode::Mode UbitrackMeasurement::MeasurementMode::getMeasurementMode() 
{
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


PoseReceiver::PoseReceiver(H3D::Inst< H3D::SFNode               > _metadata,
                           H3D::Inst< H3D::SFString             > _name,
                           H3D::Inst< MeasurementMode           > _mode,
                           H3D::Inst< TSSField(H3D::SFMatrix4f) > _matrix,
                           H3D::Inst< TSSField(H3D::SFVec3f)    > _translation,
                           H3D::Inst< TSSField(H3D::SFRotation) > _rotation
                           ) 
: UbitrackMeasurement(_metadata, _name, _mode)
, matrix(_matrix)
, translation(_translation)
, rotation(_rotation)
, push_receiver(NULL)
, pull_receiver(NULL)
{
    type_name = "PoseReceived";
    database.initFields( this );
   
}

bool PoseReceiver::connect(UbitrackInstance* instance) 
{
    // check if already connected here !
    
    Ubitrack::Facade::SimpleFacade* sf = instance->getFacadePtr();
    if (sf == NULL)
        return false;
        
    MeasurementMode::Mode _mode = mode->getMeasurementMode();
    if (_mode == MeasurementMode::PUSH) {
        // push receiver needs a registered callback
        push_receiver = new PoseReceiverCB(this);
        connected = sf->setPoseCallback(name->getValue( id ).c_str(), push_receiver);
        return connected;
    } else {
        // pull receiver
        pull_receiver = sf->getSimplePullSinkPose(name->getValue( id ).c_str());
        if (pull_receiver != NULL)
            connected = true;
            return true;
    }
    return false;
}

bool PoseReceiver::disconnect(UbitrackInstance* instance) 
{
    if (!connected)
        return true;
    
    Ubitrack::Facade::SimpleFacade* sf = instance->getFacadePtr();
    if (sf == NULL)
        return false;
    
    MeasurementMode::Mode _mode = mode->getMeasurementMode();
    if (_mode == MeasurementMode::PUSH) {
        // push receiver needs a registered callback
        connected = sf->setPoseCallback(name->getValue( id ).c_str(), NULL);
        push_receiver = NULL;
        return connected;
    } else {
        // pull receiver
        pull_receiver = NULL;
        connected = false;
        return true;
    }
}

void PoseReceiver::update(unsigned long long int ts)
{
    MeasurementMode::Mode _mode = mode->getMeasurementMode();
    if ((_mode == MeasurementMode::PULL) && (connected) && (pull_receiver != NULL)) {
        SimplePose pose;
        pull_receiver->getPose(pose, ts);
        receivePose(pose);
    }
}

void PoseReceiver::receivePose(const SimplePose& pose)
{
    H3D::Vec3f t((H3DFloat)(pose.tx), (H3DFloat)(pose.ty), (H3DFloat)(pose.tz));
    translation->setValue( t, id );

    H3D::Quaternion r((H3DFloat)(pose.rx), (H3DFloat)(pose.ry), (H3DFloat)(pose.rz), (H3DFloat)(pose.rw));
    rotation->setValue( Rotation(r), id );
    
    H3D::Matrix4f m(r);
    m.setElement(0,1, t.x);
    m.setElement(0,2, t.y);
    m.setElement(0,3, t.z);
    matrix->setValue( m, id );
}

PoseReceiver::PoseReceiverCB::PoseReceiverCB(PoseReceiver* _parent) : parent(_parent){}

void PoseReceiver::PoseReceiverCB::receivePose(const SimplePose& pose) throw()
{
    parent->receivePose(pose);
}





