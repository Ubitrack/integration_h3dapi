#include "H3DUbitrack/UbitrackMeasurement.h"

using namespace std;
using namespace H3D;
using namespace H3DUbitrack;



UbitrackMeasurement::UbitrackMeasurement(
	H3D::Inst< H3D::SFNode     > _metadata,
	H3D::Inst< H3D::SFString   > _pattern
	): H3D::X3DNode(_metadata)
, pattern(_pattern)
{
}
