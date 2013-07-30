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

class CombineMat3(TypedField(MFMatrix3d, (SFMatrix3d, SFMatrix3d))):

    def update(self, event):
        ri = self.getRoutesIn()
        if len(ri) == 1:
           return [ri[0].getValue(),]
        elif len(ri) == 2:
           return [ri[0].getValue(),ri[1].getValue(),]
        return []

camIntrinsics = CombineMat3()


class EyeTransform(TypedField(MFMatrix4f, (SFMatrix4f, SFMatrix4f))):
    def update(self, event):
        ri = self.getRoutesIn()
        if len(ri) == 1:
           return [Matrix4f(), ri[0].getValue(),]
        elif len(ri) == 2:
           return [ri[0].getValue(),ri[1].getValue(),]
        return []


eyeTransform = EyeTransform()

class MarkerVisibilityTest(AutoUpdate(SFBool)):
    def update(self, event):
        if event.getValue():
            print "Marker Visible"
        #else:
        #    print "No Marker"
        return False

isMarkerVisible = MarkerVisibilityTest()
