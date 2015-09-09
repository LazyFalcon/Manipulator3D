from Manipulator3D import *

import matplotlib
from matplotlib.pyplot import draw, figure, show
import matplotlib.pyplot as plt
from numpy import arange, sin, pi
import numpy as np

# from scipy.signal import savgol_filter

# data to collect:
#   robot: joint positions, effector velocity, acceleration
#   jacobian transpose: iterations, internal joint positions, precision, errors
#   interpolators: paths

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
			RC.goTo(CommandReturnAction.DelAndForward).to(selection[0].position+vec4(0,0,1.5,0)).orientation(vec3(0,0,-1)).name('GoTo 1').jointVelocity(0.9).insert(RC,2)
			RC.goTo(CommandReturnAction.DelAndForward).to(selection[0].position).orientation(vec3(0,0,-1)).name('GoTo 2').jointVelocity(0.9).insert(RC,3)
			RC.grab(selection[0], CommandReturnAction.DelAndForward).insert(RC, 4)
			RC.goTo(CommandReturnAction.DelAndForward).to(selection[0].position+vec4(0,0,1.5,0)).name('GoTo 1').jointVelocity(0.9).insert(RC,5)


			RC.goTo(CommandReturnAction.DelAndForward).to(getCursor()+vec4(0,0,1.2,0)).name('GoTo 3').insert(RC,6)
			# RC.goTo(CommandReturnAction.DelAndForward).to(getCursor()+vec4(0,0,0.2,0)).orientation(vec3(0,0,-1)).name('GoTo 3').insert(RC,7)
			RC.release(CommandReturnAction.DelAndBack).insert(RC,7)

			unselect()
			return True
		else:
			return self.handleTimer(dt)

# -------------------------

def handleInput(key, action, mod, RC, scene):
	if action == Press:
		if key == ord('W') and mod&Ctrl == Ctrl:
			plotData(RC, scene)
		elif key == F5:
			RC.run()
		elif key == Esc:
			RC.stop()
		elif key == Pause:
			RC.pause()

class DataCollector:
	js = 0
	jointPosList = []
	time = DoubleVec()
	effectorVelocity = DoubleVec()
	effectorAcceleration = DoubleVec()
	enableCollecting = True

	def __init__(self):
		pass

	def initialize(self, RC):
		self.js = RC.getRobot().getModuleCount()
		print 'Robot has ' + str(self.js) + ' modules'
		self.time.append(0)
		self.effectorVelocity.append(0)
		self.effectorAcceleration.append(0)

		for i in range(self.js):
			d = DoubleVec()
			d.append(0)
			self.jointPosList.append(d)

	def plotVelocity(self):
		plt.figure(1)
		# plt.plot(self.effectorVelocity[10:-1], 'ro')

		# vv = savgol_filter(self.effectorVelocity, 101, 3)
		plt.plot(self.effectorVelocity[100:-1])
		plt.xlabel('time')
		plt.ylabel('velocity')

	def plotAcceleration(self):
		plt.figure(3)
		plt.plot(self.time, self.effectorAcceleration)
		plt.xlabel('time')
		plt.ylabel('acceleration')

	def plotJoints(self):
		plt.figure(2)
		for i, it in enumerate(self.jointPosList):
			plt.plot(self.jointPosList[i])
		plt.xlabel('time')
		plt.ylabel('joints')

	def enable(self):
		self.enableCollecting = True

	def disable(self):
		self.enableCollecting = False

	def collect(self, RC, dt):
		pass
		# if self.enableCollecting:
			# for i in range(self.js):
				# self.jointPosList[i].append(RC.getRobot().module(i).value)
			# self.effectorAcceleration.append(RC.getRobot().acceleration)
			# self.effectorVelocity.append(RC.getRobot().velocity)
			# self.time.append(self.time[-1] + dt)

dataCollector = DataCollector()

def update(RC, scene, dt):
	dataCollector.collect(RC, dt)

# ---------------------------
def c_init(RC):
	print 'Hello from pyton callback!'
	return True

c_count = 10

def uppa(x):
	c_count = c_count -1
	print x*30

class c_update:
	count = 0
	def __init__(self, c):
		self.count = c

	def update(self, RC, scene,  dt):
		for i in range(20):
			if self.count >= 0:
				print 'Hue hue, krhhh!'
				self.count -= 1
				return False
		return True

def c_exit(RC, scene):
	return True

def uppka(fun):
	fun(2)
# ---------------------------
def init(RC, scene):
	print 'Hello! This is first entry in this script.'
	dataCollector.initialize(RC)

	action = MoveObjects()
	RC.pyExec(CommandReturnAction.GoToNext).name("Move objects from box").callback(action).finish(RC)

	# RC.savePosition()
	# moveBuilder = MoveCommandBuilder()
	# uppka(uppa)
	points = Vec4Vec()
	circle = Vec4Vec()
	points[:] = [vec4(-1, -3.5, 4, 1), vec4(1, -5, 2, 1), vec4(4, 0, 5, 1), vec4(2, 5, 4, 1), vec4(-3,0,3,1), vec4(-1, -3.5, 4, 1)]
	circle[:] = [vec4(-4,0,3,1), vec4(0,4,3,1), vec4(4,0,3,1), vec4(0,-4,3,1)]
	# circlePath = addInterpolator(Interpolator.HermiteFiniteDifferenceClosed, points, "--")
	circlePath = addInterpolator(Interpolator.HermiteFiniteDifferenceClosed, circle, "--")
	path = addInterpolator(Interpolator.BSpline, points, "--")
	# RC.wait().time(2.0).finish(RC);
	# RC.follow().name("Follow").target(scene.get("Cube.040")).jointVelocity(0.4).finish(RC)

	# RC.pyExec().name("Exec from py").onEnter(c_init).onUpdate(c_update).onExit(c_exit).finish(RC)

	# foo = c_update(10)
	# RC.pyExec(1).name("Exec from py").callback(foo).finish(RC)
	# RC.move(1).name("Order from python").interpolator(path).velocity(1.0).jointVelocity(0.5).finish(RC)
	# RC.move(1).name("Circle!").interpolator(circlePath).velocity(1.0).jointVelocity(0.5).finish(RC)
	print 'Now new order is created.'
	# RC.popPosition()
	# RC.savePosition()

	print 'End.'
	return True

def terminate(RC, scene):
	return False

def plotData(RC, scene):
	print 'Hello moron!'

	# dataCollector.plotJoints()
	dataCollector.plotVelocity()
	# dataCollector.plotAcceleration()
	plt.show()

	print 'Bye moron!'
	return False
