import os
from jpype import *
import jpype

jarpath = os.path.join(os.path.abspath('.'), 'E:/du2.jar')
dependency = os.path.join(os.path.abspath('.'), 'E:/')
jpype.startJVM('C:/Program Files (x86)/Charles/jre/bin/client/jvm.dll', "-ea", "-Djava.class.path=%s" %jarpath)
JClass = jpype.JClass('com.shine.support.utils.aq')
instance = JClass()
result = instance.search(5,20)
print(result)
jpype.shutdownJVM()
