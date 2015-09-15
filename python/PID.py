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

class PID:
	def __init__(self, K, Ti, Td):
		self.K = K
		self.Ti = Ti
		self.Td = Td
		self.e = 0
		self.integral = 0
	def up(self, target, value):
		K = self.K
		Ti = self.Ti
		Td = self.Td

		e = target - value
		self.integral += e
		derivative = e - self.e
		self.e = e



		out = (K*e  + Td*derivative + Ti*self.integral)
		return out


def test():
	pid = PID(0.1, 0.01, 0)

	n = 1000
	arr = np.zeros(n)
	v = 0.0

	for i in range(500):
		v += pid.up(4.24, v)
		arr[i] = v
	for i in range(500, n):
		v += pid.up(2.15, v)
		arr[i] = v

	plt.plot(arr)
	plt.show()

test()