from glm_export import *
from commandBuilders_export import *
from scene_export import *
from robotController_export import *
from helper_export import *
# this is sample script for Manipulator3D
# it contains basic functions etc.

def handleInput(key, mod, RC, scene):
	if key == ord('W') and mod == Ctrl:
		print 'This should plot data'
	elif key == F5:
		RC.run()
	elif key == Esc:
		RC.stop()
	elif key == Pause:
		RC.pause()

def update(RC, scene, dt):
	pass

def init(RC, scene):
	print 'Hello! This is first entry in this script.'

	points = Vec4Vec()
	points[:] = [vec4(2, 5, 4, 1), vec4(4, 0, 5, 1), vec4(1, -5, 2, 1), vec4(0, -5, 3, 1)]
	path = addInterpolator(Interpolator.HermiteFiniteDifference, points, "--")
	RC.move().name("Order from python").interpolator(path).velocity(1.5).jointVelocity(0.4).finish(RC)
	print 'Now new order is created.'

def terminate(RC, scene):
	pass

