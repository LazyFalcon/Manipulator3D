import * from glm
import * from RCUtils

target = EntityVector()
targetCenter = vec4(0,0,0,0)
newTargetPosition = vec4()

# cel:
# przenieść z jednego miejsca w drugie grupę obiektów, podając tylko środek końcowej grupy

# potrzebne:
    # zaznaczanie wielu obiektów, autonazywanie grupy, możliwość zmiany nazwy, widoczne w bocznym panelu, dodawanie do grupy
    # blenderowy 3D kursor, możliwośc zapisywania jego pozycji do listy z nazwą



def update(RC, scene, dt):
	return true
def init(RC, scene):
	# target[:] = ['brick.001','brick.002','brick.003','brick.004']
	target[:] = getSelectedList('itemsToGrab')
	for it in target
			targetCenter += it.position
	targetCenter /= targetCenter.w

	newTargetPosition = rotation(-2.0, vec3(0,0,1)) * targetCenter

def terminate(RC, scene):

def action_1(RC, scene):
	while len(target) > 0:
		rc.grab(target[0])
		rc.moveTo(vec4(6,3,1,1))
		rc.release()

def action_2(RC, scene):

