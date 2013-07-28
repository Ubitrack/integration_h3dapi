from H3DInterface import *
from H3DUtils import *

class StringToInt(TypedField(SFInt32, (SFString,))):
    
    def update(self, event):
        ri = self.getRoutesIn()
        if len(ri) != 1:
            return 0
        val = ri[0].getValue()
        if len(val) > 0:
            return ord(val[0])
        return 0

keyToButton = StringToInt()


class CamIntrinsicsImporter(SFMatrix3d):

    def update(self, event):
        ri = self.getRoutesIn()
        if len(ri) != 1:
            return Matrix3d()
        val = ri[0].getValue()
        # currently we expect a different format that ubitrack
        # need to find out what is going on where exactly...
        val.setElement(0,2, -1*val.getElement(0,2))
        val.setElement(1,2, -1*val.getElement(1,2))
        val.setElement(2,2, -1*val.getElement(2,2))
        return val

camIntrinsicsLeft = CamIntrinsicsImporter()
camIntrinsicsRight = CamIntrinsicsImporter()

class CombineMat3(TypedField(MFMatrix3d, (SFMatrix3d, SFMatrix3d))):

    def update(self, event):
        ri = self.getRoutesIn()
        if len(ri) == 1:
           return [ri[0].getValue(),]
        elif len(ri) == 2:
           return [ri[0].getValue(),ri[1].getValue(),]
        return []

camIntrinsics = CombineMat3()

class MarkerVisibilityTest(AutoUpdate(SFBool)):
    def update(self, event):
        if event.getValue():
            print "Marker Visible"
        else:
            print "No Marker"
        return False

isMarkerVisible = MarkerVisibilityTest()
