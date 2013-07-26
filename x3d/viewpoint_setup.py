from H3DInterface import *
from H3DUtils import *


# Camera Calibration
cameraIntrinsics = MFMatrix3d()
cameraResolution = MFVec2f()
eyeTransform = MFMatrix4f()


def initialize():

    cameraIntrinsics.setValue(
        [Matrix3d(854.3160400390625, 0, 341.238037109375, 0, 846.3531494140625, 232.31965637207031, 0, 0, 1),
         Matrix3d(814.7125244140625, 0, 337.53744506835937, 0, 815.65533447265625, 207.89939880371094, 0, 0, 1),
        ]
    )

    cameraResolution.setValue([Vec2f(640,480), Vec2f(640, 480)])

    # Left 2 Right Transform
    l2r_rot = Quaternion(-0.0045586664395528406, 0.0042876584700794404, -0.0010188895109084382, 0.99994888550864358)
    l2r_trans = Vec3f(0.060612695725155666, -0.020287252331639295, -0.035812026245098563)
    l2r_mat = Matrix4f(Rotation(l2r_rot))
    l2r_mat.setElement(0,3, l2r_trans.x)
    l2r_mat.setElement(1,3, l2r_trans.y)
    l2r_mat.setElement(2,3, l2r_trans.z)

    eyeTransform.setValue(
        [ Matrix4f(),
          l2r_mat.inverse(),
        ]
    )

