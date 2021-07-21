import subprocess
import sys

n = len(sys.argv)
if n != 2:
	print('USAGE: {} <code: 1 for update, 2 for access>'.format(sys.argv[0]))
	sys.exit()

for i in range(1, 101):
	subprocess.call(["./app", "../workloads/workload-{}.txt".format(str(i).zfill(3)), sys.argv[1]])
