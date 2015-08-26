from glm_export import *
from commandBuilders_export import *
from scene_export import *
from robotController_export import *
from helper_export import *

# target = EntityVector()
# targetCenter = vec4(0,0,0,0)
# newTargetPosition = vec4()

def update(RC, scene, dt):
	return True
def init(RC, scene):
	print 'Hello! This is first entry in this script.'
	# RC.savePosition()
	# moveBuilder = MoveCommandBuilder()
	points = Vec4Vec()
	# points[:] = [vec4(2, 5, 4, 1), vec4(4, 0, 5, 1), vec4(1, -5, 2, 1), vec4(0, -5, 3, 1), vec4(-1, -5, 2, 1),  vec4(-1, -5.3, 1.9, 1),  vec4(-4, -4, 1.9, 1)]
	points[:] = [vec4(2, 5, 4, 1), vec4(4, 0, 5, 1), vec4(1, -5, 2, 1), vec4(0, -5, 3, 1)]

	path = addInterpolator(Interpolator.HermiteCardinal, points, "--")
	# RC.wait().time(2.0).finish(RC);
	# RC.follow().name("Follow").target(scene.get("Cube.040")).jointVelocity(0.4).finish(RC)
	# RC.move().name("Order from python").interpolator(path).velocity(4.5).jointVelocity(1.2).finish(RC)
	print 'Now new order is created.'
	# RC.popPosition()
	# RC.savePosition()

	print 'End.'
	return True

def terminate(RC, scene):
	return False

def action_1(RC, scene):
	return False
	# while len(target) > 0:
		# rc.grab(target[0])
		# rc.moveTo(vec4(6,3,1,1))
		# rc.release()


