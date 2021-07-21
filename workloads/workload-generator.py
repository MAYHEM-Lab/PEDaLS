from random import choice
from random import randint
from random import seed

class WorkloadGenerator(object):
	
	def __init__(self, numNodes=3, numOps=20, minVal=1, maxVal=100, updateProb=20):
		self.numNodes = numNodes
		self.numOps = numOps
		self.minVal = minVal
		self.maxVal = maxVal
		self.nodeIDs = map(chr, range(ord('A'), ord('A') + self.numNodes))
		self.vals = {}
		self.ops = {}
		for nodeID in self.nodeIDs:
			self.vals[nodeID] = []
			self.ops[nodeID] = []
		self.updateProb = updateProb

	def generateWorkload(self):
		seed()
		opChoice = [0] * ((100 - self.updateProb) / 2) + [1] * ((100 - self.updateProb) / 2) + [2] * self.updateProb
		for numOp in range(self.numOps):
			val = -1
			op = choice(opChoice)
			target = choice(self.nodeIDs)
			if op == 2:
				if self.numNodes > 1:
					val = choice(self.nodeIDs)
					while val == target:
						val = choice(self.nodeIDs)
				else:
					opChoice = [0] * 50 + [1] * 50
					op = choice(opChoice)
			if op == 1:
				if len(self.vals[target]) < (self.maxVal - self.minVal + 1):
					val = randint(self.minVal, self.maxVal)
					while val in self.vals[target]:
						val += 1
						if val > self.maxVal:
							val = self.minVal
				else:
					op = 0
					val = choice(self.vals[target])
			elif op == 0:
				if len(self.vals[target]) == 0:
					op = 1
					val = randint(self.minVal, self.maxVal)
				else:
					val = choice(self.vals[target])
			if op == 1:
				self.vals[target].append(val)
				self.ops[target].append((op,val))
			elif op == 0:
				self.vals[target].remove(val)
				self.ops[target].append((op,val))
			else:
				pendingDels = []
				temp = []
				for x in self.ops[target]:
					if x[0] == 1:
						temp.append(x[1])
					if x[0] == 0:
						if x[1] in temp:
							temp.remove(x[1])
						else:
							pendingDels.append(x[1])
				for x in self.ops[val]:
					if x[0] == 1:
						temp.append(x[1])
					if x[0] == 0:
						if x[1] in temp:
							temp.remove(x[1])
						else:
							pendingDels.append(x[1])
				for x in pendingDels:
					if x in temp:
						temp.remove(x)
				self.vals[target] = temp
			print('{},{},{}'.format(op, target, val))
		for x in range(1, len(self.nodeIDs)):
			print('{},{},{}'.format(2, self.nodeIDs[0], self.nodeIDs[x]))
		for x in range(1, len(self.nodeIDs)):
			print('{},{},{}'.format(2, self.nodeIDs[x], self.nodeIDs[0]))
		print(-1)

if __name__ == '__main__':
	wg = WorkloadGenerator(numNodes=1, numOps=1000, minVal=1, maxVal=100, updateProb=20)
	wg.generateWorkload()
{"mode":"full","isActive":false}
