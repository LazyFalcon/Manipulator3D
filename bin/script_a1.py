# import * from glm_export
# import * from RCUtils
# import * from commandBuilders_export
# import * from scene_export
# import * from robotController_export
# import * from helper_export

# target = EntityVector()
# targetCenter = vec4(0,0,0,0)
# newTargetPosition = vec4()

def update(RC, scene, dt):
	return True
def init(RC, scene):
	return True
	# target[:] = ['brick.001','brick.002','brick.003','brick.004']
	# target[:] = getSelectedList('itemsToGrab')
	# for it in target
		# targetCenter += it.position
	# targetCenter /= targetCenter.w
	# newTargetPosition = rotation(-2.0, vec3(0,0,1)) * targetCenter

def terminate(RC, scene):
	return False

def action_1(RC, scene):
	return False
	# while len(target) > 0:
		# rc.grab(target[0])
		# rc.moveTo(vec4(6,3,1,1))
		# rc.release()


