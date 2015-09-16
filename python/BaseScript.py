from Manipulator3D import *

import os
import matplotlib.pyplot as plt
import numpy as np
import numpy.linalg
import math
from scipy.signal import savgol_filter

enableRecording = False
plotRecordedData = False
recordName = '------------'
plotColor = 'k'
ikPlotNme = '1.0mm'
count = 0

class EndTest():
	def __init__(self, name):
		self.ikPlotNme = name
	def onEnter(self, RC):
		global enableRecording
		global plotRecordedData
		global ikPlotNme
		ikPlotNme = self.ikPlotNme
		enableRecording = False
		plotRecordedData = True
		loadRobot(RC)
	def onExit(self, RC, scene):
		return True
	def onUpdate(self, RC, scene, dt):
		return True

def handleInput(key, action, mod, RC, scene):
	if action == Press:
		if key == ord('1') and mod & Ctrl == Ctrl:
			startTest_1()
		elif key == ord('2') and mod & Ctrl == Ctrl:
			startTest_2()
		elif key == ord('3') and mod & Ctrl == Ctrl:
			startTest_3()
		elif key == ord('4') and mod & Ctrl == Ctrl:
			startTest_4()
		elif key == ord('W') and mod & Ctrl == Ctrl:
			plotData(RC, scene)
		elif key == ord('1'):
			startTest_1_a()
		elif key == ord('2'):
			startTest_1_b()
		elif key == ord('3'):
			startTest_1_c()
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
		# self.EffectorDelta = np.array([])
		self.EffectorVelocity = DoubleVec()
		self.EffectorAcceleration = DoubleVec()
		self.FrameTime = DoubleVec()
		self.time = DoubleVec()
		self.totalTime = 0
		self.joints = [DoubleVec(), DoubleVec(), DoubleVec(), DoubleVec(), DoubleVec(), DoubleVec()]
		self.dt = 0.005

	def saveFrameRecords(self, dt):
		data = getRecord()

		self.IKIterationTime.append( data.IKIterationTime )
		self.IKIterarationCount.append( data.IKIterarationCount )
		self.IKPositionError.append( data.IKPositionError*1000 )
		self.IKOrientationError.append( data.IKOrientationError )

		self.EffectorDelta.append( data.EffectorDelta )
		self.EffectorVelocity.append( data.EffectorVelocity )
		self.EffectorAcceleration.append( data.EffectorAcceleration )

		for i in range(0,5):
			self.joints[i].append(data.RobotJoints[i])

		self.time = self.totalTime + dt
		self.totalTime = self.totalTime + dt

	def calcVelocity(self, data):
		smoothed = savgol_filter(self.EffectorDelta, 151, 4)
		v = np.diff(smoothed)
		for i in range(len(v)):
			v[i] /= self.dt
		return v
	def calcVelocityFromDiffs(self, data):
		smoothed = savgol_filter(data, 25, 3)
		# smoothed = data
		for i in range(len(smoothed)):
			smoothed[i] /= self.dt
		return smoothed
	def calcAccelereation(self, data):
		smoothed = savgol_filter(data, 11, 2)
		# smoothed = data
		smoothed = np.diff(smoothed)
		for i in range(len(smoothed)):
			smoothed[i] /= self.dt
		return smoothed

	def plotJoints(self):
		# data = savgol_filter(self.EffectorDelta, 11, 1)
		velocity = self.calcVelocityFromDiffs(self.EffectorDelta[1:-1])
		accel = self.calcAccelereation(velocity)
		# velocity = self.calcVelocityFromDiffs(data)

		# plt.figure(11)
		# plt.subplot(211)
		# plt.plot(velocity)
		# plt.ylabel('velocity')
		# plt.subplot(212)
		# plt.plot(accel)
		# plt.ylabel('accelereation')
		# plt.show()


		plt.figure(19)
		v1 = self.calcAccelereation(self.joints[0][2:-2])
		v2 = self.calcAccelereation(self.joints[1][2:-2])
		v3 = self.calcAccelereation(self.joints[2][2:-2])
		v4 = self.calcAccelereation(self.joints[3][2:-2])
		v5 = self.calcAccelereation(self.joints[4][2:-2])
		# v6 = self.calcAccelereation(self.joints[5][2:-2])
		plt.plot(v1)
		plt.plot(v2)
		plt.plot(v3)
		plt.plot(v4)
		plt.plot(v5)
		# plt.plot(v6)
		plt.legend(['1','2','3','4','5','6',], loc='upper right')
		plt.xlabel('-')
		plt.ylabel('Joint velocities')
		plt.savefig(recordName+'\\jointsV.png')
		plt.show()
	def savePlotsToFile(self):
		if not os.path.exists('.\\'+recordName):
			os.makedirs('.\\'+recordName)

		self.plotJoints()
		return
		plt.figure(8)
		plt.plot(self.joints[0])
		plt.plot(self.joints[1])
		plt.plot(self.joints[2])
		plt.plot(self.joints[3])
		plt.plot(self.joints[4])
		plt.plot(self.joints[5])
		plt.legend(['1','2','3','4','5','6',], loc='upper left')
		plt.xlabel('-')
		plt.ylabel('Joint values')
		plt.savefig(recordName+'\\joints.png')

		# plt.figure(19)
		# v1 = self.calcVelocityFromDiffs(self.joints[0])
		# v2 = self.calcVelocityFromDiffs(self.joints[1])
		# v3 = self.calcVelocityFromDiffs(self.joints[2])
		# v4 = self.calcVelocityFromDiffs(self.joints[3])
		# v5 = self.calcVelocityFromDiffs(self.joints[4])
		# v6 = self.calcVelocityFromDiffs(self.joints[5])
		# plt.plot(v1)
		# plt.plot(v2)
		# plt.plot(v3)
		# plt.plot(v4)
		# plt.plot(v5)
		# plt.plot(v6)
		# plt.legend(['1','2','3','4','5','6',], loc='upper left')
		# plt.xlabel('-')
		# plt.ylabel('Joint velocities')
		# plt.savefig(recordName+'\\jointsV.png')
		# plt.show()

		print '[SAVE RECORDS] ' + recordName
		plt.figure(1)
		plt.plot(self.IKIterationTime, plotColor)
		plt.xlabel('time [ms]')
		plt.ylabel('time [ms]')
		plt.title('Time of single solver')
		plt.savefig(recordName+'\\IKIterationTime.png')
		print '[Mean] ' + str(np.mean(self.IKIterationTime))+' [Median ]' + str(np.median (self.IKIterationTime))


		global ikPlotNme
		global count
		count = count+1

		store(ikPlotNme, self.IKIterarationCount)
		# if count== 3:
		plt.figure(2)

			# a = getD('1.0mm')
			# b = getD('0.5mm')
			# c = getD('0.05mm')

		plt.plot(self.IKIterarationCount, plotColor)
		# plt.plot(a, 'g', b, 'b', c, 'k')
		plt.xlabel('time [ms]')
		plt.ylabel('-')
		plt.title('Number of solver iterations')
		# plt.legend(['1.0mm', '0.5mm', '0.05mm'], loc='upper left')
		# plt.legend(['0.05mm'], loc='upper left')
		plt.savefig(recordName+'\\IKIterarationCount.png')
		# plt.show()

		plt.figure(3)
		plt.plot(self.IKOrientationError, plotColor)
		plt.xlabel('time [ms]')
		plt.ylabel('error')
		plt.title('Orientation error')
		plt.savefig(recordName+'\\IKOrientationError.png')

		plt.figure(4)
		plt.plot(self.IKPositionError, plotColor)
		plt.xlabel('time [ms]')
		plt.ylabel('error [mm]')
		plt.title('Position error')
		plt.savefig(recordName+'\\IKPositionError.png')

		plt.figure(5)
		plt.plot(self.EffectorDelta, plotColor)
		plt.xlabel('time [ms]')
		plt.ylabel('End effector translation [m]')
		plt.title('End effector translation')
		plt.savefig(recordName+'\\EffectorDelta.png')

		plt.figure(6)
		plt.plot(self.EffectorVelocity, plotColor)
		plt.xlabel('time [ms]')
		plt.ylabel('velocity [m/s]')
		plt.title('End effector velocity')
		plt.savefig(recordName+'\\EffectorVelocity.png')

		plt.figure(7)
		plt.plot(self.EffectorAcceleration, plotColor)
		plt.xlabel('time [ms]')
		plt.ylabel('accelereation [m/s^2]')
		plt.title('End effector acceleration')
		plt.savefig(recordName+'\\EffectorAcceleration.png')

'''
Proste sledzenie trasy
''
def startTest_1():
	global recordName
	global enableRecording
	global recorder

	recorder = RecordedData()

	RC = getRC()
	enableRecording = True
	recordName = 'FirstRecord'

	points = Vec4Vec()
	points[:] = [getRecord().EffectorPosition+vec4(0.1,0,0,0), vec4(-1, -3.5, 4, 1), vec4(1, -5, 2, 1), vec4(4, 0, 5, 1), vec4(2, 5, 4, 1), vec4(-3,0,3,1), vec4(1, -3.5, 4, 1)]
	path = addInterpolator(Interpolator.BSpline, points, "FirsRecordPath")
	# RC.move(1).name("FirstRecord").interpolator(path).velocity(1.0).jointVelocity(0.5).finish(RC)
	RC.move(1).name("FirstRecord").interpolator(path).velocity(4.0).acceleration(6.0).jointVelocity(0.5).finish(RC)
	foo = EndTest()
	RC.pyExec(1).name("Save records").callback(foo).finish(RC)

	RC.next()
	RC.run()

	pass
'''
def startTest_1_a():
	global recordName
	global enableRecording
	global recorder

	recorder = RecordedData()

	RC = getRC()
	saveRobot(RC)
	RC.getRobot().config.positionPrecision = 0.50/1000.0

	enableRecording = True
	recordName = 'Czysty przejazd bez ograniczen'

	points = Vec4Vec()
	points[:] = [getRecord().EffectorPosition+vec4(0.1,0,0,0), vec4(-1, -3.5, 4, 1), vec4(1, -5, 2, 1), vec4(4, 0, 5, 1), vec4(2, 5, 4, 1), vec4(2,3,3,1), vec4(2,4,3,1)]
	path = addInterpolator(Interpolator.BSpline, points, "FirsRecordPath")
	RC.move(CommandReturnAction.DelAndForward).name("FirstRecord a").interpolator(path).velocity(4.0).acceleration(200.0).jointVelocity(0.5).pid(10,0,0).finish(RC)
	foo = EndTest('1.0mm')
	RC.pyExec(CommandReturnAction.DelAndForward).name("Save records").callback(foo).finish(RC)

	RC.next()
	RC.run()

	pass

def startTest_1_b():
	global recordName
	global enableRecording
	global recorder

	recorder = RecordedData()

	RC = getRC()
	saveRobot(RC)
	RC.getRobot().config.positionPrecision = 0.5/1000.0
	enableRecording = True
	recordName = 'Czysty przejazd bez ograniczen'

	points = Vec4Vec()
	points[:] = [getRecord().EffectorPosition+vec4(0.1,0,0,0), vec4(-1, -3.5, 4, 1), vec4(1, -5, 2, 1), vec4(4, 0, 5, 1), vec4(2, 5, 4, 1), vec4(2,3,3,1), vec4(2,4,3,1)]
	path = addInterpolator(Interpolator.BSpline, points, "FirsRecordPath")
	RC.move(CommandReturnAction.DelAndForward).name("FirstRecord b").interpolator(path).velocity(4.0).acceleration(20.0).jointVelocity(0.5).finish(RC)
	foo = EndTest('0.5mm')
	RC.pyExec(CommandReturnAction.DelAndForward).name("Save records").callback(foo).finish(RC)

	RC.next()
	RC.run()

	pass

def startTest_1_c():
	global recordName
	global enableRecording
	global recorder

	recorder = RecordedData()

	RC = getRC()
	saveRobot(RC)
	RC.getRobot().config.positionPrecision = 0.05/1000.0
	enableRecording = True
	recordName = 'Czysty przejazd bez ograniczen'

	points = Vec4Vec()
	points[:] = [getRecord().EffectorPosition+vec4(0.1,0,0,0), vec4(-1, -3.5, 4, 1), vec4(1, -5, 2, 1), vec4(4, 0, 5, 1), vec4(2, 5, 4, 1), vec4(2,3,3,1), vec4(2,4,3,1)]
	path = addInterpolator(Interpolator.BSpline, points, "FirsRecordPath")
	RC.move(CommandReturnAction.DelAndForward).name("FirstRecord c").interpolator(path).velocity(4.0).acceleration(20.0).jointVelocity(100.5).finish(RC)
	foo = EndTest('0.05mm')
	RC.pyExec(CommandReturnAction.DelAndForward).name("Save records").callback(foo).finish(RC)

	RC.next()
	RC.run()

	pass

def startTest_2():
	global recordName
	global enableRecording
	global recorder

	recorder = RecordedData()

	RC = getRC()
	enableRecording = True
	recordName = 'Ograniczenie zlaczowe'

	points = Vec4Vec()
	points[:] = [getRecord().EffectorPosition+vec4(0.1,0,0,0), vec4(-1, -3.5, 4, 1), vec4(1, -5, 2, 1), vec4(4, 0, 5, 1), vec4(2, 5, 4, 1), vec4(2,3,3,1), vec4(2,4,3,1)]
	path = addInterpolator(Interpolator.BSpline, points, "FirsRecordPath")
	RC.move(1).name("SecondRecord").interpolator(path).velocity(4.0).acceleration(20.0).jointVelocity(0.5).finish(RC)
	foo = EndTest('sdf')
	RC.pyExec(1).name("Save records").callback(foo).finish(RC)

	RC.next()
	RC.run()

	pass

def startTest_3():
	global recordName
	global enableRecording
	global recorder

	recorder = RecordedData()

	RC = getRC()
	enableRecording = True
	recordName = 'Orientacja'

	points = Vec4Vec()
	points[:] = [getRecord().EffectorPosition+vec4(0.1,0,0,0), vec4(-1, -3.5, 4, 1), vec4(1, -5, 2, 1), vec4(4, 0, 5, 1), vec4(2, 5, 4, 1), vec4(2,3,3,1), vec4(2,4,3,1)]
	path = addInterpolator(Interpolator.BSpline, points, "FirsRecordPath")
	RC.move(1).name("ThirdRecord").interpolator(path).velocity(4.0).acceleration(6.0).jointVelocity(10.5).startO(angleAxis(0.2, vec3(0,0,-1))).endO(angleAxis(2.3, vec3(0.5, 0, -1))).solver(SolverType.JT2).pid(100, 0, 0).finish(RC)
	foo = EndTest('43')
	RC.pyExec(1).name("Save records").callback(foo).finish(RC)

	RC.next()
	RC.run()

	pass



def update(RC, scene, dt):
	global plotRecordedData

	if enableRecording:
		recorder.saveFrameRecords(dt)

	if plotRecordedData:
		recorder.savePlotsToFile()
		plotRecordedData = False

def init(RC, scene):
	# print 'dddduuuuuuuuuuuuuupa!'
	print 'Hello! This is first entry in this script.'


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

