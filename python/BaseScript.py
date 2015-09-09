from Manipulator3D import *

import os
# import matplotlib
# from matplotlib.pyplot import draw, figure, show
import matplotlib.pyplot as plt
# from numpy import arange, sin, pi
# import numpy as np

enableRecording = False
plotRecordedData = False
recordName = '------------'
plotColor = 'k'

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

class EndTest():
	def onEnter(self, RC):
		global enableRecording
		global plotRecordedData
		enableRecording = False
		plotRecordedData = True

	def onExit(self, RC, scene):
		return True
	def onUpdate(self, RC, scene, dt):
		return True

def startTest_1():
	global recordName
	RC = getRC()
	enableRecording = True
	recordName = 'FirstRecord'

	points = Vec4Vec()
	points[:] = [getRecord().EffectorPosition+vec4(0.1,0,0,0), vec4(-1, -3.5, 4, 1), vec4(1, -5, 2, 1), vec4(4, 0, 5, 1), vec4(2, 5, 4, 1), vec4(-3,0,3,1), vec4(1, -3.5, 4, 1)]
	path = addInterpolator(Interpolator.BSpline, points, "FirsRecordPath")
	# RC.move(1).name("FirstRecord").interpolator(path).velocity(1.0).jointVelocity(0.5).finish(RC)
	RC.move(1).name("FirstRecord").interpolator(path).velocity(4.0).jointVelocity(2.5).finish(RC)
	foo = EndTest()
	RC.pyExec(1).name("Save records").callback(foo).finish(RC)

	RC.next()
	RC.run()

	pass

def handleInput(key, action, mod, RC, scene):
	if action == Press:
		if key == ord('1') and mod & Ctrl == Ctrl:
			startTest_1()
		if key == ord('W') and mod & Ctrl == Ctrl:
			plotData(RC, scene)
		elif key == F5:
			RC.run()
		elif key == Esc:
			RC.stop()
		elif key == Pause:
			RC.pause()

class RecordedData:
	def __init__(self):
		self.IKIterationTime = DoubleVec()
		self.IKIterarationCount = DoubleVec()
		self.IKPositionError = DoubleVec()
		self.IKOrientationError = DoubleVec()

		self.EffectorDelta = DoubleVec()
		self.EffectorVelocity = DoubleVec()
		self.EffectorAcceleration = DoubleVec()
		self.FrameTime = DoubleVec()
		self.time = DoubleVec()
		self.totalTime = 0

	def saveFrameRecords(self, dt):
		data = getRecord()

		self.IKIterationTime.append( data.IKIterationTime )
		self.IKIterarationCount.append( data.IKIterarationCount )
		self.IKPositionError.append( data.IKPositionError )
		self.IKOrientationError.append( data.IKOrientationError )

		self.EffectorDelta.append( data.EffectorDelta )
		self.EffectorVelocity.append( data.EffectorVelocity )
		self.EffectorAcceleration.append( data.EffectorAcceleration )

		self.time = self.totalTime + dt
		self.totalTime = self.totalTime + dt

	def savePlotsToFile(self):
		if not os.path.exists('.\\'+recordName):
			os.makedirs('.\\'+recordName)

		print '[SAVE RECORDS] ' + recordName
		plt.figure(1)
		plt.plot(self.IKIterationTime)
		plt.xlabel('time [ms]')
		plt.ylabel('time [ms]')
		plt.title('Time of single solver')
		plt.savefig(recordName+'\\IKIterationTime.png')
		plt.show()

		plt.figure(2)
		plt.plot(self.IKIterarationCount)
		plt.xlabel('time [ms]')
		plt.ylabel('-')
		plt.title('Number of solver iterations')
		plt.savefig(recordName+'\\IKIterarationCount.png')

		plt.figure(3)
		plt.plot(self.IKOrientationError)
		plt.xlabel('time [ms]')
		plt.ylabel('error')
		plt.title('Orientation error')
		plt.savefig(recordName+'\\IKOrientationError.png')

		plt.figure(4)
		plt.plot(self.IKPositionError)
		plt.xlabel('time [ms]')
		plt.ylabel('error [m]')
		plt.title('Position error')
		plt.savefig(recordName+'\\IKPositionError.png')

		plt.figure(5)
		plt.plot(self.EffectorDelta)
		plt.xlabel('time [ms]')
		plt.ylabel('End effector ?transition? [m]')
		plt.title('Time of single solver')
		plt.savefig(recordName+'\\EffectorDelta.png')

		plt.figure(6)
		plt.plot(self.EffectorVelocity)
		plt.xlabel('time [ms]')
		plt.ylabel('velocity [m/s]')
		plt.title('End effector velocity')
		plt.savefig(recordName+'\\EffectorVelocity.png')

		plt.figure(7)
		plt.plot(self.EffectorAcceleration)
		plt.xlabel('time [ms]')
		plt.ylabel('accelereation [m/s^2]')
		plt.title('End effector acceleration')
		plt.savefig(recordName+'\\EffectorAcceleration.png')


recorder = RecordedData()

def update(RC, scene, dt):
	global plotRecordedData
	recorder.saveFrameRecords(dt)
	if plotRecordedData:
		recorder.savePlotsToFile()
		plotRecordedData = False

def init(RC, scene):
	print 'Hello! This is first entry in this script.'

	action = MoveObjects()
	# RC.pyExec(CommandReturnAction.GoToNext).name("Move objects from box").callback(action).finish(RC)

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

