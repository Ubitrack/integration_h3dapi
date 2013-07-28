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
