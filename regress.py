#!/usr/bin/env python2

import difflib
import subprocess
import time
import getopt
import sys
import os

args = lambda x: None
args.n = 100
args.t = 20000
args.q = False

def usage():
    print "Usage: regress.py [-h|--help] [-n NUMAGENTS] [-t MAXTIME] [-q]"
    print "    -h --help    : Print usage."
    print "    -n NUMAGENTS : Number of agents (0 < NUMAGENTS <= 100000)."
    print "    -t MAXTIME   : Maximum number of timesteps."
    print "    -q           : Use quadtree."

try:
    opts, _ = getopt.getopt(sys.argv[1:], "hn:t:q", ["help"])
except getopt.GetopotError as err:
    print(str(err))
    usage()
    sys.exit(2)

for o, a in opts:
    if o in ["-h", "--help"]:
        usage()
        sys.exit()
    elif o == '-n':
        args.n = int(a)
    elif o == '-t':
        args.t = int(a)
    elif o == '-q':
        args.q = True

if args.n <= 0 or args.n > 100000:
    print("[-] Invalid number of agents (0 < n <= 100000)")
    sys.exit(0)

print("[.] making libraries")
subprocess.check_output(["make"], cwd="./libpedsim")
subprocess.check_output(["make"], cwd="./libpedsimpar")

print("[.] making test executables")
subprocess.check_output("g++ ./regress/regress.cpp -o regress/regress_seq -Ilibpedsim -lpedsim -Llibpedsim -g -std=c++0x", shell=True)
if os.path.exists("/opt/cuda-8.0/"):
    cudalib = "-L/opt/cuda-8.0/lib64/ -lcudart"
else:
    cudalib = "-L/usr/local/depot/cuda-8.0/lib64/ -lcudart"
subprocess.check_output("g++ ./regress/regress.cpp -o regress/regress_par -Ilibpedsimpar -lpedsimpar -Llibpedsimpar {} -g -std=c++0x".format(cudalib), shell=True)

print("[.] running parallel")
start = time.time()
subprocess.check_output("LD_LIBRARY_PATH=./libpedsimpar/:${{LD_LIBRARY_PATH}} ./regress/regress_par -n {} -t {} {}".format(args.n, args.t, "-q" if args.q else ""), shell=True)
subprocess.check_output("mv ./pedsim_out.txt ./pedsim_out_par.txt", shell=True)
end = time.time()
partime = end-start
print("Parallel:   {} ms".format(partime*1000.0))

print("[.] running sequential")
start = time.time()
subprocess.check_output("LD_LIBRARY_PATH=./libpedsim/:${{LD_LIBRARY_PATH}} ./regress/regress_seq -n {} -t {} {}".format(args.n, args.t, "-q" if args.q else ""), shell=True)
subprocess.check_output("mv ./pedsim_out.txt ./pedsim_out_seq.txt", shell=True)
end = time.time()
seqtime = end-start
print("Sequential: {} ms".format(seqtime*1000.0))

print("Speedup: {}".format(seqtime/partime))

print("[.] checking output")

outputSeq = open("./pedsim_out_seq.txt").read().strip().splitlines()
outputPar = open("./pedsim_out_par.txt").read().strip().splitlines()

if outputSeq != outputPar:
    print("[-] MISMATCH:")
    for line in difflib.unified_diff(outputSeq, outputPar, lineterm=''):
        print(line)
else:
    print("[+] Passed.")
