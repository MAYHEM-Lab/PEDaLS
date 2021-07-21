import subprocess
import sys

n = len(sys.argv)
if n != 2:
	print('USAGE: {} <code: 1 for update, 2 for access>'.format(sys.argv[0]))
	sys.exit()

if sys.argv[1] == '1':
	extra_links = [1, 5, 10]
else:
	extra_links = [1]
for i in range(1, 101):
	for extra_link in extra_links:
		subprocess.call(["./app", "../workloads/workload-{}.txt".format(str(i).zfill(3)), str(extra_link), sys.argv[1]])
