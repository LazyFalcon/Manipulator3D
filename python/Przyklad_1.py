from Manipulator3D import *
'''
Przykladowa akcja:
	jej zadaniem jest zbierac wszystkie zaznaczone obiekty i przenosic je w jedno miejsce
	miejsce jest wskazywane kursorem(z pewnym offsetem) obiektem jest ten aktualnie zaznaczony
	jesli nie ma nic zaznaczonego przez 3s, to akcja sie konczy
'''
class MoveObjects:
	def __init__(self):
		self.timer = 3.0

	def onEnter(self, RC):
		self.timer = 3
	def onExit(self, RC, scene):
		pass

	def handleTimer(self, dt):
		if self.timer > 0:
			self.timer -= dt
			return False
		else:
			return True

	def onUpdate(self, RC, scene, dt):
		selection = getSelection()
		if len(selection) > 0:
			print 'Ha, lets grab sometthin\'!'
			print selection[0].position

			RC.goTo(CommandReturnAction.DelAndForward).to(selection[0].position+vec4(0,0,1.5,0)).name('GoTo 1').insert(RC,1)
			RC.goTo(CommandReturnAction.DelAndForward).to(selection[0].position+vec4(0,0,1.5,0)).orientation(vec3(0,0,-1)).name('GoTo 1').jointVelocity(0.3).insert(RC,2)
			RC.goTo(CommandReturnAction.DelAndForward).to(selection[0].position).orientation(vec3(0,0,-1)).name('GoTo 2').jointVelocity(0.3).insert(RC,3)
			RC.grab(selection[0], CommandReturnAction.DelAndForward).insert(RC, 4)
			RC.goTo(CommandReturnAction.DelAndForward).to(selection[0].position+vec4(0,0,1.5,0)).name('GoTo 1').jointVelocity(0.3).insert(RC,5)


			RC.goTo(CommandReturnAction.DelAndForward).to(getCursor()+vec4(0,0,1.2,0)).name('GoTo 3').insert(RC,6)
			RC.goTo(CommandReturnAction.DelAndForward).to(getCursor()+vec4(0,0,0.2,0)).orientation(vec3(0,0,-1)).name('GoTo 3').insert(RC,7)
			RC.release(CommandReturnAction.DelAndBack).insert(RC,8)

			unselect()
			return True
		else:
			return self.handleTimer(dt)

action = MoveObjects()
RC = getRC();
RC.pyExec(CommandReturnAction.GoToNext).name("Move objects from box").callback(action).finish(RC)
