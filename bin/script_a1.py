from glm_export import *
from commandBuilders_export import *
from scene_export import *
from robotController_export import *
from helper_export import *

import matplotlib
from matplotlib.pyplot import draw, figure, show
import matplotlib.pyplot as plt
from numpy import arange, sin, pi
import numpy as np

time = DoubleVec()
data1 = DoubleVec()
data2 = DoubleVec()
data3 = DoubleVec()
data4 = DoubleVec()
data5 = DoubleVec()
data6 = DoubleVec()

# data to collect:
#   robot: joint positions, effector velocity, acceleration
#   jacobian transpose: iterations, internal joint positions, precision, errors
#   interpolators: paths

def handleInput(key, mod, RC, scene):
    if key == ord('W') and mod == Ctrl:
        plotData(RC, scene)
    elif key == F5:
        RC.run()
    elif key == Esc:
        RC.stop()
    elif key == Pause:
        RC.pause()

def collectData(RC, dt):
    js = RC.getRobotJ()
    time.append(time[-1]+dt)
    data1.append(js[0])
    data2.append(js[1])
    data3.append(js[2])
    data4.append(js[3])
    data5.append(js[4])
    data6.append(js[5])

def update(RC, scene, dt):
    collectData(RC, dt)

def init(RC, scene):
    print 'Hello! This is first entry in this script.'
    time.append(0)
    data1.append(0)
    data2.append(0)
    data3.append(0)
    data4.append(0)
    data5.append(0)
    data6.append(0)
    RC.savePosition()
    # moveBuilder = MoveCommandBuilder()
    points = Vec4Vec()
    points[:] = [vec4(2, 5, 4, 1), vec4(4, 0, 5, 1), vec4(1, -5, 2, 1), vec4(0, -5, 3, 1)]

    path = addInterpolator(Interpolator.HermiteFiniteDifference, points, "--")
    # RC.wait().time(2.0).finish(RC);
    # RC.follow().name("Follow").target(scene.get("Cube.040")).jointVelocity(0.4).finish(RC)
    RC.move().name("Order from python").interpolator(path).velocity(1.5).jointVelocity(0.4).finish(RC)
    print 'Now new order is created.'
    # RC.popPosition()
    RC.savePosition()

    print 'End.'
    return True

def terminate(RC, scene):
	return False

def inNewThread():
	print 'Hello moron!'
	# f1, f2 = figure(), figure()
	# af1 = f1.add_subplot(111)
	# af2 = f2.add_subplot(111)
	# af1.plot([1,2,3])
	# af2.plot([6,5,4])

	plt.plot(time,data1,time,data2,time,data3,time,data4,time,data5,time,data6)
	plt.show()

	# t = arange(0.0, 2.0, 0.01)
	# s = sin(2*pi*t)
	# plt.xlabel('time (s)')
	# plt.ylabel('voltage (mV)')
	# plt.title('About as simple as it gets, folks')
	# # # plt.savefig("19110942_%i_test.png" % 1) # irrelevant here
	# plt.show()
	# plt.clf()
	print 'Bye moron!'
	return False

def plotData(RC, scene):
	inNewThread()



