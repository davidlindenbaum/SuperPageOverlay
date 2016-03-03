#!/usr/bin/python
import sys
import subprocess

printOutput = False

cmd = sys.argv[1]
count = int(sys.argv[2]) if len(sys.argv) > 2 else 1
results = {}

cmd = 'perf stat --log-fd 1 -e faults,dTLB-stores-misses,cs ' + cmd
for i in xrange(count):
    process = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE)
    output = process.communicate()[0].split('\n')
    foundStats = False
    for line in output:
        if foundStats:
            line = line.strip()
            if line != '':
                res = line.split()
                if not res[1] in results: results[res[1]] = [float(res[0].replace(',',''))]
                else: results[res[1]] += [float(res[0].replace(',',''))]
        elif not line.strip().startswith('Performance counter stats for'):
            if printOutput: print line
        else: foundStats = True

results['milliseconds'] = [x * 1000 for x in results.pop('seconds')]
cols = 3
row_format ="{:>" + str(max(len(k) for k in results)) + "}" + ("{:>12}" * cols)
print row_format.format('', 'average', 'min', 'max')
for key in results:
    print row_format.format(key, sum(results[key])/len(results[key]),
            min(results[key]), max(results[key]))
